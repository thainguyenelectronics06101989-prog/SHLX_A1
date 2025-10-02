#ifndef CONTEST_MANAGER_H
#define CONTEST_MANAGER_H

#include <Arduino.h>
#include <vector>

#include "Contest.h"

class ContestManager
{
  public:
    using ContestAction = void (Contest::*)();

    ContestManager();
    ~ContestManager();

    void addContest(ContestAction action);
    void deleteContest();
    void startContests();
    void stopContests();

    size_t contestCount() const;
    bool isRunning() const;

  private:
    void executeContestAction(ContestAction action);

    Contest contestInstance_;
    std::vector<ContestAction> contestStack_;
    bool running_;
};

#endif // CONTEST_MANAGER_H