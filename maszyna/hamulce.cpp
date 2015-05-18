// HAMULCE.CPP --------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak, Maciej Czapkiewicz and others


	MaSzyna EU07 - SPKS
	Brakes.
	Copyright(C) 2007 - 2014 Maciej Cierniak
	
	(C)youBy
	Co brakuje :
moze jeszcze jakis SW

Zrobione:
ESt3, ESt3AL2, ESt4R, LSt, FV4a, FD1, EP2, prosty westinghouse
duzo wersji żeliwa
KE
Tarcze od 152A
Magnetyki(implementacja w mover.pas)
Matrosow 394
H14K1(zasadniczy), H1405(pomocniczy), St113(ep)
Knorr / West EP - żeby był
*/

// Pascal to C++ conversion by queuedEU (07.05.15 - 13.05.15)
// hamulce.cpp is equal with 1166

#include "commons.h"
#include "commons_usr.h"
#include "hamulce.h"

#pragma hdrstop

const double DPL = 0.25;

double PR(double p1, double p2)
{
	double ph, pl;
	ph = Max0R(p1, p2) + 0.1;
	pl = p1 + p2 - ph + 0.2;
	return (p2 - p1) / (1.13*ph - pl);
}

double PF_old(double P1, double P2, double S)
{
	double PH, PL, ret;

	PH = Max0R(P1, P2) + 1;
	PL = P1 + P2 - PH + 2;
	if (PH - PL<0.0001) ret = 0;
	else if ((PH - PL) < 0.05)
		ret = 20 * (PH - PL)*(PH + 1) * 222 * S*(P2 - P1) / (1.13*PH - PL);
	else
		ret = (PH + 1) * 222 * S*(P2 - P1) / (1.13*PH - PL);

	return ret;
}

double Hamulce::PF(double P1, double P2, double S, double DP = 0.25)
{
	double PH, PL, SG, fm, ret;

	PH = Max0R(P1, P2) + 1; //wyzsze cisnienie absolutne
	PL = P1 + P2 - PH + 2;  //nizsze cisnienie absolutne
	SG = PL / PH; //bezwymiarowy stosunek cisnien
	fm = PH * 197 * S*sign(P2 - P1); //najwyzszy mozliwy przeplyw, wraz z kierunkiem
	if (SG > 0.5)  //jesli ponizej stosunku krytycznego
		if ((PH - PL) < DP)  //niewielka roznica cisnien
			ret = (1 - SG) / DPL*fm * 2 * sqrt((DP)*(PH - DP));
		else
			ret = fm * 2 * sqrt((SG)*(1 - SG));
	else             //powyzej stosunku krytycznego
		ret = fm;

	return ret;
}

double Hamulce::PF1(double P1, double P2, double S)
{
	double PH, PL, SG, fm, ret;
	const double DPS = 0.001;

	PH = Max0R(P1, P2) + 1; //wyzsze cisnienie absolutne
	PL = P1 + P2 - PH + 2;  //nizsze cisnienie absolutne
	SG = PL / PH; //bezwymiarowy stosunek cisnien
	fm = PH * 197 * S*sign(P2 - P1); //najwyzszy mozliwy przeplyw, wraz z kierunkiem
	if (SG > 0.5) //jesli ponizej stosunku krytycznego
		if (SG < DPS)  //niewielka roznica cisnien
			ret = (1 - SG) / DPS*fm * 2 * sqrt((DPS)*(1 - DPS));
		else
			ret = fm * 2 * sqrt((SG)*(1 - SG));
	else             //powyzej stosunku krytycznego
		ret = fm;

return ret;
}

double Hamulce::PFVa(double PH, double PL, double S, double LIM, const double DP) //zawor napelniajacy z PH do PL, PL do LIM
{
	double SG, fm, ret;

	if (LIM > PL)
	{
		LIM = LIM + 1;
		PH = PH + 1; //wyzsze cisnienie absolutne
		PL = PL + 1;  //nizsze cisnienie absolutne
		SG = PL / PH; //bezwymiarowy stosunek cisnien
		fm = PH * 197 * S; //najwyzszy mozliwy przeplyw, wraz z kierunkiem
		if ((LIM - PL) < DP)  fm = fm*(LIM - PL) / DP; //jesli jestesmy przy nastawieniu, to zawor sie przymyka
		if (SG > 0.5) //jesli ponizej stosunku krytycznego
			if ((PH - PL) < DPL)  //niewielka roznica cisnien
				ret = (PH - PL) / DPL*fm * 2 * sqrt((SG)*(1 - SG));
			else
				ret = fm * 2 * sqrt((SG)*(1 - SG));
		else             //powyzej stosunku krytycznego
			ret = fm;
	}
	else
		ret = 0;

 return ret;
}

double Hamulce::PFVd(double PH, double PL, double S, double LIM, const double DP)  //zawor wypuszczajacy z PH do PL, PH do LIM
{
	double SG, FM, ret;
	
	if (LIM < PH)
	{
		LIM = LIM + 1;
		PH = PH + 1; //wyzsze cisnienie absolutne
		PL = PL + 1;  //nizsze cisnienie absolutne
		SG = PL / PH; //bezwymiarowy stosunek cisnien
		FM = PH * 197 * S; //najwyzszy mozliwy przeplyw, wraz z kierunkiem
		if ((PH - LIM) < 0.1)  FM = FM*(PH - LIM) / DP; //jesli jestesmy przy nastawieniu, to zawor sie przymyka
		if (SG > 0.5) //jesli ponizej stosunku krytycznego
			if ((PH - PL) < DPL)  //niewielka roznica cisnien
				ret = (PH - PL) / DPL*FM * 2 * sqrt((SG)*(1 - SG));
			else
				ret = FM * 2 * sqrt((SG)*(1 - SG));
		else             //powyzej stosunku krytycznego
			ret = FM;
	}
	else
   ret = 0;

 return ret;
}



//---ZBIORNIKI---------------------------------------------------------------------------------------------------------

double TReservoir::pa()
{
 return 0.1*Vol / Cap;
}

double TReservoir::P()
{
 return Vol / Cap;
}

void TReservoir::Flow(double dv)
{
dVol = dVol + dv;
}

TReservoir::TReservoir()
{
 //-inherited Create;
 Cap = 1;
 Vol = 0;
}


void TReservoir::Act()
{
 Vol = Vol + dVol;
 dVol = 0;
}

void TReservoir::CreateCap(double Capacity)
{
 Cap = Capacity;
}

void TReservoir::CreatePress(double Press)
{
 Vol = Cap*Press;
 dVol = 0;
}



//---SILOWNIK----------------------------------------------------------------------------------------------------------
double TBrakeCyl::pa()
//var VtoC: real;
{
	//  VtoC:=Vol/Cap;
	return P()*0.1;
}

//(* STARA WERSJA
double TBrakeCyl::P()
{
 double VtoC; //stosunek cisnienia do objetosci
	
 const double VS = 0.005;
 const double pS = 0.05;
 const double VD = 1.40;
 const double cD = 1;
 const double pD = VD - cD;
	
	VtoC = Vol / Cap;
	//  P:=VtoC;
	if (VtoC < VS) return (VtoC*pS / VS);           //objetosc szkodliwa
	else if (VtoC > VD) return (VtoC - cD);        //caly silownik
	else return (pS + (VtoC - VS) / (VD - VS)*(pD - pS)); //wysuwanie tloka
}



//---HAMULEC-----------------------------------------------------------------------------------------------------------
TBrake::TBrake(double i_mbp, double i_bcr, double i_bcd, double i_brc, byte i_bcn, byte i_BD, byte i_mat, byte i_ba, byte i_nbpa)
{
//-	inherited Create;
 MaxBP = i_mbp;
 BCN = i_bcn;
 BCA = i_bcn*i_bcr*i_bcr*M_PI;  //Q: zamienilem pi na M_PI
 BA = i_ba;
 NBpA = i_nbpa;
 BrakeDelays = i_BD;
	//210.88
	//  SizeBR:=i_bcn*i_bcr*i_bcr*i_bcd*40.17*MaxBP/(5-MaxBP);  //objetosc ZP w stosunku do cylindra 14" i cisnienia 4.2 atm
 SizeBR = i_brc*0.0128;
 SizeBC = i_bcn*i_bcr*i_bcr*i_bcd*210.88*MaxBP / 4.2;            //objetosc CH w stosunku do cylindra 14" i cisnienia 4.2 atm

//  BrakeCyl:=TReservoir.Create;
 BrakeCyl = new TBrakeCyl();
 BrakeRes = new TReservoir();
 ValveRes = new TReservoir();

//tworzenie zbiornikow
 BrakeCyl->CreateCap(i_bcd*BCA * 1000);
 BrakeRes->CreateCap(i_brc);
 ValveRes->CreateCap(0.25);

	//  FM.Free;
	//materialy cierne
i_mat = i_mat &(255 - bp_MHS);

switch (i_mat)
	{
 case bp_P10Bg:  FM = new TP10Bg(); break;
 case bp_P10Bgu:  FM = new TP10Bgu(); break;
 case bp_FR513:   FM = new TFR513(); break;
 case bp_Cosid:   FM = new TCosid(); break;
 case bp_P10yBg:  FM = new TP10yBg(); break;
 case bp_P10yBgu: FM = new  TP10yBgu(); break;
 case bp_D1:      FM = new TDisk1(); break;
 case bp_D2:      FM = new TDisk2(); break;
 default: FM = new TP10();
    }
//  else //domyslnie
  
}

  //inicjalizacja hamulca (stan poczatkowy)
void TBrake::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{

}

