#ifndef MOTORBIKE_H
#define MOTORBIKE_H

#define MOTORBITE_SPEED
#define MOTORBITE_DIRECTION
#define MOTORBITE_SIGNEL
#define MOTORBITE_SENSOR






#include <Arduino.h>


class Motorbike
{
private:
    bool signelLeft;
    bool sensorHall;
    bool signelEngine;
    uint32_t encoderCount;
    uint32_t speed;
public:
    Motorbike(/* args */);
    ~Motorbike();

    void setSpeed(uint32_t speed);
    uint32_t getSpeed();
    
    void setSignelLeft(bool signelLeft);
    bool getSignelLeft();
    
    void setSensorHall(bool sensorHall);
    bool getSensorHall();
    
    void setSignelEngine(bool signelEngine);
    bool getSignelEngine();
    
    void setEncoderCount(uint32_t encoderCount);
    uint32_t getEncoderCount();

};
#endif // MOTORBIKE_H