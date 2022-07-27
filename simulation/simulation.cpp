#include <stdafx.h>
#include <fstream>
#include <string>
#include <cstring>
#include "simulation.h"
#include "profiler/profiler.h"
#include "device/device.h"


void Simulation::Play(SimulationParameters& _parameters, std::string Strategy)
{
	Profiler::StartProfiling();
	Device* device = Device::CreateDevice(_parameters.m_iNbWorkers, Strategy,true);
	double startTimeCurrent;
	double startTimePrevious;
	bool simulationLagStatus = false;
	double maxLagFrame = 200;
	double countLagFrame = 1;
	double lagProportion = 0;
	TaskSequence sequence= _parameters.m_sequence;
	std::ofstream fil;
	fil.open("Result/TotalExecution/TotalExecution_"+std::to_string(_parameters.m_seed)+"_"+std::to_string(_parameters.m_iNbWorkers)+".txt");
	std::ofstream myfile;
	std::string cont;
	if(_parameters.IsDivided)
	{
		cont="_divided_";
	}
	else if(_parameters.SortingSteps)
	{
		cont="_sorted_";
	}
	else if(!_parameters.SortingSteps)
	{
		cont="_NonSorted_";
	}
	std::string _reportFileName =Strategy+cont+_parameters.mode + "_" +std::to_string(_parameters.m_seed) + "_" +std::to_string(_parameters.m_iNbFrames) + "_" +std::to_string(_parameters.m_iNbWorkers);

	//Path to strategy: Result/Strategy/nbFrame/TXT/_reportFileName.txt
	char result_txt[100];
	char result_json[100];
	strcpy(result_txt,"Result/");
	strcat(result_txt,Strategy.c_str()); 
	strcat(result_txt,"/");
	strcat(result_txt,std::to_string(_parameters.m_iNbWorkers).c_str());
	strcat(result_txt,"/");
	strcat(result_txt,std::to_string(_parameters.m_iNbFrames).c_str()); 
	strcat(result_txt,"/TXT/"); 
	strcat(result_txt,_reportFileName.c_str()); 
	strcat(result_txt,".txt");
	//Path to strategy: Result/Strategy/nbFrame/Json/_reportFileName.json
	strcpy(result_json,"Result/");
	strcat(result_json,Strategy.c_str()); 
	strcat(result_json,"/");
	strcat(result_json,std::to_string(_parameters.m_iNbWorkers).c_str());
	strcat(result_json,"/");
	strcat(result_json,std::to_string(_parameters.m_iNbFrames).c_str()); 
	strcat(result_json,"/Json/"); 
	strcat(result_json,_reportFileName.c_str()); 
	strcat(result_json,".json");
	//
	myfile.open(result_txt);
	double beg = 0.0;
	for (int i = 0; i < _parameters.m_lags.size(); i++)
	{		
		std::map<std::string, Task*> Map = _parameters.m_sequence.GetTaskMap();
		if(Strategy=="Aco"&&i>0)
		{
			ACO* Aco=NULL;
		
			Aco=new ACO(30,0.4,0.3,0.5,0.3,Map,_parameters.m_iNbWorkers, _parameters.m_lags[i-1]);
			
			std::map<Task*,int> bestScheduling;
		
			bestScheduling=Aco->GetBestScheduling();
			
			
			for(auto elem:Map)
			{
				elem.second->AddAnt(bestScheduling[elem.second]);
			}	
		}	
		double total=0.0;
		for(auto elem: Map)
		{
			for (int j =0 ;j < _parameters.m_times[std::make_pair(i, elem.second)].size();j++)
			{
			 	total += _parameters.m_times[std::make_pair(i, elem.second)][j];
			}
		}
		fil<<i<<" "<<total/_parameters.m_iNbWorkers<<std::endl;
		for (auto elem : Map)
		{
			if(i>0)
			{
				double PT = 0.0;
				for (int j =0 ;j < _parameters.m_times[std::make_pair(i-1, elem.second)].size();j++)
				{
					PT += _parameters.m_times[std::make_pair(i-1, elem.second)][j];
				}
				elem.second->AddPT(PT);  // add the Processing time of task in the previous frame
				elem.second->AddLS( _parameters.m_times[std::make_pair(i-1, elem.second)][0]); // add the Processing time of the longest step of the task
			}
			std::pair<int, Task*> key = std::make_pair(i, elem.second);
			elem.second->PushInTime(_parameters.m_times[key]); // Add the generated time of the task
			elem.second->UpdateStepCount(_parameters.m_times[key].size()); // the number of steps in the task
		}
		
		startTimeCurrent = device->GetGlobalClockTime();
			
		beg = startTimeCurrent;
		Profiler::GetInstance()->AddKeyFrame(-1, "Frame", "GenericTask", device->GetGlobalClockTime(), Profiler::KEY_EVENT_TYPE::BEGIN, 0, _parameters.m_iNbFrames);
		std::ofstream mfile;
			
		device->PlaySequence(_parameters.m_sequence, _parameters.m_lags[i]);
		
		Profiler::GetInstance()->AddKeyFrame(-1, "Frame", "GenericTask", device->GetGlobalClockTime(), Profiler::KEY_EVENT_TYPE::END, 0, _parameters.m_iNbFrames);
			
		if (i > 0)
		{
			Profiler::GetInstance()->AddKeyFrame(-2, "CriticalPath", "GenericTask", startTimePrevious, Profiler::KEY_EVENT_TYPE::BEGIN, 0, _parameters.m_iNbFrames);
			Profiler::GetInstance()->AddKeyFrame(-2, "CriticalPath", "GenericTask", startTimePrevious + device->GetBestTime(i), Profiler::KEY_EVENT_TYPE::END, 0, _parameters.m_iNbFrames);
		}
		startTimePrevious = startTimeCurrent;
		myfile << i << " " << device->GetGlobalClockTime()- startTimePrevious<<" " <<device->GetBestTime(i)<< " " << _parameters.m_lags[i] << std::endl;
	}
	
	myfile.close();
	fil.close();
	Device::DestroyDevice(device);
	
	if(_parameters.JsonOutput)
		Profiler::GetInstance()->DumpReport(result_json);
	
	Profiler::StopProfiling();
	
}

//Another sorting method for steps; sorting in packets
void Simulation::SLACK(SimulationParameters& _parameters, std::pair<int, Task*> key)
{
	std::vector<double> vect;
	int nb = std::ceil(_parameters.m_times[key].size() / _parameters.m_iNbWorkers);
	std::map<int, std::vector<double>> tuples;
	std::map<double, int> order;
	int n = _parameters.m_times[key].size();
	int worker = _parameters.m_iNbWorkers;
	if (nb > 4)
	{
		for (int i = 0;i < nb - 1;i++)
		{
			tuples[i] = { &_parameters.m_times[key][i*worker], &_parameters.m_times[key][(i + 1)*worker - 1] };
			order[_parameters.m_times[key][i*worker] - _parameters.m_times[key][(i + 1)*worker - 1]] = i;
		}
		tuples[nb - 1] = { &_parameters.m_times[key][(nb - 1)*worker], &_parameters.m_times[key][_parameters.m_times[key].size() - 1] };
		order[_parameters.m_times[key][(nb - 1)*worker] - _parameters.m_times[key][_parameters.m_times[key].size() - 1]] = nb - 1;
		for (auto iter = order.rbegin();iter != order.rend();++iter)
		{
			vect.insert(vect.end(), tuples[iter->second].begin(), tuples[iter->second].end());
		}
		_parameters.m_times[key] = vect;
	}
}
