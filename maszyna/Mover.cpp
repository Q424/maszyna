//---------------------------------------------------------------------------

#include "Mover.h"
#include "hamulce.h"
//---------------------------------------------------------------------------

// Ra: tu nale¿y przenosiæ funcje z mover.pas, które nie s¹ z niego wywo³ywane.
// Jeœli jakieœ zmienne nie s¹ u¿ywane w mover.pas, te¿ mo¿na je przenosiæ.
// Przeniesienie wszystkiego na raz zrobi³o by zbyt wielki chaos do ogarniêcia.
using namespace std;

const double dEpsilon=0.01; //1cm (zale¿y od typu sprzêgu...)



//yB: funkcje dodatkowe hamulcowe
/*
double PR(double p1, double p2)
{
	double ph, pl;

	ph = Max0R(p1, p2) + 0.1;
	pl = p1 + p2 - ph + 0.2;
	return (ph - pl) / (1.13*ph - pl);
}

double SPR(double ph, double pl)
{
	return 0.674*(ph - pl) / (1.13*ph - pl + 0.013);
}

*/

bool TMoverParameters::SetInternalCommand(std::string NewCommand, double NewValue1, double NewValue2)
{

	return true;
}

bool __fastcall TMoverParameters::RunCommand(std::string command, double CValue1, double CValue2)
{

	return true;
}

// *************************************************************************************
// RunInternalCommand(void) - TRANSLATED
// *************************************************************************************

bool TMoverParameters::RunInternalCommand(void)
{
	bool OK;

	if (CommandIn.Command != "")
	{
		OK = RunCommand(CommandIn.Command, CommandIn.Value1, CommandIn.Value2);
		if (OK)
		{

			CommandIn.Command = ""; //kasowanie bo rozkaz wykonany
			CommandIn.Value1 = 0;
			CommandIn.Value2 = 0;
			CommandIn.Location.X = 0;
			CommandIn.Location.Y = 0;
			CommandIn.Location.Z = 0;
			//--if (!PhysicActivation)
			//--	Physic_ReActivation();
		}
	}
	else
		OK = false;
	return OK;

}


// **************************************************************************************************************************
// SendCtrlToNext()
// **************************************************************************************************************************

bool TMoverParameters::SendCtrlToNext(std::string CtrlCommand, double ctrlvalue, double dir)
{
	bool OK;
	int d;

	//Ra: by³ problem z propagacj¹, jeœli w sk³adzie jest pojazd wstawiony odwrotnie
	//Ra: problem jest równie¿, jeœli AI bêdzie na koñcu sk³adu
	OK = (dir != 0); // and Mains;
	d = (1 + sign(dir)) / 2; //-1=>0, 1=>1 - wysy³anie tylko w ty³ // bylo Sign

	if (OK)

		// with Couplers[d] do //w³asny sprzêg od strony (d)
		if (TestFlag(Couplers[d].CouplingFlag, ctrain_controll))
			if (Couplers[d].ConnectedNr != d)  //jeœli ten nastpêny jest zgodny z aktualnym
			{
				if (Couplers[d].Connected->SetInternalCommand(CtrlCommand, ctrlvalue, dir))
					OK = ((Couplers[d].Connected->RunInternalCommand()) && (OK));
			}
			else //jeœli nastêpny jest ustawiony przeciwnie, zmieniamy kierunek
				if (Couplers[d].Connected->SetInternalCommand(CtrlCommand, ctrlvalue, -dir))
					OK = (Couplers[d].Connected->RunInternalCommand() && OK);

	return OK;
}




// **************************************************************************************************************************
// MainSwitch() - TRANSLATED DONE
// **************************************************************************************************************************

bool TMoverParameters::MainSwitch(bool State)
{
	bool MainSwitch = false; //Ra: przeniesione z koñca
	if ((Mains != State) && (MainCtrlPosNo > 0))
	{
		if ((State == false) || ((ScndCtrlPos == 0) && (LastSwitchingTime>CtrlDelay) && !TestFlag(DamageFlag, dtrain_out)))
		{
			if (Mains) //jeœli by³ za³¹czony
				SendCtrlToNext("MainSwitch", int(State), CabNo); //wys³anie wy³¹czenia do pozosta³ych?
			Mains = State;
			if (Mains)  //jeœli zosta³ za³¹czony
				SendCtrlToNext("MainSwitch", int(State), CabNo); //wyslanie po wlaczeniu albo przed wylaczeniem
			MainSwitch = true; //wartoœæ zwrotna
			LastSwitchingTime = 0;
			if ((EngineType == DieselEngine) && Mains)
			{
				dizel_enginestart = State;
			}
			if (State == false)  //jeœli wy³¹czony
			{
				SetFlag(SoundFlag, sound_relay);
				SecuritySystem.Status = 0; //deaktywacja czuwaka
			}
			else
				SecuritySystem.Status = s_waiting; //aktywacja czuwaka
		}
	}

	return MainSwitch;
}

// ***********************************************************************************************************************
// MinCurrentSwitch(bool State)
// ***********************************************************************************************************************

bool TMoverParameters::MinCurrentSwitch(bool State)
{
	bool MinCurrentSwitch = false;
	if (((EngineType == ElectricSeriesMotor) && (IminHi > IminLo)) || (TrainType == dt_EZT))
	{
		if (State && (Imin == IminLo))
		{
			Imin = IminHi;
			MinCurrentSwitch = true;
			if (CabNo != 0)
				SendCtrlToNext("MinCurrentSwitch", 1, CabNo);
		}
		if ((!State) && (Imin == IminHi))
		{
			Imin = IminLo;
			MinCurrentSwitch = true;
			if (CabNo != 0)
				SendCtrlToNext("MinCurrentSwitch", 0, CabNo);
		}
	}

	return MinCurrentSwitch;
}


TMoverParameters::TMoverParameters(double VelInitial,string TypeNameInit,string NameInit,int LoadInitial,string LoadTypeInitial,int Cab)
{//g³ówny konstruktor
 DimHalf.x=0.5*Dim.W; //po³owa szerokoœci, OX jest w bok?
 DimHalf.y=0.5*Dim.L; //po³owa d³ugoœci, OY jest do przodu?
 DimHalf.z=0.5*Dim.H; //po³owa wysokoœci, OZ jest w górê?
 //BrakeLevelSet(-2); //Pascal ustawia na 0, przestawimy na odciêcie (CHK jest jeszcze nie wczytane!)
 bPantKurek3=true; //domyœlnie zbiornik pantografu po³¹czony jest ze zbiornikiem g³ównym
 iProblem=0; //pojazd w pe³ni gotowy do ruchu
 iLights[0]=iLights[1]=0; //œwiat³a zgaszone
};


double TMoverParameters::Distance(const TLocation &Loc1,const TLocation &Loc2,const TDimension &Dim1,const TDimension &Dim2)
{//zwraca odleg³oœæ pomiêdzy pojazdami (Loc1) i (Loc2) z uwzglêdnieneim ich d³ugoœci (kule!)
 return hypot(Loc2.X-Loc1.X,Loc1.Y-Loc2.Y)-0.5*(Dim2.L+Dim1.L);
};

double TMoverParameters::Distance(const vector3 &s1, const vector3 &s2, const vector3 &d1, const vector3 &d2)
{//obliczenie odleg³oœci prostopad³oœcianów o œrodkach (s1) i (s2) i wymiarach (d1) i (d2)
 return 0.0; //bêdzie zg³aszaæ warning - funkcja do usuniêcia, chyba ¿e siê przyda...
};

double TMoverParameters::CouplerDist(byte Coupler)
{//obliczenie odleg³oœci pomiêdzy sprzêgami (kula!)
 return Couplers[Coupler].CoupleDist=Distance(Loc,Couplers[Coupler].Connected->Loc,Dim,Couplers[Coupler].Connected->Dim); //odleg³oœæ pomiêdzy sprzêgami (kula!)
};

bool TMoverParameters::Attach(byte ConnectNo,byte ConnectToNr,TMoverParameters *ConnectTo,byte CouplingType,bool Forced)
{//³¹czenie do swojego sprzêgu (ConnectNo) pojazdu (ConnectTo) stron¹ (ConnectToNr)
 //Ra: zwykle wykonywane dwukrotnie, dla ka¿dego pojazdu oddzielnie
 //Ra: trzeba by odró¿niæ wymóg dociœniêcia od uszkodzenia sprzêgu przy podczepianiu AI do sk³adu
 if (ConnectTo) //jeœli nie pusty
 {
  if (ConnectToNr!=2) Couplers[ConnectNo].ConnectedNr=ConnectToNr; //2=nic nie pod³¹czone
  TCouplerType ct=ConnectTo->Couplers[Couplers[ConnectNo].ConnectedNr].CouplerType; //typ sprzêgu pod³¹czanego pojazdu
  Couplers[ConnectNo].Connected=ConnectTo; //tak podpi¹æ (do siebie) zawsze mo¿na, najwy¿ej bêdzie wirtualny
  CouplerDist(ConnectNo); //przeliczenie odleg³oœci pomiêdzy sprzêgami
  if (CouplingType==ctrain_virtual) return false; //wirtualny wiêcej nic nie robi
  if (Forced?true:((Couplers[ConnectNo].CoupleDist<=dEpsilon)&&(Couplers[ConnectNo].CouplerType!=NoCoupler)&&(Couplers[ConnectNo].CouplerType==ct)))
  {//stykaja sie zderzaki i kompatybilne typy sprzegow, chyba ¿e ³¹czenie na starcie
   if (Couplers[ConnectNo].CouplingFlag==ctrain_virtual) //jeœli wczeœniej nie by³o po³¹czone
   {//ustalenie z której strony rysowaæ sprzêg
    Couplers[ConnectNo].Render=true; //tego rysowaæ
    ConnectTo->Couplers[Couplers[ConnectNo].ConnectedNr].Render=false; //a tego nie
   };
   Couplers[ConnectNo].CouplingFlag=CouplingType; //ustawienie typu sprzêgu
   //if (CouplingType!=ctrain_virtual) //Ra: wirtualnego nie ³¹czymy zwrotnie!
   //{//jeœli ³¹czenie sprzêgiem niewirtualnym, ustawiamy po³¹czenie zwrotne
   ConnectTo->Couplers[Couplers[ConnectNo].ConnectedNr].CouplingFlag=CouplingType;
   ConnectTo->Couplers[Couplers[ConnectNo].ConnectedNr].Connected=this;
   ConnectTo->Couplers[Couplers[ConnectNo].ConnectedNr].CoupleDist=Couplers[ConnectNo].CoupleDist;
   return true;
   //}
   //pod³¹czenie nie uda³o siê - jest wirtualne
  }
 }
 return false; //brak pod³¹czanego pojazdu, zbyt du¿a odleg³oœæ, niezgodny typ sprzêgu, brak sprzêgu, brak haka
};

