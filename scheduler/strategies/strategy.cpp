#include <stdafx.h>

#include "strategy.h"
#define debug(i) std::cout <<"line= "<<__LINE__<<" , "<<#i<<" = "<< i << std::endl;

void Strategy::Initialize(Task* _lastTask, Task* _firstTask)
{
	m_availableEngineTasks.clear();
	m_availableGraphicTasks.clear();
	m_criticalPath.clear();
	m_previousFrameinducedTime.clear();
	m_tie_break=false;
	m_FirstVariantPrevious.clear();
	m_SecondVariantPrevious.clear();
	m_firstVariantCriticalPath.clear();
	m_secondVariantCriticalPath.clear();
	m_phase=0;

	if (!m_previousFrameExecTime.empty())
	{
		AddTaskInducedTime(_lastTask, 0, std::vector<Task*>());
		
	}
	m_bestTime.push_back(m_previousFrameinducedTime[_firstTask]);

	if(!m_PreviousMaxStep.empty())
	{
		FirstVariantInducedTime(_lastTask, 0, std::vector<Task*>());
	}

	if(!m_PreviousExecTime.empty())
	{
		SecondVariantInducedTime(_lastTask, 0, std::vector<Task*>());
	}
}

void Strategy::PushAvailableTask(Task* _task)
{
	if (_task->GetTaskType() == "engine")
	{
		m_availableEngineTasks.push_back(_task);
	}
	else
	{
		m_availableGraphicTasks.push_back(_task);
	}
	AddTaskTime(_task);
}

void Strategy::AddTaskTime(Task* _task)
{
	if (_task != nullptr)
	{
		double maxStepTime = 0;
		double cumulTime = 0;
		for (int i = 0; i < _task->GetStepCount(); i++)
		{
			maxStepTime = std::max(maxStepTime, _task->GetRemainingTimeForStep(i));
			cumulTime += _task->GetRemainingTimeForStep(i);
		}
		m_PreviousMaxStep[_task]=maxStepTime;
		m_PreviousExecTime[_task]=cumulTime;
		m_previousFrameExecTime[_task] = std::max(maxStepTime, cumulTime / std::min(_task->GetStepCount(), m_workerCount));
	}
}


void Strategy::AddTaskInducedTime(Task* _task, double _inducedTime, std::vector<Task*> _criticalPath)
{
	std::vector<Task*> fathers = _task->GetFathers();

	if (m_previousFrameinducedTime[_task] <= _inducedTime)
	{
		m_previousFrameinducedTime[_task] = _inducedTime;
		_criticalPath.push_back(_task);

		if (fathers.empty()) {
			m_criticalPath = _criticalPath;
			return;
		}

		for (std::vector<Task*>::iterator it = fathers.begin(); it != fathers.end(); it++)
		{
			AddTaskInducedTime(*it, m_previousFrameinducedTime[_task] + m_previousFrameExecTime[_task], _criticalPath);
		}
	}
}

Task* Strategy::GetNextAvailableTask(int _workerId, int& _stepToProcess)
{
	Task* task = nullptr;
	if (_workerId == 0)
	{
		if (m_availableGraphicTasks.size() > 0)
		{
			bubbleSort(m_availableGraphicTasks);
			task = m_availableGraphicTasks.front();
			if (task != nullptr && task->HasFreeStepToProcess())
			{
				_stepToProcess = task->ClaimFreeStep();
			}

			if (task == nullptr || !task->HasFreeStepToProcess())
			{
				m_availableGraphicTasks.erase(m_availableGraphicTasks.begin());
			}
			return task;
		}
	}
	
	if (m_availableEngineTasks.size() > 0)
	{
		bubbleSort(m_availableEngineTasks);
		
		task = m_availableEngineTasks.front();
		if (task != nullptr && task->HasFreeStepToProcess())
		{
			_stepToProcess = task->ClaimFreeStep();
		}

		if (task == nullptr || !task->HasFreeStepToProcess())
		{
			m_availableEngineTasks.erase(m_availableEngineTasks.begin());
		}
	}
	
	return task;
}

