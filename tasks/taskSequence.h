#pragma once

#include <string>
#include <map>

#include "task.h"

class TaskSequence
{

public:
    TaskSequence();

    void AddTask(Task* _task);
    Task* GetTask(std::string _taskName);
    void Clear();

    std::map<std::string, Task*>& GetTaskMap();
	double graph();
private:
    std::map<std::string, Task*> m_taskMap;
	double m_graph;
};