//-bool TMoverParameters::Attach(byte ConnectNo,byte ConnectToNr,T_MoverParameters *ConnectTo,byte CouplingType,bool Forced)
//-{//³¹czenie do (ConnectNo) pojazdu (ConnectTo) stron¹ (ConnectToNr)
//- return Attach(ConnectNo,ConnectToNr,(TMoverParameters*)ConnectTo,CouplingType,Forced);
//-};

int TMoverParameters::DettachStatus(byte ConnectNo)
{//Ra: sprawdzenie, czy odleg³oœæ jest dobra do roz³¹czania
 //powinny byæ 3 informacje: =0 sprzêg ju¿ roz³¹czony, <0 da siê roz³¹czyæ. >0 nie da siê roz³¹czyæ
 if (!Couplers[ConnectNo].Connected)
  return 0; //nie ma nic, to roz³¹czanie jest OK
 if ((Couplers[ConnectNo].CouplingFlag&ctrain_coupler)==0)
  return -Couplers[ConnectNo].CouplingFlag; //hak nie po³¹czony - roz³¹czanie jest OK
 if (TestFlag(DamageFlag,dtrain_coupling))
  return -Couplers[ConnectNo].CouplingFlag; //hak urwany - roz³¹czanie jest OK
 //ABu021104: zakomentowane 'and (CouplerType<>Articulated)' w warunku, nie wiem co to bylo, ale za to teraz dziala odczepianie... :) }
 //if (CouplerType==Articulated) return false; //sprzêg nie do rozpiêcia - mo¿e byæ tylko urwany
 //Couplers[ConnectNo].CoupleDist=Distance(Loc,Couplers[ConnectNo].Connected->Loc,Dim,Couplers[ConnectNo].Connected->Dim);
 CouplerDist(ConnectNo);
 if (Couplers[ConnectNo].CouplerType==Screw?Couplers[ConnectNo].CoupleDist<0.0:true)
  return -Couplers[ConnectNo].CouplingFlag; //mo¿na roz³¹czaæ, jeœli dociœniêty
 return (Couplers[ConnectNo].CoupleDist>0.2)?-Couplers[ConnectNo].CouplingFlag:Couplers[ConnectNo].CouplingFlag;
};

bool TMoverParameters::Dettach(byte ConnectNo)
{//rozlaczanie
 if (!Couplers[ConnectNo].Connected) return true; //nie ma nic, to odczepiono
 //with Couplers[ConnectNo] do
 int i=DettachStatus(ConnectNo); //stan sprzêgu
 if (i<0)
 {//gdy scisniete zderzaki, chyba ze zerwany sprzeg (wirtualnego nie odpinamy z drugiej strony)
  //Couplers[ConnectNo].Connected=NULL; //lepiej zostawic bo przeciez trzeba kontrolowac zderzenia odczepionych
  Couplers[ConnectNo].Connected->Couplers[Couplers[ConnectNo].ConnectedNr].CouplingFlag=0; //pozostaje sprzêg wirtualny
  Couplers[ConnectNo].CouplingFlag=0; //pozostaje sprzêg wirtualny
  return true;
 }
 else if (i>0)
 {//od³¹czamy wê¿e i resztê, pozostaje sprzêg fizyczny, który wymaga dociœniêcia (z wirtualnym nic)
  Couplers[ConnectNo].CouplingFlag&=ctrain_coupler;
  Couplers[ConnectNo].Connected->Couplers[Couplers[ConnectNo].ConnectedNr].CouplingFlag=Couplers[ConnectNo].CouplingFlag;
 }
 return false; //jeszcze nie roz³¹czony
};

void TMoverParameters::SetCoupleDist()
{//przeliczenie odleg³oœci sprzêgów
 if (Couplers[0].Connected)
 {
  CouplerDist(0);
  if (CategoryFlag&2)
  {//Ra: dla samochodów zderzanie kul to za ma³o
  }
 }
 if (Couplers[1].Connected)
 {
  CouplerDist(1);
  if (CategoryFlag&2)
  {//Ra: dla samochodów zderzanie kul to za ma³o
  }
 }
};

bool TMoverParameters::DirectionForward()
{
 if ((MainCtrlPosNo>0)&&(ActiveDir<1)&&(MainCtrlPos==0))
 {
  ++ActiveDir;
  DirAbsolute=ActiveDir*CabNo;
  //-if (DirAbsolute)
  //- if (Battery) //jeœli bateria jest ju¿ za³¹czona
  //-  BatterySwitch(true); //to w ten oto durny sposób aktywuje siê CA/SHP
  SendCtrlToNext("Direction",ActiveDir,CabNo);
  return true;
 }
 else if ((ActiveDir==1)&&(MainCtrlPos==0)&&(TrainType==dt_EZT))
  return MinCurrentSwitch(true); //"wysoki rozruch" EN57
 return false;
};

// Nastawianie hamulców

void TMoverParameters::BrakeLevelSet(double b)
{//ustawienie pozycji hamulca na wartoœæ (b) w zakresie od -2 do BrakeCtrlPosNo
 //jedyny dopuszczalny sposób przestawienia hamulca zasadniczego
 if (fBrakeCtrlPos==b) return; //nie przeliczaæ, jak nie ma zmiany
 fBrakeCtrlPos=b;
 BrakeCtrlPosR=b;
 if (fBrakeCtrlPos<Handle->GetPos(bh_MIN))
  fBrakeCtrlPos=Handle->GetPos(bh_MIN); //odciêcie
 else
  if (fBrakeCtrlPos>Handle->GetPos(bh_MAX))
   fBrakeCtrlPos=Handle->GetPos(bh_MAX);
 int x=floor(fBrakeCtrlPos); //jeœli odwo³ujemy siê do BrakeCtrlPos w poœrednich, to musi byæ obciête a nie zaokr¹gone

//-- while ((x>BrakeCtrlPos)&&(BrakeCtrlPos<BrakeCtrlPosNo)) //jeœli zwiêkszy³o siê o 1
//--  if (!T_MoverParameters::IncBrakeLevelOld()) break; //wyjœcie awaryjne
//-- while ((x<BrakeCtrlPos)&&(BrakeCtrlPos>=-1)) //jeœli zmniejszy³o siê o 1
//--  if (!T_MoverParameters::DecBrakeLevelOld()) break;
 BrakePressureActual=BrakePressureTable[BrakeCtrlPos+2]; //skopiowanie pozycji
/*
//youBy: obawiam sie, ze tutaj to nie dziala :P
//Ra 2014-03: by³o tak zrobione, ¿e dzia³a³o - po ka¿dej zmianie pozycji by³a wywo³ywana ta funkcja
// if (BrakeSystem==Pneumatic?BrakeSubsystem==Oerlikon:false) //tylko Oerlikon akceptuje u³amki
 if(false)
  if (fBrakeCtrlPos>0.0)
  {//wartoœci poœrednie wyliczamy tylko dla hamowania
   double u=fBrakeCtrlPos-double(x); //u³amek ponad wartoœæ ca³kowit¹
   if (u>0.0)
   {//wyliczamy wartoœci wa¿one
    BrakePressureActual.PipePressureVal+=-u*BrakePressureActual.PipePressureVal+u*BrakePressureTable[BrakeCtrlPos+1+2].PipePressureVal;
    //BrakePressureActual.BrakePressureVal+=-u*BrakePressureActual.BrakePressureVal+u*BrakePressureTable[BrakeCtrlPos+1].BrakePressureVal;  //to chyba nie bêdzie tak dzia³aæ, zw³aszcza w EN57
    BrakePressureActual.FlowSpeedVal+=-u*BrakePressureActual.FlowSpeedVal+u*BrakePressureTable[BrakeCtrlPos+1+2].FlowSpeedVal;
   }
  }
*/
};

bool TMoverParameters::BrakeLevelAdd(double b)
{//dodanie wartoœci (b) do pozycji hamulca (w tym ujemnej)
 //zwraca false, gdy po dodaniu by³o by poza zakresem
 BrakeLevelSet(fBrakeCtrlPos+b);
 return b>0.0?(fBrakeCtrlPos<BrakeCtrlPosNo):(BrakeCtrlPos>-1.0); //true, jeœli mo¿na kontynuowaæ
};

bool TMoverParameters::IncBrakeLevel()
{//nowa wersja na u¿ytek AI, false gdy osi¹gniêto pozycjê BrakeCtrlPosNo
 return BrakeLevelAdd(1.0);
};

bool TMoverParameters::DecBrakeLevel()
{//nowa wersja na u¿ytek AI, false gdy osi¹gniêto pozycjê -1
 return BrakeLevelAdd(-1.0);
};

