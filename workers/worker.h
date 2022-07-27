#pragma once

#include "../tasks/task.h"
#include "../scheduler/scheduler.h"
#include "../tasks/keytasks.h"

class Worker
{

public:
    Worker(int _id);

    double Execute();
    void UpdateTime(double _timeSpent);
    void FetchNewTaskIfAvailable(Scheduler* _scheduler);

    bool IsProcessingTask();
    double GetRemainingTime();
	void AddLoad(double _load) { m_load += _load; };
	double GetLoad() { return m_load; };
    double GetcurrentcoreTime() { return m_currentCoreTime; };
    bool outside;
    bool FirstPhase(){return (m_currentTask!=nullptr)?(m_currentTask->GetName()==P_TASK):false;};
    bool SecondPhase(){return (m_currentTask!=nullptr)?(m_currentTask->GetName()==S_TASK):false;};

private:
    Task* m_currentTask;
    int m_currentTaskStep;

    int m_id;
    double m_currentCoreTime;
	double m_load;
    
};