//pobranie wspolczynnika tarcia materialu
double TBrake::GetFC(double Vel, double N)
{
 return FM->GetFC(N, Vel);
}

//cisnienie cylindra hamulcowego
double TBrake::GetBCP()
{
 return BrakeCyl->P();
}

//cisnienie zbiornika pomocniczego
double TBrake::GetBRP()
{
 return BrakeRes->P();
}

//cisnienie komory wstepnej
double TBrake::GetVRP()
{
 return ValveRes->P();
}

//cisnienie zbiornika sterujacego
double TBrake::GetCRP()
{
 return 0.0;
}

//przeplyw z przewodu glowneg
double TBrake::GetPF(double PP, double dt, double Vel)
{
ValveRes->Act();
BrakeCyl->Act();
BrakeRes->Act();
return 0.0;
}

//przeplyw z przewodu zasilajacego
double TBrake::GetHPFlow(double HP, double dt)
{
 return 0.0;
}

double TBrake::GetBCF()
{
 return BCA * 100 * BrakeCyl->P();
}

bool TBrake::SetBDF(byte nBDF) //Q: CZY DOBRZE PRZETLUMACZYLEM?
{
	bool ret;
	if ( ((nBDF & BrakeDelays) == nBDF) && (nBDF != BrakeDelayFlag) )
	{
	BrakeDelayFlag = nBDF;
	ret = true;
	}
   else
  ret = false;

return ret;
}

void TBrake::Releaser(byte state)
{
 BrakeStatus = (BrakeStatus & 247) | state*b_rls;
}

void TBrake::SetEPS(double nEPS)
{

}

void TBrake::ASB(byte state)
{                         //255-b_asb(32)
 BrakeStatus = (BrakeStatus & 223) | state*b_asb;
}

byte TBrake::GetStatus()
{
 return BrakeStatus;
}

byte TBrake::GetSoundFlag()
{
 return SoundFlag;
 SoundFlag = 0;
}

void TBrake::SetASBP(double press)
{
 ASBP = press;
}

void TBrake::ForceEmptiness()
{
	ValveRes->CreatePress(0);
	BrakeRes->CreatePress(0);
	ValveRes->Act();
	BrakeRes->Act();
}


//---WESTINGHOUSE------------------------------------------------------------------------------------------------------

void TWest::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	ValveRes->CreatePress(PP);
	BrakeCyl->CreatePress(BP);
	BrakeRes->CreatePress(PP / 2 + HPP / 2);
	//   BrakeStatus:=3*Byte(BP>0.1);
}


double TWest::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1;
	double VVP, BVP, CVP, BCP;
	double temp;


	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	CVP = BrakeCyl->P();
	BCP = BrakeCyl->P();

	if ((BrakeStatus & 1) == 1)
		if (VVP + 0.03 < BVP)
			BrakeStatus = (BrakeStatus | 2);
		else if (VVP > BVP + 0.1)
			BrakeStatus = (BrakeStatus | 252);
		else if (VVP > BVP)
			BrakeStatus = (BrakeStatus & 253);
		else;
	else
		if (VVP + 0.25 < BVP)
			BrakeStatus = (BrakeStatus | 3);

	if (((BrakeStatus & b_hld) == b_off) && (!DCV))
		dV = PF(0, CVP, 0.0068 * SizeBC, 0.255555)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);

	if ((BCP > LBP + 0.01) && (DCV))
		dV = PF(0, CVP, 0.1*SizeBC, 0.255555)*dt;
	else dV = 0;

	BrakeCyl->Flow(-dV);

	//hamulec EP
	temp = BVP*byte(EPS > 0);
	dV = PF(temp, LBP, 0.0015, 0.255555) * dt * EPS * EPS * byte(LBP * EPS < MaxBP * LoadC);
	LBP = LBP - dV;
	dV = 0;

	//przeplyw ZP <-> silowniki
	if (((BrakeStatus & b_on) == b_on) && ((TareBP < 0.1) || (BCP < MaxBP*LoadC)))
		if (BVP > LBP)
		{
			DCV = false;
			dV = PF(BVP, CVP, 0.017*SizeBC, 0.255555)*dt;
		}
		else dV = 0;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);
	if (DCV)
		dVP = PF(LBP, BCP, 0.01*SizeBC, 0.255555)*dt;
	else dVP = 0;
	BrakeCyl->Flow(-dVP);
	if (dVP > 0) dVP = 0;

	//przeplyw ZP <-> rozdzielacz
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(BVP, VVP, 0.0011*SizeBR, 0.255555)*dt;
	else dV = 0;

	BrakeRes->Flow(dV);
	dV1 = dV*0.95;
	ValveRes->Flow(-0.05*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01*SizeBR, 0.255555)*dt;
	ValveRes->Flow(-dV);

	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();

 return (dV - dV1);
}

double TWest::GetHPFlow(double HP, double dt)
{
 return dVP;
}

void TWest::SetEPS(double nEPS)
{
	double BCP;

	BCP = BrakeCyl->P();
	if (nEPS > 0)
		DCV = true;
	else if (nEPS == 0)
	{
		if (EPS != 0)
		{
			if (LBP > 0.4)
				LBP = BrakeCyl->P();

			if (LBP < 0.15)
				LBP = 0;
		}
	}
	EPS = nEPS;
}

void TWest::PLC(double mass)
{
 LoadC = 1 + byte(mass < LoadM)*((TareBP + (MaxBP - TareBP)*(mass - TareM) / (LoadM - TareM)) / MaxBP - 1);
}

void TWest::SetLP(double TM, double LM, double TBP)
{
 TareM = TM;
 LoadM = LM;
 TareBP = TBP;
}


//---OERLIKON EST4---
void TESt::CheckReleaser(double dt)
{
	double VVP, CVP;

	VVP = Min0R(ValveRes->P(), BrakeRes->P() + 0.05);
	CVP = CntrlRes->P() - 0.0;

	//odluzniacz
	if ((BrakeStatus & b_rls) == b_rls)
		if (CVP - VVP < 0)
			BrakeStatus = BrakeStatus & 247;
		else
		{
			CntrlRes->Flow(+PF(CVP, 0, 0.1)*dt);
		}
}


void TESt::CheckState(double BCP, double &dV1)
{
	double VVP, BVP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	//  if (BVP<VVP) then
	//    VVP:=(BVP+VVP)/2;
	CVP = CntrlRes->P() - 0.0;

	//sprawdzanie stanu
	if (((BrakeStatus & 1) == 1) && (BCP > 0.25))
		if (VVP + 0.003 + BCP / BVM < CVP)
			BrakeStatus = (BrakeStatus | 2); //hamowanie stopniowe
		else if (VVP - 0.003 + (BCP - 0.1) / BVM > CVP)
			BrakeStatus = (BrakeStatus | 252); //luzowanie
		else if (VVP + BCP / BVM > CVP)
			BrakeStatus = (BrakeStatus | 253); //zatrzymanie napelaniania
		else;
	else
		if ((VVP + 0.10 < CVP) && (BCP < 0.25))    //poczatek hamowania
		{
			if ((BrakeStatus & 1) == 0)
			{
				ValveRes->CreatePress(0.02*VVP);
				SoundFlag = SoundFlag | sf_Acc;
				ValveRes->Act();
			}
			BrakeStatus = (BrakeStatus | 3);

			//     ValveRes.CreatePress(0);
			//     dV1:=1;
		}
		else if ((VVP + (BCP - 0.1) / BVM<CVP) && ((CVP - VVP)*BVM>0.25) && (BCP > 0.25)) //zatrzymanie luzowanie
			BrakeStatus = (BrakeStatus | 1);

	if ((BrakeStatus & 1) == 0)
		SoundFlag = (SoundFlag | sf_CylU);
}

double TESt::CVs(double bp)
{
	double VVP, BVP, CVP, retCVs;

	BVP = BrakeRes->P();
	CVP = CntrlRes->P();
	VVP = ValveRes->P();

	//przeplyw ZS <-> PG
	if ((VVP<CVP - 0.12) || (BVP<CVP - 0.3) || (bp>0.4))
		retCVs = 0;
	else
		if (VVP>CVP + 0.4)
			if (BVP > CVP + 0.2)
				retCVs = 0.23;
			else
				retCVs = 0.05;
		else
			if (BVP > CVP - 0.1)
				retCVs = 1;
			else
				retCVs = 0.3;

 return retCVs;
}

double TESt::BVs(double BCP)
{
	double VVP, BVP, CVP, retBVs;

	BVP = BrakeRes->P();
	CVP = CntrlRes->P();
	VVP = ValveRes->P();

	//przeplyw ZP <-> rozdzielacz
	if (BVP < CVP - 0.3)
		retBVs = 0.6;
	else
		if (BCP < 0.5)
			if (VVP > CVP + 0.4)
				retBVs = 0.1;
			else
				retBVs = 0.3;
		else
			retBVs = 0;

	return retBVs;
}


double TESt::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = BrakeCyl->P();
	CVP = CntrlRes->P() - 0.0;

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(BCP, dV1);
	CheckReleaser(dt);

	CVP = CntrlRes->P();
	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	temp = CVs(BCP);
	dV = PF(CVP, VVP, 0.0015*temp)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;


	//luzowanie
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.0058*SizeBC)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> silowniki
	if ((BrakeStatus & b_on) == b_on)
		dV = PF(BVP, BCP, 0.016*SizeBC)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> rozdzielacz
	temp = BVs(BCP);
	//  if(BrakeStatus and b_hld)=b_off then
	if (VVP - 0.05 > BVP)
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01)*dt;
	ValveRes->Flow(-dV);

	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();

	return (dV - dV1);
}


