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
void ContestManager::loadConfig(JsonDocument doc)
{
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
    }
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
        addLog(START_CONTEST, "Start contest");
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
    addLog(STOP_CONTEST, "Stop contest");
    _logPath = "";
    this->stop();
}
// contest runer

void ContestManager::_contest1Runer()
{
    addLog(CONTEST_1_ID, "start contest 1");

    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return;
    }
}
void ContestManager::_contest2Runer()
{
    addLog(CONTEST_2_ID, "start contest 2");

    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return;
    }
}
void ContestManager::_contest3Runer()
{
    addLog(CONTEST_3_ID, "start contest 3");
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return;
    }
}
void ContestManager::_contest4Runer()
{
    addLog(CONTEST_4_ID, "start contest 4");

    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return;
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
        hardwareManager->microSD.addData(_logPath.c_str(), logEntry.c_str());
    }
}