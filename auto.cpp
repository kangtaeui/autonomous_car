#include <pigpio.h>
#include <iostream>
#include <csignal>
#include <pthread.h>
#include <thread>
#include <chrono>
#include "motor_control.h"
#include "ultrasonic.h"
#include <unistd.h>
#include <cmath>


bool on_flag = false;
bool running = true;


const char* device = "/dev/ttyS0";    //set uart port
const int baudrate = 9600;        //set uart communication speed.

char* mutableDevice = const_cast<char*>(device);
int serial_port = serOpen(mutableDevice, baudrate, 0);

const double TURN_THRESHOLD_DISTANCE = 38.0;    //Turn left or right if the distance is less than 38.0

void *ThreadFunction(void* arg) {
    //ultrasound를 동적 생성
    UltraSound* us1 = new UltraSound();
    us1->Init(0, 1, 1);

    UltraSound* us2 = new UltraSound();
    us2->Init(5, 6, 2);

    UltraSound* us3 = new UltraSound();
    us3->Init(26, 16, 3);

    while (running) 
    {
        gpioSleep(PI_TIME_RELATIVE, 0, 500000);
        double distance1 = us1->GetDistance(30000);
        double distance2 = us2->GetDistance(30000);
        double distance3 = us3->GetDistance(30000);
        //최근값 불러오기 값을 읽어들일 때 뮤텍스를 lock과 unlock을 한다. 뮤텍스를 보호하기 위해서
        us1->SetLatestDistance(distance1);
        us2->SetLatestDistance(distance2);
        us3->SetLatestDistance(distance3);

        std::cout << "U1 Distance: " << distance1 << " cm." << std::endl;
        std::cout << "U2 Distance: " << distance2 << " cm." << std::endl;
        std::cout << "U3 Distance: " << distance3 << " cm." << std::endl;

        if (on_flag) {  //on_flag is bool type
            std::cout << "Continuing forward" << std::endl;
            Driving_forward(IN1, IN2, IN3, IN4, 113);
            if( distance2 <= TURN_THRESHOLD_DISTANCE+3) // 중앙 거리가 41보다 작다면
            {  
                if(distance1==distance3){       //같아지면 직진
                Driving_forward(IN1,IN2,IN3,IN4,110);
                }
                if(distance1 >= distance3)      //좌가 우보다 크거나 같다면 좌회전
                {
                Driving_left(IN1, IN2, IN3, IN4, 111);
                
                }
                else if (distance3 >= distance1)   //우가 좌보다 크거나 같다면 우회전
                {
                    Driving_right(IN1, IN2, IN3, IN4, 111);
                }
            }
            //모두 다 막혀있다면?
            if (distance1<=TURN_THRESHOLD_DISTANCE+10&&distance2<=TURN_THRESHOLD_DISTANCE+10&&distance3<=TURN_THRESHOLD_DISTANCE+10) {
                Driving_backward(IN1, IN2, IN3, IN4,120);
                Driving_right(IN1,IN2,IN3,IN4,110);
            }
            //좌측 센서만 33보다 작다면 우회전
            else if (distance1 <= TURN_THRESHOLD_DISTANCE-5.0 && distance2 >=45.0&&distance3>TURN_THRESHOLD_DISTANCE-3.0) {
            
                std::cout << "Performing right turn" << std::endl;
                Driving_right(IN1, IN2, IN3, IN4, 105);
            }
            //우측 센서만 33보다 작다면 좌회전
            else if (distance1>TURN_THRESHOLD_DISTANCE-5.0&&distance3 <= TURN_THRESHOLD_DISTANCE-5 && distance2 >= 45.0) {
                std::cout << "Performing left turn" << std::endl;
                Driving_left(IN1, IN2, IN3, IN4, 115);
            }
        }
        
    }
    //while문 벗어나면 delete
    delete us1;
    delete us2;
    delete us3;

    return nullptr;
}

UltraSound US1, US2, US3;
//bluetooth thread
void *BluetoothControlThread(void* arg) {
    int serial_port = *(int*)arg;

    try {
        while (running) {    //running is initialized true.
            if (serDataAvailable(serial_port) > 0) {
                char input = serReadByte(serial_port);    //receive data
                if (input == '2' && on_flag) {    //if user input '2', Drive stop and on_flag's initialized 0
                    std::cout << "Stopping motors" << std::endl;
                    Driving_stop(IN1, IN2, IN3, IN4, 100);
                    on_flag = false;
                } else if (input == '1') {
                    on_flag = true;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Bluetooth communication error: " << e.what() << std::endl;
    }
    return nullptr;
}

//if you input crtl+c, stop compile and Drive stop.
void signalHandler(int signum) {
    gpioPWM(ENA, 0);
    gpioPWM(ENB, 0);
    gpioTerminate();
    exit(signum);
}

int main() {
    gpioInitialise();    //gpioInitialze when using a multithread.

    signal(SIGINT, signalHandler);    //this code make complier can recieve end signal.

    const char* device = "/dev/ttyS0";    //set uart port
    const int baudrate = 9600;      //set Baudrate 

    //gpio initialize
    if (gpioInitialise() < 0) {
        std::cerr << "GPIO initialization failed. Exiting..." << std::endl;
        return 1;
    }

    char* mutableDevice = const_cast<char*>(device);
    int serial_port = serOpen(mutableDevice, baudrate, 0);    //serial port open
    if (serial_port < 0) {
        std::cerr << "Failed to open serial port. Exiting..." << std::endl;
        gpioTerminate();
        return 1;
    }
    //US1,2,3 초기화
    US1.Init(0, 1, 1);
    US2.Init(5, 6, 2);
    US3.Init(26, 16, 3);

    //각 스레드 생성
    pthread_t thread1, thread2, thread3, bluetoothThread;
    //ultra1
    if (pthread_create(&thread1, nullptr, ThreadFunction, reinterpret_cast<void*>(&US1)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }
    //ultra2
    if (pthread_create(&thread2, nullptr, ThreadFunction, reinterpret_cast<void*>(&US2)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }
    //ultra3
    if (pthread_create(&thread3, nullptr, ThreadFunction, reinterpret_cast<void*>(&US3)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }
    //bluetooth
    if (pthread_create(&bluetoothThread, nullptr, BluetoothControlThread, reinterpret_cast<void*>(&serial_port)) != 0) {
        std::cerr << "Thread creation failed. Exiting..." << std::endl;
        return -1;
    }

    //thread join
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);
    pthread_join(thread3, nullptr);
    pthread_join(bluetoothThread, nullptr);
    
    //gpio mapping     IN1,IN2,IN3,IN4,ENA,ENB
    //Initialize ENA, ENB PWM
    gpioSetMode(IN1, PI_OUTPUT);
    gpioSetMode(IN2, PI_OUTPUT);
    gpioSetMode(IN3, PI_OUTPUT);
    gpioSetMode(IN4, PI_OUTPUT);
    gpioSetMode(ENA, PI_OUTPUT);
    gpioSetMode(ENB, PI_OUTPUT);
    gpioSetPWMfrequency(ENA, 50);
    gpioSetPWMfrequency(ENB, 50);
    
    //Serial close
    serClose(serial_port);
    
    //gpio close
    gpioTerminate();

    return 0;
}
