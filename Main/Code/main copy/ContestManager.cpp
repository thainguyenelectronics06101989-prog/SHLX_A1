#include "ContestManager.h"

ContestManager::ContestManager()
    : contestInstance_(), contestStack_(), running_(false)
{
}

ContestManager::~ContestManager() = default;

void ContestManager::addContest(ContestAction action)
{
    if (action == nullptr)
    {
        Serial.println(F("[ContestManager] Invalid contest action"));
        return;
    }

    contestStack_.push_back(action);
}

void ContestManager::deleteContest()
{
    if (contestStack_.empty())
    {
        Serial.println(F("[ContestManager] No contest action to remove"));
        return;
    }

    contestStack_.pop_back();
}

void ContestManager::startContests()
{
    if (contestStack_.empty())
    {
        Serial.println(F("[ContestManager] No contest actions queued"));
        return;
    }

    if (running_)
    {
        Serial.println(F("[ContestManager] Already running"));
        return;
    }

    running_ = true;
    Serial.println(F("[ContestManager] Starting contest actions"));

    for (ContestAction action : contestStack_)
    {
        if (!running_)
        {
            break;
        }
        executeContestAction(action);
    }

    running_ = false;
    Serial.println(F("[ContestManager] All contest actions completed"));
}

void ContestManager::stopContests()
{
    if (!running_)
    {
        Serial.println(F("[ContestManager] Already stopped"));
        return;
    }

    running_ = false;
    Serial.println(F("[ContestManager] Stop requested"));
}

size_t ContestManager::contestCount() const
{
    return contestStack_.size();
}

bool ContestManager::isRunning() const
{
    return running_;
}

void ContestManager::executeContestAction(ContestAction action)
{
    if (action == nullptr)
    {
        Serial.println(F("[ContestManager] Skipping null action"));
        return;
    }

    (contestInstance_.*action)();
}