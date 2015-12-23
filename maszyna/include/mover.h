//---------------------------------------------------------------------------
// 1166
#ifndef MoverH
#define MoverH
//---------------------------------------------------------------------------
//#include "Mover.hpp"
// Ra: Niestety "_mover.hpp" siê nieprawid³owo generuje - przek³ada sobie TCoupling na sam koniec.
// Przy wszelkich poprawkach w "_mover.pas" trzeba skopiowaæ rêcznie "_mover.hpp" do "mover.hpp" i
// poprawiæ b³êdy! Tak a¿ do wydzielnia TCoupling z Pascala do C++...
// Docelowo obs³ugê sprzêgów (³¹czenie, roz³¹czanie, obliczanie odleg³oœci, przesy³ komend)
// trzeba przenieœæ na poziom DynObj.cpp.
// Obs³ugê silniników te¿ trzeba wydzieliæ do osobnego modu³u, bo ka¿dy osobno mo¿e mieæ poœlizg.
#include "../commons.h"
#include "../commons_usr.h"
#include "dumb3d.h"
#include "track.h"
#include "segment.h"
#include "classes.h"
#include "hamulce.h" 

using namespace Math3D;


class  TMoverParameters;
typedef TMoverParameters* PMoverParameters;  // bylo **



static const bool Go = true;
static const bool Hold = false;
static const short int ResArraySize = 0x40;
static const short int MotorParametersArraySize = 0x8;
static const short int maxcc = 0x4;
//static const short int LocalBrakePosNo = 0xa;
static const short int MainBrakeMaxPos = 0xa;
static const short int dtrack_railwear = 0x2;
static const short int dtrack_freerail = 0x4;
static const short int dtrack_thinrail = 0x8;
static const short int dtrack_railbend = 0x10;
static const short int dtrack_plants = 0x20;
static const short int dtrack_nomove = 0x40;
static const int dtrack_norail = 0x80;
static const short int dtrain_thinwheel = 0x1;
static const short int dtrain_loadshift = 0x1;
static const short int dtrain_wheelwear = 0x2;
static const short int dtrain_bearing = 0x4;
static const short int dtrain_coupling = 0x8;
static const short int dtrain_ventilator = 0x10;
static const short int dtrain_loaddamage = 0x10;
static const short int dtrain_engine = 0x20;
static const short int dtrain_loaddestroyed = 0x20;
static const short int dtrain_axle = 0x40;
static const int dtrain_out = 0x80;
#define p_elengproblem  (1.000000E-02)
#define p_elengdamage  (1.000000E-01)
#define p_coupldmg  (2.000000E-02)
#define p_derail  (1.000000E-03)
#define p_accn  (1.000000E-01)
#define p_slippdmg  (1.000000E-03)
static const short int ctrain_virtual = 0x0;
static const short int ctrain_coupler = 0x1;
static const short int ctrain_pneumatic = 0x2;
static const short int ctrain_controll = 0x4;
static const short int ctrain_power = 0x8;
static const short int ctrain_passenger = 0x10;
static const short int ctrain_scndpneumatic = 0x20;
static const short int ctrain_heating = 0x40;
static const short int dbrake_none = 0x0;
static const short int dbrake_passive = 0x1;
static const short int dbrake_switch = 0x2;
static const short int dbrake_reversal = 0x4;
static const short int dbrake_automatic = 0x8;
//static const short int bdelay_P = 0x0;
//static const short int bdelay_G = 0x1;
//static const short int bdelay_R = 0x2;
//static const short int bdelay_E = 0x4;
//static const short int b_off = 0x0;
//static const short int b_on = 0x1;
//static const short int b_dmg = 0x2;
static const short int b_release = 0x4;
static const short int b_antislip = 0x8;
static const short int b_epused = 0x10;
static const short int b_Rused = 0x20;
static const short int b_Ractive = 0x40;
static const short int bp_classic = 0x0;
static const short int bp_diameter = 0x1;
static const short int bp_magnetic = 0x2;
static const short int s_waiting = 0x1;
static const short int s_aware = 0x2;
static const short int s_active = 0x4;
static const short int s_alarm = 0x8;
static const short int s_ebrake = 0x10;
static const short int sound_none = 0x0;
static const short int sound_loud = 0x1;
static const short int sound_couplerstretch = 0x2;
static const short int sound_bufferclamp = 0x4;
static const short int sound_bufferbump = 0x8;
static const short int sound_relay = 0x10;
static const short int sound_manyrelay = 0x20;
static const short int sound_brakeacc = 0x40;

