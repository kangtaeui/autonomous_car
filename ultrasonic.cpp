#include "ultrasonic.h"
#include <iostream>
#include <cmath>

const double UltraSound::DISTANCE_CONVERSION_FACTOR = 58.7734;
//나눠주는 값 58.7734

void UltraSound::Init(unsigned int trigger, unsigned int echo, int sensorNumber) {
    this->trigger = trigger;    //trigger pin set
    this->echo = echo;          //echo pin set
    this->sensorNumber = sensorNumber;    //sensor's Number set

    gpioSetMode(trigger, PI_OUTPUT);        //trigger pin mapping in gpio
    gpioSetMode(echo, PI_INPUT);            //echo pin mapping in gpio
    gpioWrite(trigger, 0);
    gpioSleep(PI_TIME_RELATIVE, 0.5, 0);

    //mutex initialize
    if (pthread_mutex_init(&mutex, nullptr) != 0) {
        std::cerr << "Mutex initialization failed. Exiting..." << std::endl;
        exit(-1);
    }
}

double UltraSound::GetDistance(unsigned int timeout) {
    //this is mutex method, so you have to 
    pthread_mutex_lock(const_cast<pthread_mutex_t*>(&mutex));

    //sensor trigger occur
    endTimeUsec = 0;
    gpioWrite(trigger, 1);
    gpioDelay(10);
    gpioWrite(trigger, 0);

    now = gpioTick();

    while (endTimeUsec == 0 && gpioTick() - now < timeout) {
        RecordPulseLength();
    }

    
    DifferenceTimeUsec = endTimeUsec - startTimeUsec;
    distanceCm = (double)DifferenceTimeUsec / DISTANCE_CONVERSION_FACTOR;        //divide 58.7734
    
    //distance == previous? unlock
    if (distanceCm == previousDistanceCm) {
        pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));
        return distanceCm;
    }

    //new value in previousDistanceCm
    previousDistanceCm = distanceCm;
    //저장했으면 unlock
    pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));

    if (endTimeUsec != 0)
        return distanceCm;
    else
        return 0;
}

double UltraSound::GetLatestDistance() const {
    pthread_mutex_lock(const_cast<pthread_mutex_t*>(&mutex));
    //mutex를 사용하여 다른 스레드와의 동기화보장
    double latestDistance = distanceCm;

    pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));
    //저장하고 unlock
    return latestDistance;
}

void UltraSound::SetLatestDistance(double distance) {
    pthread_mutex_lock(const_cast<pthread_mutex_t*>(&mutex));
    //mutex를 사용하여 다른 스레드와의 동기화보장
    distanceCm = distance;

    pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));
    //저장하고 unlock
}

void UltraSound::RecordPulseLength() {
    startTimeUsec = gpioTick();
    while (gpioRead(echo) == 1) {
        endTimeUsec = gpioTick();
    }
}

int UltraSound::GetSensorNumber() const {
    return sensorNumber;
}