void TESt::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	ValveRes->CreatePress(PP);
	BrakeCyl->CreatePress(BP);
	BrakeRes->CreatePress(PP);
	CntrlRes = new TReservoir();
	CntrlRes->CreateCap(15);
	CntrlRes->CreatePress(HPP);
	BrakeStatus = 0;

	BVM = 1 / (HPP - LPP)*MaxBP;

	BrakeDelayFlag = BDF;
}

void TESt::EStParams(double i_crc)
{
	//
}

double TESt::GetCRP()
{
 return CntrlRes->P();
}



//---EP2---------------------------------------------------------------------------------------------------------------

void TEStEP2::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	inherited;
	ImplsRes->CreateCap(1);
	ImplsRes->CreatePress(BP);

	BrakeRes->CreatePress(PP);

	BrakeDelayFlag = bdelay_P;
	BrakeDelays = bdelay_P;
}

double TEStEP2::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp, ret;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = ImplsRes->P();
	CVP = CntrlRes->P(); //110115 - konsultacje warszawa1

	dV = 0; dV1 = 0;

	//odluzniacz
	CheckReleaser(dt);

	//sprawdzanie stanu
	if (((BrakeStatus & 1) == 1) && (BCP > 0.25))
		if (VVP + 0.003 + BCP / BVM < CVP - 0.12)
			BrakeStatus = (BrakeStatus | 2);//hamowanie stopniowe
		else if (VVP - 0.003 + BCP / BVM > CVP - 0.12)
			BrakeStatus = (BrakeStatus | 252); //luzowanie
		else if (VVP + BCP / BVM > CVP - 0.12)
			BrakeStatus = (BrakeStatus | 253); //zatrzymanie napelaniania
		else;
	else
		if ((VVP + 0.10 < CVP - 0.12) && (BCP < 0.25))    //poczatek hamowania
		{
			if ((BrakeStatus & 1) == 0)
			{
				//       ValveRes.CreatePress(0.5*VVP);  //110115 - konsultacje warszawa1
				//       SoundFlag:=SoundFlag or sf_Acc;
				//       ValveRes.Act;
			}
			BrakeStatus = (BrakeStatus | 3);
		}
		else if ((VVP + BCP / BVM<CVP - 0.12) && (BCP>0.25)) //zatrzymanie luzowanie
			BrakeStatus = (BrakeStatus | 1);

	//przeplyw ZS <-> PG
	if ((BVP < CVP - 0.2) || (BrakeStatus>0) || (BCP > 0.25))
		temp = 0;
	else
		if (VVP>CVP + 0.4)
			temp = 0.1;
		else
			temp = 0.5;

	dV = PF(CVP, VVP, 0.0015*temp / 1.8)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;

	//hamulec EP
	temp = BVP*byte(EPS > 0);
	dV = PF(temp, LBP, 0.00053 + 0.00060*byte(EPS < 0))*dt*EPS*EPS*byte(LBP*EPS < MaxBP*LoadC);
	LBP = LBP - dV;

	//luzowanie KI
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.00083)*dt;
	else dV = 0;
	ImplsRes->Flow(-dV);
	//przeplyw ZP <-> KI
	if (((BrakeStatus & b_on) == b_on) && (BCP < MaxBP * LoadC))
		dV = PF(BVP, BCP, 0.0006)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	ImplsRes->Flow(-dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01*SizeBR)*dt;
	ValveRes->Flow(-dV);

	ret = dV - dV1; // TUTAJ ZWRACA? CO Z RESZTA KODU?

	temp = Max0R(BCP, LBP);

	if (ImplsRes->P() > LBP + 0.01)
	LBP = 0;

	//luzowanie CH
	if ((BrakeCyl->P() > temp + 0.005) || (Max0R(ImplsRes->P(), 8 * LBP) < 0.05))
		dV = PF(0, BrakeCyl->P(), 0.25*SizeBC*(0.01 + (BrakeCyl->P() - temp)))*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> CH
	if ((BrakeCyl->P() < temp - 0.005) && (Max0R(ImplsRes->P(), 8 * LBP) > 0.10) && (Max0R(BCP, LBP) < MaxBP * LoadC))
		dV = PF(BVP, BrakeCyl->P(), 0.35*SizeBC*(0.01 - (BrakeCyl->P() - temp)))*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);

	ImplsRes->Act();
	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();

	return ret;
}

void TEStEP2::PLC(double mass)
{
 LoadC = 1 + byte(mass < LoadM)*((TareBP + (MaxBP - TareBP)*(mass - TareM) / (LoadM - TareM)) / MaxBP - 1);
}

void TEStEP2::SetEPS(double nEPS)
{
 EPS = nEPS;
  if ((EPS>0) && (LBP + 0.01 < BrakeCyl->P()))
	LBP = BrakeCyl->P();
}

void TEStEP2::SetLP(double TM, double LM, double TBP)
{
 TareM = TM;
 LoadM = LM;
 TareBP = TBP;
}

//---EST3--------------------------------------------------------------------------------------------------------------

double TESt3::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = BrakeCyl->P();
	CVP = CntrlRes->P() - 0.0;

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(BCP, dV1);
	CheckReleaser(dt);

	CVP = CntrlRes->P();
	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	temp = CVs(BCP);
	dV = PF(CVP, VVP, 0.0015*temp)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;


	//luzowanie
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.0042*(1.37 - byte(BrakeDelayFlag = bdelay_G))*SizeBC)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);
	//przeplyw ZP <-> silowniki
	if ((BrakeStatus & b_on) == b_on)
		dV = PF(BVP, BCP, 0.017*(1 + byte((BCP<0.58) && (BrakeDelayFlag = bdelay_G)))*(1.13 - byte((BCP>0.6) && (BrakeDelayFlag = bdelay_G)))*SizeBC)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);
	//przeplyw ZP <-> rozdzielacz
	temp = BVs(BCP);
	if (VVP - 0.05 > BVP)
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01)*dt;
	ValveRes->Flow(-dV);

	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();

	return (dV - dV1);
}



//---EST4-RAPID--------------------------------------------------------------------------------------------------------

double TESt4R::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp, ret;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = ImplsRes->P();
	CVP = CntrlRes->P() - 0.0;

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(BCP, dV1);
	CheckReleaser(dt);

	CVP = CntrlRes->P();
	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	temp = CVs(BCP);
	dV = PF(CVP, VVP, 0.0015*temp / 1.8)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;


	//luzowanie KI
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.00037*1.14 * 15 / 19)*dt;
	else dV = 0;
	ImplsRes->Flow(-dV);
	//przeplyw ZP <-> KI
	if ((BrakeStatus & b_on) == b_on)
		dV = PF(BVP, BCP, 0.0014)*dt;
	else dV = 0;
	//  BrakeRes.Flow(dV);
	ImplsRes->Flow(-dV);
	//przeplyw ZP <-> rozdzielacz
	temp = BVs(BCP);
	if (BVP < VVP - 0.05)  //or((PP<CVP)and(CVP<PP-0.1)
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01*SizeBR)*dt;
	ValveRes->Flow(-dV);

	ret = (dV - dV1);



	RapidStatus = (BrakeDelayFlag = bdelay_R) && (((Vel > 55) && (RapidStatus)) || (Vel > 70));

	RapidTemp = RapidTemp + (0.9*byte(RapidStatus) - RapidTemp)*dt / 2;
	temp = 1.9 - RapidTemp;
	if ((BrakeStatus & b_asb) == b_asb)
		temp = 1000;
	//luzowanie CH
	if ((BrakeCyl->P()*temp > ImplsRes->P() + 0.005) || (ImplsRes->P() < 0.25))
		if ((BrakeStatus & b_asb) == b_asb)
			dV = PFVd(BrakeCyl->P(), 0, 0.115*SizeBC * 4, ImplsRes->P() / temp)*dt;
		else
			dV = PFVd(BrakeCyl->P(), 0, 0.115*SizeBC, ImplsRes->P() / temp)*dt;
	//   dV:=PF(0,BrakeCyl.P,0.115*sizeBC/2)*dt
	//   dV:=PFVd(BrakeCyl.P,0,0.015*sizeBC/2,ImplsRes.P/temp)*dt
	else dV = 0;
	BrakeCyl->Flow(-dV);
	//przeplyw ZP <-> CH
	if ((BrakeCyl->P()*temp<ImplsRes->P() - 0.005) && (ImplsRes->P() > 0.3))
		//   dV:=PFVa(BVP,BrakeCyl.P,0.020*sizeBC,ImplsRes.P/temp)*dt
		dV = PFVa(BVP, BrakeCyl->P(), 0.60*SizeBC, ImplsRes->P() / temp)*dt;
	else dV = 0;
	BrakeRes->Flow(-dV);
	BrakeCyl->Flow(+dV);

	ImplsRes->Act();
	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();

	return ret;
}

void TESt4R::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	inherited;
	ImplsRes = new TReservoir();
	ImplsRes->CreateCap(1);
	ImplsRes->CreatePress(BP);

	BrakeDelayFlag = bdelay_R;
}


//---EST3/AL2----------------------------------------------------------------------------------------------------------