bool TMoverParameters::ChangeCab(int direction)
{//zmiana kabiny i resetowanie ustawien
 if (abs(ActiveCab+direction)<2)
 {
//  if (ActiveCab+direction=0) then LastCab:=ActiveCab;
   ActiveCab=ActiveCab+direction;
   if ((BrakeSystem==Pneumatic)&&(BrakeCtrlPosNo>0))
   {
//    if (BrakeHandle==FV4a)   //!!!POBIERAÆ WARTOŒÆ Z KLASY ZAWORU!!!
//     BrakeLevelSet(-2); //BrakeCtrlPos=-2;
//    else if ((BrakeHandle==FVel6)||(BrakeHandle==St113))
//     BrakeLevelSet(2);
//    else
//     BrakeLevelSet(1);
    BrakeLevelSet(Handle->GetPos(bh_NP));
    LimPipePress=PipePress;
    ActFlowSpeed=0;
   }
   else
    //if (TrainType=dt_EZT) and (BrakeCtrlPosNo>0) then
    //  BrakeCtrlPos:=5; //z Megapacka
    //else
//    BrakeLevelSet(0); //BrakeCtrlPos=0;
    BrakeLevelSet(Handle->GetPos(bh_NP));
//   if not TestFlag(BrakeStatus,b_dmg) then
//    BrakeStatus:=b_off; //z Megapacka
   MainCtrlPos=0;
   ScndCtrlPos=0;
   //Ra: to poni¿ej jest bez sensu - mo¿na przejœæ nie wy³¹czaj¹c
   //if ((EngineType!=DieselEngine)&&(EngineType!=DieselElectric))
   //{
   // Mains=false;
   // CompressorAllow=false;
   // ConverterAllow=false;
   //}
  //ActiveDir=0;
  //DirAbsolute=0;
  return true;
 }
 return false;
};


//przelacznik pradu wysokiego rozruchu
bool TMoverParameters::MaxCurrentSwitch(bool State)
{
	bool MaxCurrentSwitch = false;
	if (EngineType == ElectricSeriesMotor)
		if (ImaxHi > ImaxLo)
		{
			if (State && (Imax == ImaxLo) && (RList[MainCtrlPos].Bn < 2) && !((TrainType == dt_ET42) && (MainCtrlPos > 0)))
			{
				Imax = ImaxHi;
				MaxCurrentSwitch = true;
				if (CabNo != 0)
					SendCtrlToNext("MaxCurrentSwitch", 1, CabNo);
			}
			if (!State)
				if (Imax == ImaxHi)
					if (!((TrainType == dt_ET42) && (MainCtrlPos > 0)))
					{
						Imax = ImaxLo;
						MaxCurrentSwitch = true;
						if (CabNo != 0)  SendCtrlToNext("MaxCurrentSwitch", 0, CabNo);
					}
		}
	return MaxCurrentSwitch;
}


bool TMoverParameters::CurrentSwitch(int direction)
{//rozruch wysoki (true) albo niski (false)
 //Ra: przenios³em z Train.cpp, nie wiem czy ma to sens
 if (MaxCurrentSwitch(direction))
 {if (TrainType!=dt_EZT)
   return (MinCurrentSwitch(direction));
 }
 if (EngineType==DieselEngine) //dla 2Ls150
  if (ShuntModeAllow)
   if (ActiveDir==0) //przed ustawieniem kierunku
    ShuntMode=direction;
 return false;
};

void TMoverParameters::UpdatePantVolume(double dt)
{//KURS90 - sprê¿arka pantografów; Ra 2014-07: teraz jest to zbiornik rozrz¹du, chocia¿ to jeszcze nie tak
 if (EnginePowerSource.SourceType==CurrentCollector) //tylko jeœli pantografuj¹cy
 {
  //Ra 2014-07: zasadniczo, to istnieje zbiornik rozrz¹du i zbiornik pantografów - na razie mamy razem
  //Ra 2014-07: kurek trójdrogowy ³¹czy spr.pom. z pantografami i wy³¹cznikiem ciœnieniowym WS
  //Ra 2014-07: zbiornika rozrz¹du nie pompuje siê tu, tylko pantografy; potem mo¿na zamkn¹æ WS i odpaliæ resztê
  if ((TrainType==dt_EZT)?(PantPress<ScndPipePress):bPantKurek3) //kurek zamyka po³¹czenie z ZG
  {//zbiornik pantografu po³¹czony ze zbiornikiem g³ównym - ma³¹ sprê¿ark¹ siê tego nie napompuje
   //Ra 2013-12: Niebugoc³aw mówi, ¿e w EZT nie ma potrzeby odcinaæ kurkiem
   PantPress=EnginePowerSource.CollectorParameters.MaxPress; //ograniczenie ciœnienia do MaxPress (tylko w pantografach!)
   if (PantPress>ScndPipePress) PantPress=ScndPipePress; //oraz do ScndPipePress
   PantVolume=(PantPress+1)*0.1; //objêtoœæ, na wypadek odciêcia kurkiem
  }
  else
  {//zbiornik g³ówny odciêty, mo¿na pompowaæ pantografy
   if (PantCompFlag&&Battery) //w³¹czona bateria i ma³a sprê¿arka
    PantVolume+=dt*(TrainType==dt_EZT?0.003:0.005)*(2*0.45-((0.1/PantVolume/10)-0.1))/0.45; //nape³nianie zbiornika pantografów
   //Ra 2013-12: Niebugoc³aw mówi, ¿e w EZT nabija 1.5 raz wolniej ni¿ jak by³o 0.005
   PantPress=(10.0*PantVolume)-1.0; //tu by siê przyda³a objêtoœæ zbiornika
  }
  if (!PantCompFlag&&(PantVolume>0.1))
   PantVolume-=dt*0.0003; //nieszczelnoœci: 0.0003=0.3l/s
  if (Mains) //nie wchodziæ w funkcjê bez potrzeby
   if (EngineType==ElectricSeriesMotor) //nie dotyczy... czego w³aœciwie?
    if (PantPress<EnginePowerSource.CollectorParameters.MinPress)
     if ((TrainType&(dt_EZT|dt_ET40|dt_ET41|dt_ET42))?(GetTrainsetVoltage()<EnginePowerSource.CollectorParameters.MinV):true) //to jest trochê proteza; zasilanie cz³onu mo¿e byæ przez sprzêg WN
      if (MainSwitch(false))
       EventFlag=true; //wywalenie szybkiego z powodu niskiego ciœnienia
  if (TrainType!=dt_EZT) //w EN57 pompuje siê tylko w silnikowym
  //pierwotnie w CHK pantografy mia³y równie¿ rozrz¹dcze EZT
  for (int b=0;b<=1;++b)
   if (TestFlag(Couplers[b].CouplingFlag,ctrain_controll))
    if (Couplers[b].Connected->PantVolume<PantVolume) //bo inaczej trzeba w obydwu cz³onach przestawiaæ
     Couplers[b].Connected->PantVolume=PantVolume; //przekazanie ciœnienia do s¹siedniego cz³onu
  //czy np. w ET40, ET41, ET42 pantografy cz³onów maj¹ po³¹czenie pneumatyczne?
  //Ra 2014-07: raczej nie - najpierw siê za³¹cza jeden cz³on, a potem mo¿na podnieœæ w drugim
 }
 else
 {//a tu coœ dla SM42 i SM31, aby pokazywaæ na manometrze
  PantPress=CntrlPipePress;
 }
};


// **************************************************************************************************************************
// EmergencyBrakeSwitch() - TRANSLATED
// **************************************************************************************************************************

bool __fastcall TMoverParameters::EmergencyBrakeSwitch(bool Switch)
{
	bool EmergencyBrakeSwitch;

	if ((BrakeSystem != Individual) && (BrakeCtrlPosNo > 0))
	{
		if ((!EmergencyBrakeFlag) && Switch)
		{
			EmergencyBrakeFlag = Switch;
			EmergencyBrakeSwitch = true;
		}
		else
		{
			if ((abs(V) < 0.1) && (Switch == false)) //  {odblokowanie hamulca bezpieczenistwa tylko po zatrzymaniu}
			{
				EmergencyBrakeFlag = Switch;
				EmergencyBrakeSwitch = true;
			}
			else
				EmergencyBrakeSwitch = false;
		}
	}
	else
		EmergencyBrakeSwitch = false;   //nie ma hamulca bezpieczenstwa gdy nie ma hamulca zesp.}
	return EmergencyBrakeSwitch;
}

// **************************************************************************************************************************
// SecuritySystemCheck(double dt) - TRANSLATED
// **************************************************************************************************************************