bool Strategy::HasAvailableTask()
{
	return  (!m_availableEngineTasks.empty() && !m_availableGraphicTasks.empty());
}

double Strategy::GetBestTime(int _frameIndex)
{
	return m_bestTime[_frameIndex];
}

bool Strategy::IsOnCriticalPath(Task* _task) {
	return (std::find(m_criticalPath.begin(), m_criticalPath.end(), _task) != m_criticalPath.end());
}

void Strategy::bubbleSort(std::vector<Task*> &vec) {
	if (m_LessOrMore == "more")
	{
		for (size_t i = 0; i < vec.size() - 1; ++i) {
			for (size_t j = 0; j < vec.size() - i - 1; ++j) {
				if (Priority(vec.at(j)) < Priority(vec.at(j + 1)))
				{
					std::swap(vec.at(j), vec.at(j + 1));
				}
				if (Priority(vec.at(j))==Priority(vec.at(j + 1))&& m_tie_break)
				{
					if(vec.at(j)->GetMean()<vec.at(j)->GetMean())
						std::swap(vec.at(j), vec.at(j + 1));
				}
					
			}
		}
	}
	if (m_LessOrMore == "less")
	{
		for (size_t i = 0; i < vec.size() - 1; ++i) {
			for (size_t j = 0; j < vec.size() - i - 1; ++j) {
				if (Priority(vec.at(j)) > Priority(vec.at(j + 1)))
					std::swap(vec.at(j), vec.at(j + 1));
				if (Priority(vec.at(j))==Priority(vec.at(j + 1))&& m_tie_break)
				{
					if(vec.at(j)->GetMean()<vec.at(j)->GetMean())
						std::swap(vec.at(j), vec.at(j + 1));
				}
			}
		}
	}
	if(m_LessOrMore == "other")
	{

	}
}


void CriticalPath::PushAvailableTask(Task* _task)
{
	/*Push tasks in EngineWorkQueues*/
	if (_task->GetTaskType() == "engine")
	{
		if (m_availableEngineTasks.empty())
		{
			m_availableEngineTasks.push_back(_task);
		}
		else
		{
			bool found = false;

			std::vector<Task*>::iterator insertPos = m_availableEngineTasks.begin();
			while (!found)
			{
				if (insertPos == m_availableEngineTasks.end())
				{
					m_availableEngineTasks.push_back(_task);
					found = true;
				}
				else
				{
					if (IsOnCriticalPath(_task))
					{
						m_availableEngineTasks.insert(insertPos, _task);
						found = true;
					}

					else if ((m_previousFrameinducedTime[*insertPos] <= m_previousFrameinducedTime[_task]) && !IsOnCriticalPath(*insertPos))
					{
						m_availableEngineTasks.insert(insertPos, _task);
						found = true;
					}

					else
					{
						insertPos++;
					}
				}
			}
		}
	}

	/*Push tasks into Graphic WorkQueues*/
	else
	{
		if (m_availableGraphicTasks.empty())
		{
			m_availableGraphicTasks.push_back(_task);
		}
		else
		{
			bool found = false;

			std::vector<Task*>::iterator insertPos = m_availableGraphicTasks.begin();
			while (!found)
			{
				if (insertPos == m_availableGraphicTasks.end())
				{
					m_availableGraphicTasks.push_back(_task);
					found = true;
				}
				else
				{
					if (IsOnCriticalPath(_task))
					{
						m_availableGraphicTasks.insert(insertPos, _task);
						found = true;
					}

					else if ((m_previousFrameinducedTime[*insertPos] <= m_previousFrameinducedTime[_task]) && !IsOnCriticalPath(*insertPos))
					{
						m_availableGraphicTasks.insert(insertPos, _task);
						found = true;
					}

					else
					{
						insertPos++;
					}
				}
			}
		}
	}
	AddTaskTime(_task);
}

