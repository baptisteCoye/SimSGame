#include <stdafx.h>
#include "worker.h"
#include "../profiler/profiler.h"

Worker::Worker(int _id)
{
    m_id = _id;
    m_currentTask = nullptr;
    m_currentCoreTime = 0.0;
    m_currentTaskStep = 0;
	m_load = 0.0;
}

double Worker::Execute()
{
    double timeSpent = 0.0;
    if (m_currentTask != nullptr)
    {
        timeSpent = m_currentTask->GetRemainingTimeForStep(m_currentTaskStep);
        if (m_currentTask->GetStepCount() == m_currentTaskStep+1)
		{
			m_currentTask->AddFT(m_currentCoreTime + timeSpent);
		}
        if(outside)
            Profiler::GetInstance()->AddKeyFrame(m_id, m_currentTask->GetName(), "GenericTask", m_currentCoreTime + timeSpent, Profiler::KEY_EVENT_TYPE::END, m_currentTaskStep, m_currentTask->GetStepCount());
        m_currentTask->Execute(m_currentTaskStep);
        
        m_currentTask = nullptr;
    }

    return timeSpent;
}

void Worker::UpdateTime(double _timeSpent)
{
    m_currentCoreTime += _timeSpent;
    if (m_currentTask != nullptr)
    {
        m_currentTask->SpendTime(m_currentTaskStep, _timeSpent);
    }
}

void Worker::FetchNewTaskIfAvailable(Scheduler* _scheduler)
{
    if (m_currentTask == nullptr)
    {
        m_currentTask = _scheduler->GetNextAvailableTask(m_id, m_currentTaskStep);
        if (m_currentTask != nullptr)
		{
			if (m_currentTask->GetCurrentStep() == 1)
			{
				m_currentTask->AddEW(m_currentCoreTime);
			}
		}
        if (m_currentTask != nullptr&&outside)
        {
            Profiler::GetInstance()->AddKeyFrame(m_id, m_currentTask->GetName(), "GenericTask", m_currentCoreTime, Profiler::KEY_EVENT_TYPE::BEGIN, m_currentTaskStep, m_currentTask->GetStepCount());
        }
    }
}

bool Worker::IsProcessingTask()
{
    return m_currentTask != nullptr;
}

double Worker::GetRemainingTime()
{
    return (m_currentTask != nullptr ? m_currentTask->GetRemainingTimeForStep(m_currentTaskStep) : 0.0);
}