void TMoverParameters::SecuritySystemCheck(double dt)
{
	if ((SecuritySystem.SystemType > 0) && (SecuritySystem.Status > 0))
	{
		SecuritySystem.SystemTimer = SecuritySystem.SystemTimer + dt;
		if (TestFlag(SecuritySystem.Status, s_aware) || TestFlag(SecuritySystem.Status, s_active))  //jeœli œwieci albo miga
			SecuritySystem.SystemSoundTimer = SecuritySystem.SystemSoundTimer + dt;
		if (TestFlag(SecuritySystem.Status, s_alarm))   //jeœli buczy
			SecuritySystem.SystemBrakeTimer = SecuritySystem.SystemBrakeTimer + dt;
		if (TestFlag(SecuritySystem.SystemType, 1))
			if ((SecuritySystem.SystemTimer>SecuritySystem.AwareDelay) && (SecuritySystem.AwareDelay >= 0)) //  -1 blokuje
				if (!SetFlag(SecuritySystem.Status, s_aware))  //juz wlaczony sygnal swietlny
					if ((SecuritySystem.SystemSoundTimer>SecuritySystem.SoundSignalDelay) && (SecuritySystem.SoundSignalDelay >= 0))
						if (!SetFlag(SecuritySystem.Status, s_alarm))  // juz wlaczony sygnal dzwiekowy
							if ((SecuritySystem.SystemBrakeTimer>SecuritySystem.EmergencyBrakeDelay) && (SecuritySystem.EmergencyBrakeDelay >= 0))
								SetFlag(SecuritySystem.Status, s_ebrake);       //przeterminowanie czuwaka, hamowanie awaryjne}

		if (TestFlag(SecuritySystem.SystemType, 2) && TestFlag(SecuritySystem.Status, s_active))
			if ((Vel > SecuritySystem.VelocityAllowed) && (SecuritySystem.VelocityAllowed >= 0))
				SetFlag(SecuritySystem.Status, s_ebrake);
			else
				if (((SecuritySystem.SystemSoundTimer>SecuritySystem.SoundSignalDelay) && (SecuritySystem.SoundSignalDelay >= 0)) || ((Vel>SecuritySystem.NextVelocityAllowed) && (SecuritySystem.NextVelocityAllowed >= 0)))
					if (!SetFlag(SecuritySystem.Status, s_alarm)) //juz wlaczony sygnal dzwiekowy}
						if ((SecuritySystem.SystemBrakeTimer>SecuritySystem.EmergencyBrakeDelay) && (SecuritySystem.EmergencyBrakeDelay >= 0))
							SetFlag(SecuritySystem.Status, s_ebrake);
		if (TestFlag(SecuritySystem.Status, s_ebrake))
			if (!EmergencyBrakeFlag)
				EmergencyBrakeSwitch(true);
	}
}

void TMoverParameters::UpdateBatteryVoltage(double dt)
{//przeliczenie obci¹¿enia baterii
 double sn1,sn2,sn3,sn4,sn5; //Ra: zrobiæ z tego amperomierz NN
 if ((BatteryVoltage>0)&&(EngineType!=DieselEngine)&&(EngineType!=WheelsDriven)&&(NominalBatteryVoltage>0))
 {
  if ((NominalBatteryVoltage/BatteryVoltage<1.22)&&Battery)
  {//110V
   if (!ConverterFlag)
    sn1=(dt*2.0); //szybki spadek do ok 90V
   else sn1=0;
   if (ConverterFlag)
    sn2=-(dt*2.0); //szybki wzrost do 110V
   else sn2=0;
   if (Mains)
    sn3=(dt*0.05);
   else sn3=0;
   if (iLights[0]&63) //64=blachy, nie ci¹gn¹ pr¹du //rozpisaæ na poszczególne ¿arówki...
    sn4=dt*0.003;
   else sn4=0;
   if (iLights[1]&63) //64=blachy, nie ci¹gn¹ pr¹du
    sn5=dt*0.001;
   else sn5=0;
  };
  if ((NominalBatteryVoltage/BatteryVoltage>=1.22)&&Battery)
  {//90V
   if (PantCompFlag)
    sn1=(dt*0.0046);
   else sn1=0;
   if (ConverterFlag)
    sn2=-(dt*50); //szybki wzrost do 110V
   else sn2=0;
   if (Mains)
    sn3=(dt*0.001);
   else sn3=0;
   if (iLights[0]&63) //64=blachy, nie ci¹gn¹ pr¹du
    sn4=(dt*0.0030);
   else sn4=0;
   if (iLights[1]&63) //64=blachy, nie ci¹gn¹ pr¹du
    sn5=(dt*0.0010);
   else sn5=0;
  };
  if (!Battery)
  {
   if (NominalBatteryVoltage/BatteryVoltage<1.22)
    sn1=dt*50;
   else
    sn1=0;
    sn2=dt*0.000001;
    sn3=dt*0.000001;
    sn4=dt*0.000001;
    sn5=dt*0.000001; //bardzo powolny spadek przy wy³¹czonych bateriach
  };
  BatteryVoltage-=(sn1+sn2+sn3+sn4+sn5);
  if (NominalBatteryVoltage/BatteryVoltage>1.57)
    if (MainSwitch(false)&&(EngineType!=DieselEngine)&&(EngineType!=WheelsDriven))
      EventFlag=true; //wywalanie szybkiego z powodu zbyt niskiego napiecia
  if (BatteryVoltage>NominalBatteryVoltage)
   BatteryVoltage=NominalBatteryVoltage; //wstrzymanie ³adowania pow. 110V
  if (BatteryVoltage<0.01)
   BatteryVoltage=0.01;
 }
 else
  if (NominalBatteryVoltage==0)
   BatteryVoltage=0;
  else
   BatteryVoltage=90;
};


// ***********************************************************************************************************************
// AutoRelayCheck(void)
// ***********************************************************************************************************************

bool __fastcall TMoverParameters::AutoRelayCheck(void)
{
	bool OK, AutoRelayCheck;

	//yB: wychodzenie przy odcietym pradzie
	if (ScndCtrlActualPos == 255)
	{
		AutoRelayCheck = false;
		MainCtrlActualPos = 0;
	}
	else
		if ((!Mains) || (FuseFlag) || (StLinFlag))  //hunter-111211: wylacznik cisnieniowy
		{
			AutoRelayCheck = false;
			MainCtrlActualPos = 0;
			ScndCtrlActualPos = 0;
		}
		else
		{
			OK = false;
			if (DelayCtrlFlag && (MainCtrlPos == 1) && (MainCtrlActualPos == 1) && (LastRelayTime > InitialCtrlDelay))
			{
				DelayCtrlFlag = false;
				SetFlag(SoundFlag, sound_relay);
				SetFlag(SoundFlag, sound_loud);
			}
			if ((LastRelayTime > CtrlDelay) && (!DelayCtrlFlag))
			{
				if (MainCtrlPos == 0)
					DelayCtrlFlag = true;
				if ((((RList[MainCtrlActualPos].R == 0) && ((!CoupledCtrl) || (Imin == IminLo))) || (MainCtrlActualPos == RlistSize))
					&& ((ScndCtrlActualPos > 0) || (ScndCtrlPos>0)))
				{   //zmieniaj scndctrlactualpos}
					if ((!AutoRelayFlag) || (!MotorParam[ScndCtrlActualPos].AutoSwitch))
					{                                               //scnd bez samoczynnego rozruchu
						OK = true;
						if (ScndCtrlActualPos < ScndCtrlPos)
							ScndCtrlActualPos++;
						else
							if (ScndCtrlActualPos > ScndCtrlPos)
								ScndCtrlActualPos--;
							else OK = false;
					}
					else
					{                                             //scnd z samoczynnym rozruchem
						if (ScndCtrlPos < ScndCtrlActualPos)
						{
							ScndCtrlActualPos--;
							OK = true;
						}
						else
							if (ScndCtrlPos>ScndCtrlActualPos)
								if (abs(Im) < Imin)
									if (MotorParam[ScndCtrlActualPos].AutoSwitch)
									{
										ScndCtrlActualPos++;
										OK = true;
									}
					}
				}
				else
				{          //zmieniaj mainctrlactualpos}
					if (((TrainType == dt_EZT) && (Imin == IminLo)) || ((ActiveDir < 0) && (TrainType != dt_PseudoDiesel)))
						if (RList[MainCtrlActualPos + 1].Bn > 1)
						{
							AutoRelayCheck = false;
							// exit;
						}
					if ((!AutoRelayFlag) || (!RList[MainCtrlActualPos].AutoSwitch))
					{                                             //main bez samoczynnego rozruchu}
						OK = true;
						if (RList[MainCtrlActualPos].Relay < MainCtrlPos)
						{
							MainCtrlActualPos++;
							//---------
							//hunter-111211: poprawki
							if (MainCtrlActualPos > 0)
								if ((RList[MainCtrlActualPos].R == 0) && (!(MainCtrlActualPos == MainCtrlPosNo))) //wejscie na bezoporowa
								{
									SetFlag(SoundFlag, sound_manyrelay); SetFlag(SoundFlag, sound_loud);
								}
								else if ((RList[MainCtrlActualPos].R > 0) && (RList[MainCtrlActualPos - 1].R == 0))  //wejscie na drugi uklad
								{
									SetFlag(SoundFlag, sound_manyrelay);
								}
						}
						else if (RList[MainCtrlActualPos].Relay > MainCtrlPos)
						{
							MainCtrlActualPos--;
							if (MainCtrlActualPos > 0)  //hunter-111211: poprawki
								if (RList[MainCtrlActualPos].R == 0)  //dzwieki schodzenia z bezoporowej}
								{
									SetFlag(SoundFlag, sound_manyrelay);
								}
						}
						else
							if ((RList[MainCtrlActualPos].R > 0) && (ScndCtrlActualPos > 0))
								ScndCtrlActualPos--; //boczniki nie dzialaja na poz. oporowych
							else
								OK = false;
					}
					else                                                  //main z samoczynnym rozruchem
					{
						OK = false;
						if (MainCtrlPos < RList[MainCtrlActualPos].Relay)
						{
							--MainCtrlActualPos;
							OK = true;
						}
						else
							if ((MainCtrlPos > RList[MainCtrlActualPos].Relay)
								|| ((MainCtrlActualPos < RlistSize) && (MainCtrlPos == RList[MainCtrlActualPos + 1].Relay)))
								if (abs(Im) < Imin)
								{
									++MainCtrlActualPos;
									OK = true;
								}
					}
				}
			}
			else  //DelayCtrlFlag
				if (((MainCtrlPos > 1) && (MainCtrlActualPos > 0) && DelayCtrlFlag))
				{
					MainCtrlActualPos = 0;
					OK = true;
				}
				else
					if ((MainCtrlPos == 1) && (MainCtrlActualPos == 0))
						MainCtrlActualPos = 1;
					else
						if ((MainCtrlPos == 0) && (MainCtrlActualPos > 0))
						{
							--MainCtrlActualPos;
							OK = true;
						}
			if (OK) LastRelayTime = 0;
			AutoRelayCheck = OK;
		}
	return AutoRelayCheck;
}


