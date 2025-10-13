#ifndef LOG_LIBRARY_H
#define LOG_LIBRARY_H

#include <Arduino.h>
#include <HardwareSerial.h>

class Log
{
private:
    HardwareSerial &logSerial;
    int logNumber;
    String *logs;

public:
    Log(HardwareSerial &serial, int baudRate);
    ~Log();
    void init(int number);
    void logI(int line, const char *message);
    void logE(int line, const char *message);
    void logD(int line, const char *message);
    void showLog();
};
#endif
