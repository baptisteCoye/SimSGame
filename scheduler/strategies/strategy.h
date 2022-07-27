#pragma once
#include <map>
#include <string>
#include <vector>
#include "../../tasks/task.h"

class Strategy
{
public:
    void Initialize(Task* _lastTask, Task* _firstTask);
    virtual void PushAvailableTask(Task* _task);
    virtual Task* GetNextAvailableTask(int _workerId, int& _stepToProcess);
	void AddTaskTime(Task* _task);
	void AddTaskInducedTime(Task* _task, double _inducedTime, std::vector<Task*> _criticalPath);
    bool HasAvailableTask();
	double GetBestTime(int _frameIndex);
	virtual bool IsOnCriticalPath(Task* _task);
	void bubbleSort(std::vector<Task*> &vec);
	virtual double Priority(Task*) = 0;
	double GetInducedTime(Task* task) { return m_previousFrameinducedTime[task]; };
	void FirstVariantInducedTime(Task* _task, double _inducedTime, std::vector<Task*> _criticalPath);
	void SecondVariantInducedTime(Task* _task, double _inducedTime, std::vector<Task*> _criticalPath);
	//ThreePhase:
    void AddPhase(int _pahseNumber){m_phase=_pahseNumber;};
    int GetNumber(){return m_phase;};

protected:
	std::vector<Task*> m_availableEngineTasks;
	std::vector<Task*> m_availableGraphicTasks;
	std::vector<Task*> m_criticalPath;
	int m_phase;

	std::vector<Task*> m_firstVariantCriticalPath;
	std::vector<Task*> m_secondVariantCriticalPath;

	std::map<Task*,double> m_PreviousExecTime;
	std::map<Task*,double> m_PreviousMaxStep;

	std::map<Task*, double> m_previousFrameExecTime;
	std::map<Task*, double> m_previousFrameinducedTime;
	std::map<Task*, double> m_FirstVariantPrevious;
	std::map<Task*, double> m_SecondVariantPrevious;
	std::vector<double> m_bestTime;
	std::string m_LessOrMore;
	int m_workerCount;
	bool m_tie_break;
};

class LPT : public Strategy
{
public:
	LPT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~LPT() {};
	double Priority(Task* task) { return task->GetPT(); };	
};


class SPT : public Strategy
{
public:
	SPT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~SPT() {};
	double Priority(Task* task) { return task->GetPT(); };
};

class Aco : public Strategy
{
public:
	Aco(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~Aco() {};
	double Priority(Task* task) { return task->GetAnt(); };
};

class HLF : public Strategy
{
public:
	HLF(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
		m_tie_break=true;
	};

	~HLF() {};
	double Priority(Task* task) { return task->GetLevel(); };
};

class Hu : public Strategy
{
public:
	Hu(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~Hu() {};
	double Priority(Task* task) { return task->GetHu(); };
};


class WL : public Strategy
{
public:
	WL(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~WL() {};
	double Priority(Task* task) { return task->GetWl(); };
};

class LFF : public Strategy
{
public:
	LFF(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~LFF() {};
	double Priority(Task* task) { return task->GetFT(); };
};

class HRRN: public Strategy
{
public:
	HRRN(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~HRRN() {};
	double Priority(Task* task) { return task->GetResponseRatio(); };
};

class MostScussors : public Strategy
{
public:
	MostScussors(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~MostScussors() {};
	double Priority(Task* task) { return task->GetSons().size(); };
};

class Coffman : public Strategy
{
public:
	Coffman(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~Coffman() {};
	double Priority(Task* task) { return task->GetLabel(); };
};

class TOPO : public Strategy
{
public:
	TOPO(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~TOPO() {};
	double Priority(Task* task) { return task->GetTopoOrder(); };
};

class LRT : public Strategy
{
public:
	LRT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~LRT() {};
	double Priority(Task* task) { return task->GetRemainingPreviousTime(); };
};

class SRT : public Strategy
{
public:
	SRT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~SRT() {};
	double Priority(Task* task) { return task->GetRemainingPreviousTime(); };
};

class SLRT : public Strategy
{
public:
	SLRT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~SLRT() {};
	double Priority(Task* task) { return task->GetRemainingTimeForStep(task->GetCurrentStep()); };
};

class SSRT : public Strategy
{
public:
	SSRT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~SSRT() {};
	double Priority(Task* task) { return task->GetRemainingTimeForStep(task->GetCurrentStep()); };
};

class CriticalPath : public Strategy
{
public:
	CriticalPath(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "other";
	};
	void PushAvailableTask(Task* _task) override;
	double Priority(Task* task) { return 0.0; };
};

class FIFO : public Strategy
{
public:
	FIFO(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "other";
	};

	~FIFO() {};
	double Priority(Task* task) { return 0.0; };
};

class firstVariantCriticalPath : public Strategy
{
public:
	firstVariantCriticalPath(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "other";
	};
	bool IsOnCriticalPath(Task* _task)  override;
	void PushAvailableTask(Task* _task) override;
	double Priority(Task* task) { return 0.0; };
};

class secondVariantCriticalPath : public Strategy
{
public:
	secondVariantCriticalPath(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "other";
	};
	bool IsOnCriticalPath(Task* _task) override;
	void PushAvailableTask(Task* _task) override;
	double Priority(Task* task) { return 0.0; };
};

class NEH:public Strategy
{
public:
    NEH(const int _iNbWorkers);
    std::vector<Task*> Sort(std::vector<Task*> _tasks);
    int GetAvailableWorker();
    double GetMakespan();
    void FillWorkers(Task* _task);
    double ScheduleTasks(std::vector<Task*> _tasks);
    double Priority(Task* task) { return task->GetPT(); };
	Task* GetNextAvailableTask(int _workerId, int& _stepToProcess);
	void Transform();
private:
    std::map<int,double> m_Workers;
    std::map<int,double> m_WorkersTest;
};

class ThreePhases:public Strategy
{
	public:
	ThreePhases(const int _iNbWorkers){
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~ThreePhases() {};
	double Priority(Task* task);
};
class TwoPhases:public Strategy
{
	public:
	TwoPhases(const int _iNbWorkers){
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~TwoPhases() {};
	double Priority(Task* task);
};

class TwoPhasesStatic:public Strategy
{
	public:
	TwoPhasesStatic(const int _iNbWorkers){
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "more";
	};

	~TwoPhasesStatic() {};
	double Priority(Task* task);
};

class WT: public Strategy
{
public:
	WT(const int _iNbWorkers) {
		m_workerCount = _iNbWorkers;
		m_LessOrMore = "less";
	};

	~WT() {};
	double Priority(Task* task) { return task->GetWaitingTime(); };
};

class Infinity : public Strategy
{
public:
	Infinity(const int _iNbWorkers) {
		m_workerCount =_iNbWorkers;
		m_LessOrMore = "other";
	};

	~Infinity() {};
	Task* GetNextAvailableTask(int _workerId, int& _stepToProcess) override;
	double Priority(Task* task) { return 0.0; };
};