void TMoverParameters::CompressorCheck(double dt)
{
	if (VeselVolume > 0)
	{
		if (MaxCompressor - MinCompressor < 0.0001)
		{
			//        if Mains and (MainCtrlPos>1) then}
			if ((CompressorAllow) && (Mains) && (MainCtrlPos > 0))
			{
				if (Compressor < MaxCompressor)
					if ((EngineType == DieselElectric) && (CompressorPower > 0))
						CompressedVolume = CompressedVolume + dt * CompressorSpeed * (2 * MaxCompressor - Compressor) / MaxCompressor * (DElist[MainCtrlPos].RPM / DElist[MainCtrlPosNo].RPM);
					else
						CompressedVolume = CompressedVolume + dt * CompressorSpeed * (2 * MaxCompressor - Compressor) / MaxCompressor;
				else
				{
					CompressedVolume = CompressedVolume * 0.8;
					SetFlag(SoundFlag, sound_relay);
					SetFlag(SoundFlag, sound_loud);
				}
			}
		}
		else
		{
			//       if (not CompressorFlag) and (Compressor<MinCompressor) and Mains and (LastSwitchingTime>CtrlDelay) then}
			if ((!CompressorFlag) && (Compressor<MinCompressor) && (Mains) && (CompressorAllow) && ((ConverterFlag) || (CompressorPower == 0)) && (LastSwitchingTime>CtrlDelay))
			{
				CompressorFlag = true;
				LastSwitchingTime = 0;
			}
			if ((Compressor > MaxCompressor) || (!CompressorAllow) || ((CompressorPower != 0) && (!ConverterFlag)) || (!Mains))
				CompressorFlag = false;
			if (CompressorFlag)
				if ((EngineType == DieselElectric) && (CompressorPower > 0))
					CompressedVolume = CompressedVolume + dt * CompressorSpeed * (2 * MaxCompressor - Compressor) / MaxCompressor * (DElist[MainCtrlPos].RPM / DElist[MainCtrlPosNo].RPM);
				else
					CompressedVolume = CompressedVolume + dt * CompressorSpeed * (2 * MaxCompressor - Compressor) / MaxCompressor;
		}
	}
}



// **************************************************************************************************************************
// IncBrakePress() - TRANSLATED
// **************************************************************************************************************************

bool __fastcall TMoverParameters::IncBrakePress(double &brake, double PressLimit, double dp)
{
	bool IncBrakePress;

	//if ((DynamicBrakeType!=dbrake_switch) && (DynamicBrakeType!=dbrake_none) && ((BrakePress>0.2) || (PipePress<0.37)) )

	if ((DynamicBrakeType != dbrake_switch) && (DynamicBrakeType != dbrake_none) && (BrakePress > 0.2) && (TrainType != dt_EZT)) //yB radzi nie sprawdzaæ ciœnienia w przewodzie
		//hunter-301211: dla EN57 silnikow nie odlaczamy
	{
		DynamicBrakeFlag = true;                //uruchamianie hamulca ED albo odlaczanie silnikow}
		if ((DynamicBrakeType == dbrake_automatic) && (abs(Im) > 60))           //nie napelniaj wiecej, jak na EP09}
			dp = 0.0;
	}
	if ((brake + dp) < PressLimit)
	{
		brake = brake + dp;
		IncBrakePress = true;
		//WriteLog("IncBrakePress = true");
	}
	else
	{
		IncBrakePress = false;
		brake = PressLimit;
		//WriteLog("IncBrakePress = false");
	}

	return IncBrakePress;
}


// *************************************************************************************************
// DecBrakePress() - TRANSLATED
// *************************************************************************************************

bool __fastcall TMoverParameters::DecBrakePress(double &brake, double PressLimit, double dp)
{
	bool DecBrakePress;

	if (brake - dp > PressLimit)
	{
		brake = brake - dp;
		DecBrakePress = true;
	}
	else
	{
		DecBrakePress = false;
		brake = PressLimit;
	}
	if ((DynamicBrakeType != dbrake_switch) && ((BrakePress < 0.1))) //yB radzi nie sprawdzaæ ciœnienia w przewodzie
		DynamicBrakeFlag = false;                //wylaczanie hamulca ED i/albo zalaczanie silnikow}

	return DecBrakePress;
}


// **************************************************************************************************************************
// UpdateBrakePressure() - TRANSLATED
// **************************************************************************************************************************

void TMoverParameters::UpdateBrakePressure(double dt)
{
/*
	const double LBDelay = 5.0; //stala czasowa hamulca}
	double Rate, Speed, dp, sm;

	dpLocalValve = 0;
	dpBrake = 0;

	if ((MaxBrakePress > 0) && (!TestFlag(BrakeStatus, b_dmg)))
	{
		//hydrauliczny hamulec pomocniczy}
		if ((LocalBrake == HydraulicBrake) && (BrakeStatus == b_off))
		{
			LocBrakePress = abs(MaxBrakePress * LocalBrakeRatio()); // Q DODALEM abs()
		}

		//elektropneumatyczny hamulec zasadniczy

		if ((BrakePressureTable[BrakeCtrlPos].BrakeType == ElectroPneumatic) && (Mains) && (ActiveDir != 0))
			//WriteLog("ep");
			// with BrakePressureTable[BrakeCtrlPos] do
			if (BrakePressureTable[BrakeCtrlPos].BrakePressureVal != -1)
			{
				if (TestFlag(BrakeStatus, b_epused))
				{
					if (MBPM > TotalMass)
						sm = sm * TotalMass / MBPM; //sk¹d pierwsza wartoœæ?
					else
						sm = 1;

					dp = (Min0R(BrakeVP(), sm * BrakePressureTable[BrakeCtrlPos].BrakePressureVal)); // Q DDALEM abs()

					dpBrake = abs(MaxBrakePress * BrakePressureTable[BrakeCtrlPos].FlowSpeedVal / 5 * sign(dp - LocBrakePress) * PR(dp, LocBrakePress) * dt / (1 + BrakeDelay[1 + byte(dp>LocBrakePress) + 2 * BrakeDelayFlag]));

					LocBrakePress = LocBrakePress + dpBrake;
				}
			} //sterowanie cisnieniem}

		//pneumatyczny hamulec zasadniczy
		if (((BrakeCtrlPosNo > 0) && (BrakePressureTable[BrakeCtrlPos].BrakeType == Pneumatic)) || (BrakeSystem == Pneumatic) || (BrakeSystem == ElectroPneumatic))
		{
			// WriteLog("ep-");
			if (TestFlag(BrakeStatus, b_release) || ((TrainType == dt_ET42) && (ScndCtrlActualPos < 255) && (DynamicBrakeFlag)))  //odluzniacz
			{
				//WriteLog("ep-1");

				//               dpBrake:=(PipeBrakePress+sd)*dt/2.5;
				dpBrake = MaxBrakePress * (PipeBrakePress) / (1.13 * PipeBrakePress + 0.013) * dt / 2.5;
				DecBrakePress(PipeBrakePress, 0, dpBrake);
				if (PipeBrakePress < 0.01) { BrakeStatus = (BrakeStatus && 154); PipeBrakePress = 0; }
			}
			else  //normalna praca
			{
				//  WriteLog("ep-2");
				sm = 1.0;
				//test pozycji pospiesznej
				if (TestFlag(BrakeDelays, bdelay_R))
					if (!TestFlag(BrakeDelayFlag, bdelay_R) && (BrakeCtrlPosNo == 0) && ((BrakeMethod && 1) == 1))
						sm = 0.72;  // 1/1.4}
					else if ((TestFlag(BrakeStatus, b_Ractive)) && (BrakeCtrlPosNo > 0))
						sm = 1.5;

				if (MBPM > TotalMass)
					sm = sm * TotalMass / MBPM;



				Rate = abs(PipeRatio() * sm);  // Q dalem abs bo bylo na minusie


				// TU sm = 1 zas rate = 0, jest identycznie

				if (TestFlag(BrakeDelays, bdelay_R))
					if ((!TestFlag(BrakeStatus, b_Ractive)) && (BrakeCtrlPosNo == 0) && ((BrakeMethod && 1) == 0))
						sm = sm * 0.5;

				Speed = RealPipeRatio() * sm;

				// TU sm = 1 zas Spdeed = 0.00023???!!

				//(Volume/(10*BrakeVVol ume+0.1)>MaxBrakePress/2))}
				//WriteLogSS("rate, BrakeVP() > PipeBrakePress ", ftoss(Rate) + ", " + ftoss(BrakeVP()) + ", " + ftoss(PipeBrakePress));
				// PONIZSZE WYKONUJE SIE GDY HAMUJEMY ZASADNICZYM
				if ((Rate*MaxBrakePress > PipeBrakePress) && (BrakeVP() > PipeBrakePress) && (TestFlag(BrakeStatus, b_on)))
				{
					if ((PipeBrakePress > MaxBrakePress * 0.15) && (BrakeDelayFlag == bdelay_G))
					{
						dpBrake = (0.2 + Speed) / 1.2*sm*MaxBrakePress*PR(MaxBrakePress*sm, PipeBrakePress)*dt / (BrakeDelay[4]);
						//WriteLog("ax");
					}
					else
					{
						dpBrake = (0.2 + Speed) / 1.2*sm*MaxBrakePress*PR(MaxBrakePress*sm, PipeBrakePress)*dt / (BrakeDelay[2]);
						//WriteLog("zx");
					}


					WriteLogSS("SM, Speed, dpBrake", ftoss(sm) + ", " + ftoss(Speed) + ", " + ftoss(dpBrake));

					IncBrakePress(PipeBrakePress, Rate*MaxBrakePress, dpBrake);
				}


				if (PipeBrakePress > (Rate * MaxBrakePress))
				{
					if ((TestFlag(BrakeStatus, b_Rused)) && (!TestFlag(BrakeStatus, b_Ractive)))
					{
						SetFlag(BrakeStatus, -b_Rused);
						sm = 1 + 0.5 * byte(TestFlag(BrakeStatus, b_Rused) && (BrakeCtrlPosNo > 0));
						PipeBrakePress = PipeBrakePress * (1.5 - sm) + (sm - 1) / sm * PipeBrakePress * 2;
						//                  dpBrake:=(PipeBrakePress-MaxBrakePress*(1.5*sm);
					}
					else if (BrakeDelayFlag == bdelay_G)
						dpBrake = (1.2 - Speed) / 1.2*sm*MaxBrakePress*PR(PipeBrakePress, 0)*dt / (BrakeDelay[3]);
					else
						dpBrake = (1.2 - Speed) / 1.2*sm*MaxBrakePress*PR(PipeBrakePress, 0)*dt / (BrakeDelay[1]);

					if (dpBrake < 0)  dpBrake = 0;

					DecBrakePress(PipeBrakePress, Rate * MaxBrakePress, dpBrake);
				}
			}

			if (BrakeSystem != ElectroPneumatic)
			{
				Rate = LocalBrakeRatio();
				if ((Rate*MaxBrakePress > LocBrakePress) && (Compressor > LocBrakePress))
				{
					dpLocalValve = (0.01 + Rate)*MaxBrakePress*PR(MaxBrakePress, LocBrakePress)*dt / 4.0;  //5*2
					IncBrakePress(LocBrakePress, Rate*MaxBrakePress, dpLocalValve);
				}
				if (LocBrakePress > Rate*MaxBrakePress)
				{
					dpLocalValve = (1.01 - Rate)*MaxBrakePress*PR(LocBrakePress, 0)*dt / 7;  //8*2
					DecBrakePress(LocBrakePress, Rate*MaxBrakePress, dpLocalValve);
				}
			}
		} //Pneumatic

		if ((BrakeSystem == Pneumatic) || (BrakeSystem == ElectroPneumatic))
		{
			Speed = BrakePress;  //yB: uzycie jednej zmiennej - optymalka pamieci
			dp = Max0R(PipeBrakePress, LocBrakePress);
			dpBrake = abs(sign(dp - BrakePress) * MaxBrakePress * dt / 3); // Q DALEM abs() BO BYLO NA MINUSIE, CZEMU???
			//--WriteLogSS("dpBrake",  ftoss(dpBrake));


			if (dpBrake > 0)
			{
				//WriteLog("ibp");
				IncBrakePress(BrakePress, dp, dpBrake);
				dpBrake = BrakePress - Speed;
				if ((PipeBrakePress < LocBrakePress) && ((BrakeSystem == Pneumatic) || ((BrakeSystem == ElectroPneumatic) && (BrakeSubsystem != Oerlikon))))
					CompressedVolume = CompressedVolume - 10 * dpBrake * BrakeVolume;
				else
					Volume = Volume - 10 * dpBrake * BrakeVolume;
			}
			else
			{
				//WriteLog("dbp");
				DecBrakePress(BrakePress, dp, -dpBrake);
				dpBrake = BrakePress - Speed;
			}
		}
	} //cylindry hamulcowe

*/
} // updatebrakepressure



