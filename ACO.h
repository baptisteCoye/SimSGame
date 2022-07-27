#pragma once

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <map>

#include <cmath>
#include <utility>
#include "simulation/simulation.h"
#include "device/device.h"

enum Rank
{
    scout,
    queen
};
struct Ant
{
    int id;
    std::map<Task*,int> visitedNodes;
    double makespan;
    Rank rank;
};
struct Swarm
{
    int size;
    std::vector<Ant*> Ants;
};

class ACO
{
    public:
    ACO(int _SwarmSize, double _InitialPhereomone,double _q0, double _beta,double _evaporationRate,std::map<std::string,Task*> _Map, int _nbWorkers,int _lag);
    
    double Pheremone(Task* _task,Task* _nextTask){ return (m_Pheromone[std::make_pair(_task,_nextTask)]==0.0)? m_InitialPhereomone:m_Pheromone[std::make_pair(_task,_nextTask)];};
    void UpdatePheremone(Task* _task,Task* _nextTask);
    double PassageQuality(Task* _nextTask){return 1.0/_nextTask->GetResponseRatio();};//1.0/(_nextTask->GetPT()+1.0)
    std::vector<Task*> ReadyTasks(Task* _StartTask,Ant* ant);
    double TaskDecisonValue(Task* _task,Task* _nextTask){ return Pheremone(_task,_nextTask)*pow(PassageQuality(_nextTask),m_beta); };
    Task* GetNextTask(Task* _StartTask,Ant* ant);
    void WorkAnt(Ant* ant);
    void QueenStroll(Ant* ant);
    std::map<Task*,int> GetAntPath(Ant* ant){return ant->visitedNodes;};
    int GetNextPosition(Ant* ant);
    void WorkSwarm();
    std::map<Task*,int> GetBestScheduling();
    void AddLuckyAnt(Ant* _Ant){m_LuckyAnt=_Ant;};
    Ant* GetLuckyAnt(){return m_LuckyAnt;}
    private:
    std::map<Task*,bool> m_IsVisited;
    std::map<std::pair<Task*,Task*>,double> m_Pheromone;
    std::map<std::string,Task*> m_Map;
    double m_InitialPhereomone;
    double m_q0;
    double m_beta;
    double m_evaporationRate;
    int m_nbWorkers;
    Swarm* m_swarm;
    Ant* m_LuckyAnt;
    int m_wave;
    TaskSequence sequence;
    int m_lag;
};
