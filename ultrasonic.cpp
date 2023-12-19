#include "ultrasonic.h"
#include <iostream>
#include <cmath>

const double UltraSound::DISTANCE_CONVERSION_FACTOR = 58.7734;

void UltraSound::Init(unsigned int trigger, unsigned int echo, int sensorNumber, double xCoordinate) {
    this->trigger = trigger;
    this->echo = echo;
    this->sensorNumber = sensorNumber;
    this->xCoordinate = xCoordinate;

    gpioSetMode(trigger, PI_OUTPUT);
    gpioSetMode(echo, PI_INPUT);
    gpioWrite(trigger, 0);
    gpioSleep(PI_TIME_RELATIVE, 0.5, 0);

    if (pthread_mutex_init(&mutex, nullptr) != 0) {
        std::cerr << "Mutex initialization failed. Exiting..." << std::endl;
        exit(-1);
    }
}
double UltraSound::GetHorizontalDistance(const UltraSound& otherSensor) const {
    return std::abs(xCoordinate - otherSensor.xCoordinate);
}


double UltraSound::GetDistance(unsigned int timeout) {
    pthread_mutex_lock(const_cast<pthread_mutex_t*>(&mutex));

    endTimeUsec = 0;
    gpioWrite(trigger, 1);
    gpioDelay(10);
    gpioWrite(trigger, 0);

    now = gpioTick();

    while (endTimeUsec == 0 && gpioTick() - now < timeout) {
        RecordPulseLength();
    }

    DifferenceTimeUsec = endTimeUsec - startTimeUsec;
    distanceCm = (double)DifferenceTimeUsec / DISTANCE_CONVERSION_FACTOR;

    if (distanceCm == previousDistanceCm) {
        pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));
        return distanceCm;
    }

    previousDistanceCm = distanceCm;

    pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));

    if (endTimeUsec != 0)
        return distanceCm;
    else
        return 0;
}

double UltraSound::GetLatestDistance() const {
    pthread_mutex_lock(const_cast<pthread_mutex_t*>(&mutex));

    double latestDistance = distanceCm;

    pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));

    return latestDistance;
}

void UltraSound::SetLatestDistance(double distance) {
    pthread_mutex_lock(const_cast<pthread_mutex_t*>(&mutex));

    distanceCm = distance;

    pthread_mutex_unlock(const_cast<pthread_mutex_t*>(&mutex));
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



