#include "motor_control.h"
#include <pigpio.h>
//하나하나 맵핑하여 어떻게 동작할지 넣어주기
void Driving_forward(int in1, int in2, int in3, int in4, int speed)
{
    gpioWrite(in1, 0);
    gpioWrite(in2, 1);
    gpioWrite(in3, 0);
    gpioWrite(in4, 1);
    gpioPWM(ENA, speed);
    gpioPWM(ENB, speed);
}

void Driving_backward(int in1, int in2, int in3, int in4, int speed)
{
    gpioWrite(in1, 1);
    gpioWrite(in2, 0);
    gpioWrite(in3, 1);
    gpioWrite(in4, 0);
    gpioPWM(ENA, speed);
    gpioPWM(ENB, speed);
}

void Driving_left(int in1, int in2, int in3, int in4, int speed)
{
    gpioWrite(in1, 0);
    gpioWrite(in2, 1);
    gpioWrite(in3, 0);
    gpioWrite(in4, 0);
    gpioPWM(ENA, speed);
    gpioPWM(ENB, speed);
}

void Driving_right(int in1, int in2, int in3, int in4, int speed)
{
    gpioWrite(in1, 0);
    gpioWrite(in2, 0);
    gpioWrite(in3, 0);
    gpioWrite(in4, 1);
    gpioPWM(ENA, speed);
    gpioPWM(ENB, speed);
}

void Driving_stop(int in1, int in2, int in3, int in4)
{
    gpioWrite(in1, 1);
    gpioWrite(in2, 1);
    gpioWrite(in3, 1);
    gpioWrite(in4, 1);
    gpioPWM(ENA, speed);
    gpioPWM(ENB, speed);
}