double TESt3AL2::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp, ret;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = ImplsRes->P();
	CVP = CntrlRes->P() - 0.0;

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(BCP, dV1);
	CheckReleaser(dt);

	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	temp = CVs(BCP);
	dV = PF(CVP, VVP, 0.0015*temp)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;

	//luzowanie KI
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.00017*(1.37 - byte(BrakeDelayFlag = bdelay_G)))*dt;
	else dV = 0;
	ImplsRes->Flow(-dV);
	//przeplyw ZP <-> KI
	if (((BrakeStatus & b_on) == b_on) && (BCP < MaxBP))
		dV = PF(BVP, BCP, 0.0008*(1 + byte((BCP<0.58) && (BrakeDelayFlag = bdelay_G)))*(1.13 - byte((BCP>0.6) && (BrakeDelayFlag = bdelay_G))))*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	ImplsRes->Flow(-dV);
	//przeplyw ZP <-> rozdzielacz
	temp = BVs(BCP);
	if (VVP - 0.05>BVP)
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01)*dt;
	ValveRes->Flow(-dV);

	ret = dV - dV1;

	//luzowanie CH
	if ((BrakeCyl->P() > ImplsRes->P() * LoadC + 0.005) || (ImplsRes->P() < 0.15))
		dV = PF(0, BrakeCyl->P(), 0.015*SizeBC)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> CH
	if ((BrakeCyl->P() < ImplsRes->P() * LoadC - 0.005) && (ImplsRes->P() > 0.15))
		dV = PF(BVP, BrakeCyl->P(), 0.020*SizeBC)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);

	ImplsRes->Act();
	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();
	
	return ret;
}

void TESt3AL2::PLC(double mass)
{
	LoadC = 1 + byte(mass < LoadM)*((TareBP + (MaxBP - TareBP)*(mass - TareM) / (LoadM - TareM)) / MaxBP - 1);
}

void TESt3AL2::SetLP(double TM, double LM, double TBP)
{
TareM = TM;
LoadM = LM;
TareBP = TBP;
}

void TESt3AL2::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
 inherited;
 ImplsRes = new TReservoir();
 ImplsRes->CreateCap(1);
 ImplsRes->CreatePress(BP);
}


//---LSt---------------------------------------------------------------------------------------------------------------

double TLSt::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp, ret;
	double VVP, BVP, BCP, CVP;


	// ValveRes.CreatePress(LBP);
	// LBP:=0;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = ImplsRes->P();
	CVP = CntrlRes->P();

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	if (((BrakeStatus & b_rls) == b_rls))
		if (CVP < 1 * 0)
			BrakeStatus = BrakeStatus & 247;
		else
		{         //008
			dV = PF1(CVP, BCP, 0.024)*dt;
			CntrlRes->Flow(+dV);
			//     dV1:=+dV; //minus potem jest
			//     ImplsRes.Flow(-dV1);
		}

	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	if ((CVP - BCP)*BVM > 0.5)
		temp = 0;
	else
		if (VVP > CVP + 0.4)
			temp = 0.5;
		else
			temp = 0.5;

	dV = PF1(CVP, VVP, 0.0015*temp / 1.8 / 2)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;


	//luzowanie KI  {G}
	//   if VVP>BCP then
	//    dV:=PF(VVP,BCP,0.00004)*dt
	//   else if (CVP-BCP)<1.5 then
	//    dV:=PF(VVP,BCP,0.00020*(1.33-Byte((CVP-BCP)*BVM>0.65)))*dt
	//  else dV:=0;      0.00025 P
	//P
	if (VVP > BCP)
		dV = PF(VVP, BCP, 0.00043*(1.5 - byte(((CVP - BCP)*BVM > 1) && (BrakeDelayFlag = bdelay_G))), 0.1)*dt;
	else if ((CVP - BCP) < 1.5)
		dV = PF(VVP, BCP, 0.001472*(1.36 - byte(((CVP - BCP)*BVM > 1) && (BrakeDelayFlag = bdelay_G))), 0.1)*dt;
	else dV = 0;

	ImplsRes->Flow(-dV);
	ValveRes->Flow(+dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01, 0.1)*dt;
	ValveRes->Flow(-dV);

	ret = (dV - dV1);

	//  if Vel>55 then temp:=0.72 else
	//    temp:=1;{R}
	//cisnienie PP
	RapidTemp = RapidTemp + (RM*byte((Vel > 55) && (BrakeDelayFlag = bdelay_R)) - RapidTemp)*dt / 2;
	temp = 1 - RapidTemp;
	if (EDFlag > 0.2) temp = 10000;

	//powtarzacz — podwojny zawor zwrotny
	temp = Max0R(((CVP - BCP)*BVM + ASBP*byte((BrakeStatus& b_asb) == b_asb)) / temp, LBP);
	//luzowanie CH
	if ((BrakeCyl->P() > temp + 0.005) || (temp < 0.28))
		//   dV:=PF(0,BrakeCyl.P,0.0015*3*sizeBC)*dt
		//   dV:=PF(0,BrakeCyl.P,0.005*3*sizeBC)*dt
		dV = PFVd(BrakeCyl->P(), 0, 0.005 * 7 * SizeBC, temp)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);
	//przeplyw ZP <-> CH
	if ((BrakeCyl->P() < temp - 0.005) && (temp > 0.29))
		//   dV:=PF(BVP,BrakeCyl.P,0.002*3*sizeBC*2)*dt
		dV = -PFVa(BVP, BrakeCyl->P(), 0.002 * 7 * SizeBC * 2, temp)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);

	ImplsRes->Act();
	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();
	//  LBP:=ValveRes.P;
	//  ValveRes.CreatePress(ImplsRes.P);

	return ret;
}

void TLSt::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	inherited;
	ValveRes->CreateCap(1);
	ImplsRes->CreateCap(8);
	ImplsRes->CreatePress(PP);
	BrakeRes->CreatePress(8);
	ValveRes->CreatePress(PP);

	EDFlag = 0;

	BrakeDelayFlag = BDF;
}

void TLSt::SetLBP(double P)
{
 LBP = P;
}

double TLSt::GetEDBCP()
{
	double CVP, BCP;

	CVP = CntrlRes->P();
	BCP = ImplsRes->P();
	return (CVP - BCP)*BVM;
}

void TLSt::SetED(double EDstate)
{
 EDFlag = EDstate;
}

void TLSt::SetRM(double RMR)
{
 RM = 1 - RMR;
}

double TLSt::GetHPFlow(double HP, double dt)
{
 double dV;

 dV = Min0R(PF(HP, BrakeRes->P(), 0.01*dt), 0);
 BrakeRes->Flow(-dV);
 return dV;
}



//---EStED-------------------------------------------------------------------------------------------------------------

double TEStED::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp;
	double VVP, BVP, BCP, CVP, MPP, nastG;
	//byte i;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = ImplsRes->P();
	CVP = CntrlRes->P() - 0.0;
	MPP = Miedzypoj->P();
	dV1 = 0;

	nastG = (BrakeDelayFlag & bdelay_G);

	//sprawdzanie stanu
	if ((BCP<0.25) && (VVP + 0.08 > CVP)) Przys_blok = false;

	//sprawdzanie stanu
	if ((VVP + 0.002 + BCP / BVM < CVP - 0.05) && (Przys_blok))
		BrakeStatus = (BrakeStatus | 3); //hamowanie stopniowe
	else if ((VVP - 0.002 + (BCP - 0.1) / BVM > CVP - 0.05))
		BrakeStatus = (BrakeStatus & 252); //luzowanie
	else if (VVP + BCP / BVM > CVP - 0.05)
		BrakeStatus = (BrakeStatus & 253); //zatrzymanie napelaniania
	else if ((VVP + (BCP - 0.1) / BVM<CVP - 0.05) && (BCP>0.25)) //zatrzymanie luzowania
		BrakeStatus = (BrakeStatus | 1);

	if ((VVP + 0.10 < CVP) && (BCP < 0.25))    //poczatek hamowania
		if (!Przys_blok)
		{
			ValveRes->CreatePress(0.75*VVP);
			SoundFlag = SoundFlag | sf_Acc;
			ValveRes->Act();
			Przys_blok = true;
		}


	if (BCP > 0.5)
		Zamykajacy = true;
	else if (VVP - 0.6 < MPP)
		Zamykajacy = false;

	if ((BrakeStatus & b_rls) == b_rls)
	{
		dV = PF(CVP, BCP, 0.024)*dt;
		CntrlRes->Flow(+dV);
	}

	//luzowanie
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, Nozzles[3] * nastG + (1 - nastG)*Nozzles[1])*dt;
	else dV = 0;
	ImplsRes->Flow(-dV);
	if (((BrakeStatus & b_on) == b_on) && (BCP < MaxBP))
		dV = PF(BVP, BCP, Nozzles[2] * (nastG + 2 * byte(BCP < 0.8)) + Nozzles[0] * (1 - nastG))*dt;
	else dV = 0;
	ImplsRes->Flow(-dV);
	BrakeRes->Flow(dV);

	//przeplyw testowy miedzypojemnosci
	if (MPP < CVP - 0.3)
		temp = Nozzles[4];
	else
		if (BCP < 0.5)
			if (Zamykajacy)
				temp = Nozzles[8];  //1.25
			else
				temp = Nozzles[7];
		else
			temp = 0;
	dV = PF(MPP, VVP, temp);

	if (MPP<CVP - 0.17)
		temp = 0;
	else
		if (MPP>CVP - 0.08)
			temp = Nozzles[5];
		else
			temp = Nozzles[6];
	dV = dV + PF(MPP, CVP, temp);

	if (MPP - 0.05>BVP)
		dV = dV + PF(MPP - 0.05, BVP, Nozzles[10] * nastG + (1 - nastG)*Nozzles[9]);
	if (MPP > VVP) dV = dV + PF(MPP, VVP, 0.02);
	Miedzypoj->Flow(dV*dt*0.15);


	RapidTemp = RapidTemp + (RM*byte((Vel > 55) && (BrakeDelayFlag == bdelay_R)) - RapidTemp)*dt / 2;
	temp = 1 - RapidTemp;
	//  if EDFlag then temp:=1000;
	//  temp:=temp/(1-);

	//powtarzacz — podwojny zawor zwrotny
	temp = Max0R(BCP / temp*Min0R(Max0R(1 - EDFlag, 0), 1), LBP);

	if (BrakeCyl->P() > temp)
		dV = -PFVd(BrakeCyl->P(), 0, 0.02, temp)*dt;
	else
		if (BrakeCyl->P() < temp)
			dV = PFVa(BVP, BrakeCyl->P(), 0.02, temp)*dt;
		else dV = 0;

		BrakeCyl->Flow(dV);
		if (dV > 0)
			BrakeRes->Flow(-dV);


		//przeplyw ZS <-> PG
		if (MPP<CVP - 0.17)
			temp = 0;
		else
			if (MPP>CVP - 0.08)
				temp = Nozzles[5];
			else
				temp = Nozzles[6];
		dV = PF(CVP, MPP, temp)*dt;
		CntrlRes->Flow(+dV);
		ValveRes->Flow(-0.02*dV);
		dV1 = dV1 + 0.98*dV;

		//przeplyw ZP <-> MPJ
		if (MPP - 0.05 > BVP)
			dV = PF(BVP, MPP - 0.05, Nozzles[10] * nastG + (1 - nastG)*Nozzles[9])*dt;
		else dV = 0;
		BrakeRes->Flow(dV);
		dV1 = dV1 + dV*0.98;
		ValveRes->Flow(-0.02*dV);
		//przeplyw PG <-> rozdzielacz
		dV = PF(PP, VVP, 0.005)*dt;    //0.01
		ValveRes->Flow(-dV);


		ValveRes->Act();
		BrakeCyl->Act();
		BrakeRes->Act();
		CntrlRes->Act();
		Miedzypoj->Act();
		ImplsRes->Act();

		return (dV - dV1);
}

