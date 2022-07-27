#pragma once

#include <vector>

#include "../tasks/taskSequence.h"
#include "../scheduler/scheduler.h"
#include "../workers/worker.h"

class Device
{
public:
    Device(const int _iNbWorker, std::string Strategy,bool outside);
    ~Device();

    static Device* CreateDevice(const int _iNbWorker,std::string Strategy,bool outside);
    static void DestroyDevice(Device* _device);
    
    void PlaySequence(TaskSequence& _sequence, double _lagProportion);
    void UpdateWorkers();
    int GetNextAvailableWorkerId();
    int GetNextAvailableWorkerId(bool _includeIdleWorkers);
    bool IsProcessingTask();

    double GetGlobalClockTime();
    double GetBestTime(int _frameIndex);

    //FIRST PHASE:
    bool FirstPhase();
    void AddFirstPhase(double _beginFirstPhase){m_beginFirstPhase=_beginFirstPhase;};
    double GetFirstPhase(){return m_beginFirstPhase;};
    //SECOND PHASE:
    bool SecondPhase();
    void AddSecondPhase(double _beginSecondPhase){m_beginSecondPhase=_beginSecondPhase;};
    double GetSecondPhase(){return m_beginSecondPhase;};

private:
    Scheduler* m_scheduler;
    std::vector<Worker*> m_workers;
    int cpt;
    int cpt2;
    double m_beginFirstPhase;
    double m_beginSecondPhase;
    double m_clockTime;
};