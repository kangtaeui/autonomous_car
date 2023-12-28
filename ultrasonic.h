#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <pigpio.h>

class UltraSound {
public:
    void Init(unsigned int trigger, unsigned int echo, int sensorNumber, double xCoordinate);
    double GetDistance(unsigned int timeout);
    int GetSensorNumber() const;
    double GetLatestDistance() const;
    void SetLatestDistance(double distance);

private:
    void RecordPulseLength();
    int trigger;
    int echo;
    int sensorNumber;
    double xCoordinate;

    volatile long startTimeUsec;
    volatile long endTimeUsec;

    double distanceCm;
    double previousDistanceCm;
    long DifferenceTimeUsec;
    long now;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    static const double DISTANCE_CONVERSION_FACTOR;
};

#endif // ULTRASONIC_H