void TEStED::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	int i;

	inherited;

	ValveRes->CreatePress(1 * PP);
	BrakeCyl->CreatePress(1 * BP);

	//  CntrlRes:=TReservoir.Create;
	//  CntrlRes.CreateCap(15);
	//  CntrlRes.CreatePress(1*HPP);

	BrakeStatus = byte(BP > 1) * 1;
	Miedzypoj = new TReservoir();
	Miedzypoj->CreateCap(5);
	Miedzypoj->CreatePress(PP);

	ImplsRes->CreateCap(1);
	ImplsRes->CreatePress(BP);

	BVM = 1 / (HPP - 0.05 - LPP)*MaxBP;

	BrakeDelayFlag = BDF;
	Zamykajacy = false;
	EDFlag = 0;

	Nozzles[0] = 1.250 / 1.7;
	Nozzles[1] = 0.907;
	Nozzles[2] = 0.510 / 1.7;
	Nozzles[3] = 0.524 / 1.17;
	Nozzles[4] = 7.4;
	Nozzles[7] = 5.3;
	Nozzles[8] = 2.5;
	Nozzles[9] = 7.28;
	Nozzles[10] = 2.96;
	Nozzles[5] = 1.1;
	Nozzles[6] = 0.9;

	for (i = 0; i < 10; i++)
	{
		Nozzles[i] = Nozzles[i] * Nozzles[i] * 3.14159 / 4000;
	}

}

double TEStED::GetEDBCP()
{
	return ImplsRes->P();
}



//---DAKO CV1-----------------------------------------------------------------------------------------------------------

void TCV1::CheckState(double BCP, double &dV1)
{
	double VVP, BVP, CVP;

	BVP = BrakeRes->P();
	VVP = Min0R(ValveRes->P(), BVP + 0.05);
	CVP = CntrlRes->P() - 0.0;

	//odluzniacz
	if (((BrakeStatus & b_rls) == b_rls) && (CVP - VVP < 0))
		BrakeStatus = (BrakeStatus & 247);

	//sprawdzanie stanu
	if ((BrakeStatus & 1) == 1)
		if (VVP + 0.003 + BCP / BVM < CVP)
			BrakeStatus = (BrakeStatus | 2); //hamowanie stopniowe
		else if (VVP - 0.003 + BCP / BVM > CVP)
			BrakeStatus = (BrakeStatus & 252); //luzowanie
		else if (VVP + BCP / BVM > CVP)
			BrakeStatus = (BrakeStatus & 253); //zatrzymanie napelaniania
		else;
	else
		if ((VVP + 0.10 < CVP) && (BCP < 0.1))   //poczatek hamowania
		{
			BrakeStatus = (BrakeStatus | 3);
			dV1 = 1.25;
		}
		else if ((VVP + BCP / BVM<CVP) && (BCP>0.25))  //zatrzymanie luzowanie
			BrakeStatus = (BrakeStatus | 1);
}

double TCV1::CVs(double bp)
{
	//przeplyw ZS <-> PG
	if (bp > 0.05)
		return 0.0;
	else
		return 0.23;
}

double TCV1::BVs(double BCP)
{
	double VVP, BVP, CVP;

	BVP = BrakeRes->P();
	CVP = CntrlRes->P();
	VVP = ValveRes->P();

	//przeplyw ZP <-> rozdzielacz
	if (BVP < CVP - 0.1)
		return 1;
	else
		if (BCP > 0.05)
			return 0.0;
		else
			return 0.2*(1.5 - byte(BVP > VVP));
}

double TCV1::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = Min0R(ValveRes->P(), BVP + 0.05);
	BCP = BrakeCyl->P();
	CVP = CntrlRes->P() - 0.0;

dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(BCP, dV1);

	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	temp = CVs(BCP);
	dV = PF(CVP, VVP, 0.0015*temp)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;


	//luzowanie
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.0042*(1.37 - byte(BrakeDelayFlag = bdelay_G))*SizeBC)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> silowniki
	if ((BrakeStatus & b_on) == b_on)
		dV = PF(BVP, BCP, 0.017*(1 + byte((BCP<0.58) && (BrakeDelayFlag = bdelay_G)))*(1.13 - byte((BCP>0.6) && (BrakeDelayFlag = bdelay_G)))*SizeBC)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> rozdzielacz
	temp = BVs(BCP);
	if (VVP + 0.05 > BVP)
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01)*dt;
	ValveRes->Flow(-dV);

	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();

	return dV - dV1;
}

void TCV1::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	ValveRes->CreatePress(PP);
	BrakeCyl->CreatePress(BP);
	BrakeRes->CreatePress(PP);
	CntrlRes = new TReservoir();
	CntrlRes->CreateCap(15);
	CntrlRes->CreatePress(HPP);
	BrakeStatus = 0;

	BVM = 1 / (HPP - LPP)*MaxBP;

	BrakeDelayFlag = BDF;
}

double TCV1::GetCRP()
{
 return CntrlRes->P();
}



//---CV1-L-TR----------------------------------------------------------------------------------------------------------

void TCV1L_TR::SetLBP(double P)
{
	LBP = P;
}

double TCV1L_TR::GetHPFlow(double HP, double dt)
{
	double dV;

	dV = PF(HP, BrakeRes->P(), 0.01)*dt;
	dV = Min0R(0, dV);
	BrakeRes->Flow(-dV);

	return dV;
}

void TCV1L_TR::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	inherited;
    ImplsRes = new TReservoir();
	ImplsRes->CreateCap(2.5);
	ImplsRes->CreatePress(BP);
}

double TCV1L_TR::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp, ret;
	double VVP, BVP, BCP, CVP;

	BVP = BrakeRes->P();
	VVP = Min0R(ValveRes->P(), BVP + 0.05);
	BCP = ImplsRes->P();
	CVP = CntrlRes->P() - 0.0;

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(BCP, dV1);

	VVP = ValveRes->P();
	//przeplyw ZS <-> PG
	temp = CVs(BCP);
	dV = PF(CVP, VVP, 0.0015*temp)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;


	//luzowanie KI
	if ((BrakeStatus & b_hld) == b_off)
		dV = PF(0, BCP, 0.000425*(1.37 - byte(BrakeDelayFlag = bdelay_G)))*dt;
	else dV = 0;
	ImplsRes->Flow(-dV);
	//przeplyw ZP <-> KI
	if (((BrakeStatus & b_on) == b_on) && (BCP < MaxBP))
		dV = PF(BVP, BCP, 0.002*(1 + byte((BCP<0.58) && (BrakeDelayFlag = bdelay_G)))*(1.13 - byte((BCP>0.6) && (BrakeDelayFlag = bdelay_G))))*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	ImplsRes->Flow(-dV);

	//przeplyw ZP <-> rozdzielacz
	temp = BVs(BCP);
	if (VVP + 0.05>BVP)
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01)*dt;

	ret = dV - dV1;

	temp = Max0R(BCP, LBP);

	//luzowanie CH
	if ((BrakeCyl->P() > temp + 0.005) || (Max0R(ImplsRes->P(), 8 * LBP) < 0.25))
		dV = PF(0, BrakeCyl->P(), 0.015*SizeBC)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);

	//przeplyw ZP <-> CH
	if ((BrakeCyl->P() < temp - 0.005) && (Max0R(ImplsRes->P(), 8 * LBP) > 0.3) && (Max0R(BCP, LBP) < MaxBP))
		dV = PF(BVP, BrakeCyl->P(), 0.020*SizeBC)*dt;
	else dV = 0;

	BrakeRes->Flow(dV);
	BrakeCyl->Flow(-dV);

	ImplsRes->Act();
	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();

	return ret;
}



