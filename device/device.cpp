#include <stdafx.h>
#include <assert.h>

#include "device.h"

Device::Device(const int _iNbWorker, std::string Strategy,bool outside)
{
    m_clockTime = 0.0;
    m_scheduler = new Scheduler(_iNbWorker, Strategy);
    for (int i = 0; i < _iNbWorker; i++)
    {
        m_workers.push_back(new Worker(i));
    }
    for (int i = 0; i < _iNbWorker; i++)
    {
        m_workers[i]->outside=outside;
    }
    cpt=0;
    cpt2=0;
}

Device::~Device()
{
    delete m_scheduler;
    m_scheduler = nullptr;
}

Device* Device::CreateDevice(const int _iNbWorker, std::string Strategy,bool outside)
{
    return new Device(_iNbWorker,Strategy,outside);
}

void Device::PlaySequence(TaskSequence& _sequence, double _lagProportion)
{
    cpt=0;
    cpt2=0;
    if (m_scheduler != nullptr)
    {
        m_scheduler->SetSequence(_sequence, _lagProportion);
        do
        {
            UpdateWorkers();

        } while (IsProcessingTask());

        assert(!m_scheduler->HasWaitingTask());
    }
}

void Device::UpdateWorkers()
{
    if (m_scheduler != nullptr)
    {
        int iWorkerId = GetNextAvailableWorkerId();
        double timeSpent = m_workers[iWorkerId]->Execute();

        m_scheduler->Update(m_workers[iWorkerId]->GetcurrentcoreTime());
        for (int i = 0; i < m_workers.size(); i++)
        {
            m_workers[i]->UpdateTime(timeSpent);
            m_workers[i]->FetchNewTaskIfAvailable(m_scheduler);
        }
        
        m_clockTime += timeSpent;
        if(FirstPhase()&cpt==0)
        {
            AddFirstPhase(m_clockTime);
            cpt=1;
        }
        if(SecondPhase()&cpt==1)
        {
            AddSecondPhase(m_clockTime);
            cpt=2;
        }
        m_scheduler->AddPhase(cpt);
    }
}

int Device::GetNextAvailableWorkerId()
{
    return GetNextAvailableWorkerId(m_scheduler->HasAvailableTask());
}

int Device::GetNextAvailableWorkerId(bool _includeIdleWorkers)
{
    int iWorkerId = 0;
    double fMinRemainingTime = std::numeric_limits<double>().max();
    for (int i = 0; i < m_workers.size(); i++)
    {
        if (_includeIdleWorkers || m_workers[i]->IsProcessingTask())
        {
            double fRemainingTime = m_workers[i]->GetRemainingTime();
            if (fRemainingTime < fMinRemainingTime)
            {
                fMinRemainingTime = fRemainingTime;
                iWorkerId = i;
            }
        }
    }

    return iWorkerId;
}

bool Device::IsProcessingTask()
{
    for (Worker* worker : m_workers)
    {
        if (worker->IsProcessingTask())
        {
            return true;
        }
    }
    return false;
}

void Device::DestroyDevice(Device* _device)
{
    delete _device;
    _device = nullptr;
}

double Device::GetGlobalClockTime()
{
    return m_clockTime;
}

double Device::GetBestTime(int _frameIndex)
{
    return m_scheduler->GetBestTime(_frameIndex);
}

bool Device::FirstPhase()
{
    bool EndPahase=false;
    for(int i=0;i<m_workers.size();i++)
    {
        if(m_workers[i]->FirstPhase())
        {
            return true;
        }
    }
    return false;
};

bool Device::SecondPhase()
{
    bool EndPahase=false;
    for(int i=0;i<m_workers.size();i++)
    {
        if(m_workers[i]->SecondPhase())
        {
            return true;
        }
    }
    return false;
};

