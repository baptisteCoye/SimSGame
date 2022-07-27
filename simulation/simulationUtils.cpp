#include <stdafx.h>
#include <assert.h>
#include <fstream>
#include <map>
#include <string>
#include "simulationUtils.h"
#include "tasks/keytasks.h"

using namespace std;

void SimulationUtils::ParseParameters(const std::string& _parameterFileName, const std::string& _sequenceFileName, SimulationParameters& _parameters)
{
	ifstream parameterStream;
	ifstream sequenceStream;

	parameterStream.open(_parameterFileName);
	sequenceStream.open(_sequenceFileName);

	assert(parameterStream);
	assert(sequenceStream);

	string taskName;
	string taskCount;
	string taskMean;
	string taskMin;
	string taskMax;
	string taskStdDev;
	string taskCountLag;
	string taskMeanLag;
	string taskMinLag;
	string taskMaxLag;
	string taskStdDevLag;
	string taskType;

	while (!parameterStream.eof())
	{
		getline(parameterStream, taskName, ' ');
		getline(parameterStream, taskCount, ' ');
		getline(parameterStream, taskMean, ' ');
		getline(parameterStream, taskMin, ' ');
		getline(parameterStream, taskMax, ' ');
		getline(parameterStream, taskStdDev, ' ');
		getline(parameterStream, taskCountLag, ' ');
		getline(parameterStream, taskMeanLag, ' ');
		getline(parameterStream, taskMinLag, ' ');
		getline(parameterStream, taskMaxLag, ' ');
		getline(parameterStream, taskStdDevLag, ' ');
		getline(parameterStream, taskType, '\n');

		_parameters.m_sequence.AddTask(new Task(taskName, stoi(taskCount), stod(taskMean), stod(taskMin), stod(taskMax), stod(taskStdDev), stoi(taskCountLag), stod(taskMeanLag), stod(taskMinLag), stod(taskMaxLag), stod(taskStdDevLag), taskType));
	}


	string line;
	string delimiter = " ";

	while (!sequenceStream.eof())
	{
		size_t pos = 0;
		std::string token;

		getline(sequenceStream, line);
		if ((pos = line.find(delimiter)) != string::npos)
		{
			//Get the task
			taskName = line.substr(0, pos);
			line.erase(0, pos + delimiter.length());

			Task* task = _parameters.m_sequence.GetTask(taskName);
			if (task != nullptr)
			{
				// Get fathers of the task
				while ((pos = line.find(delimiter)) != string::npos)
				{
					token = line.substr(0, pos);
					task->AddFather(_parameters.m_sequence.GetTask(token));
					line.erase(0, pos + delimiter.length());
				}

				task->AddFather(_parameters.m_sequence.GetTask(line));
			}
		}
	}
}


void SimulationUtils::SortUtil(Task* _task, std::stack<Task*> &sorted, std::vector<Task*> &visited)
{
	for (int i = 0; i < _task->GetSons().size(); i++) {
		if (!contained(visited, _task->GetSons().at(i)))
			SortUtil(_task->GetSons().at(i), sorted, visited);
	}
	visited.push_back(_task);
	sorted.push(_task);
}


bool SimulationUtils::contained(std::vector<Task*> &vect, Task* check) {
	bool doesContain = false;

	for (int i = 0; i < vect.size(); i++) {
		if (vect.at(i) == check) {
			doesContain = true;
		}
	}
	return doesContain;
}

bool SimulationUtils::permetted(Task* _task,std::vector<Task*> _setTask)
{
	bool perm = true;
	if (_task->GetSons().size() > 0)
	{
		for (int i = 0;i < _task->GetSons().size();i++)
		{
			if(!contained(_setTask, _task->GetSons().at(i)))
			{
				perm = false;
			}
		}
	}
	return perm;
}