//--- KNORR KE --------------------------------------------------------------------------------------------------------
void TKE::CheckReleaser(double dt)
{
	double VVP, CVP;

	VVP = ValveRes->P();
	CVP = CntrlRes->P();

	//odluzniacz
	if ((BrakeStatus & b_rls) == b_rls)
		if (CVP - VVP < 0)
			BrakeStatus = BrakeStatus & 247;
		else
		{
			CntrlRes->Flow(+PF(CVP, 0, 0.1)*dt);
		}
}

void TKE::CheckState(double BCP, double &dV1)
{
	double VVP, BVP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	CVP = CntrlRes->P();

	//sprawdzanie stanu
	if ((BrakeStatus & 1) == 1)
		if (VVP + 0.003 + BCP / BVM < CVP)
			BrakeStatus = (BrakeStatus | 2); //hamowanie stopniowe
		else if (VVP - 0.003 + BCP / BVM > CVP)
			BrakeStatus = (BrakeStatus & 252); //luzowanie
		else if (VVP + BCP / BVM > CVP)
			BrakeStatus = (BrakeStatus & 253); //zatrzymanie napelaniania
		else;
	else
		if ((VVP + 0.10 < CVP) & (BCP < 0.1))    //poczatek hamowania
		{
			BrakeStatus = (BrakeStatus | 3);
			ValveRes->CreatePress(0.8*VVP); //przyspieszacz
		}
		else if ((VVP + BCP / BVM<CVP) && ((CVP - VVP)*BVM>0.25)) //zatrzymanie luzowanie
			BrakeStatus = (BrakeStatus | 1);
}

double TKE::CVs(double bp)
{
	double VVP, BVP, CVP;

	BVP = BrakeRes->P();
	CVP = CntrlRes->P();
	VVP = ValveRes->P();

	//przeplyw ZS <-> PG
	if (bp > 0.2)
		return 0.0;
	else
		if (VVP > CVP + 0.4)
			return 0.05;
		else
			return 0.23;
}

double TKE::BVs(double BCP)
{
	double VVP, BVP, CVP;

	BVP = BrakeRes->P();
	CVP = CntrlRes->P();
	VVP = ValveRes->P();

	//przeplyw ZP <-> rozdzielacz
	if (BVP > VVP)
		return 0.0;
	else
		if (BVP < CVP - 0.3)
			return 0.6;
		else
			return 0.13;
}


template <class DstType, class SrcType>
bool Is(const SrcType* src)
{
	return dynamic_cast<const DstType*>(src) != 0;
}

double TKE::GetPF(double PP, double dt, double Vel)
{
	double dV, dV1, temp;
	double VVP, BVP, BCP, IMP, CVP;

	BVP = BrakeRes->P();
	VVP = ValveRes->P();
	BCP = BrakeCyl->P();
	IMP = ImplsRes->P();
	CVP = CntrlRes->P();

	dV = 0; dV1 = 0;

	//sprawdzanie stanu
	CheckState(IMP, dV1);
	CheckReleaser(dt);

	//przeplyw ZS <-> PG
	temp = CVs(IMP);
	dV = PF(CVP, VVP, 0.0015*temp)*dt;
	CntrlRes->Flow(+dV);
	ValveRes->Flow(-0.04*dV);
	dV1 = dV1 - 0.96*dV;

	//luzowanie
	if ((BrakeStatus & b_hld) == b_off)
	{
		if ((BrakeDelayFlag & bdelay_G) == 0)
			temp = 0.283 + 0.139;
		else
			temp = 0.139;
		dV = PF(0, IMP, 0.001*temp)*dt;
	}
	else dV = 0;
	ImplsRes->Flow(-dV);

	//przeplyw ZP <-> silowniki
	if (((BrakeStatus & b_on) == b_on) && (IMP < MaxBP))
	{
		temp = 0.113;
		if ((BrakeDelayFlag & bdelay_G) == 0)
			temp = temp + 0.636;
		if (BCP < 0.5)
			temp = temp + 0.785;
		dV = PF(BVP, IMP, 0.001*temp)*dt;
	}
	else dV = 0;
	BrakeRes->Flow(dV);
	ImplsRes->Flow(-dV);

	
	//rapid
	if (!((typeid(FM) == typeid(TDisk1)) || (typeid(FM) == typeid(TDisk2))))   //jesli zeliwo to schodz //Q: typeid DO POROWNANIA KLAS, BEDZIE OK?
		RapidStatus = ((BrakeDelayFlag & bdelay_R) == bdelay_R) && (((Vel > 50) && (RapidStatus)) || (Vel > 70));
	else                         //jesli tarczowki, to zostan
		RapidStatus = ((BrakeDelayFlag & bdelay_R) == bdelay_R);

	//  temp:=1.9-0.9*Byte(RapidStatus);

	if (RM*RM > 0.1) //jesli jest rapid
		if (RM > 0) //jesli dodatni (naddatek);
			temp = 1 - RM*byte(RapidStatus);
		else
			temp = 1 - RM*(1 - byte(RapidStatus));
	else
		temp = 1;
	temp = temp / LoadC;
	//luzowanie CH
	//  temp:=Max0R(BCP,LBP);
	IMP = Max0R(IMP / temp, Max0R(LBP, ASBP*byte((BrakeStatus & b_asb) == b_asb)));

	//luzowanie CH
	if ((BCP > IMP + 0.005) || (Max0R(ImplsRes->P(), 8 * LBP) < 0.25))
		dV = PFVd(BCP, 0, 0.05, IMP)*dt;
	else dV = 0;
	BrakeCyl->Flow(-dV);
	if ((BCP<IMP - 0.005) && (Max0R(ImplsRes->P(), 8 * LBP)>0.3))
		dV = PFVa(BVP, BCP, 0.05, IMP)*dt;
	else dV = 0;
	BrakeRes->Flow(-dV);
	BrakeCyl->Flow(+dV);

	//przeplyw ZP <-> rozdzielacz
	temp = BVs(IMP);
	//  if(BrakeStatus and b_hld)=b_off then
	if ((IMP<0.25) || (VVP + 0.05>BVP))
		dV = PF(BVP, VVP, 0.02*SizeBR*temp / 1.87)*dt;
	else dV = 0;
	BrakeRes->Flow(dV);
	dV1 = dV1 + dV*0.96;
	ValveRes->Flow(-0.04*dV);
	//przeplyw PG <-> rozdzielacz
	dV = PF(PP, VVP, 0.01)*dt;
	ValveRes->Flow(-dV);

	ValveRes->Act();
	BrakeCyl->Act();
	BrakeRes->Act();
	CntrlRes->Act();
	ImplsRes->Act();
	
	return (dV - dV1);
}

void TKE::Init(double PP, double HPP, double LPP, double BP, byte BDF)
{
	ValveRes->CreatePress(PP);
	BrakeCyl->CreatePress(BP);
	BrakeRes->CreatePress(PP);

	CntrlRes = new TReservoir(); //komora sterujaca
	CntrlRes->CreateCap(5);
	CntrlRes->CreatePress(HPP);

	ImplsRes = new TReservoir(); //komora zastepcza silownika
	ImplsRes->CreateCap(1);
	ImplsRes->CreatePress(BP);

	BrakeStatus = 0;

	BVM = 1 / (HPP - LPP)*MaxBP;

    BrakeDelayFlag = BDF;
}

double TKE::GetCRP()
{
 return CntrlRes->P();
}

double TKE::GetHPFlow(double HP, double dt)
{
	double dV;

	dV = PF(HP, BrakeRes->P(), 0.01)*dt;
	dV = Min0R(0, dV);
	BrakeRes->Flow(-dV);
	return dV;
}

void TKE::PLC(double mass)
{
	LoadC = 1 + byte(mass < LoadM)*((TareBP + (MaxBP - TareBP)*(mass - TareM) / (LoadM - TareM)) / MaxBP - 1);
}

void TKE::SetLP(double TM, double LM, double TBP)
{
TareM = TM;
LoadM = LM;
TareBP = TBP;
}

void TKE::SetRM(double RMR)
{
	RM = 1 - RMR;
}

void TKE::SetLBP(double P)
{
	LBP = P;
}



//---KRANY-------------------------------------------------------------------------------------------------------------

double THandle::GetPF(double i_bcp, double pp, double hp, double dt, double ep)
{
	return 0.0;
}

void THandle::Init(double press)
{
	Time = false;
	TimeEP = false;
}

void THandle::SetReductor(double nAdj)
{

}

double THandle::GetCP()
{
	return 0.0;
}

double THandle::GetSound(byte i)
{
	return 0.0;
}

double THandle::GetPos(byte i)
{
	return 0.0;
}


double qround(double x, int n) {
	x = x * pow(10, n);
	x = x + 0.5;
	x = floor(x) / pow(10, n);

	return x;
}



//---FV4a--------------------------------------------------------------------------------------------------------------

