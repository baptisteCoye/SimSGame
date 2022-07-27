#include <stdafx.h>
#include "taskSequence.h"

TaskSequence::TaskSequence()
{
	m_graph = 0.0;
}

void TaskSequence::AddTask(Task* _task)
{
    if (_task != nullptr)
    {
        m_taskMap[_task->GetName()] = _task;
    }
}

Task* TaskSequence::GetTask(std::string _taskName)
{
    if (m_taskMap.find(_taskName) != m_taskMap.end())
    {
        return m_taskMap[_taskName];
    }

    return nullptr;
}

std::map<std::string, Task*>& TaskSequence::GetTaskMap()
{
    return m_taskMap;
}

void TaskSequence::Clear()
{ 
    std::map<std::string, Task*>::iterator it;
    for (it = m_taskMap.begin(); it != m_taskMap.end(); it++)
    {
        delete it->second;
    }

    m_taskMap.clear();
}

double TaskSequence::graph()
{
	return m_graph;
}