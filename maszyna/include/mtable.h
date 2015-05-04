#ifndef mtableH
#define mtableH

#pragma hdrstop


struct TMTableLine
{
	double km;
	double vmax;
	std::string StationName;
    std::string StationWare;
	int TrackNo;
	int Ah;
	int Am;
	int Dh;
	int Dm;
	double tm;
	int WaitTime;
};

typedef TMTableLine TMTable[101];

class TTrainParameters;
typedef TTrainParameters* *PTrainParameters;



class TTrainParameters 
{
public:
	std::string TrainName;
	double TTVmax;
	std::string Relation1;
	std::string Relation2;
	double BrakeRatio;
	std::string LocSeries;
	double LocLoad;
	TMTableLine TimeTable[101];
	int StationCount;
	int StationIndex;
	std::string NextStationName;
	double LastStationLatency;
	int Direction;
	double CheckTrainLatency(void);
	std::string ShowRelation();
	double WatchMTable(double DistCounter);
	std::string NextStop();
	bool IsStop(void);
	bool IsTimeToGo(double hh, double mm);
	bool UpdateMTable(double hh, double mm, std::string NewName);
	TTrainParameters(std::string NewTrainName);
	void NewName(std::string NewTrainName);
	bool LoadTTfile(std::string scnpath);
	bool DirectionChange(void);
	void StationIndexInc(void);
public:

	/* TObject.Create */ inline TTrainParameters(void) { }

	/* TObject.Destroy */ inline ~TTrainParameters(void) { }

	
};


class TMTableTime 
{

public:
	double GameTime;
	int dd;
	int hh;
	int mm;
	int ss;
	int srh;
	int srm;
	int ssh;
	int ssm;
	int h1;
	int h2;
	int m1;
	int m2;
	int s1;
	int s2;
	std::string shh;
	std::string smm;
	std::string sss;
	float mr;
	double bysec;
	void UpdateMTableTime(double deltaT);
	void Init(int InitH, int InitM, int InitSRH, int InitSRM, int InitSSH, int InitSSM);
	std::string GetTimeStr();
public:

 TMTableTime(void) { }

~TMTableTime(void) { }

	
};
extern TMTableTime* GlobalTime;

//---------------------------------------------------------------------------
#endif