double TFV4a::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	const int LBDelay = 100;

	double LimPP, dpPipe, dpMainValve, ActFlowSpeed;

	ep = pp; //SPKS!!
	LimPP = Min0R(BPT[int(round(i_bcp))][1], HP);
	ActFlowSpeed = BPT[int(round(i_bcp))][0];

	if (i_bcp == i_bcpno) LimPP = 2.9;

	CP = CP + 20 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt / 1;
	RP = RP + 20 * Min0R(abs(ep - RP), 0.05)*PR(RP, ep)*dt / 2.5;


	LimPP = CP;
	dpPipe = Min0R(HP, LimPP);

	dpMainValve = PF(dpPipe, pp, ActFlowSpeed / (LBDelay))*dt;
	if (CP > RP + 0.05)
		dpMainValve = PF(Min0R(CP + 0.1, HP), pp, 1.1*(ActFlowSpeed) / (LBDelay))*dt;
	if (CP < RP - 0.05)
		dpMainValve = PF(CP - 0.1, pp, 1.1*(ActFlowSpeed) / (LBDelay))*dt;

	if (glm::round(i_bcp) == -1)
	{
		CP = CP + 5 * Min0R(abs(LimPP - CP), 0.2)*PR(CP, LimPP)*dt / 2;
		if (CP < RP + 0.03)
			if (TP < 5) TP = TP + dt;
		//            if(cp+0.03<5.4)then
		if ((RP + 0.03 < 5.4) || (CP + 0.03 < 5.4)) //fala
			dpMainValve = PF(Min0R(HP, 17.1), pp, (ActFlowSpeed) / (LBDelay))*dt;
		//              dpMainValve:=20*Min0R(abs(ep-7.1),0.05)*PF(HP,pp,(ActFlowSpeed)/(LBDelay))*dt
		else
		{
			RP = 5.45;
			if (CP < pp - 0.01)                 //: /34*9
				dpMainValve = PF(dpPipe, pp, (ActFlowSpeed) / 34 * 9 / (LBDelay))*dt;
			else
				dpMainValve = PF(dpPipe, pp, (ActFlowSpeed) / (LBDelay))*dt;
		}
	}

	if (glm::round(i_bcp) == 0)
	{
		if (TP > 0.1)
		{
			CP = 5 + (TP - 0.1)*0.08;
			TP = TP - dt / 12 / 2;
		}
		if ((CP>RP + 0.1) && (CP <= 5))
			dpMainValve = PF(Min0R(CP + 0.25, HP), pp, 2 * (ActFlowSpeed) / (LBDelay))*dt;
		else
			if (CP > 5)
				dpMainValve = PF(Min0R(CP, HP), pp, 2 * (ActFlowSpeed) / (LBDelay))*dt;
			else
				dpMainValve = PF(dpPipe, pp, (ActFlowSpeed) / (LBDelay))*dt;
	}

	if (glm::round(i_bcp) == i_bcpno)
	{
		dpMainValve = PF(0, pp, (ActFlowSpeed) / (LBDelay))*dt;
	}

	return dpMainValve;
}

void TFV4a::Init(double press)
{
	CP = press;
	TP = 0;
	RP = press;
	Time = false;
	TimeEP = false;
}

double LPP_RP(double pos) //cisnienie z zaokraglonej pozycji;
{
	int i_pos;

	i_pos = glm::round(pos - 0.5); //zaokraglone w dol
	return BPT[i_pos][1] + (BPT[i_pos + 1][1] - BPT[i_pos][1])*(pos - i_pos); //interpolacja liniowa
}

bool EQ(double pos, double i_pos)
{
	return ((pos <= i_pos + 0.5) && (pos > i_pos - 0.5));
}



//---FV4a/M--- nowonapisany kran bez poprawki IC ----------------------------------------------------------------------

double TFV4aM::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	const int LBDelay = 100;
	const double xpM = 0.3; //mnoznik membrany komory pod

	double LimPP, dpPipe, dpMainValve, ActFlowSpeed, dp;
	double pom;
	byte i;

	ep = pp / 2 * 1.5 + ep / 2 * 0.5; //SPKS!!
	//          ep:=pp;
	//          ep:=cp/3+pp/3+ep/3;
	//          ep:=cp;

	for (i = 0; i < 4; i++)
		Sounds[i] = 0;
	dp = 0;

	i_bcp = Max0R(Min0R(i_bcp, 5.999), -1.999); //na wszelki wypadek, zeby nie wyszlo poza zakres

	if (TP > 0)
	{  //jesli czasowy jest niepusty
		//            dp:=0.07; //od cisnienia 5 do 0 w 60 sekund ((5-0)*dt/75)
		dp = 0.045;  //2.5 w 55 sekund (5,35->5,15 w PG)
		TP = TP - dp*dt;
		Sounds[s_fv4a_t] = dp;
	}
	else       //.08
	{
		TP = 0;
	}

	if (XP > 0) //jesli komora pod niepusta jest niepusty
	{
		dp = 2.5;
		Sounds[s_fv4a_x] = dp*XP;
		XP = XP - dt*dp * 2; //od cisnienia 5 do 0 w 10 sekund ((5-0)*dt/10)
	}
	else       //.75
		XP = 0;         //jak pusty, to pusty

	LimPP = Min0R(LPP_RP(i_bcp) + TP*0.08 + RedAdj, HP); //pozycja + czasowy lub zasilanie
	ActFlowSpeed = BPT[int(round(i_bcp))][0];

	if (EQ(i_bcp, -1)) pom = Min0R(HP, 5.4 + RedAdj); else pom = Min0R(CP, HP);

	if (pom > RP + 0.25) Fala = true;
	if (Fala)
		if (pom > RP + 0.3)
			//              if(ep>rp+0.11)then
			XP = XP - 20 * PR(pom, XP)*dt;
	//              else
	//                xp:=xp-16*(ep-(ep+0.01))/(0.1)*PR(ep,xp)*dt
		else Fala = false;

		if (LimPP > CP) //podwyzszanie szybkie
			CP = CP + 5 * 60 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt; //zbiornik sterujacy
		else
			CP = CP + 13 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt; //zbiornik sterujacy

		LimPP = pom; //cp
		dpPipe = Min0R(HP, LimPP + XP*xpM);

		if (dpPipe > pp)
			dpMainValve = -PFVa(HP, pp, ActFlowSpeed / (LBDelay), dpPipe, 0.4);
		else
			dpMainValve = PFVd(pp, 0, ActFlowSpeed / (LBDelay), dpPipe, 0.4);

		if (EQ(i_bcp, -1))
		{
			if (TP < 5) TP = TP + dt; //5/10
			if (TP < 1) TP = TP - 0.5*dt; //5/10            
			//            dpMainValve:=dpMainValve*2;//+1*PF(dpPipe,pp,(ActFlowSpeed)/(LBDelay))//coby nie przeszkadzal przy ladowaniu z zaworu obok
		}

		if (EQ(i_bcp, 0))
		{
			if (TP > 2)
				dpMainValve = dpMainValve*1.5;//+0.5*PF(dpPipe,pp,(ActFlowSpeed)/(LBDelay))//coby nie przeszkadzal przy ladowaniu z zaworu obok
		}





		ep = dpPipe;
		if (EQ(i_bcp, 0) || (RP>ep))
			RP = RP + PF(RP, ep, 0.0007)*dt; //powolne wzrastanie, ale szybsze na jezdzie
		else
			RP = RP + PF(RP, ep, 0.000093 / 2 * 2)*dt; //powolne wzrastanie i to bardzo  //jednak trzeba wydluzyc, bo obecnie zle dziala
		if ((RP < ep) && (RP < BPT[int(round(i_bcpno))][1])) //jesli jestesmy ponizej cisnienia w sterujacym (2.9 bar)
			RP = RP + PF(RP, CP, 0.005)*dt; //przypisz cisnienie w PG - wydluzanie napelniania o czas potrzebny do napelnienia PG

		if ((EQ(i_bcp, i_bcpno)) || (EQ(i_bcp, -2)))
		{
			dp = PF(0, pp, (ActFlowSpeed) / (LBDelay));
			dpMainValve = dp;
			Sounds[s_fv4a_e] = dp;
			Sounds[s_fv4a_u] = 0;
			Sounds[s_fv4a_b] = 0;
			Sounds[s_fv4a_x] = 0;
		}
		else
		{
			if (dpMainValve > 0)
				Sounds[s_fv4a_b] = dpMainValve;
			else
				Sounds[s_fv4a_u] = -dpMainValve;
		}

		return dpMainValve*dt;
}

void TFV4aM::Init(double press)
{
	CP = press;
	TP = 0;
	RP = press;
	XP = 0;
	Time = false;
	TimeEP = false;
}

void TFV4aM::SetReductor(double nAdj)
{
	RedAdj = nAdj;
}

double TFV4aM::GetSound(byte i)
{
	if (i > 4)
		return 0;
	else
		return Sounds[i];
}

double TFV4aM::GetPos(byte i)
{
  const
 double table[11] = { -2, 6, -1, 0, -2, 1, 4, 6, 0, 0, 0 };

  return table[i];
}



//---M394--- Matrosow -------------------------------------------------------------------------------------------------

double TM394::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	const int LBDelay = 65;

	double LimPP, dpPipe, dpMainValve, ActFlowSpeed;
	int bcp;

	bcp = round(i_bcp);
	if (bcp < -1) bcp = 1;

	LimPP = Min0R(BPT_394[bcp][1], HP);

	ActFlowSpeed = BPT_394[bcp][0];
	if ((bcp == 1) || (bcp == i_bcpno))
		LimPP = pp;
	if (bcp == 0)
		LimPP = LimPP + RedAdj;
	if (bcp != 2)
		if (CP < LimPP)
			CP = CP + 4 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt; //zbiornik sterujacy
	//      cp:=cp+6*(2+Byte(bcp<0))*Min0R(abs(Limpp-cp),0.05)*PR(cp,Limpp)*dt //zbiornik sterujacy
		else
			if (bcp == 0)
				CP = CP - 0.2*dt / 100;
			else
				CP = CP + 4 * (1 + byte(bcp != 3) + byte(bcp > 4))*Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt; //zbiornik sterujacy

	LimPP = CP;
	dpPipe = Min0R(HP, LimPP);

	//  if(dpPipe>pp)then //napelnianie
	//    dpMainValve:=PF(dpPipe,pp,ActFlowSpeed/(LBDelay))*dt
	//  else //spuszczanie
	dpMainValve = PF(dpPipe, pp, ActFlowSpeed / (LBDelay))*dt;

	if (bcp == -1)
		//           begin
		dpMainValve = PF(HP, pp, (ActFlowSpeed) / (LBDelay))*dt;
	//           end;

	if (bcp == i_bcpno)
		//           begin
		dpMainValve = PF(0, pp, (ActFlowSpeed) / (LBDelay))*dt;
	//           end;

	return dpMainValve;
}