// *************************************************************************************************************************
// ABu: funkcja zwracajaca napiecie dla calego skladu, przydatna dla EZT - TRANSLATED DONE
// *************************************************************************************************************************

bool TMoverParameters::GetTrainsetVoltage(void)
{
	bool voltf, voltr, GetTrainsetVoltage;

	if (Couplers[0].Connected != NULL)
	{
		if ((Couplers[0].Connected->PantFrontVolt) || (Couplers[0].Connected->PantRearVolt))
			voltf = true;
		else
			voltf = false;
	}
	else
		voltf = false;
	if (Couplers[1].Connected != NULL)
	{
		if ((Couplers[1].Connected->PantFrontVolt) || (Couplers[1].Connected->PantRearVolt))
			voltr = true;
		else
			voltr = false;
	}
	else
		voltr = false;
	GetTrainsetVoltage = (voltf || voltr);

	return GetTrainsetVoltage;
}

// *************************************************************************************
// Physic_ReActivation() - TRANSLATED DONE
// *************************************************************************************

bool TMoverParameters::Physic_ReActivation(void)
{
	if (PhysicActivation)
		return false;
	else
	{
		PhysicActivation = true;
		LastSwitchingTime = 0;
		return true;
	}
}


// *************************************************************************************
// LocalBrakeRatio(void) - TRANSLATED DONE
// *************************************************************************************

double TMoverParameters::LocalBrakeRatio(void)
{
	double LBR;
	if (LocalBrakePosNo > 0)
		return (LocalBrakePos / LocalBrakePosNo);
	else
		return 0;
	if (TestFlag(BrakeStatus, b_antislip))
		LBR = Max0R(LBR, PipeRatio()) + 0.4;
	return LBR;
}


// *************************************************************************************
// PipeRatio(void) - TRANSLATED DONE
// *************************************************************************************

double TMoverParameters::PipeRatio(void)
{
	double pr;
	/*
	if (DeltaPipePress > 0)
		if (BrakeSubsystem == Oerlikon)
		{
			if ((3 * PipePress) > (HighPipePress + LowPipePress + LowPipePress))
				pr = (HighPipePress - Min0R(HighPipePress, PipePress)) / (DeltaPipePress * 4.0 / 3.0);
			else
				pr = (HighPipePress - 1.0 / 3.0 * DeltaPipePress - Max0R(LowPipePress, PipePress)) / (DeltaPipePress * 2.0 / 3.0);
			if ((!TestFlag(BrakeStatus, b_Ractive)) && (BrakeMethod && 1 == 0) && TestFlag(BrakeDelays, bdelay_R) && (Power < 1) && (BrakeCtrlPos<1))
				pr = Min0R(0.5, pr);
		}
		else
			pr = (HighPipePress - Max0R(LowPipePress, Min0R(HighPipePress, PipePress))) / DeltaPipePress;
	else
		pr = 0;
		*/
	return pr;
}


// *************************************************************************************
// RealPipeRatio(void) - TRANSLATED DONE
// *************************************************************************************

double __fastcall TMoverParameters::RealPipeRatio(void)
{
	double rpp;
	if (DeltaPipePress > 0)
	{
		//WriteLog("dpp > 0");
		//WriteLogSS("dpp=", ftoss(DeltaPipePress)); 
		rpp = (CntrlPipePress - PipePress) / (DeltaPipePress);
	}
	else
		rpp = 0;
	return rpp;
}


// *************************************************************************************
// BrakeVP(void) - TRANSLATED DONE
// *************************************************************************************

double TMoverParameters::BrakeVP(void)
{
	double BVP;
	if (BrakeVVolume > 0)
		return Volume / (10.0 * BrakeVVolume);
	else return 0;

	// return BVP;
}

// ***********************************************************************************************************************
// UpdatePipePressure(double dt)
// ***********************************************************************************************************************