#define Spg  (5.067000E-01)
extern bool PhysicActivationFlag;
const short int dt_Default = 0x0;
const short int dt_EZT = 0x1;
const short int dt_ET41 = 0x2;
const short int dt_ET42 = 0x4;
const short int dt_PseudoDiesel = 0x8;
const short int dt_ET22 = 0x10;
const short int dt_SN61 = 0x20;
const short int dt_EP05 = 0x40;
const short int dt_ET40 = 0x80;
const short int dt_181 = 0x100;



class TMoverParameters 
{//Ra: wrapper na kod pascalowy, przejmuj¹cy jego funkcje
public:
std::string Name;

double dMoveLen;

TLocation Loc;
TRotation Rot;
TCoupling Couplers[2];                  //urzadzenia zderzno-sprzegowe, polaczenia miedzy wagonami}
int CategoryFlag;                       // 1 - pociag, 2 - samochod, 4 - statek, 8 - samolot
std::string TypeName;                   // nazwa serii/typu
int TrainType;                          //typ: EZT/elektrowoz - Winger 040304
TEngineTypes EngineType;                //typ napedu
TPowerParameters EnginePowerSource;     //zrodlo mocy dla silnikow
TPowerParameters SystemPowerSource;     //zrodlo mocy dla systemow sterowania/przetwornic/sprezarek
TPowerParameters HeatingPowerSource;    //zrodlo mocy dla ogrzewania
TPowerParameters AlterHeatPowerSource;  //alternatywne zrodlo mocy dla ogrzewania
TPowerParameters LightPowerSource;      //zrodlo mocy dla oswietlenia
TPowerParameters AlterLightPowerSource; //alternatywne mocy dla oswietlenia
double Vmax;                            //max. predkosc kontrukcyjna, 
double Mass;                            //masa wlasna,
double Power;                           //moc
double Mred;                            //Ra: zredukowane masy wiruj¹ce; potrzebne do obliczeñ hamowania
double TotalMass;                       //wyliczane przez ComputeMass
double HeatingPower;                    //moc pobierana na ogrzewanie
double LightPower;                      //moc pobierana na oswietlenie
int BatteryVoltage;                     //Winger - baterie w elektrykach}
TDimension Dim;                         //wymiary
double Cx;                              //wsp. op. aerodyn.
double WheelDiameter;                   //srednica kol napednych
double TrackW;                          //nominalna szerokosc toru [m]
double AxleInertialMoment;              //moment bezwladnosci zestawu kolowego
std::string AxleArangement;             //uklad osi np. Bo'Bo' albo 1'C
int NPoweredAxles;                      //ilosc osi napednych liczona z powyzszego
int NAxles;                             //ilosc wszystkich osi j.w.
int BearingType;                        //lozyska: 0 - slizgowe, 1 - toczne
double ADist;                           //odlegosc osi oraz czopow skretu
double BDist;
int NBpA;                               //ilosc el. ciernych na os: 0 1 2 lub 4
int SandCapacity;                       //zasobnik piasku [kg]}
TBrakeSystem BrakeSystem;               //rodzaj hamulca zespolonego
TBrakeSubsystem BrakeSubsystem;
double MBPM;                            //masa najwiekszego cisnienia
Hamulce::TBrake* Hamulec;
Hamulce::THandle* Handle;
Hamulce::THandle* LocHandle;
Hamulce::TReservoir* Pipe;
Hamulce::TReservoir* Pipe2;
TLocalBrake LocalBrake;                 //rodzaj hamulca indywidualnego
TBrakePressure BrakePressureTable[13];  //wyszczegolnienie cisnien w rurze
TBrakePressure BrakePressureActual;
int ASBType;                            //0: brak hamulca przeciwposlizgowego, 1: reczny, 2: automat
int TurboTest;
double MaxBrakeForce;                   //maksymalna sila nacisku hamulca
double MaxBrakePress;
double P2FTrans;
double TrackBrakeForce;                 //sila nacisku hamulca szynowego
BYTE BrakeMethod;                        //flaga rodzaju hamulca
double HighPipePress;                   //max. robocze cisnienie w przewodzie glownym
double LowPipePress;                    //min. robocze cisnienie w przewodzie glownym 
double DeltaPipePress;                  //roznica miedzy max i min
double CntrlPipePress;                  //cisnienie z zbiorniku sterujacym
double BrakeVolume;                     //pojemnosc powietrza w ukladzie hamulcowym, w ukladzie glownej sprezarki [m^3] }
double BrakeVVolume;
double VeselVolume;
int BrakeCylNo;                         //ilosc cylindrow ham.
double BrakeCylRadius;
double BrakeCylDist;
double BrakeCylMult[4];
int BCMFlag;                            //promien cylindra, skok cylindra, przekladnia hamulcowa
double MinCompressor;                   //cisnienie wlaczania, zalaczania sprezarki
double MaxCompressor;
double CompressorSpeed;                 //wydajnosc sprezarki
double BrakeDelay[4];                   //opoznienie hamowania/odhamowania t/o
int BrakeCtrlPosNo;                     //ilosc pozycji hamulca}
int MainCtrlPosNo;                      //ilosc pozycji nastawnika}
int ScndCtrlPosNo;                      //ilosc pozycji bocznika
bool ScndInMain;                        //zaleznosc bocznika od nastawnika
TSecuritySystem SecuritySystem;
TScheme RList[65];                      //lista rezystorow rozruchowych i polaczen silnikow, dla dizla: napelnienia}
int RlistSize;
TMotorParameters MotorParam[9];         //rozne parametry silnika przy bocznikowaniach
mover__2 Transmision;
double NominalVoltage;                  //nominalne napiecie silnika
double WindingRes;
double CircuitRes;                      //rezystancje silnika i obwodu
int IminLo;                             //prady przelacznika automatycznego rozruchu, uzywane tez przez ai_driver}
int IminHi;
int ImaxLo;                             //maksymalny prad niskiego i wysokiego rozruchu
int ImaxHi;
double nmax;                            //maksymalna dop. ilosc obrotow /s
double InitialCtrlDelay;                //ile sek. opoznienia po wl. silnika
double CtrlDelay;                       //ile sek. opoznienia miedzy kolejnymi poz.
int AutoRelayType;                      //0 -brak, 1 - jest, 2 - opcja
bool CoupledCtrl;                       //czy mainctrl i scndctrl sa sprzezone
//TCouplerNr CouplerNr[2];
bool IsCoupled;                         //czy jest sprzezony ale jedzie z tylu
int DynamicBrakeType;
int RVentType;                          //0 - brak, 1 - jest, 2 - automatycznie wlaczany
double RVentnmax;                       //maks. obroty wentylatorow oporow rozruchowych
double RVentCutOff;                     //rezystancja wylaczania wentylatorow dla RVentType=2
int CompressorPower;                    //0: bezp. z obwodow silnika, 1: z przetwornicy, reczne, 2: w przetwornicy, stale}
int SmallCompressorPower;               //Winger ZROBIC
double dizel_Mmax;
double dizel_nMmax;
double dizel_Mnmax;
double dizel_nmax;
double dizel_nominalfill;
double dizel_Mstand;
double dizel_nmax_cutoff;
double dizel_nmin;
double dizel_minVelfullengage;
double dizel_AIM;
double dizel_engageDia;
double dizel_engageMaxForce;
double dizel_engagefriction;
double AnPos;                           // pozycja sterowania dokladnego (analogowego)
bool AnalogCtrl;
bool AnMainCtrl;
bool ShuntModeAllow;
bool ShuntMode;
bool Flat;
double Vhyp;
TDEScheme DElist[33];
double Vadd;
TMPTRelay MPTRelay[8];
int RelayType;
TShuntScheme SST[33];
double PowerCorRatio;                   //Wspolczynnik korekcyjny    
double Ftmax;
int MaxLoad;                            //masa w T lub ilosc w sztukach - ladownosc
std::string LoadAccepted;               //co moze byc zaladowane, 
std::string LoadQuantity;               //jednostki miary ladunku
double OverLoadFactor;                  //ile razy moze byc przekroczona ladownosc}
double LoadSpeed;                       //szybkosc naladunku jednostki/s}
double UnLoadSpeed;                     //szybkosc rozladunku jednostki/s}
int DoorOpenCtrl;                       //0: przez pasazera, 1: przez maszyniste, 2: samoczynne (zamykanie)
int DoorCloseCtrl;
double DoorStayOpen;                    //jak dlugo otwarte w przypadku DoorCloseCtrl=2
bool DoorClosureWarning;                //czy jest ostrzeganie przed zamknieciem
double DoorOpenSpeed;                   //predkosc otwierania 
double DoorCloseSpeed;                  //i zamykania w j.u. 
double DoorMaxShiftL;                   //szerokosc otwarcia lub kat
double DoorMaxShiftR;
int DoorOpenMethod;                     //sposob otwarcia - 1: przesuwne, 2: obrotowe

int ScanCounter;                        //pomocnicze do skanowania sprzegow
bool EventFlag;                         //!o True jesli cos nietypowego sie wydarzy
int SoundFlag;                          //!o patrz stale sound_ 
double DistCounter;                     //! licznik kilometrow 
double DistCounterD;                    //LICZNIK DNIOWY 
double DriveTime;                       //CZAS JAZDY NALICZANY W SEKUNDACH W WORLD.CPP 
double MaxVelo;                         //NAJWIEKSZA UZYSKANA PREDKOSC 
double V;                               //predkosc w m/s
double Vel;                             //predkosc w km/h
double AccS;                            //przyspieszenie styczne
double AccN;                            //przyspieszenie normalne w m/s^2
double AccV;
double nrot;                            //! rotacja kol [obr/s]
double EnginePower;                     //! chwilowa moc silnikow 
double dL;                              //przesuniecie pojazdu 
double Fb;                              //sila hamowania
double Ff;                              //sila tarcia
double FTrain;                          //! sila pociagowa
double FStand;                          // sila oporow ruchu
double FTotal;                          //! calkowita sila dzialajaca na pojazd
double UnitBrakeForce;                  //!s si³a hamowania przypadaj¹ca na jeden element
bool SlippingWheels;                    //! poslizg kol,
bool SandDose;                          //sypanie piasku
double Sand;                            //ilosc piasku
double BrakeSlippingTimer;              //pomocnicza zmienna do wylaczania przeciwposlizgu
double dpBrake;
double dpPipe;
double dpMainValve;
double dpLocalValve;
double ScndPipePress;                   //cisnienie w przewodzie zasilajacym
double BrakePress;                      //!o cisnienie w cylindrach hamulcowych
double LocBrakePress;                   //!o cisnienie w cylindrach hamulcowych z pomocniczego
double PipeBrakePress;                  //!o cisnienie w cylindrach hamulcowych z przewodu
double PipePress;                       //!o cisnienie w przewodzie glownym
double PPP;                             //!o poprzednie cisnienie w przewodzie glownym
double Volume;                          //objetosc spr. powietrza w zbiorniku hamulca
double CompressedVolume;                //objetosc spr. powietrza w ukl. zasilania
double PantVolume;                      //objetosc spr. powietrza w ukl. pantografu
double Compressor;                      //! cisnienie w ukladzie zasilajacym
bool CompressorFlag;                    //!o czy wlaczona sprezarka
bool PantCompFlag;                      //!o czy wlaczona sprezarka pantografow
bool CompressorAllow;                   //! zezwolenie na uruchomienie sprezarki  NBMX
bool ConverterFlag;                     //!  czy wlaczona przetwornica NBMX
bool ConverterAllow;                    //zezwolenie na prace przetwornicy NBMX
int BrakeCtrlPos;                       //nastawa hamulca zespolonego
int LocalBrakePos;                      //nastawa hamulca indywidualnego
int BrakeStatus;                        //0 - odham, 1 - ham., 2 - uszk., 4 - odluzniacz, 8 - antyposlizg, 16 - uzyte EP, 32 - pozycja R, 64 - powrot z R}
bool EmergencyBrakeFlag;                //hamowanie nagle
int BrakeDelayFlag;                     //nastawa opoznienia ham. osob/towar/posp/exp 0/1/2/4
int BrakeDelays;                        //nastawy mozliwe do uzyskania
bool DynamicBrakeFlag;                  //czy wlaczony hamulec elektrodymiczny
double LimPipePress;                    //stabilizator cisnienia
double ActFlowSpeed;                    //szybkosc stabilizatora
int DamageFlag;                         //kombinacja bitowa stalych dtrain_* 
int DerailReason;                       //przyczyna wykolejenia
TCommand CommandIn;
std::string CommandOut;
std::string CommandLast;                //Ra: ostatnio wykonana komenda do podgl¹du
double ValueOut;                        //argument komendy która ma byc przekazana na zewnatrz
TTrackShape RunningShape;               //geometria toru po ktorym jedzie pojazd
TTrackParam RunningTrack;               //parametry toru po ktorym jedzie pojazd
double OffsetTrackH;                    //przesuniecie poz. i pion. w/m toru
double OffsetTrackV;
bool Mains;                             //polozenie glownego wylacznika
int MainCtrlPos;                        //polozenie glownego nastawnika
int ScndCtrlPos;                        //polozenie dodatkowego nastawnika
int ActiveDir;                          //czy lok. jest wlaczona i w ktorym kierunku:
int CabNo;                              //! numer kabiny: 1 lub -1. W przeciwnym razie brak sterowania - rozrzad
int DirAbsolute;                        //zadany kierunek jazdy wzglêdem sprzêgów (1=w strone 0,-1=w stronê 1)
int ActiveCab;                          //! numer kabiny, w ktorej sie jest
int LastCab;                            //numer kabiny przed zmiana 
double LastSwitchingTime;               //czas ostatniego przelaczania czegos
//int WarningSignal;                      //0: nie trabi, 1,2: trabi
bool DepartureSignal;                   //sygnal odjazdu
bool InsideConsist;
TTractionParam RunningTraction;         //parametry sieci trakcyjnej najblizej lokomotywy
double enrot;                           //ilosc obrotow
double Im;                              //prad silnika
double Itot;                            //prad calkowity
double Mm;
double Mw;
double Fw;
double Ft;
int Imin;                               //prad przelaczania automatycznego rozruchu
int Imax;                               //prad bezpiecznika
double Voltage;                         //aktualne napiecie sieci zasilajacej
int MainCtrlActualPos;                  //wskaznik Rlist
int ScndCtrlActualPos;                  //wskaznik MotorParam
bool DelayCtrlFlag;                     //opoznienie w zalaczaniu
double LastRelayTime;                   //czas ostatniego przelaczania stycznikow
bool AutoRelayFlag;                     //mozna zmieniac jesli AutoRelayType=2
bool FuseFlag;                          //!o bezpiecznik nadmiarowy
bool StLinFlag;                         //!o styczniki liniowe
bool ResistorsFlag;                     //!o jazda rezystorowa
double RventRot;                        //!s obroty wentylatorow rozruchowych
bool UnBrake;                           //w EZT - nacisniete odhamowywanie


//-zmienne dla lokomotywy spalinowej z przekladnia mechaniczna
double dizel_fill;
double dizel_engagestate;
double dizel_engage;
double dizel_automaticgearstatus;
bool dizel_enginestart;
double dizel_engagedeltaomega;

//-zmienne dla drezyny recznej
double PulseForce;                      //przylozona sila
double PulseForceTimer;
int PulseForceCount;

double eAngle;
int Load;                               //masa w T lub ilosc w sztukach - zaladowane
std::string LoadType;                   //co jest zaladowane
int LoadStatus;                         //+1=trwa rozladunek,+2=trwa zaladunek,+4=zakoñczono,0=zaktualizowany model
double LastLoadChangeTime;              //ostatni raz (roz)³adowania
bool DoorLeftOpened;                    //stan drzwi lewych
bool DoorRightOpened;                   //stan drzwi prawych
bool PantFrontUp;
bool PantRearUp;
bool PantFrontSP;
bool PantRearSP;
int PantFrontStart;
int PantRearStart;
bool PantFrontVolt;
bool PantRearVolt;
std::string PantSwitchType;
std::string ConvSwitchType;
bool Heating;                           //ogrzewanie 'Winger 020304
int DoubleTr;                           //trakcja ukrotniona - przedni pojazd 'Winger 160304
bool PhysicActivation;
double FrictConst1;
double FrictConst2s;
double FrictConst2d;
double TotalMassxg;                     //masa calkowita pomnozona przez przyspieszenie ziemskie


