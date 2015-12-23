#ifndef hamulceH
#define hamulceH

#include <cmath>
#include "friction.h"	



namespace Hamulce
{

// ********************************************************************************************************************
class TReservoir //: public System::TObject
{
	//typedef System::TObject inherited;

protected:
	double Cap;
	double Vol;
	double dVol;

public:
	
	void CreateCap(double Capacity);
	void CreatePress(double Press);
	virtual double pa();
	virtual double P();
	void Flow(double dv);
	void Act();
public:
	TReservoir();
   ~TReservoir() { }

};

typedef TReservoir* *PReservoir;


// ********************************************************************************************************************
class TBrakeCyl : public TReservoir
{
	typedef TReservoir inherited;

public:
	virtual double pa();
	virtual double P();
public:

 TBrakeCyl() : TReservoir() { }
 virtual ~TBrakeCyl() { }
};

// ********************************************************************************************************************
class TBrake // : public System::TObject
{
	//--typedef System::TObject inherited;

protected:
	TReservoir* BrakeCyl;
	TReservoir* BrakeRes;
	TReservoir* ValveRes;
	byte BCN;
	double BCM;
	double BCA;
	byte BrakeDelays;
	byte BrakeDelayFlag;
    //Friction::
		TFricMat* FM;
	double MaxBP;
	byte BA;
	byte NBpA;
	double SizeBR;
	double SizeBC;
	bool DCV;
	double ASBP;
	byte BrakeStatus;
	byte SoundFlag;

public:
	TBrake(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa);
	virtual ~TBrake() { }

	virtual double GetFC(double Vel, double N);
	virtual double GetPF(double PP, double dt, double Vel);
	double GetBCF();
	virtual double GetHPFlow(double HP, double dt);
	virtual double GetBCP();
	double GetBRP();
	double GetVRP();
	virtual double GetCRP();
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	bool SetBDF(byte nBDF);
	void Releaser(byte state);
	virtual void SetEPS(double nEPS);
	void ASB(byte state);
	byte GetStatus();
	void SetASBP(double press);
	virtual void ForceEmptiness();
	byte GetSoundFlag();
};


class TWest : public TBrake
{
	typedef TBrake inherited;

private:
	double LBP;
	double dVP;
	double EPS;
	double TareM;
	double LoadM;
	double TareBP;
	double LoadC;

public:
	void SetLBP(double P);
	virtual double GetPF(double PP, double dt, double Vel);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	virtual double GetHPFlow(double HP, double dt);
	void PLC(double mass);
	virtual void SetEPS(double nEPS);
	void SetLP(double TM, double LM, double TBP);

 TWest(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TBrake(i_mbp, i_bcr, i_bcd, i_brc, i_bcn, i_BD, i_mat, i_ba, i_nbpa) { }
~TWest() { }

};

// ********************************************************************************************************************
class THandle 
{
	//--typedef System::TObject inherited;

public:
	bool Time;
	bool TimeEP;
	double Sounds[5];
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
	virtual double GetCP();
	virtual void SetReductor(double nAdj);
	virtual double GetSound(byte i);
	virtual double GetPos(byte i);
public:

