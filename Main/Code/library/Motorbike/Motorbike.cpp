#include "Motorbike.h"

Motorbike::Motorbike(uint8_t H_PIN, uint8_t SL_PIN, uint8_t EN_PIN, uint8_t ENA_PIN, uint8_t ENB_PIN)
{
    this->hall_pin = H_PIN;
    this->signel_left_pin = SL_PIN;
    this->engine_pin = EN_PIN;
    this->encoder_a_pin = ENA_PIN;
    this->encoder_b_pin = ENB_PIN;
    this->mode_encoder = TWO_SIGNEL_ENCODER_MODE;
}
Motorbike::Motorbike(uint8_t H_PIN, uint8_t SL_PIN, uint8_t EN_PIN, uint8_t F_PIN)
{
    this->hall_pin = H_PIN;
    this->signel_left_pin = SL_PIN;
    this->engine_pin = EN_PIN;
    this->encoder_a_pin = F_PIN;
    this->mode_encoder = ONE_SIGNEL_ENCODER_MODE;
}

Motorbike::~Motorbike()
{
}
void Motorbike::attachPhaseA(void *arg)
{
    Motorbike* m = static_cast<Motorbike*>(arg);
    m->encoderCount++;
}
void Motorbike::attachPhaseB(void *arg)
{
    Motorbike* m = static_cast<Motorbike*>(arg);
    m->encoderCount--;
}
void Motorbike::init()
{
    pinMode(this->hall_pin, INPUT);
    pinMode(this->signel_left_pin, INPUT);
    pinMode(this->engine_pin, INPUT);

    pinMode(this->encoder_a_pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(this->encoder_a_pin), Motorbike::attachPhaseA, this, RISING);
    if (this->mode_encoder == TWO_SIGNEL_ENCODER_MODE)
    {
        pinMode(this->encoder_b_pin, INPUT_PULLUP);
        attachInterruptArg(digitalPinToInterrupt(this->encoder_b_pin), Motorbike::attachPhaseB, this, RISING);
    }
    this->encoderCount = 0;
    this->distance = 0;
    this->speed = 0;
    this->signelLeft = false;
    this->sensorHall = false;
    this->signelEngine = false;
    this->attachLine = false;
}
void Motorbike::update() // update sensor sate of motor
{
    this->signelLeft = digitalRead(this->signel_left_pin);
    this->sensorHall = digitalRead(this->hall_pin);
    this->signelEngine = digitalRead(this->engine_pin);

    this->distance = this->encoderCount * ENCODER_SCALE;

    if (millis() - this->lastTime > TIMING_UPDATE_SPEED)
    {
        uint32_t deltaTime = millis() - this->lastTime;
        this->speed = this->distance * 1000 / deltaTime;
        this->lastTime = millis();
    }
}
void Motorbike::setAttachLine(bool attach) // set attach line of motor
{
    this->attachLine = attach;
}
bool Motorbike::getSignelLeft() // return signel left of motor
{
    return this->signelLeft;
}
bool Motorbike::getSensorHall() // return sensor hall of motor
{
    return this->sensorHall;
}
bool Motorbike::getSignelEngine() // return signel engine of motor
{
    return this->signelEngine;
}
uint32_t Motorbike::getEncoderCount() // return encoder count
{
    return this->encoderCount;
}
uint32_t Motorbike::getDistance() // return distance of motor
{
    return this->distance;
}
uint32_t Motorbike::getSpeed() // return speed of motor
{
    return this->speed;
}
