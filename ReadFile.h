#ifndef _DATA_FILE_H
#include <string>
#include <vector>
#include <iostream>
#include "simulation/simulation.h"

class Readfile
{
private:
  std::string file_name_;
  int _nbFrame, _nbStartSeed, _nbEndSeed;
  std::vector<int> _nbWorkers;
  std::string _mode, FileSequenceName, FileTaskName;
  bool _IsDivided, _JsonOutput,_SortingSteps;
  std::vector<std::string> Methods ;

  bool print_info_;
  bool if_nbFrame;
  bool if_nbWorkers;
  bool if_nbStartSeed;
  bool if_nbEndSeed;
  bool if_mode;
  bool if_FileSequenceName;
  bool if_FileTaskName;
  bool if_IsDivided;
  bool if_Methods;
  bool if_JsonOutput;
  bool if_SortingSteps;
public:
  //constructor
  Readfile(std::string file_name);

  //ones to define
  void Read_data_file();

  //header defined
  const int Get_nbFrame() const { return _nbFrame; };
  const std::vector<int> Get_nbWorkers() const { return _nbWorkers; };
  const int Get_nbStartSeed() const { return _nbStartSeed; };
  const int Get_nbEndSeed() const { return _nbEndSeed; };
  const std::string Get_mode() const { return _mode; };
  const bool Get_IsDivided() const { return _IsDivided; };
  const bool Get_JsonOutput() const { return _JsonOutput; };
  const bool Get_SortingSteps() const { return _SortingSteps; };
  const std::vector<std::string> Get_methods() const { return Methods; };
  void PrintStatus();
  void FillParameters(SimulationParameters& _parameters);
};

#define _DATA_FILE_H
#endif