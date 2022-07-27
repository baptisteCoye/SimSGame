#pragma once

#include <vector>

#include "scheduler/strategies/strategy.h"
#include "../tasks/taskSequence.h"
#include "../tasks/task.h"

class Scheduler
{
public:
    Scheduler(const int _iNbWorkers,std::string Strategy);
    ~Scheduler();

    void SetSequence(TaskSequence& _sequence, double _lagProportion);
    bool HasWaitingTask();
    bool HasAvailableTask();

    void Update(double _BW);
    Task* GetNextAvailableTask(int _workerId, int& _stepToProcess);
    double GetBestTime(int _frameIndex);

    //ThreePhase:
    void AddPhase(int _pahseNumber){m_phase=_pahseNumber;};
    int GetNumber(){return m_phase;};

private:
	Strategy* m_strategy;
    std::vector<Task*> m_waitingTasks;
    int m_phase;
};