 THandle() { }
~THandle() { }


};

// ********************************************************************************************************************
class TESt : public TBrake
{
	typedef TBrake inherited;

public: //Q: ZAMIENIAM NA PUBLIC BO NIE WIDZIALO BVM i CntrlRes W TEStEP2::GetPF(...)
	TReservoir* CntrlRes;
	double BVM;

public:
	virtual double GetPF(double PP, double dt, double Vel);
	void EStParams(double i_crc);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	virtual double GetCRP();
	void CheckState(double BCP, double &dV1);
	void CheckReleaser(double dt);
	double CVs(double bp);
	double BVs(double BCP);
public:

TESt(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TBrake(i_mbp, i_bcr, i_bcd, i_brc, i_bcn
		, i_BD, i_mat, i_ba, i_nbpa) { }

	virtual ~TESt() { }


};


class TESt3 : public TESt
{
	typedef TESt inherited;

private:
	double CylFlowSpeed[2][2];

public:
	virtual double GetPF(double PP, double dt, double Vel);

TESt3(double i_mbp, double i_bcr, double i_bcd, double i_brc,
		byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TESt(i_mbp, i_bcr, i_bcd, i_brc, i_bcn, i_BD, i_mat, i_ba, i_nbpa) { }

virtual ~TESt3() { }


};

// ********************************************************************************************************************
class TESt3AL2 : public TESt3
{
	typedef TESt3 inherited;

private:
	double TareM;
	double LoadM;
	double TareBP;
	double LoadC;

public:
	TReservoir* ImplsRes;
	virtual double GetPF(double PP, double dt, double Vel);
	void PLC(double mass);
	void SetLP(double TM, double LM, double TBP);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
public:

TESt3AL2(double i_mbp, double i_bcr, double i_bcd, double i_brc
		, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TESt3(i_mbp, i_bcr, i_bcd, i_brc, i_bcn, i_BD, i_mat, i_ba, i_nbpa) { }


virtual ~TESt3AL2() { }
};


// ********************************************************************************************************************
class  TESt4R : public TESt
{
	typedef TESt inherited;

protected: //Q: BYLO private
	bool RapidStatus;
	double RapidTemp;

public:
	TReservoir* ImplsRes;
	virtual double GetPF(double PP, double dt, double Vel);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
public:

TESt4R(double i_mbp, double i_bcr, double i_bcd, double i_brc
		, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TESt(i_mbp, i_bcr, i_bcd, i_brc, i_bcn, i_BD, i_mat, i_ba, i_nbpa) { }
virtual ~TESt4R() { }
};


// ********************************************************************************************************************
class TLSt : public TESt4R
{
	typedef TESt4R inherited;

protected:
	double LBP; //Q: BYLO W PRIVATE ALE NIE BYLO DOSTEPU W TEStEP2::GetPF(...)
	double RM;
	double EDFlag;

private:
	double CylFlowSpeed[2][2];
	
public:
	void SetLBP(double P);
	void SetRM(double RMR);
	virtual double GetPF(double PP, double dt, double Vel);
	virtual double  GetHPFlow(double HP, double dt);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	virtual double GetEDBCP();
	void SetED(double EDstate);
public:

TLSt(double i_mbp, double i_bcr, double i_bcd, double i_brc,
		byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TESt4R(i_mbp, i_bcr, i_bcd, i_brc, i_bcn, i_BD, i_mat, i_ba, i_nbpa) { }
virtual ~TLSt(void) { }


};


// ********************************************************************************************************************
class TEStED : public TLSt
{
	typedef TLSt inherited;

private:
	double Nozzles[11];
	bool Zamykajacy;
	bool Przys_blok;
	TReservoir* Miedzypoj;

public:
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	virtual double GetPF(double PP, double dt, double Vel);
	virtual double GetEDBCP();

TEStED(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TLSt(i_mbp, i_bcr, i_bcd, i_brc, i_bcn
		, i_BD, i_mat, i_ba, i_nbpa) { }
virtual ~TEStED() { }


};


// ********************************************************************************************************************
class TEStEP2 : public TLSt
{
	typedef TLSt inherited;

private:
	double TareM;
	double LoadM;
	double TareBP;
	double LoadC;
	double EPS;

public:
	virtual double GetPF(double PP, double dt, double Vel);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	void PLC(double mass);
	virtual void SetEPS(double nEPS);
	void SetLP(double TM, double LM, double TBP);
public:

TEStEP2(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TLSt(i_mbp, i_bcr, i_bcd, i_brc, i_bcn
		, i_BD, i_mat, i_ba, i_nbpa) { }

 virtual ~TEStEP2(void) { }
};


// ********************************************************************************************************************
class TCV1 : public TBrake
{
	typedef TBrake inherited;

protected:
	TReservoir* CntrlRes;
	double BVM;

public:
	virtual double GetPF(double PP, double dt, double Vel);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	virtual double GetCRP(void);
	void CheckState(double BCP, double &dV1);
	double CVs(double bp);
	double BVs(double BCP);
public:

TCV1(double i_mbp, double i_bcr, double i_bcd, double i_brc,
		byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TBrake(i_mbp, i_bcr, i_bcd, i_brc, i_bcn
		, i_BD, i_mat, i_ba, i_nbpa) { }

 virtual ~TCV1(void) { }
};


// ********************************************************************************************************************
class TCV1R : public TCV1
{
	typedef TCV1 inherited;

private:
	TReservoir* ImplsRes;
	bool RapidStatus;
public:

TCV1R(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TCV1(i_mbp, i_bcr, i_bcd, i_brc, i_bcn, i_BD, i_mat, i_ba, i_nbpa) { }

virtual ~TCV1R(void) { }


};


// ********************************************************************************************************************
class TCV1L_TR : public TCV1
{
	typedef TCV1 inherited;

private:
	TReservoir* ImplsRes;
	double LBP;

public:
	virtual double GetPF(double PP, double dt, double Vel);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	void SetLBP(double P);
	virtual double GetHPFlow(double HP, double dt);
public:

