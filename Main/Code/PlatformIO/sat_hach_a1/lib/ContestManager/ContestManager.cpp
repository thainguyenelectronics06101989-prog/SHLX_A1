
#include <ContestManager.h>

TaskHandle_t _contestHandle = NULL;

ContestManager::ContestManager(HardwareManager &hwManager)
{
    this->hardwareManager = &hwManager;
    this->_isRunning = false;
    for (int i = 0; i < 4; i++)
    {
        this->_contestIndex[i] = 0;
    }
}
ContestManager::~ContestManager()
{
}
void ContestManager::begin()
{
    this->_isRunning = false;
    this->_startTime = 0;
    for (int i = 0; i < 4; i++)
    {
        this->_contestIndex[i] = 0;
    }
}
void ContestManager::loadConfig(String str)
{
    JsonDocument doc;
    deserializeJson(doc, str.c_str());
    hardwareManager->serialLog.println("Load config:");

    for (int i = 0; i < 11; i++)
    {
        String key = "distance" + String(i + 1);
        if (!doc[key].isNull())
        {
            this->_distance[i] = doc[key];
        }
        else
        {
            this->_distance[i] = 0;
        }
        hardwareManager->serialLog.print("  D" + String(i) + ": " + String(this->_distance[i]));
    }
    hardwareManager->serialLog.println(" ");
}
bool ContestManager::addContest(uint8_t contestID, uint8_t index)
{
    bool added = false;

    if (index < 4 && (contestID == CONTEST_1_ID || contestID == CONTEST_2_ID || contestID == CONTEST_3_ID || contestID == CONTEST_4_ID))
    {
        _contestIndex[index] = contestID;
        added = true;
    }
    return added;
}
void ContestManager::_Runner(void *pvParameters)
{
    ContestManager *self = static_cast<ContestManager *>(pvParameters);

    // xử lý logic
    while (1)
    {
        self->_runContest(); // ví dụ gọi hàm thành viên
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
bool ContestManager::run()
{
    if (!_isRunning)
    {
        _contestHandle = NULL;
        _isRunning = xTaskCreatePinnedToCore(_Runner, "ContestRuner", 4096, this, 1, &_contestHandle, 1);
        _logPath = String(LOG_PATH) + "/" + String(hardwareManager->dateTime.getTimestamp()) + ".txt";
        addLog(START_CONTEST_COMMAND, "Start contest");
        loadConfig(hardwareManager->microSD.readData(CONFIG_CONTEST1_PATH));
    }
    hardwareManager->serialLog.println("Contest run: " + String(_isRunning ? "true" : "false"));
    return _isRunning;
}
void ContestManager::stop()
{
    if (this->_isRunning)
    {
        this->_isRunning = false;
        if (_contestHandle != NULL)
        {

            hardwareManager->serialLog.println("Contest task deleted");
            vTaskDelete(_contestHandle);
        }
    }
}
void ContestManager::_runContest()
{
    // reset biến đếm và time
    hallCount = 0;
    startTime = millis();
    errOverrideStart = 0;
    errRunoutTimeStart = 0;
    errRunoutTimeContest = 0;
    errGoWrongWay = 0;
    errStopEngine = 0;
    errNoSignalLeft = 0;

    for (int i = 0; i < 4; i++)
    {
        switch (this->_contestIndex[i])
        {
        case CONTEST_1_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[0]));
            this->_contest1Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[0]));
            break;
        case CONTEST_2_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[1]));
            this->_contest2Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[1]));
            break;
        case CONTEST_3_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[2]));
            this->_contest3Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[2]));
            break;
        case CONTEST_4_ID:
            hardwareManager->serialLog.println("Bắt đầu cuộc thi: " + String(this->_contestName[3]));
            this->_contest4Runer();
            hardwareManager->serialLog.println("Kết thúc cuộc thi: " + String(this->_contestName[3]));
            break;
        default:
            break;
        }
    }
    addLog(STOP_CONTEST_COMMAND, "Stop contest");
    _logPath = "";
    this->stop();
}
// contest runer