//VARIANT CRITICAL PATH METHODS:

//1- USING THE LONGEST STEP OF EACH TASK:
void Strategy::FirstVariantInducedTime(Task* _task, double _inducedTime, std::vector<Task*> _criticalPath)
{
	std::vector<Task*> fathers = _task->GetFathers();

	if (m_FirstVariantPrevious[_task] <= _inducedTime)
	{
		m_FirstVariantPrevious[_task] = _inducedTime;
		_criticalPath.push_back(_task);

		if (fathers.empty()) {
			m_firstVariantCriticalPath = _criticalPath;
			return;
		}

		for (std::vector<Task*>::iterator it = fathers.begin(); it != fathers.end(); it++)
		{
			AddTaskInducedTime(*it, m_FirstVariantPrevious[_task] + m_PreviousMaxStep[_task], _criticalPath);
		}
	}
};

bool firstVariantCriticalPath::IsOnCriticalPath(Task* _task) {
	return (std::find(m_firstVariantCriticalPath.begin(), m_firstVariantCriticalPath.end(), _task) != m_firstVariantCriticalPath.end());
}

void firstVariantCriticalPath::PushAvailableTask(Task* _task)
{
	/*Push tasks in EngineWorkQueues*/
	if (_task->GetTaskType() == "engine")
	{
		if (m_availableEngineTasks.empty())
		{
			m_availableEngineTasks.push_back(_task);
		}
		else
		{
			bool found = false;

			std::vector<Task*>::iterator insertPos = m_availableEngineTasks.begin();
			while (!found)
			{
				if (insertPos == m_availableEngineTasks.end())
				{
					m_availableEngineTasks.push_back(_task);
					found = true;
				}
				else
				{
					if (IsOnCriticalPath(_task))
					{
						m_availableEngineTasks.insert(insertPos, _task);
						found = true;
					}

					else if ((m_FirstVariantPrevious[*insertPos] <= m_FirstVariantPrevious[_task]) && !IsOnCriticalPath(*insertPos))
					{
						m_availableEngineTasks.insert(insertPos, _task);
						found = true;
					}

					else
					{
						insertPos++;
					}
				}
			}
		}
	}

	/*Push tasks into Graphic WorkQueues*/
	else
	{
		if (m_availableGraphicTasks.empty())
		{
			m_availableGraphicTasks.push_back(_task);
		}
		else
		{
			bool found = false;

			std::vector<Task*>::iterator insertPos = m_availableGraphicTasks.begin();
			while (!found)
			{
				if (insertPos == m_availableGraphicTasks.end())
				{
					m_availableGraphicTasks.push_back(_task);
					found = true;
				}
				else
				{
					if (IsOnCriticalPath(_task))
					{
						m_availableGraphicTasks.insert(insertPos, _task);
						found = true;
					}

					else if ((m_FirstVariantPrevious[*insertPos] <= m_FirstVariantPrevious[_task]) && !IsOnCriticalPath(*insertPos))
					{
						m_availableGraphicTasks.insert(insertPos, _task);
						found = true;
					}

					else
					{
						insertPos++;
					}
				}
			}
		}
	}
	AddTaskTime(_task);
}

//2- USING THE PROCESSING TIME OF EACH TASK:
void Strategy::SecondVariantInducedTime(Task* _task, double _inducedTime, std::vector<Task*> _criticalPath)
{
	std::vector<Task*> fathers = _task->GetFathers();

	if (m_SecondVariantPrevious[_task] <= _inducedTime)
	{
		m_SecondVariantPrevious[_task] = _inducedTime;
		_criticalPath.push_back(_task);

		if (fathers.empty()) {
			m_secondVariantCriticalPath = _criticalPath;
			return;
		}

		for (std::vector<Task*>::iterator it = fathers.begin(); it != fathers.end(); it++)
		{
			SecondVariantInducedTime(*it, m_SecondVariantPrevious[_task] + m_PreviousExecTime[_task], _criticalPath);
		}
	}
};

