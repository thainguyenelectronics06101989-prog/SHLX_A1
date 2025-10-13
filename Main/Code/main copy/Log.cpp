#include "Log.h"
#include <Arduino.h>
#include <HardwareSerial.h>

Log::Log(HardwareSerial &serial, int baudRate)
    : logSerial(serial)
{
  logSerial.begin(baudRate);
  logNumber = 0;
}
Log::~Log()
{
  logSerial.end();
}
void Log::init(int number)
{
  logNumber = number;
  if (logs != nullptr)
  {
    delete[] logs;
  }
  logs = new String[number]; // cấp phát động mảng String
}
void Log::logI(int line, const char *message)
{
  if (line < logNumber)
  {
    logs[line] = "L" + String(line) + ":\t|I|\t" + String(message) + "\n";
  }
}
void Log::logE(int line, const char *message)
{
  if (line < logNumber)
  {
    logs[line] = String(line) + ":\t|E|\t" + String(message) + "\n";
  }
}
void Log::logD(int line, const char *message)
{
  if (line < logNumber)
  {
    logs[line] = String(line) + ":\t|D|\t" + String(message) + "\n";
  }
}
void Log::showLog()
{
  logSerial.println("-------------------------------------------------------------");
  logSerial.print("N:\t|L|\tmessenge\n");
  for (int i = 0; i < logNumber; i++)
  {
    logSerial.print(logs[i]);
  }
}