 TCV1L_TR(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TCV1(i_mbp, i_bcr, i_bcd, i_brc, i_bcn
		, i_BD, i_mat, i_ba, i_nbpa) { }

virtual ~TCV1L_TR(void) { }


};


// ********************************************************************************************************************
class TKE : public TBrake
{
	typedef TBrake inherited;

private:
	bool RapidStatus;
	TReservoir* ImplsRes;
	TReservoir* CntrlRes;
	TReservoir* Brak2Res;
	double BVM;
	double TareM;
	double LoadM;
	double TareBP;
	double LoadC;
	double RM;
	double LBP;

public:
	void SetRM(double RMR);
	virtual double GetPF(double PP, double dt, double Vel);
	virtual void Init(double PP, double HPP, double LPP, double BP, byte BDF);
	virtual double GetHPFlow(double HP, double dt);
	virtual double GetCRP(void);
	void CheckState(double BCP, double &dV1);
	void CheckReleaser(double dt);
	double CVs(double bp);
	double BVs(double BCP);
	void PLC(double mass);
	void SetLP(double TM, double LM, double TBP);
	void SetLBP(double P);
public:

TKE(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa) : TBrake(i_mbp, i_bcr, i_bcd, i_brc, i_bcn,
		i_BD, i_mat, i_ba, i_nbpa) { }
virtual ~TKE(void) { }
};


// ********************************************************************************************************************
class TFV4a : public THandle
{
	typedef THandle inherited;

private:
	double CP;
	double TP;
	double RP;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
public:

TFV4a(void) : THandle() { }
virtual ~TFV4a(void) { }
};


// ********************************************************************************************************************
class TFV4aM : public THandle
{
	typedef THandle inherited;

private:
	double CP;
	double TP;
	double RP;
	double XP;
	double RedAdj;
	bool Fala;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
	virtual void SetReductor(double nAdj);
	virtual double GetSound(byte i);
	virtual double GetPos(byte i);

