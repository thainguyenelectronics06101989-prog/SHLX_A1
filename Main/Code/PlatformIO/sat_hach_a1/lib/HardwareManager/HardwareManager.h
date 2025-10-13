#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <MicroSDTask.h>
#include <CameraHandler.h>
#include <SerialHandle.h>
#include <DateTime.h>

#define SERIAL_OUTPUT Serial0
#define SERIAL_BAUDRATE 115200

class HardwareManager
{
private:
    // Private members and methods for hardware management
public:
    MicroSDTask microSD;
    CameraHandler camera;
    SerialHandle serialLog;
    DateTime dateTime;
    HardwareManager();
    ~HardwareManager();
    void begin();
    void update();
};