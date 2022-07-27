#ifndef _DATA_FILE_CPP

#include <fstream>
#include <iostream>
#include <cmath>
#include <regex>
#include <vector>
#include <string>
#include <sstream>

#include "ReadFile.h"
using namespace std;


Readfile::Readfile(std::string file_name)
: file_name_(file_name),  if_nbFrame(false), if_nbWorkers(false), if_nbStartSeed(false), if_nbEndSeed(false), if_mode(false), if_IsDivided(false)
{
}

void Readfile::Read_data_file()
{
    //test of opening
    ifstream data_file(file_name_.data());
    if (!data_file.is_open())
    {
        cout << "Unable to open file " << file_name_ << endl;
        exit(0);
    }
    else
    {
        cout << "---------------------------------------" << endl;
        cout << "Reading data file " << file_name_ << endl;
    }

    string file_line;
    //while not empty
    while (!data_file.eof())
    {
        getline(data_file, file_line);

        if (file_line.find("#") != std::string::npos)
        {
        // Ignore this line (comment)
        }
        else
        {
        if (file_line.find("_nbFrame") != std::string::npos)
        {
            data_file >> _nbFrame; 
            if_nbFrame = true;
        }
        if (file_line.find("_nbWorkers") != std::string::npos)
        {
            
           while(getline(data_file, file_line))
           {
               int workers;
                data_file >> workers;
                if (workers > 0) // prevents issues with empty names that may happen in Linux
                {
                    _nbWorkers.push_back(workers);
                } 
                if(workers==0)
                { 
                    break;
                }
            
           }    
            if_nbWorkers = true;
        }
        if (file_line.find("_nbStartSeed") != std::string::npos)
        {
            data_file >> _nbStartSeed;
            if_nbStartSeed= true;
        }
        if (file_line.find("_nbEndSeed") != std::string::npos)
        {
            data_file >> _nbEndSeed;
            if_nbEndSeed= true;
        }
        if (file_line.find("_mode") != std::string::npos)
        {
            data_file >> _mode;
            if_mode= true;
        }
        if (file_line.find("_IsDivided") != std::string::npos)
        {
            data_file >> _IsDivided;
            if_IsDivided= true;
        }
        if (file_line.find("_JsonOutput") != std::string::npos)
        {
            data_file >> _JsonOutput;
            if_JsonOutput= true;
        }
        if (file_line.find("_SortingSteps") != std::string::npos)
        {
            data_file >> _SortingSteps;
            if_JsonOutput= true;
        }
        if (file_line.find("Methods") != std::string::npos)
        {
            while (!data_file.eof())
            {
                getline(data_file, file_line);
                std::string name;
                data_file >> name;
                if (name.length() > 0) // prevents issues with empty names that may happen in Linux
                {
                    Methods.push_back(name);
                }
                //TODO: add a way to verify that the names are valid
            }
        }
        }
    }
    
    PrintStatus();
    if (_nbEndSeed <= _nbStartSeed)
    {
        std::cerr << "Error: End seed is smaller or equal to the starting seed. Aborting!\n";
        exit(1);
    }

}

void Readfile::PrintStatus()
{
    std::cout << "-- Information read from input file: --\n";
    std::cout << "Number of frames: " << _nbFrame << "\n";
    std::cout << "RNG Seeds: [" << _nbStartSeed << "," << _nbEndSeed <<"[\n";
    std::cout << "Scheduling methods to run: \n";
    for (std::string method: Methods)
    {
        std::cout << "- " << method << "\n";
    }
    std::cout << "---------------------------------------\n";
}

void Readfile::FillParameters(SimulationParameters& parameters)
{
    parameters.m_iNbFrames = this->Get_nbFrame();
	parameters.m_startSeed = this->Get_nbStartSeed();
    parameters.m_endSeed = this->Get_nbEndSeed();
	parameters.mode=this->Get_mode();
	parameters.IsDivided = this->Get_IsDivided();
    parameters.JsonOutput = this->Get_JsonOutput();
    parameters.SortingSteps = this->Get_SortingSteps();
    
};


#define _DATA_FILE_CPP
#endif