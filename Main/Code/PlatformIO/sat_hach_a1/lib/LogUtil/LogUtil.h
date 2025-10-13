#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define SERIAL_LOG Serial

extern SemaphoreHandle_t serialMutex;

void logSerial(String message);