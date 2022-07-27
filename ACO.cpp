#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <set>
#include <stdlib.h>
#include <random>
#include "stdafx.h"
#include "ACO.h"
#include "tasks/keytasks.h"
#define debug(i) std::cout <<"line= "<<__LINE__<<" , "<<#i<<" = "<< i << std::endl;

ACO::ACO(int _SwarmSize, double _InitialPhereomone,double _q0, double _beta,double _evaporationRate,std::map<std::string,Task*> _Map, int _nbWorkers,int _lag)
{
   
    m_swarm=new Swarm;
    m_swarm->size=_SwarmSize;
    m_nbWorkers=_nbWorkers;
   
    for(int i=0;i<m_swarm->size;i++)
    {
        Ant* ant=new Ant;
        ant->id=i;
        ant->makespan=0.0;
        if(i==0)
        {
            ant->rank=queen;
        }
        else
        {
            ant->rank=scout;
        }
        m_swarm->Ants.push_back(ant);
    }
  
    m_InitialPhereomone=_InitialPhereomone;
    m_q0=_q0;
    m_beta=_beta;
    m_evaporationRate=_evaporationRate;
    m_Map=_Map;
    m_wave=0;
    sequence.GetTaskMap()=m_Map;
    m_lag=_lag;
};
void ACO::UpdatePheremone(Task* _task,Task* _nextTask)
{
    std::pair<Task*,Task*> key=std::make_pair(_task,_nextTask);
    m_Pheromone[key]=(1-m_evaporationRate)*m_Pheromone[key]+m_evaporationRate*m_InitialPhereomone;
};

std::vector<Task*> ACO::ReadyTasks(Task* _StartTask,Ant* ant)
{
    std::vector<Task*> vec;
    for(auto elem:m_Map)
    {
        if(elem.second->GetTaskType()=="engine")
        {
            bool IsIn=false;
            if(ant->visitedNodes.count(elem.second)==0)
            {
                IsIn = true;
            }
            bool found=true;
            if(!elem.second->GetFathers().empty())
            {
                for(auto father:elem.second->GetFathers())
                {
                    if(father->GetTaskType()=="engine")
                    {
                        if(ant->visitedNodes.count(father)==0)
                        {
                            found = false;
                        }
                    }
                }
            }
            if(found&&IsIn)
            {
                bool firstTime=true;
                if(std::find(vec.begin(), vec.end(), elem.second) != vec.end())
                {
                    firstTime = false;
                }
                if(firstTime)
                {
                    vec.push_back(elem.second);
                }
            }
        }
    }
    return vec;
};

Task* ACO::GetNextTask(Task* _StartTask,Ant* ant)
{
   
    std::vector<Task*> ReadyTask=ReadyTasks(_StartTask,ant);
    
    unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
    std::minstd_rand0 generator(seed);
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    double q=distribution(generator);
    if(q<m_q0)
    {
        Task* task=nullptr;
        std::map<Task*,double> TaskValue;
        for(auto elem: ReadyTask)
        {
            TaskValue[elem]=this->TaskDecisonValue(_StartTask,elem);
           
        }
        
        double max=0.0;
        for(auto elem:TaskValue)
        {
            if(max<elem.second)
            {
                max=elem.second;
                task=elem.first;
            }
        }
      
        int size=ant->visitedNodes.size();
        
        ant->visitedNodes[task]=size+1;
    
        UpdatePheremone(_StartTask,task);
        std::pair<Task*,Task*> key=std::make_pair(_StartTask,task);
       
        return task;
    }
    else
    {
        double norme=0.0;
        std::map<Task*,double> TaskProbability;
       
     
      
        for(auto elem: ReadyTask)
        {
            
            if(std::isnan(this->TaskDecisonValue(_StartTask,elem)))
            {
                TaskProbability[elem]=pow(10,-5);
                
            }
            else
            {
                TaskProbability[elem]=this->TaskDecisonValue(_StartTask,elem);
                
            }
                
         
            
        }
        
        std::vector<Task*> TaskCluster;
       
        
        double* min=new double;
        *min=std::numeric_limits<double>().max();
        for(auto elem:TaskProbability)
        {
            if(elem.second<*min)
            {
                *min=elem.second;
            }       
        }
        
        for(auto elem:TaskProbability)
        {
            for(int i=0;i<std::ceil(elem.second/(*min));i++)
            {
                TaskCluster.push_back(elem.first);
            }
        }
        srand(time(0));
        Task* task=nullptr;
       
        task=TaskCluster[rand()%TaskCluster.size()];
       
        int size=ant->visitedNodes.size();
     
        ant->visitedNodes[task]=size+1;
       
        UpdatePheremone(_StartTask,task);
        std::pair<Task*,Task*> key=std::make_pair(_StartTask,task);
       
        return task;
    }
};


void ACO::WorkAnt(Ant* ant)
{
  
    Task* task=nullptr;
   
    task=m_Map[INIT_TASK];
   
    while(task!=m_Map[END_TASK])
    {
        task=GetNextTask(task,ant);
      
    } 
};

void ACO::QueenStroll(Ant* ant)
{
    Task* task=nullptr;
  
    task=m_Map[INIT_TASK];
   bool stop=true;
   
    while(ant->visitedNodes.count(m_Map[END_TASK])==0&&stop)
    {
        std::vector<Task*> ReadyTask=ReadyTasks(task,ant);
        
        int* min=new int;
        *min=0.0;
        
        for(auto elem:ReadyTask)
        {
            
            if(std::isnan(elem->GetLabel()))
            {
                stop=false;
                return;
            }
            else
            {
                
                if(*min<elem->GetLabel())
                {
                    
                    *min=elem->GetLabel();
                    task=elem;
                }
            }
        }
        
        if(task!=nullptr)
        {
            int size=ant->visitedNodes.size();
        
            ant->visitedNodes[task]=size+1;   
        }
          
    } 
}
void ACO::WorkSwarm()
{
    double TimeCurrent;
    double TimePrevious;
    Device* dev = Device::CreateDevice(m_nbWorkers, "Aco",false);
    for(auto ant:m_swarm->Ants)
    {
        if(ant->rank==queen)
        {
           
            this->QueenStroll(ant);
        
        }
        else
        {
           
            this->WorkAnt(ant);
            
        }
      
       for(auto elem:m_Map)
			{
				elem.second->AddAnt(ant->visitedNodes[elem.second]);
		
			}
            TimeCurrent = dev->GetGlobalClockTime();
            dev->PlaySequence(sequence,m_lag);
            TimePrevious = TimeCurrent;
            ant->makespan=dev->GetGlobalClockTime()- TimePrevious;
            
    }
   
    Device::DestroyDevice(dev);
 
};

std::map<Task*,int> ACO::GetBestScheduling()
{
 
    this->WorkSwarm();

    double bestMakespan=std::numeric_limits<double>().max();
   
    Ant* LuckyAnt=new Ant;
   
    for(auto ant:m_swarm->Ants)
    {
     
     
        if(ant->makespan<bestMakespan&&ant->makespan!=0.0)
        {
           
            bestMakespan=ant->makespan;
            
            LuckyAnt=ant;
        }
    }
  
    this->AddLuckyAnt(LuckyAnt);
   
    std::map<Task*,int> visitedNode=LuckyAnt->visitedNodes;

    return LuckyAnt->visitedNodes;
};