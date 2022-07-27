#include <random>
#pragma once


class Task
{

public:
	Task(std::string _name, int _steps, double _mean, double _min, double _max, double _stdDev, int _stepsLag, double _meanLag, double _minLag, double _maxLag, double _stdDevLag, std::string _type);

	void Initialize(double _lagProportion);
	void AssignRandomTimes(double _lagProportion);
	
	void AssignMediane(double _lagProportion);
	std::vector<double> GenerateTime(double _lagProportion);
	double GenerateTimeSeed(double _lagProportion, std::minstd_rand0 generator);
	void AddFather(Task* _father);
	void AddSon(Task* _son);

	void Execute(int _step);
	void SpendTime(int _step, double _time);
	double GetRemainingTimeForStep(int _step);
	double GetRemainingTotalTime();
	bool IsReady();
	bool IsDone();

	bool HasFreeStepToProcess();
	int ClaimFreeStep();
	int GetStepCount();

	//UPDATE THE NUMBER OF STEPS FOR EACH FRAME
	void UpdateStepCount(int _nb);

	//GET THE TYPE OF THE TASK: GRAPHIC OR ENGINE 
	std::string GetTaskType();

	int GetCurrentStep() { return m_iCurrentStep; };
	std::string GetName();
	std::vector<Task*> GetFathers();
	std::vector<Task*> GetSons();
	double Get_meanLog();
	double ProceesTime();
	double meanTime();


	//ADD AND GET THE LEVEL OF A TASK: USED FOR HLF ALGORITHM
	void AddLevel(int _level) { m_level = _level; };
	int GetLevel() { return m_level; };

	//ADD AND GET FOR Hu ALGORITHM AND WEIGHTED LENGTH ALGORITHM
	void AddHu(double _Hu) { m_Hu = _Hu; }
	double GetHu() { return m_Hu; };

	void AddWl(double _Wl) { m_Wl = _Wl; }
	double GetWl() { return m_Wl; };

	//ADD AND GET THE PROCESSING TIME FOR A TASK: USED FOR LPT AND SPT ALGORITHMS
	void AddPT(double _PT) { m_TotalProceesingTime = _PT; }
	double GetPT() { return m_TotalProceesingTime; };

	//FOR COFFMAN GRAHAM  ALGORITHM
	void AddLabel(int _label);   // Add  label of Coffman-Graham Algorithm
	int GetLabel();             // Get label of Coffman-Graham Algorithm

	//FOR TOPOLOGICAL SORTING 
	void AddTopoOrder(int _label);// Topological order
	int GetTopoOrder();

	//COMPRARE BETWEEN TWO TASKS TASK1==TASK2
	bool const operator==(Task& rhs);

	//ADD MODE TO WORK WORK: RANDOM OR MEDIAN
	void WichToAssign(std::string);

	//ASSIGN STOCKED EXECUTION TIMES TO THE TASK 
	void PushInTime(std::vector<double> _Times);
	std::vector<double> GetTime() { return m_Times; };

	//GET THE NUMBER OF STEPS 
	double GetStep(double _lag) { return std::ceil(m_steps*(1-_lag)+_lag*m_stepsLag); };

	//GET THE MEAN OF THE EXECUTION TIME
	double GetMean() { return m_mean; };

	//GET THE PRIVATE GENERATOR OF THE TASK
	std::minstd_rand0 GetGenerator(double _seed);
	
	//ADD AND GET THE FINISHING TIME OF THE TASK IN THE PREVIOUS FRAME
	void AddFT(double _FT) { m_FinishTime = _FT; }
	double GetFT() { return m_FinishTime; };
	
	//ADD AND GET THE BEGINING OF THE WAITING TIME OF THE TASK IN THE PREVIOUS FRAME
	void AddBW(double _FT) { m_BW = _FT; }
	double GetBW() { return m_BW; };

	//ADD AND GET THE BEGINING TIME OF THE EXECUTION OF THE TASK IN THE PREVIOUS FRAME
	void AddEW(double _FT) { m_EW = _FT; }
	double GetEW() { return m_EW; };

	//GET THE RATIO RESPONSE TIME: USED FOR HRRN ALGORITHM
	double GetResponseRatio() { return (m_FinishTime- m_BW)/ (m_FinishTime - m_EW); };

	//GET THE WAITING TIME OF THE TASK IN THE PREVIOUS FRAME

	double GetWaitingTime(){return m_EW-m_BW;};

	//ADD AND GET THE LONGEST PROCESSING TIME OF TASK'S STEP
	void AddLS(double _LS) { m_LS = _LS; }
	double GetLS() { return m_LS; };

	//ADD AND GET ANT SCHEDULING
	void AddAnt(int _antChoice) { m_antChoice= _antChoice; }
	int GetAnt() { return m_antChoice; };

	//GET THE REMAINING TIME FOR THE TASK BASED IN THE PREVIOUS EXECUTION TIME
	double GetRemainingPreviousTime();
	
private:
	std::string m_name;
	std::vector<Task*> m_fathers;
	std::vector<Task*> m_sons;
	std::vector<double> m_remainingTimes;
	std::vector<double> m_Times;
	std::string m_type;
	int m_iCurrentStep;
	std::string m_choice;
	int m_level;
	// Simulation Parameters
	int m_totalSteps;
	int m_steps;
	double m_mean;
	double m_min;
	double m_max;
	double m_stdDev;
	int m_stepsLag;
	double m_meanLag;
	double m_minLag;
	double m_maxLag;
	double m_stdDevLag;
	double m_meanLog;
	double m_meanTime;
	double m_graphicTime;
	int m_label;
	int m_TopoOrder;
	double m_Wl;
	double m_Hu;
	double m_TotalProceesingTime;
	double m_FinishTime;
	double m_BW;
	double m_EW;
	double m_LS;
	int m_antChoice;
	std::minstd_rand0 m_generator;
	
};

