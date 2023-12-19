#ifndef ULTRASOUND_H
#define ULTRASOUND_H

#include <pigpio.h>
#include <pthread.h>

class UltraSound {
public:
    UltraSound(unsigned int trigger, unsigned int echo, int sensorNumber, double xCoordinate);
    void Init();
    double GetDistance(unsigned int timeout);
    double GetHorizontalDistance(const UltraSound& otherSensor) const;
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

#endif
