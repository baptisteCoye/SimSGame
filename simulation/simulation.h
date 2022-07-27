#pragma once

#include <string>
#include "ACO.h"
#include "tasks/taskSequence.h"

struct SimulationParameters
{
    int m_iNbFrames;
    int m_iNbWorkers;

    TaskSequence m_sequence;

	std::map<int, double> m_lags;

	std::map<std::pair<int, Task*>, std::vector<double>> m_times;

	std::string mode;

	int m_seed;
	
	int m_startSeed;
	int m_endSeed;
	
	bool IsDivided;

	double Upperbound;

	double Lowerbound;

	bool JsonOutput;

	bool SortingSteps;
	inline SimulationParameters& operator=(const SimulationParameters& a)
	{
		m_iNbFrames = a.m_iNbFrames;
		m_iNbWorkers = a.m_iNbWorkers;
		m_sequence = a.m_sequence;
		return *this;
	}
};

class Simulation
{
public:
    static void Play(SimulationParameters& _parameters, std::string Strategy);
	static void SLACK(SimulationParameters& _parameters, std::pair<int, Task*> key);
	
private:
	
};