bool ContestManager::_contest1Runer()
{
    addLog(CONTEST_1_ID, "start contest 1");

    bool lastSensorHall = false;
    hallCount = 0;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    startTime = millis();

    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    // chờ tín hiệu đi qua vạch xuất phát
    while (!motor.getSensorHall())
    {
        if (millis() - startTime > 60000 && errRunoutTimeStart == 0)
        {
            errRunoutTimeStart = 1;
            hardwareManager->serialLog.println("--> Contest1: ERROR_RUNOUT_START_TIME");
        }
        // loại bỏ lỗi đè vạch start
        if (errOverrideStart == 1)
        {
            errOverrideStart = 0;
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    // bắt đầu tính giờ thi
    startTime = millis();

    // lỗi không xi nhan trái
    if (!motor.getSignelLeft() && errNoSignalLeft == 0)
    {
        errNoSignalLeft = 1;
        hardwareManager->serialLog.println("--> Contest1: ERROR_NO_SIGNAL_LEFT_IN");
    }
    // lỗi đè vạch xuất phát
    if (errOverrideStart == 1)
    {
        hardwareManager->serialLog.println("--> Contest1: ERROR_OVERRIDE_START");
    }

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    // bắt đầu bài thi
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;

        if (motor.getSensorHall() && !lastSensorHall && deltaDistance > 80)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 40))
            {
                // pass
            }
            else
            {
                errGoWrongWay++;
                hardwareManager->serialLog.println("--> Contest1: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest1: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = motor.getSensorHall();

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            errRunoutTimeContest++;
            hardwareManager->serialLog.println("--> Contest1: ERROR_RUNOUT_CONTEST_TIME");
        }

        if (hallCount == 4)
        {
            return true;
        }
        if (hallCount > 4)
        {
            return false;
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
bool ContestManager::_contest2Runer()
{
    addLog(CONTEST_2_ID, "start contest 2");

    bool lastSensorHall = false;
    hallCount = 4;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    // uint32_t startTime = millis();

    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;

        if (motor.getSensorHall() && !lastSensorHall && deltaDistance > 80)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 40))
            {
                // pass
            }
            else
            {
                errGoWrongWay++;
                hardwareManager->serialLog.println("--> Contest2: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest2: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = motor.getSensorHall();

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            errRunoutTimeContest++;
            hardwareManager->serialLog.println("--> Contest2: ERROR_RUNOUT_CONTEST_TIME");
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 5)
            return true;
        if (hallCount > 5)
            return false;
    }
}
bool ContestManager::_contest3Runer()
{
    addLog(CONTEST_3_ID, "start contest 3");
    bool lastSensorHall = false;
    hallCount = 5;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    // uint32_t startTime = millis();

    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;

        if (motor.getSensorHall() && !lastSensorHall && deltaDistance > 60)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 40))
            {
                // pass
            }
            else
            {
                errGoWrongWay++;
                hardwareManager->serialLog.println("--> Contest3: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest3: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = motor.getSensorHall();

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            errRunoutTimeContest++;
            hardwareManager->serialLog.println("--> Contest3: ERROR_RUNOUT_CONTEST_TIME");
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 7)
            return true;
        if (hallCount > 7)
            return false;
    }
}
bool ContestManager::_contest4Runer()
{
    bool lastSensorHall = false;
    hallCount = 7;
    uint32_t lastDistance = 0;
    uint32_t deltaDistance = 0;
    // uint32_t startTime = millis();

    // uint8_t errOverrideStart = 0;
    // uint8_t errRunoutTimeStart = 0;
    // uint8_t errRunoutTimeContest = 0;
    // uint8_t errGoWrongWay = 0;
    // uint8_t errStopEngine = 0;
    // uint8_t errNoSignalLeft = 0;

    lastSensorHall = motor.getSensorHall();
    lastDistance = motor.getDistance();
    while (1)
    {
        deltaDistance = motor.getDistance() - lastDistance;

        if (motor.getSensorHall() && !lastSensorHall && deltaDistance > 60)
        {

            lastDistance = motor.getDistance();

            if (inDeltaValue(deltaDistance, _distance[hallCount], 40))
            {
                // pass
            }
            else
            {
                errGoWrongWay++;
                hardwareManager->serialLog.println("--> Contest4: ERROR_GO_WRONG_WAY");
            }
            hardwareManager->serialLog.println("--> Contest4: distance" + String(hallCount) + "= " + String(_distance[hallCount]) + "   DeltaDistance =" + String(deltaDistance));
            hallCount++;
        }
        lastSensorHall = motor.getSensorHall();

        // lỗi quá thời gian bài thi
        if (millis() - startTime > 600000 && errRunoutTimeContest == 0)
        {
            errRunoutTimeContest++;
            hardwareManager->serialLog.println("--> Contest4: ERROR_RUNOUT_CONTEST_TIME");
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);

        if (hallCount == 9)
            return true;
        if (hallCount > 9)
            return false;
    }
}
void ContestManager::addLog(uint8_t eventID, String description)
{
    if (_isRunning && _logPath.length() > 0)
    {
        _logEntries.counter++;
        _logEntries.eventID = eventID;
        _logEntries.description = description;
        _logEntries.timestamp = hardwareManager->dateTime.getTimestamp();

        String logEntry = String(_logEntries.counter) + "," + String(_logEntries.eventID, HEX) + "," + _logEntries.description + "," + String(_logEntries.timestamp) + "\n";
        hardwareManager->serialLog.println(logEntry);
        // hardwareManager->microSD.addData(_logPath.c_str(), logEntry.c_str());
    }
}
bool ContestManager::inDeltaValue(uint32_t value, uint32_t target, uint32_t delta)
{
    return (value >= (target - delta)) && (value <= (target + delta));
}
// void ContestManager::