void SimulationUtils::AddSons(SimulationParameters& _parameters)
{
	std::map<std::string, Task*> Map = _parameters.m_sequence.GetTaskMap();
	std::vector<Task*> v;
	std::vector<Task*> fathers;
	for (auto elem : Map)
	{
		v.push_back(elem.second);
	}
	for (auto task : v)
	{
		for (auto s : v)
		{
			fathers = s->GetFathers();
			if (std::find(fathers.begin(), fathers.end(), task) != fathers.end())
			{
				task->AddSon(s);
			}

		}

	}
}

void SimulationUtils::GenerateLagProportion(SimulationParameters& _parameters)
{
	bool simulationLagStatus = false;
	double maxLagFrame = 200;
	double countLagFrame = 1;
	double lagProportion = 0;
	std::map<int, double> lags;
	for (int i = 0; i < _parameters.m_iNbFrames; i++)
	{

		if (simulationLagStatus == false) {
			countLagFrame = 1;
			lags[i] = lagProportion;
			simulationLagStatus = ((std::rand() % 2000) > 200);
		}

		else {
			if (countLagFrame < (maxLagFrame / 2))
				lagProportion = 2 * countLagFrame / maxLagFrame;

			else
				lagProportion = 2 * (1 - countLagFrame / maxLagFrame);

			lags[i] = lagProportion;
			countLagFrame++;
			simulationLagStatus = (countLagFrame != maxLagFrame);
		}

	}
	_parameters.m_lags = lags;
}
void SimulationUtils::ExecutionTimeControl(SimulationParameters& _parameters)
{
	std::map<std::string, Task*> Map = _parameters.m_sequence.GetTaskMap();
	std::map<std::pair<int, Task*>, vector<double>> Times;
	for (int i = 0;i < _parameters.m_lags.size();i++)
	{
		for (auto elem : Map)
		{
			std::pair<double, Task* > key = std::make_pair(i, elem.second);
			int steps = elem.second->GetStep(_parameters.m_lags[i]);

			std::vector<double> distributionParameters = elem.second->GenerateTime(_parameters.m_lags[i]);
			std::lognormal_distribution<double> distribution(distributionParameters[0], distributionParameters[1]);
			std::minstd_rand0 generator = elem.second->GetGenerator(elem.second->GetTopoOrder()*_parameters.m_seed* _parameters.m_iNbFrames +i);
			for (int j = 0;j < steps;j++)
			{
				double execution_time = std::min(distribution(generator), distributionParameters[2]);
				execution_time = std::max(execution_time, distributionParameters[3]);
				if (_parameters.IsDivided)
				{
					if(elem.first==P_TASK||elem.first==P2_TASK)
					{
						Times[key].push_back(execution_time/2.0);
						Times[key].push_back(execution_time/2.0);
					}
					else{
						Times[key].push_back(execution_time);
					}

				}
				else
				{
					Times[key].push_back(execution_time);
				}


			}
			if(_parameters.SortingSteps)
				std::sort(Times[key].begin(), Times[key].end(), greater<double>());

		}
	}

	_parameters.m_times = Times;

	//////Assign choice for tasks time: Randome or Median.
	for (auto elem : Map)
	{
		elem.second->WichToAssign(_parameters.mode);
	}
}


void SimulationUtils::TopologicalSorting(SimulationParameters& _parameters)
{
	std::map<std::string, Task*> Map = _parameters.m_sequence.GetTaskMap();
	std::stack<Task*> sorted;
	std::vector<Task*> visited;
	std::vector<Task*> topOrder;
	for (auto elem : Map) {
		if (!contained(visited, elem.second))
			SortUtil(elem.second, sorted, visited);
	}
	int cpt = 0;
	while (!sorted.empty()) {
		cpt++;
		sorted.top()->AddTopoOrder(cpt);
		topOrder.push_back(sorted.top());
		sorted.pop();
	}
}

