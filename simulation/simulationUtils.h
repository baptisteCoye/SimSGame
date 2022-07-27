#pragma once

#include <string>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <cstdlib>
#include "simulation.h"
#include "../scheduler/strategies/strategy.h"
#include "../tasks/taskSequence.h"
#include "../tasks/task.h"

#define RNG_SEED_SPACING 100 		//spacing between two different RNG seeds

class SimulationUtils
{
public:
    static void ParseParameters(const std::string& _parameterFileName, const std::string& _sequenceFileName, SimulationParameters& _parameters);
	static void SortUtil(Task* _task,std::stack<Task*> &sorted, std::vector<Task*> &visited);
	static bool contained(std::vector<Task*> &vect, Task* check);
	static bool permetted(Task* _task, std::vector<Task*> _setTask);
	static void AddSons(SimulationParameters& _parameters);
	static void GenerateLagProportion(SimulationParameters& _parameters);
	static void ExecutionTimeControl(SimulationParameters& _parameters);
	static void TopologicalSorting(SimulationParameters& _parameters);
	static void LexicographicalSorting(SimulationParameters& _parameters);
	static void HU(SimulationParameters& _parameters);
	static void HLF(SimulationParameters& _parameters);
	
private:
	
};

