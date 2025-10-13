#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Motorbike.h>
#include <LogUtil.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include "defineName.h"
#include <HardwareManager.h>

extern TaskHandle_t _contestHandle;

class ContestManager
{
private:
    HardwareManager *hardwareManager;
    uint8_t _contestIndex[4];
    const char *_contestName[4] = {
        "Đi qua hình số 8",
        "Đi qua vạch đường thẳng",
        "Đi qua đường có vạch cản",
        "Đi qua đường gồ ghề"};
    bool _isRunning;
    unsigned long _startTime;

    struct LogEntry
    {
        uint16_t counter;
        uint8_t eventID;
        String description;
        uint32_t timestamp;
    };
    String _logPath;
    LogEntry _logEntries;

    uint32_t _distance[11];

    static void _Runner(void *pvParameters);
    void _runContest();

    void _contest1Runer();
    void _contest2Runer();
    void _contest3Runer();
    void _contest4Runer();

public:
    ContestManager(HardwareManager &hwManager);
    ~ContestManager();
    void begin();
    void loadConfig(JsonDocument doc);
    bool run();
    void stop();
    bool addContest(uint8_t contestID, uint8_t index);
    void removeContest(int index);
    void clearAllContest();
    void addLog(uint8_t eventID, String description);
};