 TFV4aM(void) : THandle() { }
virtual ~TFV4aM(void) { }


};


// ********************************************************************************************************************
class TM394 : public THandle
{
	typedef THandle inherited;

private:
	double CP;
	double RedAdj;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
	virtual void SetReductor(double nAdj);
	virtual double GetCP(void);
	virtual double GetPos(byte i);

TM394(void) : THandle() { }
virtual ~TM394(void) { }


};


// ********************************************************************************************************************
class TH14K1 : public THandle
{
	typedef THandle inherited;

protected:   //Q: ZMIENIAM private NA protected
	double CP;
	double RedAdj;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
	virtual void SetReductor(double nAdj);
	virtual double GetCP(void);
	virtual double GetPos(byte i);
public:

TH14K1(void) : THandle() { }
virtual ~TH14K1(void) { }
};


// ********************************************************************************************************************
class TSt113 : public TH14K1
{
	typedef TH14K1 inherited;

private:
	double EPS;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual double GetCP();
	virtual double GetPos(byte i);
	virtual void Init(double press);

TSt113() : TH14K1() { }
virtual ~TSt113() { }
};


// ********************************************************************************************************************
class  Ttest : public THandle
{
	typedef THandle inherited;

private:
	double CP;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
public:

Ttest() : THandle() { }
virtual ~Ttest() { }
};


// ********************************************************************************************************************
class TFD1 : public THandle
{
	typedef THandle inherited;

private:
	double MaxBP;
	double BP;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
	virtual double GetCP(void);

TFD1(void) : THandle() { }
virtual ~TFD1(void) { }
};


// ********************************************************************************************************************
class TH1405 : public THandle
{
	typedef THandle inherited;

private:
	double MaxBP;
	double BP;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual void Init(double press);
	virtual double GetCP(void);
public:

TH1405(void) : THandle() { }
virtual ~TH1405(void) { }


};


// ********************************************************************************************************************
class TFVel6 : public THandle
{
	typedef THandle inherited;

private:
	double EPS;

public:
	virtual double GetPF(double i_bcp, double pp, double hp, double dt, double ep);
	virtual double  GetCP(void);
	virtual double GetPos(byte i);
	virtual double GetSound(byte i);
	virtual void Init(double press);
public:
TFVel6(void) : THandle() { }
virtual ~TFVel6(void) { }
};


//-- var, const, procedure ---------------------------------------------------
const int LocalBrakePosNo = 0xa;
const int MainBrakeMaxPos = 0xa;
const int bdelay_G = 0x1;
const int bdelay_P = 0x2;
const int bdelay_R = 0x4;
const int bdelay_M = 0x8;
const byte bdelay_GR = 0x80;
const int b_off = 0x0;
const int b_hld = 0x1;
const int b_on = 0x2;
const int b_rfl = 0x4;
const int b_rls = 0x8;
const int b_ep = 0x10;
const int b_asb = 0x20;
const byte b_dmg = 0x80;
const int df_on = 0x1;
const int df_off = 0x2;
const int df_br = 0x4;
const int df_vv = 0x8;
const int df_bc = 0x10;
const int df_cv = 0x20;
const int df_PP = 0x40;
const byte df_RR = 0x80;
const int s_fv4a_b = 0x0;
const int s_fv4a_u = 0x1;
const int s_fv4a_e = 0x2;
const int s_fv4a_x = 0x3;
const int s_fv4a_t = 0x4;
const int bp_P10 = 0x0;
const int bp_P10Bg = 0x2;
const int bp_P10Bgu = 0x1;
const int bp_LLBg = 0x4;
const int bp_LLBgu = 0x3;
const int bp_LBg = 0x6;
const int bp_LBgu = 0x5;
const int bp_KBg = 0x8;
const int bp_KBgu = 0x7;
const int bp_D1 = 0x9;
const int bp_D2 = 0xa;
const int bp_FR513 = 0xb;
const int bp_Cosid = 0xc;
const int bp_PKPBg = 0xd;
const int bp_PKPBgu = 0xe;
const byte bp_MHS = 0x80;
const int bp_P10yBg = 0xf;
const int bp_P10yBgu = 0x10;
const int sf_Acc = 0x1;
const int sf_BR = 0x2;
const int sf_CylB = 0x4;
const int sf_CylU = 0x8;
const int sf_rel = 0x10;
const int sf_ep = 0x20;
const int bh_MIN = 0x0;
const int bh_MAX = 0x1;
const int bh_FS = 0x2;
const int bh_RP = 0x3;
const int bh_NP = 0x4;
const int bh_MB = 0x5;
const int bh_FB = 0x6;
const int bh_EB = 0x7;
const int bh_EPR = 0x8;
const int bh_EPN = 0x9;
const int bh_EPB = 0xa;
#define SpgD  (7.917000E-01)
#define SpO  (5.067000E-01)
static double BPT[9][2];          //BYLO extern, DALEM static
static double BPT_394[7][2];      //BYLO extern, DALEM static  
static const short int i_bcpno = 0x6;
extern double PF(double P1, double P2, double S, double DP);
extern double PF1(double P1, double P2, double S);
extern double PFVa(double PH, double PL, double S, double LIM, const double DP = 0.1);
extern double PFVd(double PH, double PL, double S, double LIM, const double DP = 0.1);

}

using namespace Hamulce;

#endif