bool secondVariantCriticalPath::IsOnCriticalPath(Task* _task) {
	return (std::find(m_secondVariantCriticalPath.begin(), m_secondVariantCriticalPath.end(), _task) != m_secondVariantCriticalPath.end());
}

void secondVariantCriticalPath::PushAvailableTask(Task* _task)
{
	/*Push tasks in EngineWorkQueues*/
	if (_task->GetTaskType() == "engine")
	{
		if (m_availableEngineTasks.empty())
		{
			m_availableEngineTasks.push_back(_task);
		}
		else
		{
			bool found = false;

			std::vector<Task*>::iterator insertPos = m_availableEngineTasks.begin();
			while (!found)
			{
				if (insertPos == m_availableEngineTasks.end())
				{
					m_availableEngineTasks.push_back(_task);
					found = true;
				}
				else
				{
					if (IsOnCriticalPath(_task))
					{
						m_availableEngineTasks.insert(insertPos, _task);
						found = true;
					}

					else if ((m_SecondVariantPrevious[*insertPos] <= m_SecondVariantPrevious[_task]) && !IsOnCriticalPath(*insertPos))
					{
						m_availableEngineTasks.insert(insertPos, _task);
						found = true;
					}

					else
					{
						insertPos++;
					}
				}
			}
		}
	}

	/*Push tasks into Graphic WorkQueues*/
	else
	{
		if (m_availableGraphicTasks.empty())
		{
			m_availableGraphicTasks.push_back(_task);
		}
		else
		{
			bool found = false;

			std::vector<Task*>::iterator insertPos = m_availableGraphicTasks.begin();
			while (!found)
			{
				if (insertPos == m_availableGraphicTasks.end())
				{
					m_availableGraphicTasks.push_back(_task);
					found = true;
				}
				else
				{
					if (IsOnCriticalPath(_task))
					{
						m_availableGraphicTasks.insert(insertPos, _task);
						found = true;
					}

					else if ((m_SecondVariantPrevious[*insertPos] <= m_SecondVariantPrevious[_task]) && !IsOnCriticalPath(*insertPos))
					{
						m_availableGraphicTasks.insert(insertPos, _task);
						found = true;
					}

					else
					{
						insertPos++;
					}
				}
			}
		}
	}
	AddTaskTime(_task);
}

//////////////
/// NEH
NEH::NEH(const int _iNbWorkers)
{
    m_workerCount = _iNbWorkers;
	m_LessOrMore = "more";
    m_Workers.clear();
    for(int i=0;i<12;i++)
    {
        m_Workers[i]=0.0;
    }
    m_WorkersTest=m_Workers;
	debug("const")
}

int NEH::GetAvailableWorker()
{
    double min=std::numeric_limits<double>().max();
    int availableWorker=0;
    for(int i=0;i<12;i++)
    {
        if(m_Workers[i]<min)
        {
            min=m_Workers[i];
            availableWorker=i;
        }
    }
    return availableWorker;
}

double NEH::GetMakespan()
{
    double max=0.0;
    for(int i=0;i<12;i++)
    {
        if(max<m_Workers[i])
        {
            max=m_Workers[i];
        }
    }
    return max;
};
void NEH::FillWorkers(Task* _task)
{
    std::vector<double> Times=_task->GetTime();
    std::sort(Times.begin(), Times.end(), std::greater<double>());
    int pos=0;
    for(auto elem:Times)
    {
        pos=GetAvailableWorker();
        m_WorkersTest[pos]=elem;
    }
};

double NEH::ScheduleTasks(std::vector<Task*> _tasks)
{
    m_WorkersTest=m_Workers;
    for(int i=0;i<_tasks.size();i++)
    {
        FillWorkers(_tasks[i]);
    }
    double makespan=0.0;
    for(int i=0;i<12;i++)
    {
        if(makespan<m_WorkersTest[i])
        {
            makespan=m_WorkersTest[i];
        }
    }
    return makespan;
    
};

