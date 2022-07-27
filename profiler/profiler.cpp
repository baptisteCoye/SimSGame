#include <stdafx.h>
#include "profiler.h"
#include <iostream>
#include <sstream>
#include "assert.h"


Profiler* Profiler::ms_instance = nullptr;

Profiler::Profiler(){}
Profiler::~Profiler(){}

void Profiler::StartProfiling()
{
    ms_instance = new Profiler();
}

Profiler* Profiler::GetInstance()
{
    return ms_instance;
}

void Profiler::DumpReport(const std::string& _report)
{
    std::ofstream reportStream;
    reportStream.open(_report);
   
    assert(reportStream);
    assert(m_profiledTasks.size() > 1);
    
    reportStream << "{" << std::endl;
    reportStream << "\"traceEvents\": [" << std::endl;

    for (int i=0; i < m_profiledTasks.size(); i++) 
    {
        KeyFrame key = m_profiledTasks[i];
        reportStream << "{ ";
        reportStream << "\"pid\":1, ";
        reportStream << "\"tid\":" << key.m_workerId << ", ";
        reportStream << "\"ts\":" << key.m_timeStart << ", ";
        reportStream << "\"ph\":" << (key.m_keyType == KEY_EVENT_TYPE::BEGIN ? "\"B\", " : "\"E\", ");
        reportStream << "\"name\":\"" << key.m_taskName << "[" << key.m_currentStep + 1 << "|" << key.m_step << "]\"";
        reportStream << " }" << (i < m_profiledTasks.size() - 1 ? "," : "") << std::endl;
    }

    reportStream << "]" << std::endl;
    reportStream << "}" << std::endl;

    reportStream.close();
}

void Profiler::AddKeyFrame(int _workerId, std::string _taskName, std::string _taskType, double _timeStart, KEY_EVENT_TYPE _type, int _currentStep, int _step)
{
    KeyFrame key = {
        _workerId,
        _taskName,
        _taskType,
        _timeStart,
        _type,
        _currentStep,
        _step 
    };

    m_profiledTasks.push_back(key);
}

void Profiler::Clear()
{
    m_profiledTasks.clear();
}

void Profiler::StopProfiling()
{
    ms_instance->Clear();

    delete ms_instance;
    ms_instance = nullptr;
}