void TM394::Init(double press)
{
	CP = press;
	RedAdj = 0;
	Time = true;
	TimeEP = false;
}

void TM394::SetReductor(double nAdj)
{
	RedAdj = nAdj;
}

double TM394::GetCP()
{
	return CP;
}

double TM394::GetPos(byte i)
{
	const double table[11] = { -1, 5, -1, 0, 1, 2, 4, 5, 0, 0, 0 };

	return table[i];
}



//---H14K1-- Knorr ----------------------------------------------------------------------------------------------------

double TH14K1::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	const int LBDelay = 100;                                    //szybkosc + zasilanie sterujacego
//BPT_K: array[-1..4] of array[0..1] of real = ((10, 0), (4, 1), (0, 1), (4, 0), (4, -1), (15, -1)); //Q: KTO PRZETLUMACZY? :)

	double BPT_K[6][2] ={
						{ 10, 0},   
						{  4, 1},  
						{  0, 1},  
						{  4, 0},
						{  4,-1},
						{ 15, 1}};

	const double NomPress = 5.0;

	double LimPP, dpMainValve, ActFlowSpeed;
	int bcp;

	bcp = round(i_bcp);
	if (i_bcp < -1) bcp = 1;
	LimPP = BPT_K[bcp][1];
	if (LimPP < 0)
		LimPP = 0.5*pp;
	else if (LimPP > 0)
		LimPP = pp;
	else
		LimPP = CP;
	ActFlowSpeed = BPT_K[bcp][0];

	CP = CP + 6 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt; //zbiornik sterujacy

	dpMainValve = 0;

	if (bcp == -1)
		dpMainValve = PF(HP, pp, (ActFlowSpeed) / (LBDelay))*dt;
	if (bcp == 0)
		dpMainValve = -PFVa(HP, pp, (ActFlowSpeed) / (LBDelay), NomPress + RedAdj)*dt;
	if ((bcp>1) && (pp > CP))
		dpMainValve = PFVd(pp, 0, (ActFlowSpeed) / (LBDelay), CP)*dt;
	if (bcp == i_bcpno)
		dpMainValve = PF(0, pp, (ActFlowSpeed) / (LBDelay))*dt;

	return dpMainValve;
}

void TH14K1::Init(double press)
{
	CP = press;
	RedAdj = 0;
	Time = true;
	TimeEP = true;
}

void TH14K1::SetReductor(double nAdj)
{
	RedAdj = nAdj;
}

double TH14K1::GetCP()
{
	return CP;
}

double TH14K1::GetPos(byte i)
{
	const double table[11] = { -1, 4, -1, 0, 1, 2, 3, 4, 0, 0, 0 };

	return table[i];
}



//---St113-- Knorr EP -------------------------------------------------------------------------------------------------

double TSt113::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	const
		int LBDelay = 100;                                    //szybkosc + zasilanie sterujacego

	const double BEP_K[7] = { 0, -1, 1, 0, 0, 0, 0 };
	const double BPT_K[6][2] = { { 10, 0 },
	{ 4, 1 },
	{ 0, 1 },
	{ 4, 0 },
	{ 4, -1 },
	{ 15, 1 } };

	const double NomPress = 5.0;
	double LimPP, dpMainValve, ActFlowSpeed;
	int bcp;

	bcp = round(i_bcp);

	EPS = BEP_K[bcp];

	if (bcp > 0)  bcp = bcp - 1;

	if (bcp < -1)  bcp = 1;
	LimPP = BPT_K[bcp][1];
	if (LimPP < 0)
		LimPP = 0.5*pp;
	else if (LimPP > 0)
		LimPP = pp;
	else
		LimPP = CP;
	ActFlowSpeed = BPT_K[bcp][0];

CP = CP + 6 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt; //zbiornik sterujacy

	dpMainValve = 0;

	if (bcp == -1)
		dpMainValve = PF(HP, pp, (ActFlowSpeed) / (LBDelay))*dt;
	if (bcp == 0)
		dpMainValve = -PFVa(HP, pp, (ActFlowSpeed) / (LBDelay), NomPress + RedAdj)*dt;
	if ((bcp>1) && (pp > CP))
		dpMainValve = PFVd(pp, 0, (ActFlowSpeed) / (LBDelay), CP)*dt;
	if (bcp == i_bcpno)
		dpMainValve = PF(0, pp, (ActFlowSpeed) / (LBDelay))*dt;

	return dpMainValve;
}

double TSt113::GetCP()
{
 return EPS;
}

double TSt113::GetPos(byte i)
{
	const double table[11] = { -1, 5, -1, 0, 2, 3, 4, 5, 0, 0, 1 };

	return table[i];
}

void TSt113::Init(double press)
{
 Time = true;
 TimeEP = true;
}



//--- test ------------------------------------------------------------------------------------------------------------

double Ttest::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	const int LBDelay = 100;

	double LimPP, dpPipe, dpMainValve, ActFlowSpeed;

	LimPP = BPT[int(round(i_bcp))][1];
	ActFlowSpeed = BPT[int(round(i_bcp))][0];

	if (i_bcp == i_bcpno) LimPP = 0.0;

	if (i_bcp == -1) LimPP = 7;

	CP = CP + 20 * Min0R(abs(LimPP - CP), 0.05)*PR(CP, LimPP)*dt / 1;

	LimPP = CP;
	dpPipe = Min0R(HP, LimPP);

	dpMainValve = PF(dpPipe, pp, ActFlowSpeed / (LBDelay))*dt;

	if (round(i_bcp) == i_bcpno)
	{
		dpMainValve = PF(0, pp, (ActFlowSpeed) / (LBDelay))*dt;
	}

	return dpMainValve;
}

void Ttest::Init(double press)
{
 CP = press;
}



//---FD1---------------------------------------------------------------------------------------------------------------

double TFD1::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	double dp, temp;

	//  MaxBP:=4;
	//  temp:=Min0R(i_bcp*MaxBP,Min0R(5.0,HP));
	temp = Min0R(i_bcp*MaxBP, HP);              //0011
	dp = 10 * Min0R(abs(temp - BP), 0.1)*PF(temp, BP, 0.0006*(2 + byte(temp > BP)))*dt;
	BP = BP - dp;
	return -dp;
}

void TFD1::Init(double press)
{
	BP = 0;
	MaxBP = press;
	Time = false;
	TimeEP = false;
}

double TFD1::GetCP()
{
	return BP;
}



//---KNORR-------------------------------------------------------------------------------------------------------------

double TH1405::GetPF(double i_bcp, double pp, double HP, double dt, double ep)
{
	double dp, temp, A;

	pp = Min0R(pp, MaxBP);
	if (i_bcp > 0.5)
	{
		temp = Min0R(MaxBP, HP);
		A = 2 * (i_bcp - 0.5)*0.0011;
		BP = Max0R(BP, pp);
	}
	else
	{
		temp = 0;
		A = 0.2*(0.5 - i_bcp)*0.0033;
		BP = Min0R(BP, pp);
	}
	dp = PF(temp, BP, A)*dt;
	BP = BP - dp;
 return -dp;
}

void TH1405::Init(double press)
{
 BP = 0;
 MaxBP = press;
 Time = true;
 TimeEP = false;
}

double TH1405::GetCP()
{
	return BP;
}



//---FVel6-------------------------------------------------------------------------------------------------------------

double TFVel6::GetPF(double i_bcp, double pp, double hp, double dt, double ep)
{
	const int LBDelay = 100;

	double LimPP, dpMainValve, ActFlowSpeed, ret;

	LimPP = Min0R(5 * byte(i_bcp<3.5), hp);
	if ((i_bcp >= 3.5) && ((i_bcp<4.3) || (i_bcp>5.5)))
		ActFlowSpeed = 0;
	else if ((i_bcp>4.3) && (i_bcp < 4.8))
		ActFlowSpeed = 4 * (i_bcp - 4.3);//konsultacje wawa1 - bylo 8
	else if (i_bcp < 4)
		ActFlowSpeed = 2;
	else
		ActFlowSpeed = 4;
	dpMainValve = PF(LimPP, pp, ActFlowSpeed / (LBDelay))*dt;


	Sounds[s_fv4a_e] = 0;
	Sounds[s_fv4a_u] = 0;
	Sounds[s_fv4a_b] = 0;
	if (i_bcp < 3.5) Sounds[s_fv4a_u] = -dpMainValve; else
		if (i_bcp < 4.8) Sounds[s_fv4a_b] = dpMainValve; else
			if (i_bcp < 5.5) Sounds[s_fv4a_e] = dpMainValve;

	ret = dpMainValve;

	if (i_bcp < -0.5)
		EPS = -1;
	else if ((i_bcp > 0.5) && (i_bcp < 4.7))
		EPS = 1;
	else
		EPS = 0;
	//    EPS:=i_bcp*Byte(i_bcp<2)

	return ret;
}

double TFVel6::GetCP()
{
 return EPS;
}

double TFVel6::GetPos(byte i)
{
	const double table[11] = { -1, 5, -1, 0, 6, 4, 4.7, 5, -1, 0, 1 };

	return table[i];
}

double TFVel6::GetSound(byte i)
{
	if (i > 2)
		return 0;
	else
 return Sounds[i];
}

void TFVel6::Init(double press)
{
	Time = true;
	TimeEP = true;
}