void  TMoverParameters::UpdatePipePressure(double dt)
{
	const double LBDelay = 15.0;
	const double kL = 0.5;
	double dV;
	PMoverParameters c;

	//  ABu: zmieniam na zlecenie Olo_EU
	//  PWSpeed:=800/sqr(Dim.L); a
	//  PWSpeed:=1600/sqr(Dim.L); b
	//yB: zmieniam po konsulatacjach z Olo
	//  PWSpeed:=180/sqrt(Dim.L); c
	//yB: to wynika z ksiazek, ze conajmniej 250 m/s ma fala hamowania
	//  PWSpeed:=280/Dim.L; // d 280

	dpMainValve = 0;

/*
	if ((HighPipePress > 0) || (LowPipePress < 0))
	{
		if ((BrakeCtrlPosNo > 0) && (ActiveCab != 0))
		{
			switch (BrakeSubsystem)
			{
			case Oerlikon:
			{
				TBrakePressure BPT = BrakePressureTable[BrakeCtrlPos];

				if (BrakeSystem == ElectroPneumatic)
				{
					ActFlowSpeed = BrakePressureTable[BrakeCtrlPos].FlowSpeedVal * byte(BrakePressureTable[BrakeCtrlPos].BrakeType != ElectroPneumatic) + 4 * byte(BrakePressureTable[BrakeCtrlPos].BrakeType == ElectroPneumatic);
					if (BPT.PipePressureVal > 0)
						LimPipePress = BrakePressureTable[BrakeCtrlPos].PipePressureVal;
				}
				else
				{
					if ((BrakeCtrlPos > 0) || (BrakeCtrlPos == -2))
					{
						if (BrakePressureTable[BrakeCtrlPos].PipePressureVal >= 0)
							LimPipePress = BrakePressureTable[BrakeCtrlPos].PipePressureVal;
						ActFlowSpeed = BrakePressureTable[BrakeCtrlPos].FlowSpeedVal;
					}
					else
						if (BrakeCtrlPos == 0)
						{
							LimPipePress = BrakePressureTable[BrakeCtrlPos].PipePressureVal;
							if (PipePress > BrakePressureTable[BrakeCtrlPos].PipePressureVal)
								//                      if (LimPipePress>0.55) then
								//                        ActFlowSpeed:=FlowSpeedVal*1.5
								//                      else
								ActFlowSpeed = Min0R(BrakePressureTable[BrakeCtrlPos].FlowSpeedVal, (0.05 * LBDelay / PR(PipePress, BrakePressureTable[BrakeCtrlPos].PipePressureVal)));
							else
								ActFlowSpeed = BrakePressureTable[BrakeCtrlPos].FlowSpeedVal;
						}
						else
							if (BrakeCtrlPos == -1)
								if (PipePress > 0.699)
								{
									LimPipePress = 0.54;
									ActFlowSpeed = 5;
								}
				}
			}
			case Knorr:
			{
				TBrakePressure BPT = BrakePressureTable[BrakeCtrlPos];
				if (!BPT.PipePressureVal == -1)
					LimPipePress = BPT.PipePressureVal;
				if ((BrakeCtrlPos == 0) && (PipePress > LimPipePress))
					ActFlowSpeed = 2;
				else
					ActFlowSpeed = BPT.FlowSpeedVal;
			}
			} // switch
			// }  // if ((BrakeCtrlPosNo>0) && (ActiveCab != 0))


			dpPipe = Min0R(Compressor, LimPipePress);

			dpMainValve = ActFlowSpeed * sign(dpPipe - PipePress) * PR(dpPipe, PipePress) * dt / (LBDelay)* (1 - int(EmergencyBrakeFlag));

			if (dpMainValve > 0)            //50
				CompressedVolume = CompressedVolume - dpMainValve*Dim.L*Spg / 100.0*(1 + kL*byte((BrakeCtrlPosNo>0) && (Power>1) && (BrakeSystem == Pneumatic)));

		} //sterowanie cisnieniem

		if (EmergencyBrakeFlag)         //ulepszony hamulec bezp.}
			dpMainValve = -PipePress * 2 * dt;


		dpPipe = 0;
		//yB: napelnianie zbiornika pomocniczego pojazdu
		switch (BrakeSubsystem)
		{
		case Oerlikon:
			if ((BrakeSystem == Pneumatic) && (Power < 1))
			{
				if (BrakeVP() > PipePress)
				{        //SPR(BrakeVP,PipePress);(-PipePress+BrakeVP)
					dpPipe = (1 - (BrakeStatus && b_on)) * SPR(BrakeVP(), PipePress) * (Dim.L * Spg / (Dim.L * Spg + 100.0 * BrakeVVolume)) * dt / 120.0;
				}
				else if (BrakeVP() < PipePress)
					if (BrakeVP() < CntrlPipePress)
					{
						//             dpPipe:=-PR(PipePress,BrakeVP)*dt/100
						//                       SPR(PipePress,BrakeVP)
						dpPipe = -SPR(PipePress, BrakeVP()) * (Dim.L*Spg / (Dim.L*Spg + 100.0*BrakeVVolume)) * dt / BrakeDelay[1 + 2 * byte(BrakeDelayFlag == 1)];
					}     //PR(PipePress,BrakeVP)
					else
						dpPipe = -SPR(PipePress, BrakeVP()) * (Dim.L*Spg / (Dim.L*Spg + 100.0*BrakeVVolume)) * dt / 100.0 / (1.0 + 3.5 * byte(PipePress - 0.04 > BrakeVP()));
			}
		case KE:
			if ((BrakeVP() < CntrlPipePress - 0.03) && (BrakeVP() < PipePress))
				dpPipe = -(1 - (BrakeStatus && b_on)) * SPR(PipePress, BrakeVP()) * (Dim.L * Spg / (Dim.L * Spg + 100.0 * BrakeVVolume)) * dt / BrakeDelay[1 + 2 * byte(BrakeDelayFlag == 1)];
			else                                 //PR(PipePress,BrakeVP)
				dpPipe = (1 - (BrakeStatus && b_on)) * sign(BrakeVP() - PipePress) * PR(PipePress, BrakeVP()) * (Dim.L * Spg / (Dim.L * Spg + 100.0 * BrakeVVolume)) * dt / 75.0;
			if (!TestFlag(BrakeStatus, b_on))
				dpPipe = sign(BrakeVP() - PipePress) * PR(PipePress, BrakeVP()) * (Dim.L*Spg / (Dim.L*Spg + 100 * BrakeVVolume)) * dt / 50.0;
		}        //PR*sign

		dpPipe = dpPipe * 5;
		dpPipe = (dpPipe)*(10.0*BrakeVVolume) / (Spg*Dim.L);
		if ((dpPipe != 0) && ((BrakeVP() - PipePress) / (dpPipe)<2.0))
			dpPipe = (BrakeVP() - PipePress) / 5.0;
		Volume = Volume - dpPipe * Spg * Dim.L / 10.0;

	
		//0.02+Random/140
		dpPipe = (dpPipe - 0.028 * dt / (Dim.L * Spg * 10.0)) / (1 + kL * byte((BrakeCtrlPosNo > 0) && (Power>1) && (BrakeSystem == Pneumatic)));  //nieszczelnosci

		//yB: jednokrokowe liczenie tego wszystkiego
		PipePress = PipePress + dpPipe + dpMainValve;

		if (PipePress < 0) PipePress = 0;

	} //PipePress

	//************* yB: ROZDZIELACZ POWIETRZA *******************************)
	if (BrakeDelayFlag == bdelay_R)
		if (BrakeCtrlPosNo > 0)
			if (Vel > 55)
				SetFlag(BrakeStatus, +b_Rused + b_Ractive);
			else
				SetFlag(BrakeStatus, -b_Ractive);
		else
			if (Vel > 70)
				SetFlag(BrakeStatus, +b_Rused + b_Ractive);
			else if (Vel < 50)
				SetFlag(BrakeStatus, -b_Ractive);



	//else //if (BrakeStatus=b_off) or (BrakeStatus=b_release) then
	if (TestFlag(BrakeStatus, b_on))
	{
		switch (BrakeSubsystem)
		{
		case Oerlikon:
		case Hik:
		case KE:
			if (((PipePress>HighPipePress) && (PPP>HighPipePress)) || (PipePress>CntrlPipePress))
			{
				SetFlag(BrakeStatus, -b_on);
				HighPipePress = 0.00002;
				LowPipePress = 0.00001;
			}

			else
				if (BrakeVP() < PipePress - 0.01)
				{
					BrakeStatus = (BrakeStatus && 254);
					HighPipePress = 0.00002;
					LowPipePress = 0.00001;
				}
				else
					if (dpBrake < 0)
						BrakePress = BrakePress - dpBrake;
		} // switch
	}

	else
	{
		if (PipeBrakePress < 0.0002)
		{
			switch (BrakeSubsystem)
			{
			case Hik:
			case KE:
			{
				if (CntrlPipePress > PipePress)
					CntrlPipePress = CntrlPipePress - PR(PipePress, CntrlPipePress) * dt / 50.0;
				CntrlPipePress = Max0R(CntrlPipePress, BrakeVP());
			}
			case Oerlikon:
			{
				if (CntrlPipePress>PipePress)
					CntrlPipePress = CntrlPipePress - PR(PipePress, CntrlPipePress) * dt / 50.0;
				else
					if ((Power < 1) && (BrakeSystem == Pneumatic))
						CntrlPipePress = Max0R(CntrlPipePress, BrakeVP());
					else
					{
						if ((CabNo != 0) && (BrakeSystem == Pneumatic))
						{
							CntrlPipePress = PipePress;
							if (CntrlPipePress > BrakePressureTable[0].PipePressureVal)
								CntrlPipePress = BrakePressureTable[0].PipePressureVal;
						}
						else
							CntrlPipePress = CntrlPipePress + SPR(PipePress, CntrlPipePress) * dt / 50.0;
					}
			}
			// else
			CntrlPipePress = BrakeVP();
			} // switch

		}

		if ((CntrlPipePress - 0.03 + 0.015 * byte((BrakeSubsystem == Oerlikon) || (BrakeSubsystem == KE)) > PipePress) || (CntrlPipePress - 0.03 + 0.015 * byte((BrakeSubsystem == Oerlikon) || (BrakeSubsystem == KE)) > PPP))
		{
			SetFlag(BrakeStatus, b_on);
			switch (BrakeSubsystem)
			{
			case Oerlikon:
			case Hik:
			case KE:
			{
				HighPipePress = PipePress;
				LowPipePress = PipePress - DeltaPipePress;
			}
			default:
			{
				HighPipePress = BrakeVP();
				LowPipePress = Volume / (10.0 * (BrakeVVolume + BrakeVolume));
				DeltaPipePress = HighPipePress - LowPipePress;
			}
			}

			if ((BrakeCtrlPosNo == 0) && (PipeBrakePress < 0.002) && (BrakeVP() >(CntrlPipePress - 0.005)))
			{
				SetFlag(SoundFlag, +sound_brakeacc);
				PipePress = PipePress * (Dim.L * Spg / (Dim.L * Spg + 0.5));
			}
		}

	}
	//  else
	//{   
	//	if(abs(HighPipePress-PPP)<0.000001)
	//    if (BrakeCtrlPosNo == 0)
	//	{
	//       SetFlag(SoundFlag,sound_brakeacc);
	//       PipePress = PipePress*(Dim.L*Spg/(Dim.L*Spg+0.5));
	//	}
	//}
	//  if (PPP-PipePress)<0 then
	//    Brakestatus:=b_off;
	//************* yB: ROZDZIELACZ POWIETRZA - KONIEC **********************)
	//yB: na chwile zakomentuje, zobaczymy, czy sie na tym sypalo
	//  if(TrainType=dt_ET42)and(DynamicBrakeFlag)then
	//    if(PipePress<0.3)then ScndCtrlActualPos:=255
	//    else if(PipePress>0.45)and(ScndCtrlActualPos=255)then ScndCtrlActualPos:=0;

	if (CompressedVolume < 0)
		CompressedVolume = 0;
*/
	PPP = PipePress;
}  //updatepipepressure}



// ***********************************************************************************************************************
// UpdateScndPipePressure(double dt)
// ***********************************************************************************************************************

