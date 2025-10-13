#ifndef MICROSDTASK_H
#define MICROSDTASK_H

#include <Arduino.h>
#include "SD_MMC.h"
#include "FS.h"

#define SD_MMC_CMD 38 // Please do not modify it.
#define SD_MMC_CLK 39 // Please do not modify it.
#define SD_MMC_D0 40  // Please do not modify it.

#define WEBSERVER_HTML_PATH "/sdcard/Webserver/index.html"
#define WEBSERVER_CSS_PATH "/sdcard/Webserver/style.css"
#define WEBSERVER_JS_PATH "/sdcard/Webserver/script.js"

#define CONFIG_CONTEST1_PATH "/sdcard/Contest/contest1.txt"
#define CONFIG_CONTEST2_PATH "/sdcard/Contest/contest2.txt"
#define CONFIG_CONTEST3_PATH "/sdcard/Contest/contest3.txt"
#define CONFIG_CONTEST4_PATH "/sdcard/Contest/contest4.txt"

#define SYSTEM_LOG_PATH "/sdcard/System/log.txt"
#define SYSTEM_IP_PATH "/sdcard/System/ip.txt"
#define SYSTEM_WIFI_CONFIG_PATH "/sdcard/System/wifi.txt"

class MicroSDTask
{
private:
    uint8_t _cardType;
    uint64_t _cardSize;
    bool _isMounted;
    String _error;

public:
    MicroSDTask();
    bool begin();
    String readData(const char *path);
    bool writeData(const char *path, const char *data);
    bool addData(const char *path, const char *data);
    String getError() { return _error; }
    File openFile(const char *path, const char *mode = FILE_READ);
    void closeFile(File &file);
};

#endif // MICROSDTASK_H