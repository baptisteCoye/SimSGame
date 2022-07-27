#include <stdafx.h>

#include "scheduler.h"
#include "strategies/strategy.h"
#include "tasks/keytasks.h"


Scheduler::Scheduler(const int _iNbWorkers, std::string Strategy)
{
	if (Strategy == "FIFO") //FIRST IN FIRST OUT
	{
		m_strategy = new FIFO(_iNbWorkers);
	}
	if (Strategy == "Priority") //DYNAMIC CRITICAL PATH PRIORITY
	{
		m_strategy = new CriticalPath(_iNbWorkers);
	}
	if (Strategy == "LPT") //LONGEST PROCESSING TIME FIRST
	{
		m_strategy = new LPT(_iNbWorkers);
	}
	if (Strategy == "HLF")  //Hu's level algorithm
	{
		m_strategy = new HLF(_iNbWorkers);
	}
	if (Strategy == "Hu") //hu algorithm with mean execution time 
	{
		m_strategy = new Hu(_iNbWorkers);
	}
	if (Strategy == "WL") //Weighted length 
	{
		m_strategy = new WL(_iNbWorkers);
	}
	if (Strategy == "LFF") //LATEST FINISHED FIRST
	{
		m_strategy = new LFF(_iNbWorkers);
	}
	if (Strategy == "HRRN")  //HIGEST RESPONSE RATION NEXT 
	{
		m_strategy = new HRRN(_iNbWorkers);
	}
	if (Strategy == "SPT") //SHORTEST PROCESSING TIME FIRST
	{
		m_strategy = new SPT(_iNbWorkers);
	}
	if (Strategy == "MostSuccessors")   //MOST SUCCESSORS FIRST
	{
		m_strategy = new MostScussors(_iNbWorkers);
	}
	if (Strategy == "Coffman") //COFFMAN GRAHAM ALGORITHM
	{
		m_strategy = new Coffman(_iNbWorkers);
	}
	if (Strategy == "LRT") //LONGEST REMAINING TIME FIRST
	{
		m_strategy = new LRT(_iNbWorkers);
	}
	if (Strategy == "SRT")  //SHORTSET REMAINING TIME FIRST
	{
		m_strategy = new SRT(_iNbWorkers);
	}
	if (Strategy == "SLRT")  //STEP WITH THE LONGEST PROCESSING TIME FIRST 
	{
		m_strategy = new SLRT(_iNbWorkers);
	}
	if (Strategy == "SSRT")  //STEP WITH THE SHORTEST PROCESSING TIME FIRST 
	{
		m_strategy = new SSRT(_iNbWorkers);
	}
	if (Strategy == "Aco")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new Aco(_iNbWorkers);
	}
	if (Strategy == "firstVariantCriticalPath")  //STEP WITH THE SHORTEST PROCESSING TIME FIRST 
	{
		m_strategy = new firstVariantCriticalPath(_iNbWorkers);
	}
	if (Strategy == "secondVariantCriticalPath")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new secondVariantCriticalPath(_iNbWorkers);
	}
	if (Strategy == "NEH")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new NEH(_iNbWorkers);
	}
	if (Strategy == "ThreePhases")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new ThreePhases(_iNbWorkers);
	}
	if (Strategy == "TwoPhases")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new TwoPhases(_iNbWorkers);
	}
	if (Strategy == "WT")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new WT(_iNbWorkers);
	}
	if (Strategy == "Infinity")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new Infinity(_iNbWorkers);
	}
	if (Strategy == "TwoPhasesStatic")  //ANT COLONY OPTIMISATION ALGORITHM
	{
		m_strategy = new TwoPhasesStatic(_iNbWorkers);
	}
}

Scheduler::~Scheduler()
{
    delete m_strategy;
    m_strategy = nullptr;
}

void Scheduler::SetSequence(TaskSequence& _sequence, double _lagProportion)
{
    m_waitingTasks.clear();
    std::map<std::string, Task*> taskMap = _sequence.GetTaskMap();
    std::map<std::string, Task*>::iterator it;
    m_strategy->Initialize(taskMap[END_TASK], taskMap[INIT_TASK]);

    for (it = taskMap.begin(); it != taskMap.end(); it++)
    {
        Task* task = it->second;
        task->Initialize(_lagProportion);
        m_waitingTasks.push_back(task);
    }
}

bool Scheduler::HasWaitingTask()
{
    return !m_waitingTasks.empty();
}

bool Scheduler::HasAvailableTask()
{
    return m_strategy->HasAvailableTask();
}

void Scheduler::Update(double _BW)
{
	
    for (int i = m_waitingTasks.size(); i > 0; i--)
    {
		m_strategy->AddPhase(m_phase);
        Task* task = m_waitingTasks[i - 1];
        if (task != nullptr && task->IsReady())
        {
			task->AddBW(_BW);
            m_waitingTasks.erase(m_waitingTasks.begin() + i - 1);
            m_strategy->PushAvailableTask(task);
        }
    }
}

Task* Scheduler::GetNextAvailableTask(int _workerId, int& _stepToProcess)
{
    return m_strategy->GetNextAvailableTask(_workerId, _stepToProcess);
}

double Scheduler::GetBestTime(int _frameIndex)
{
    return m_strategy->GetBestTime(_frameIndex);
}
