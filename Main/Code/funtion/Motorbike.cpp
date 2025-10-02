#include "Motorbike.h"

Motorbike::Motorbike(/* args */)
{
}

Motorbike::~Motorbike()
{
}

void Motorbike::setSpeed(uint32_t speed)
{
    this->speed = speed;
}
uint32_t Motorbike::getSpeed()
{
    return this->speed;
}

void Motorbike::setSignelLeft(bool signelLeft)
{
    this->signelLeft = signelLeft;
}
bool Motorbike::getSignelLeft()
{
    return this->signelLeft;
}
bool Motorbike::getSensorHall()
{
    return this->sensorHall;
}
void Motorbike::setSensorHall(bool sensorHall)
{
    this->sensorHall = sensorHall;
}
void Motorbike::setSignelEngine(bool signelEngine)
{
    this->signelEngine = signelEngine;
}
bool Motorbike::getSignelEngine()
{
    return this->signelEngine;
}
void Motorbike::setEncoderCount(uint32_t encoderCount)
{
    this->encoderCount = encoderCount;
}
uint32_t Motorbike::getEncoderCount()
{
    return this->encoderCount;
}
