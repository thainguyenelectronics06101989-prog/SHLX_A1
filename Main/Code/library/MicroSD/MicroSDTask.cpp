#include "MicroSDTask.h"
MicroSDTask::MicroSDTask()
{
}
bool MicroSDTask::begin()
{
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5))
    {
        _isMounted = false;
    }
    // cardtye  = CARD_NONE, CARD_MMC, CARD_SD, CARD_SDHC
    _cardType = SD_MMC.cardType();
    _cardSize = SD_MMC.cardSize() / (1024 * 1024);
    _isMounted = true;
    _error = "";
    return _isMounted;
}
String MicroSDTask::readData(const char *path)
{
    _error = "";
    String data;
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return "";
    }
    File file = SD_MMC.open(path, FILE_READ);
    if (!file)
    {
        _error = "Failed to open file for reading";
        return "";
    }
    while (file.available())
    {
        data += (char)file.read();
    }
    file.flush();
    file.close();
    return data;
}
bool MicroSDTask::writeData(const char *path, const char *data)
{
    _error = "";
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return false;
    }
    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file)
    {
        _error = "Failed to open file for writing";
        return false;
    }
    bool isSuccess = false;
    if (file.print(data))
    {
        isSuccess = true;
    }
    else
    {
        _error = "Write failed";
        isSuccess = false;
    }
    file.flush();
    file.close();
    return isSuccess;
}
bool MicroSDTask::addData(const char *path, const char *data)
{
    _error = "";
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return false;
    }
    File file = SD_MMC.open(path, FILE_APPEND);
    if (!file)
    {
        _error = "Failed to open file for appending";
        return false;
    }
    bool isSuccess = false;
    if (file.print(data))
    {
        isSuccess = true;
    }
    else
    {
        _error = "Append failed";
        isSuccess = false;
    }
    file.flush();
    file.close();
    return isSuccess;
}
File MicroSDTask::openFile(const char *path, const char *mode)
{
    _error = "";
    if (!_isMounted)
    {
        _error = "Card Mount Failed";
        return File();
    }
    File file = SD_MMC.open(path, mode);
    if (!file)
    {
        _error = "Failed to open file";
    }
    return file;
}
void MicroSDTask::closeFile(File &file)
{
    if (file)
    {
        file.flush();
        file.close();
    }
}