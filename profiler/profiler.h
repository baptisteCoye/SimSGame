#pragma once

class Profiler
{

public:
    enum KEY_EVENT_TYPE
    {
        BEGIN,
        END
    };

    Profiler();
    ~Profiler();
    static void StartProfiling();
    static Profiler* GetInstance();
    void DumpReport(const std::string& _report);
    void Clear();
    static void StopProfiling();

    void AddKeyFrame(int _workerId, std::string _taskName, std::string _taskType, double _timeStart, KEY_EVENT_TYPE _type, int _currentStep, int _step);
    
private:

    static Profiler* ms_instance;

    struct KeyFrame {
        int m_workerId;
        std::string m_taskName;
        std::string m_taskType;
        double m_timeStart;
        KEY_EVENT_TYPE m_keyType;
        int m_currentStep;
        int m_step; //  [X:Y] step number X out of Y
    };

    std::vector<KeyFrame> m_profiledTasks;
};

