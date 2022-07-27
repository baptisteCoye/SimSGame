#include "stdafx.h"
#include "ReadFile.h"
#include "simulation/simulationUtils.h"
#include <cstdlib>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    SimulationParameters parameters;
	
    const std::string data_file_name = argv[1];
  
    Readfile *Rf = new Readfile(data_file_name);
    Rf->Read_data_file();
	Rf->FillParameters(parameters);
	
	SimulationUtils::ParseParameters("engine_tasks.json", "engine_sequence.json", parameters);
	SimulationUtils::AddSons(parameters);  //Give successors of each task
	SimulationUtils::LexicographicalSorting(parameters); // Give lexicographical order to each task
	SimulationUtils::GenerateLagProportion(parameters); // Give lag to each frame
	SimulationUtils::TopologicalSorting(parameters);   // Give topological order to each task
	SimulationUtils::HU(parameters);    // Give each task its level (using mean execution time) for Hu algorithm
	SimulationUtils::HLF(parameters); // Give each task its level (esxecution time=1) for Hu algorithm
	for(int j=0;j<Rf->Get_nbWorkers().size();j++)
	{
		parameters.m_iNbWorkers=Rf->Get_nbWorkers()[j];
		std::cout << "Number of worker threads: " << Rf->Get_nbWorkers()[j] << "\n";
		for (int i = parameters.m_startSeed; i < parameters.m_endSeed; ++i)
		{
			std::cout << "-- Starting the run with RNG seed " << i << "\n";
			
			parameters.m_seed = i;
			SimulationUtils::ExecutionTimeControl(parameters);
			for(auto elem:Rf->Get_methods())
			{
				if(elem=="Infinity")
				{
					parameters.m_iNbWorkers=1000;
				}
				// Create folders to save results: Result/Strategy/nbFrame/TXT/ and Result/Strategy/nbFrame/Json/
				char result_txt[100];
				char result_json[100];
				strcpy(result_txt,"mkdir -p Result/");
				strcat(result_txt,elem.c_str()); 
				strcat(result_txt,"/");
				strcat(result_txt,std::to_string(parameters.m_iNbWorkers).c_str()); 
				strcat(result_txt,"/");
				strcat(result_txt,std::to_string(Rf->Get_nbFrame()).c_str()); 
				strcpy(result_json,"mkdir -p Result/");
				strcat(result_json,elem.c_str()); 
				strcat(result_json,"/");
				strcat(result_json,std::to_string(parameters.m_iNbWorkers).c_str());
				strcat(result_json,"/");
				strcat(result_json,std::to_string(Rf->Get_nbFrame()).c_str()); 
				strcat(result_txt,"/TXT");
				strcat(result_json,"/Json");
				const int dir_err_txt = system(result_txt);
				const int dir_err_json = system(result_json);
				const int dir_err_totalExec=system("mkdir -p Result/TotalExecution/");
				//
				std::cout << " - Running scheduling algorithm " << elem << "\n";
				if(elem=="Infinity")
				{
					Simulation::Play(parameters, elem);
					parameters.m_iNbWorkers=Rf->Get_nbWorkers()[j];
				}
				else{
					Simulation::Play(parameters, elem);
				}
			}
			std::cout << "---- ---- ---- ---- ---- ---- ---- ----\n";
		}
	}
	parameters.m_sequence.Clear();

    return 0;
}
