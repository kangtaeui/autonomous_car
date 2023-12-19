// main.cpp

#include <pigpio.h>
#include <iostream>
#include <csignal>
#include <pthread.h>
#include <thread>
#include <chrono>
#include "motor_control.h"
#include <unistd.h>
#include "ultrasonic.h"
#include "motor_control.h"

bool on_flag = false;
bool running = true;

double x = 0.0;
double y = 10.0;
double z = 20.0;

const double TURN_THRESHOLD_DISTANCE = 38.0;

void *ThreadFunction(void* arg);

int main() {
    gpioInitialise();

    signal(SIGINT, signalHandler);

    const char* device = "/dev/ttyS0";
    const int baudrate = 9600;

    if (gpioInitialise() < 0) {
        std::cerr << "GPIO initialization failed. Exiting..." << std::endl;
        return 1;
    }

    char* mutableDevice = const_cast<char*>(device);
    int serial_port = serOpen(mutableDevice, baudrate, 0);
    if (serial_port < 0) {
        std::cerr << "Failed to open serial port. Exiting..." << std::endl;
        gpioTerminate();
        return 1;
    }

    double xCoordinateUS1 = 0.0;
    double xCoordinateUS2 = 10.0;
    double xCoordinateUS3 = 20.0;

    UltraSound US1, US2, US3;

    US1.Init(0, 1, 1, xCoordinateUS1);
    US2.Init(5, 6, 2, xCoordinateUS2);
    US3.Init(26, 16, 3, xCoordinateUS3);

    pthread_t thread1, thread2, thread3, bluetoothThread;

    if (pthread_create(&thread1, nullptr, ThreadFunction, reinterpret_cast<void*>(&US1)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }

    if (pthread_create(&thread2, nullptr, ThreadFunction, reinterpret_cast<void*>(&US2)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }

    if (pthread_create(&thread3, nullptr, ThreadFunction, reinterpret_cast<void*>(&US3)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }

    if (pthread_create(&bluetoothThread, nullptr, BluetoothControlThread, reinterpret_cast<void*>(&serial_port)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }

    while (running) {
        // Do nothing
    }

    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    pthread_join(thread3, nullptr);
    pthread_join(bluetoothThread, nullptr);

    gpioSetMode(IN1, PI_OUTPUT);
    gpioSetMode(IN2, PI_OUTPUT);
    gpioSetMode(IN3, PI_OUTPUT);
    gpioSetMode(IN4, PI_OUTPUT);
    gpioSetMode(ENABLE, PI_OUTPUT);
    gpioSetMode(ENB, PI_OUTPUT);

    gpioSetPWMfrequency(ENABLE, 50);
    gpioSetPWMfrequency(ENB, 50);

    serClose(serial_port);
    gpioTerminate();

    return 0;
}
