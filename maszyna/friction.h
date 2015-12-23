#ifndef frictionH
#define frictionH


class TFricMat
{
public:
	virtual double  GetFC(double N, double Vel);
};

class TP10Bg : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};

class TP10Bgu : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};

class TP10yBg : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};

class TP10yBgu : public TFricMat
{
public:
	double GetFC( double N, double Vel);
};

class TP10 : public TFricMat
{
public:
	double GetFC(double N, double real);
};

class TFR513 : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};

class TFR510 : public TFricMat
{
	double GetFC(double N, double Vel);
};

class TCosid : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};

class TDisk1 : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};

class TDisk2 : public TFricMat
{
public:
	double GetFC(double N, double Vel);
};



#endif