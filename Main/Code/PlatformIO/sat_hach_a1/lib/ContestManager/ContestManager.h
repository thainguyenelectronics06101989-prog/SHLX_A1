#pragma once
#include <Myconfig.h>
#include "defineName.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Motorbike.h>
#include <LogUtil.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
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
    uint32_t startTime = 0;
    uint8_t hallCount = 0;
    uint8_t errOverrideStart = 0;
    uint8_t errRunoutTimeStart = 0;
    uint8_t errRunoutTimeContest = 0;
    uint8_t errGoWrongWay = 0;
    uint8_t errStopEngine = 0;
    uint8_t errNoSignalLeft = 0;

    static void _Runner(void *pvParameters);
    void _runContest();

    bool _contest1Runer();
    bool _contest2Runer();
    bool _contest3Runer();
    bool _contest4Runer();
    bool inDeltaValue(uint32_t value, uint32_t target, uint32_t delta);

public:
    ContestManager(HardwareManager &hwManager);
    ~ContestManager();
    void begin();
    void loadConfig(String str);
    bool run();
    void stop();
    bool addContest(uint8_t contestID, uint8_t index);
    void removeContest(int index);
    void clearAllContest();
    void addLog(uint8_t eventID, String description);
};