void SimulationUtils::LexicographicalSorting(SimulationParameters& _parameters)
{
	//Lexicographical sorting of the Coffman-Graham Algorithm:

	std::map<std::string, Task*> Map = _parameters.m_sequence.GetTaskMap();
	std::map<Task*, int> labelTask;
	std::vector<Task*> Setlabels(0);
	std::vector<Task*> currentGeneration;
	std::map < Task*, std::vector<int>> vectorTask;
	std::vector<std::vector<int>> vectorsSorter;
	std::map<std::vector<int>, Task*> vectorOrder;
	Task* firstTask = nullptr;
	for (auto elem : Map)
	{
		if (elem.first == END_TASK)
		{
			labelTask[elem.second] = 1;
			Setlabels.push_back(elem.second);
		}
		if (elem.first == U_TASK)
		{
			labelTask[elem.second] = 2;
			Setlabels.push_back(elem.second);
		}
		if (elem.first == INIT_TASK)
		{
			firstTask = elem.second;
		}
	}
	int compt = 3;
	for (auto elem : Map)
	{
		currentGeneration.push_back(elem.second);
	}
	while (!contained(Setlabels, firstTask))
	{
		for (auto elem : currentGeneration)
		{
			if (!contained(Setlabels, elem) && permetted(elem, Setlabels))
			{
				std::vector<int> labelsHolder;
				for (auto sons : elem->GetSons())
				{
					labelsHolder.push_back(labelTask[sons]);
				}
				std::sort(labelsHolder.begin(), labelsHolder.end(), std::greater <int>());
				vectorTask[elem] = labelsHolder;
				vectorOrder[labelsHolder] = elem;
				vectorsSorter.push_back(labelsHolder);
				labelsHolder.clear();
			}
		}
		std::sort(vectorsSorter.begin(), vectorsSorter.end());
		if (!contained(Setlabels, vectorOrder[vectorsSorter[0]]))
		{
			Setlabels.push_back(vectorOrder[vectorsSorter[0]]);
			labelTask[vectorOrder[vectorsSorter[0]]] = compt;
			compt++;
			std::remove(vectorsSorter.begin(), vectorsSorter.end(), vectorsSorter[0]);
		}
		vectorOrder.clear();
		vectorsSorter.clear();
	}
	int CharacterAi=0;
	int PhysicStep=0;
	for (auto elem : Setlabels)
	{

		elem->AddLabel(labelTask[elem]);
	}
}

void SimulationUtils::HU(SimulationParameters& _parameters)
{
	std::map<std::string, Task*> Map = _parameters.m_sequence.GetTaskMap();
	std::map<int,Task*> TopoTask;
	for(auto elem:Map)
	{
		TopoTask[elem.second->GetTopoOrder()]=elem.second;
	}
	std::map<Task*,double> Hu;
	for(auto it = TopoTask.rbegin(); it != TopoTask.rend(); ++it)
	{
		if(it->second->GetSons().empty())
		{
			Hu[it->second]=0.0;
		}
		else
		{
			double level=0.0;
			for(auto son:it->second->GetSons())
			{
				level=std::max(level,Hu[son]+son->GetMean());
			}
			Hu[it->second]=level;
		}
	}

	for (auto elem:Hu)
	{
		elem.first->AddHu(Hu[elem.first]);
	}
};

void SimulationUtils::HLF(SimulationParameters& _parameters)
{
	std::map<std::string,Task*> Map=_parameters.m_sequence.GetTaskMap();
	std::map<int,Task*> TopoTask;
	for(auto elem:Map)
	{
		TopoTask[elem.second->GetTopoOrder()]=elem.second;
	}
	for(auto it = TopoTask.rbegin(); it != TopoTask.rend(); ++it)
	{
		if(it->second->GetSons().empty())
		{
			it->second->AddLevel(0);
		}
		else
		{
			int level=0;
			for(auto son:it->second->GetSons())
			{
				level=std::max(level,son->GetLevel());
			}
			it->second->AddLevel(level+1);
		}
	}
};