void NEH::Transform()
{
	int cpt=0;
	int size=m_availableEngineTasks.size();
	Task* task=nullptr;
	std::vector<Task*> Classed;
	while(cpt<size)
	{
		task=m_availableEngineTasks[cpt];
		if(Classed.empty())
		{
			Classed.push_back(task);
		}
		else
		{
			double min=std::numeric_limits<double>().max();
			int rightPos=0;
			double makespan=0.0;
			for(int i=0;i<Classed.size();i++)
			{
				Classed.insert(Classed.begin()+i,task);
				makespan=ScheduleTasks(Classed);
				if(makespan<min)
				{
					min=makespan;
					rightPos=i;
				}
				Classed.erase(Classed.begin()+i);
			}
			Classed.insert(Classed.begin()+rightPos,task);
		}
		cpt++;
	}
	m_availableEngineTasks=Classed;
};

Task* NEH::GetNextAvailableTask(int _workerId, int& _stepToProcess)
{
	Task* task = nullptr;
	if (_workerId == 0)
	{
		if (m_availableGraphicTasks.size() > 0)
		{
			bubbleSort(m_availableGraphicTasks);
			task = m_availableGraphicTasks.front();
			if (task != nullptr && task->HasFreeStepToProcess())
			{
				_stepToProcess = task->ClaimFreeStep();
			}

			if (task == nullptr || !task->HasFreeStepToProcess())
			{
				m_availableGraphicTasks.erase(m_availableGraphicTasks.begin());
			}
			return task;
		}
	}

	if (m_availableEngineTasks.size() > 0)
	{
		bubbleSort(m_availableEngineTasks);
		Transform();
		task = m_availableEngineTasks.front();
		if (task != nullptr && task->HasFreeStepToProcess())
		{
			_stepToProcess = task->ClaimFreeStep();
		}

		if (task == nullptr || !task->HasFreeStepToProcess())
		{
			m_availableEngineTasks.erase(m_availableEngineTasks.begin());
		}
	}

	return task;
}

double ThreePhases::Priority(Task* task)
{
	if(m_phase==0)
	{
		return task->GetLevel();
	}
	if(m_phase==1)
	{
		return task->GetHu();
	}
	else
	{
		return task->GetRemainingTimeForStep(task->GetCurrentStep());
	}
};

double TwoPhases::Priority(Task* task)
{
	if(m_phase==0)
	{
		m_LessOrMore = "more";
		return task->GetLevel();
	}
	else
	{
		m_LessOrMore = "less";
		return task->GetResponseRatio();
	}
};

double TwoPhasesStatic::Priority(Task* task)
{
	if(m_phase==0)
	{
		return task->GetLevel();
	}
	else
	{
		return task->GetLabel();
	}
};

//Infinity is a FIFO algorith scheduling executed with 1000 threads:
Task* Infinity::GetNextAvailableTask(int _workerId, int& _stepToProcess)
{
	Task* task = nullptr;
	if (_workerId == 0)
	{
		if (m_availableGraphicTasks.size() > 0)
		{
			bubbleSort(m_availableGraphicTasks);
			task = m_availableGraphicTasks.front();
			if (task != nullptr && task->HasFreeStepToProcess())
			{
				_stepToProcess = task->ClaimFreeStep();
			}

			if (task == nullptr || !task->HasFreeStepToProcess())
			{
				m_availableGraphicTasks.erase(m_availableGraphicTasks.begin());
			}
			return task;
		}
	}
	else{
	if (m_availableEngineTasks.size() > 0)
	{
		bubbleSort(m_availableEngineTasks);
		
		task = m_availableEngineTasks.front();
		if (task != nullptr && task->HasFreeStepToProcess())
		{
			_stepToProcess = task->ClaimFreeStep();
		}

		if (task == nullptr || !task->HasFreeStepToProcess())
		{
			m_availableEngineTasks.erase(m_availableEngineTasks.begin());
		}
	}
	}

	return task;
}