 vector3 vCoulpler[2]; //powtórzenie wspó³rzêdnych sprzêgów z DynObj :/
 vector3 DimHalf; //po³owy rozmiarów do obliczeñ geometrycznych
 //int WarningSignal; //0: nie trabi, 1,2: trabi syren¹ o podanym numerze
 unsigned char WarningSignal; //tymczasowo 8bit, ze wzglêdu na funkcje w MTools
 double fBrakeCtrlPos; //p³ynna nastawa hamulca zespolonego
 bool bPantKurek3; //kurek trójdrogowy (pantografu): true=po³¹czenie z ZG, false=po³¹czenie z ma³¹ sprê¿ark¹
 int iProblem; //flagi problemów z taborem, aby AI nie musia³o porównywaæ; 0=mo¿e jechaæ
 int iLights[2]; //bity zapalonych œwiate³ tutaj, ¿eby da³o siê liczyæ pobór pr¹du 

 double PantPress;
 bool Battery;
 double NominalBatteryVoltage;
 double BrakeCtrlPosR;
 double BrakeCtrlPos2;
private:
 double CouplerDist(byte Coupler);
public:
 TMoverParameters(double VelInitial,std::string TypeNameInit,std::string NameInit, int LoadInitial,std::string LoadTypeInitial,int Cab);
 //obs³uga sprzêgów
 double Distance(const TLocation &Loc1,const TLocation &Loc2,const TDimension &Dim1,const TDimension &Dim2);
 double Distance(const vector3 &Loc1,const vector3 &Loc2,const vector3 &Dim1,const vector3 &Dim2);
 bool Attach(byte ConnectNo,byte ConnectToNr,TMoverParameters *ConnectTo,byte CouplingType,bool Forced=false);
// bool Attach(byte ConnectNo,byte ConnectToNr,T_MoverParameters *ConnectTo,byte CouplingType,bool Forced=false);
 int DettachStatus(byte ConnectNo);
 bool Dettach(byte ConnectNo);
 void SetCoupleDist();
 bool DirectionForward();
 void BrakeLevelSet(double b);
 bool BrakeLevelAdd(double b);
 bool IncBrakeLevel(); //wersja na u¿ytek AI
 bool DecBrakeLevel();
 bool ChangeCab(int direction);
 bool CurrentSwitch(int direction);
 void UpdateBatteryVoltage(double dt);
 double ComputeMovement(double dt,double dt1,const TTrackShape &Shape,TTrackParam &Track,TTractionParam &ElectricTraction, 
 const TLocation &NewLoc,TRotation &NewRot );
 double FastComputeMovement (double dt,const TTrackShape &Shape,TTrackParam &Track,const	TLocation &NewLoc,TRotation &NewRot);
 double ShowEngineRotation(int VehN);
 bool GetTrainsetVoltage(void);
 bool Physic_ReActivation(void);
 double LocalBrakeRatio(void);
 //double ManualBrakeRatio(void);
 double PipeRatio(void);
 double RealPipeRatio(void);
 double BrakeVP(void);
 //bool DynamicBrakeSwitch(bool Switch);
 //bool SendCtrlBroadcast(std::string CtrlCommand, double ctrlvalue);
 bool SendCtrlToNext(std::string CtrlCommand, double ctrlvalue, double dir);
 //bool CabActivisation(void);
 //bool CabDeactivisation(void);
 //bool IncMainCtrl(int CtrlSpeed);
 //bool DecMainCtrl(int CtrlSpeed);
 //bool IncScndCtrl(int CtrlSpeed);
 //bool DecScndCtrl(int CtrlSpeed);
 //bool AddPulseForce(int Multipler);
 //bool SandDoseOn(void);
 //bool SecuritySystemReset(void);
 void SecuritySystemCheck(double dt);
 //bool BatterySwitch(bool State);
 //bool EpFuseSwitch(bool State);
 //bool IncBrakeLevelOld(void);
 //bool DecBrakeLevelOld(void);
 //bool IncLocalBrakeLevel(byte CtrlSpeed);
 //bool DecLocalBrakeLevel(byte CtrlSpeed);
 //bool IncLocalBrakeLevelFAST(void);
 //bool DecLocalBrakeLevelFAST(void);
 //bool IncManualBrakeLevel(byte CtrlSpeed);
 //bool DecManualBrakeLevel(byte CtrlSpeed);
 bool EmergencyBrakeSwitch(bool Switch);
 //bool AntiSlippingBrake(void);
 //bool BrakeReleaser(byte state);
 //bool SwitchEPBrake(byte state);
 //bool AntiSlippingButton(void);
 bool IncBrakePress(double &brake, double PressLimit, double dp);
 bool DecBrakePress(double &brake, double PressLimit, double dp);
 //bool BrakeDelaySwitch(byte BDS);
 //bool IncBrakeMult(void);
 //bool DecBrakeMult(void);
 void UpdateBrakePressure(double dt);
 void UpdatePipePressure(double dt);
 void CompressorCheck(double dt);
 void UpdatePantVolume(double dt);
 void UpdateScndPipePressure(double dt);
 //void UpdateBatteryVoltage(double dt);
 //double GetDVc(double dt);
 //void ComputeConstans(void);
 //double ComputeMass(void);
 //double Adhesive(double staticfriction);
 //double TractionForce(double dt);
 //double FrictionForce(double R, byte TDamage);
 //double BrakeForce(const TTrackParam &Track);
 //double CouplerForce(byte CouplerN, double dt);
 //void CollisionDetect(byte CouplerN, double dt);
 //double ComputeRotatingWheel(double WForce, double dt, double n);
 bool SetInternalCommand(std::string NewCommand, double NewValue1, double NewValue2);
 //double GetExternalCommand(std::string &Command);
 bool RunCommand(std::string command, double CValue1, double CValue2);
 bool RunInternalCommand(void);
 //void PutCommand(std::string NewCommand, double NewValue1, double NewValue2, const TLocation
 //	&NewLocation);
 //bool DirectionBackward(void);
 bool MainSwitch(bool State);
 //bool ConverterSwitch(bool State);
 //bool CompressorSwitch(bool State);
 void ConverterCheck();
 //bool FuseOn(void);
 //bool FuseFlagCheck(void);
 //void FuseOff(void);
 int ShowCurrent(byte AmpN);
 //double v2n(void);
 //double current(double n, double U);
 //double Momentum(double I);
 //double MomentumF(double I, double Iw, byte SCP);
 //bool CutOffEngine(void);
 bool MaxCurrentSwitch(bool State);
 //bool ResistorsFlagCheck(void);
 bool MinCurrentSwitch(bool State);
 //bool AutoRelaySwitch(bool State);
 bool AutoRelayCheck(void);
 //bool dizel_EngageSwitch(double state);
 //bool dizel_EngageChange(double dt);
 //bool dizel_AutoGearCheck(void);
 //double dizel_fillcheck(byte mcp);
 //double dizel_Momentum(double dizel_fill, double n, double dt);
 //bool dizel_Update(double dt);
 //bool LoadingDone(double LSpeed, std::string LoadInit);
 //void ComputeTotalForce(double dt, double dt1, bool FullVer);
 //double ComputeMovement(double dt, double dt1, const TTrackShape &Shape, TTrackParam &Track
 //	, TTractionParam &ElectricTraction, const TLocation &NewLoc, TRotation &NewRot);
 //double FastComputeMovement(double dt, const TTrackShape &Shape, TTrackParam &Track, const
 //	TLocation &NewLoc, TRotation &NewRot);
 //bool ChangeOffsetH(double DeltaOffset);
 //T_MoverParameters(double VelInitial, std::string TypeNameInit, std::string NameInit, int LoadInitial
 //	, std::string LoadTypeInitial, int Cab);
 //bool LoadChkFile(std::string chkpath);
 //bool CheckLocomotiveParameters(bool ReadyFlag, int Dir);
 //std::string EngineDescription(int what);
 //bool DoorLeft(bool State);
 //bool DoorRight(bool State);
 //bool DoorBlockedFlag(void);
 //bool PantFront(bool State);
 //bool PantRear(bool State);
 //
};

#endif
