#include <stdafx.h>
#include "task.h"
#include <random>


Task::Task(std::string _name, int _steps, double _mean, double _min, double _max, double _stdDev, int _stepsLag, double _meanLag, double _minLag, double _maxLag, double _stdDevLag, std::string _type)
{
	m_name = _name;
	m_steps = _steps;
	m_mean = _mean;
	m_min = _min;
	m_max = _max;
	m_stdDev = _stdDev;
	m_stepsLag = _stepsLag;
	m_meanLag = _meanLag;
	m_minLag = _minLag;
	m_maxLag = _maxLag;
	m_stdDevLag = _stdDevLag;
	m_type = _type;
	
	
}

void Task::Initialize(double _lagProportion)
{
	m_iCurrentStep = 0;
	
	m_totalSteps = this->GetStepCount();
	
	if (m_choice == "Random")
	{
	
		m_remainingTimes = this->GetTime();
	}
	if (m_choice == "Median")
	{
		AssignMediane(_lagProportion);
	}
	
}

void Task::AssignRandomTimes(double _lagProportion)
{
	m_meanTime = 0.0;
	m_remainingTimes.clear();
	double k = 0;
	m_graphicTime = 0.0;
	for (int i = 0; i < m_totalSteps; i++)
	{
		
		m_remainingTimes.push_back(k);
		
		m_meanTime += k;
	}
	
}

void Task::PushInTime(std::vector<double> _Times)
{ 
	m_Times = _Times; 
};


void Task::AssignMediane(double _lagProportion)
{
	m_meanTime = 0.0;
	m_remainingTimes.clear();
	double k = 0;
	m_graphicTime = 0.0;
	for (int i = 0; i < m_totalSteps; i++)
	{
		k =(1 - _lagProportion) * m_mean + _lagProportion * m_meanLag;;
		m_remainingTimes.push_back(k);
		
		m_meanTime += k;
	}
}

std::minstd_rand0 Task::GetGenerator(double seed)
{ 
	
	m_generator.seed(seed);
	return m_generator; 
};
std::vector<double> Task::GenerateTime(double _lagProportion)
{
	
	double meanWithLag = (1 - _lagProportion) * m_mean + _lagProportion * m_meanLag;

	double stdDevWithLag = (1 - _lagProportion) * m_stdDev + _lagProportion * m_stdDevLag;
	double maxWithLag = (1 - _lagProportion) * m_max + _lagProportion * m_maxLag;
	double minWithLag = (1 - _lagProportion) * m_min + _lagProportion * m_minLag;

	double meanLog = log((meanWithLag * meanWithLag) / sqrt(meanWithLag * meanWithLag + stdDevWithLag * stdDevWithLag));
	double stdDevLog = sqrt(log(1 + (stdDevWithLag * stdDevWithLag) / (meanWithLag * meanWithLag)));
	std::vector<double> ret(0);
	ret.push_back(meanLog);
	ret.push_back(stdDevLog);
	ret.push_back(maxWithLag);
	ret.push_back(minWithLag);
	return ret;
}

void Task::UpdateStepCount(int _nb)
{ 
	m_totalSteps = _nb; 
}

double Task::GenerateTimeSeed(double _lagProportion, std::minstd_rand0 generator)
{
	//m_generator.seed(this->GetTopoOrder);
	double meanWithLag = (1 - _lagProportion) * m_mean + _lagProportion * m_meanLag;

	double stdDevWithLag = (1 - _lagProportion) * m_stdDev + _lagProportion * m_stdDevLag;
	double maxWithLag = (1 - _lagProportion) * m_max + _lagProportion * m_maxLag;
	double minWithLag = (1 - _lagProportion) * m_min + _lagProportion * m_minLag;

	double meanLog = log((meanWithLag * meanWithLag) / sqrt(meanWithLag * meanWithLag + stdDevWithLag * stdDevWithLag));
	double stdDevLog = sqrt(log(1 + (stdDevWithLag * stdDevWithLag) / (meanWithLag * meanWithLag)));


	std::lognormal_distribution<double> distribution(meanLog, stdDevLog);


	double execution_time = std::min(distribution(generator), maxWithLag);
	execution_time = std::max(execution_time, minWithLag);
	m_meanLog = meanLog;
	return execution_time;
}


void Task::AddFather(Task* _father)
{
	m_fathers.push_back(_father);
}

void Task::AddSon(Task* _son)
{
	m_sons.push_back(_son);
}

void Task::Execute(int _step)
{
	SpendTime(_step, GetRemainingTimeForStep(_step));
}

void Task::SpendTime(int _step, double _time)
{
	if (_step < m_remainingTimes.size())
	{
		m_remainingTimes[_step] = std::max(0.0, m_remainingTimes[_step] - _time);
	}
}

double Task::GetRemainingTimeForStep(int _step)
{
	if (_step < m_remainingTimes.size())
	{
		return m_remainingTimes[_step];
	}

	return 0.0;
}

double Task::GetRemainingTotalTime()
{
	double remainingTime = 0.0;
	for (int i = 0; i < m_remainingTimes.size(); i++)
	{
		remainingTime += m_remainingTimes[i];
	}

	return remainingTime;
}

bool Task::IsReady()
{
	for (Task* father : m_fathers)
	{
		if (!father->IsDone())
		{
			return false;
		}
	}

	return true;
}

bool Task::IsDone()
{
	return std::abs(GetRemainingTotalTime()) < std::numeric_limits<double>::epsilon();
}

bool Task::HasFreeStepToProcess()
{
	return m_iCurrentStep != m_totalSteps;
}

int Task::ClaimFreeStep()
{
	return m_iCurrentStep++;
}

int Task::GetStepCount()
{
	return m_totalSteps;
}

std::string Task::GetName()
{
	return m_name;
}

std::vector<Task*> Task::GetFathers() 
{
    return m_fathers;
}

std::vector<Task*> Task::GetSons()
{
	return m_sons;
}

std::string Task::GetTaskType()
{
	return m_type;
}

double Task::Get_meanLog()
{
	return m_meanLog;
}
double Task::ProceesTime()
{
	return m_totalSteps*m_meanTime;
}

double Task::meanTime()
{
	return m_mean;
}

void Task::AddTopoOrder(int _label)
{
	m_TopoOrder = _label;
}

int Task::GetTopoOrder()
{
	return m_TopoOrder;
}

void Task::AddLabel(int _label)
{
	m_label = _label;
}

int Task::GetLabel()
{
	return m_label;
}

bool const Task::operator==(Task& rhs)
{ 
	return this->GetName() == rhs.GetName(); 
}

void Task::WichToAssign(std::string _choice)
{
	m_choice = _choice;
}

double Task::GetRemainingPreviousTime()
{
	double ExecutedTimePortion=0.0;
	for(int i=0;i<m_iCurrentStep;i++)
	{
		ExecutedTimePortion+=m_Times[i];
	}
	return std::max(0.0,this->GetPT()-ExecutedTimePortion);
};