void TMoverParameters::UpdateScndPipePressure(double dt)
{
	/*
	const double Spz = 0.5067;
	double  USPP, dpSPipe, dpSpipeBV;
	PMoverParameters c;

	if (VeselVolume > 0)
		USPP = -sign(ScndPipePress - Compressor) * 10 * PR(Compressor, ScndPipePress) * dt;
	else
		USPP = 0;

	ScndPipePress = ScndPipePress + USPP;
	CompressedVolume = CompressedVolume - USPP * Dim.L * Spz / 100.0;

	dpSPipe = 0;

	dpSpipeBV = 0;

	if (((Power > 1) || (BrakeSystem == ElectroPneumatic)) && (BrakeSubsystem == Oerlikon))
		if (BrakeVP() < ScndPipePress)
		{
			dpSpipeBV = sign(BrakeVP() - ScndPipePress) * PR(ScndPipePress, BrakeVP()) * (Dim.L * Spz) / (Dim.L*Spz + BrakeVVolume) * dt / 50.0;
			Volume = Volume - dpSpipeBV * BrakeVVolume * 10.0;
			dpSpipeBV = (dpSpipeBV * 1000.0 * BrakeVVolume) / (Spz * Dim.L);
		}

	ScndPipePress = ScndPipePress + dpSPipe + dpSpipeBV;
	if (ScndPipePress < 0) ScndPipePress = 0;

	*/
}

/* Ukrotnienie EN57:
1 //uk³ad szeregowy
2 //uk³ad równoleg³y
3 //bocznik 1
4 //bocznik 2
5 //bocznik 3
6 //do przodu
7 //do ty³u
8 //1 przyspieszenie
9 //minus obw. 2 przyspieszenia
10 //jazda na oporach
11 //SHP
12A //podnoszenie pantografu przedniego
12B //podnoszenie pantografu tylnego
13A //opuszczanie pantografu przedniego
13B //opuszczanie wszystkich pantografów
14 //za³¹czenie WS
15 //rozrz¹d (WS, PSR, wa³ ku³akowy)
16 //odblok PN
18 //sygnalizacja przetwornicy g³ównej
19 //luzowanie EP
20 //hamowanie EP
21 //rezerwa** (1900+: zamykanie drzwi prawych)
22 //za³. przetwornicy g³ównej
23 //wy³. przetwornicy g³ównej
24 //za³. przetw. oœwietlenia
25 //wy³. przetwornicy oœwietlenia
26 //sygnalizacja WS
28 //sprê¿arka
29 //ogrzewanie
30 //rezerwa* (1900+: zamykanie drzwi lewych)
31 //otwieranie drzwi prawych
32H //zadzia³anie PN siln. trakcyjnych
33 //sygna³ odjazdu
34 //rezerwa (sygnalizacja poœlizgu)
35 //otwieranie drzwi lewych
ZN //masa
*/

double TMoverParameters::ComputeMovement(double dt,double dt1, const TTrackShape &Shape,TTrackParam &Track,TTractionParam &ElectricTraction,
                                         const TLocation &NewLoc,TRotation &NewRot
)
{//trzeba po ma³u przenosiæ tu tê funkcjê
 double d;
//-- T_MoverParameters::ComputeMovement(dt,dt1,Shape,Track,ElectricTraction,NewLoc,NewRot);

 if (Power>1.0) //w rozrz¹dczym nie (jest b³¹d w FIZ!) - Ra 2014-07: teraz we wszystkich
  UpdatePantVolume(dt); //Ra 2014-07: obs³uga zbiornika rozrz¹du oraz pantografów

 if (EngineType==WheelsDriven)
  d=CabNo*dL; //na chwile dla testu
 else
  d=dL;
 DistCounter=DistCounter+fabs(dL)/1000.0;
 dL=0;

 //koniec procedury, tu nastepuja dodatkowe procedury pomocnicze

 //sprawdzanie i ewentualnie wykonywanie->kasowanie poleceñ
 if (LoadStatus>0) //czas doliczamy tylko jeœli trwa (roz)³adowanie
  LastLoadChangeTime=LastLoadChangeTime+dt; //czas (roz)³adunku
 RunInternalCommand();
 //automatyczny rozruch
 if (EngineType==ElectricSeriesMotor)
  if (AutoRelayCheck())
   SetFlag(SoundFlag,sound_relay);


 //--if (EngineType==DieselEngine)
 //-- if (dizel_Update(dt))

    SetFlag(SoundFlag,sound_relay);
 //uklady hamulcowe:
  if (VeselVolume>0)
   Compressor=CompressedVolume/VeselVolume;
  else
  {
   Compressor=0;
   CompressorFlag=false;
  };
 ConverterCheck();
 if (CompressorSpeed>0.0) //sprê¿arka musi mieæ jak¹œ niezerow¹ wydajnoœæ
  CompressorCheck(dt); //¿eby rozwa¿aæ jej za³¹czenie i pracê
 //--UpdateBrakePressure(dt);
 //--UpdatePipePressure(dt);
 //--UpdateBatteryVoltage(dt);
 //--UpdateScndPipePressure(dt); // druga rurka, youBy
 //hamulec antypoœlizgowy - wy³¹czanie
 if ((BrakeSlippingTimer>0.8)&&(ASBType!=128)) //ASBSpeed=0.8
  Hamulec->ASB(0);
 //SetFlag(BrakeStatus,-b_antislip);
 BrakeSlippingTimer=BrakeSlippingTimer+dt;
 //sypanie piasku - wy³¹czone i piasek siê nie koñczy - b³êdy AI
 //if AIControllFlag then
 // if SandDose then
 //  if Sand>0 then
 //   begin
 //     Sand:=Sand-NPoweredAxles*SandSpeed*dt;
 //     if Random<dt then SandDose:=false;
 //   end
 //  else
 //   begin
 //     SandDose:=false;
 //     Sand:=0;
 //   end;
 //czuwak/SHP
 //if (Vel>10) and (not DebugmodeFlag) then
 if (!Global::DebugModeFlag)
  SecuritySystemCheck(dt1);
 return d;
};

double TMoverParameters::FastComputeMovement(double dt,const TTrackShape &Shape,TTrackParam &Track,const TLocation &NewLoc,TRotation &NewRot)
{//trzeba po ma³u przenosiæ tu tê funkcjê
 double d;

//-- T_MoverParameters::FastComputeMovement(dt,Shape,Track,NewLoc,NewRot);

 if (Power>1.0) //w rozrz¹dczym nie (jest b³¹d w FIZ!)
  UpdatePantVolume(dt); //Ra 2014-07: obs³uga zbiornika rozrz¹du oraz pantografów
 if (EngineType==WheelsDriven)
  d=CabNo*dL; //na chwile dla testu
 else
  d=dL;
 DistCounter=DistCounter+fabs(dL)/1000.0;
 dL=0;

 //koniec procedury, tu nastepuja dodatkowe procedury pomocnicze

 //sprawdzanie i ewentualnie wykonywanie->kasowanie poleceñ
 if (LoadStatus>0) //czas doliczamy tylko jeœli trwa (roz)³adowanie
  LastLoadChangeTime=LastLoadChangeTime+dt; //czas (roz)³adunku
 RunInternalCommand();
 //--if (EngineType==DieselEngine)
 //-- if (dizel_Update(dt))
   SetFlag(SoundFlag,sound_relay);

 //uklady hamulcowe:
 if (VeselVolume>0)
  Compressor=CompressedVolume/VeselVolume;
 else
 {
  Compressor=0;
  CompressorFlag=false;
 };
 ConverterCheck();
 if (CompressorSpeed>0.0) //sprê¿arka musi mieæ jak¹œ niezerow¹ wydajnoœæ
  CompressorCheck(dt); //¿eby rozwa¿aæ jej za³¹czenie i pracê
 //--UpdateBrakePressure(dt);
 //--UpdatePipePressure(dt);
 //--UpdateScndPipePressure(dt); // druga rurka, youBy
 //--UpdateBatteryVoltage(dt);

 //hamulec antyposlizgowy - wy³¹czanie
 if ((BrakeSlippingTimer>0.8)&&(ASBType!=128)) //ASBSpeed=0.8
  Hamulec->ASB(0);
 BrakeSlippingTimer=BrakeSlippingTimer+dt;
 return d;
};

double TMoverParameters::ShowEngineRotation(int VehN)
{//pokazywanie obrotów silnika, równie¿ dwóch dalszych pojazdów (3×SN61)
 int b;
 switch (VehN)
 {//numer obrotomierza
  case 1:
   return fabs(enrot);
  case 2:
   for (b=0;b<=1;++b)
    if (TestFlag(Couplers[b].CouplingFlag,ctrain_controll))
     if (Couplers[b].Connected->Power>0.01)
      return fabs(Couplers[b].Connected->enrot);
   break;   
  case 3: //to nie uwzglêdnia ewentualnego odwrócenia pojazdu w œrodku
   for (b=0;b<=1;++b)
    if (TestFlag(Couplers[b].CouplingFlag,ctrain_controll))
     if (Couplers[b].Connected->Power>0.01)
      if (TestFlag(Couplers[b].Connected->Couplers[b].CouplingFlag,ctrain_controll))
       if (Couplers[b].Connected->Couplers[b].Connected->Power>0.01)
        return fabs(Couplers[b].Connected->Couplers[b].Connected->enrot);
   break;
 };
 return 0.0;
};

void TMoverParameters::ConverterCheck()
{//sprawdzanie przetwornicy
 if (ConverterAllow&&Mains)
  ConverterFlag=true;
 else
  ConverterFlag=false;
};



int TMoverParameters::ShowCurrent(byte AmpN)
{//odczyt ampera¿u
	/*
 switch (EngineType)
 {case ElectricInductionMotor:
   switch (AmpN)
   {//do asynchronicznych
    case 1: return WindingRes*Mm/Vadd;
    case 2: return dizel_fill*WindingRes;
    default: return T_MoverParameters::ShowCurrent(AmpN);
   }
  break;
  case DieselElectric:
   return fabs(Im);
  break;
  default:
   return T_MoverParameters::ShowCurrent(AmpN);
 }
 */
	return 200;
};
