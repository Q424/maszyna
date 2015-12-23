//---------------------------------------------------------------------------
//TODO: przepisac tu 1161
#ifndef DynObjH
#define DynObjH
#include "../commons.h"
#include "../commons_usr.h"

//#include "QueryParserComp.hpp"
#include "model3d.h"
#include "AnimModel.h"
#include "Classes.h"
#include "trackfollower.h"
#include "parser.h"
#include "mover.h"
#include "mtable.h"
#include "TractionPower.h"
//McZapkie:
#include "mdlmngr.h"
#include "RealSound.h"
#include "AdvSound.h"
#include "button.h"
#include "aircoupler.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const int ANIM_TYPES = 7; //Ra: ilo�� typ�w animacji
const int ANIM_WHEELS = 0; //ko�a
const int ANIM_DOORS = 1; //drzwi
const int ANIM_LEVERS = 2; //elementy obracane (wycieraczki, ko�a skr�tne, przestawiacze, klocki ham.)
const int ANIM_BUFFERS = 3; //elementy przesuwane (zderzaki)
const int ANIM_BOOGIES = 4; //w�zki (s� skr�cane w dw�ch osiach)
const int ANIM_PANTS = 5; //pantografy
const int ANIM_STEAMS = 6; //nap�d parowozu

class TAnim;
typedef void ( *TUpdate)(TAnim *pAnim); //typ funkcji aktualizuj�cej po�o�enie submodeli

//McZapkie-250202
  const int MaxAxles=16; //ABu 280105: zmienione z 8 na 16
  const int MaxAnimatedAxles=16; //i to tez.
  const int MaxAnimatedDoors=8;  //NBMX  wrzesien 2003
  /*
  Ra: Utworzy� klas� wyposa�enia opcjonalnego, z kt�rej b�d� dziedziczy� klasy drzwi,
  pantograf�w, nap�du parowozu i innych ruchomych cz�ci pojazd�w. Klasy powinny by�
  pseudo-wirtualne, bo wirtualne mog� obni�a� wydajnos��.
  Przy wczytywaniu MMD utworzy� tabel� wska�nikow na te dodatki. Przy wy�wietlaniu
  pojazdu wykonywa� Update() na kolejnych obiektach wyposa�enia.
  Rozwa�y� u�ycie oddzielnych modeli dla niekt�rych pojazd�w (np. lokomotywy), co
  zaoszcz�dzi�o by czas ustawiania animacji na modelu wsp�lnym dla kilku pojazd�w,
  szczeg�lnie dla pojazd�w w danej chwili nieruchomych (przy du�ym zag�szczeniu
  modeli na stacjach na og� przewaga jest tych nieruchomych).
  */
class TController;

class TAnimValveGear
{//wsp�czynniki do animacji parowozu (warto�ci przyk�adowe dla Pt47)
	int iValues; //ilo�� liczb (wersja):
	float fKorbowodR; //d�ugo�� korby (p� skoku t�oka) [m]: 0.35
	float fKorbowodL; //d�ugo�� korbowodu [m]: 3.8
	float fDrazekR;   //promie� mimo�rodu (dr��ka) [m]: 0.18
	float fDrazekL;   //d�. dr��ka mimo�rodowego [m]: 2.55889
	float fJarzmoV;   //wysoko�� w pionie osi jarzma od osi ko�a [m]: 0.751
	float fJarzmoH;   //odleg�o�� w poziomie osi jarzma od osi ko�a [m]: 2.550
	float fJarzmoR;   //promie� jarzma do styku z dr��kiem [m]: 0.450
	float fJarzmoA;   //k�t mimo�rodu wzgl�dem k�ta ko�a [�]: -96.77416667
	float fWdzidloL;  //d�ugo�� wodzid�a [m]: 2.0
	float fWahaczH;   //d�ugo�� wahacza (g�ra) [m]: 0.14
	float fSuwakH;    //wysoko�� osi suwaka ponad osi� ko�a [m]: 0.62
	float fWahaczL;   //d�ugo�� wahacza (d�) [m]: 0.84
	float fLacznikL;  //d�ugo�� ��cznika wahacza [m]: 0.75072
	float fRamieL;    //odleg�o�� ramienia krzy�ulca od osi ko�a [m]: 0.192
	float fSuwakL;    //odleg�o�� �rodka t�oka/suwaka od osi ko�a [m]: 5.650
	//do�o�y� parametry dr��ka nastawnicy
	//albo nawet zrobi� dynamiczn� tablic� float[] i w ni� pakowa� wszelkie wsp�czynniki, potem u�ywa� indeks�w
	//wsp�czynniki mog� by� wsp�lne dla 2-4 t�ok�w, albo ka�dy t�ok mo�e mie� odr�bne
};

class TAnimPant
{//wsp�czynniki do animacji pantografu
public:
	vector3 vPos; //Ra: wsp�rz�dne punktu zerowego pantografu (X dodatnie dla przedniego)
	double fLenL1; //d�ugo�� dolnego ramienia 1, odczytana z modelu
	double fLenU1; //d�ugo�� g�rnego ramienia 1, odczytana z modelu
	double fLenL2; //d�ugo�� dolnego ramienia 2, odczytana z modelu
	double fLenU2; //d�ugo�� g�rnego ramienia 2, odczytana z modelu
	double fHoriz; //przesuni�cie �lizgu w d�ugo�ci pojazdu wzgl�dem osi obrotu dolnego ramienia
	double fHeight; //wysoko�� �lizgu ponad o� obrotu, odejmowana od wysoko�ci drutu
	double fWidth; //po�owa szeroko�ci roboczej �lizgu, do wykrycia ze�lizgni�cia si� drutu
	double fAngleL0; //Ra: pocz�tkowy k�t dolnego ramienia (odejmowany przy animacji)
	double fAngleU0; //Ra: pocz�tkowy k�t g�rnego ramienia (odejmowany przy animacji)
	double PantTraction; //Winger 170204: wysoko�� drutu ponad punktem na wysoko�ci vPos.y p.g.s.
	double PantWys; //Ra: aktualna wysoko�� uniesienia �lizgu do por�wnania z wysoko�ci� drutu
	double fAngleL; //Winger 160204: aktualny k�t ramienia dolnego
	double fAngleU; //Ra: aktualny k�t ramienia g�rnego
	double NoVoltTime; //czas od utraty kontaktu z drutem
	TTraction *hvPowerWire; //aktualnie podczepione druty, na razie tu
	float fWidthExtra; //dodatkowy rozmiar poziomy poza cz�� robocz� (fWidth)
	float fHeightExtra[5]; //�amana symuluj�ca kszta�t nabie�nika
	//double fHorizontal; //Ra 2015-01: po�o�enie drutu wzgl�dem osi pantografu 
	void __fastcall AKP_4E();
};

class TAnim
{//klasa animowanej cz�ci pojazdu (ko�a, drzwi, pantografy, burty, nap�d parowozu, si�owniki itd.)
public:
	union
	{
		TSubModel *smAnimated; //animowany submodel (je�li tylko jeden, np. o�)
		TSubModel **smElement; //je�li animowanych element�w jest wi�cej (pantograf, nap�d parowozu)
		int iShift; //przesuni�cie przed przydzieleniem wska�nika
	};
	union
	{//parametry animacji
		TAnimValveGear *pValveGear; //wsp�czynniki do animacji parowozu
		double *dWheelAngle; //wska�nik na k�t obrotu osi
		float *fParam; //r�ne parametry dla animacji
		TAnimPant *fParamPants; //r�ne parametry dla animacji
	};
	union
	{//wska�nik na obiekt odniesienia
		double *fDoubleBase; //jaki� double w fizyce
		float *fFloatBase; //jaki� float w fizyce
		int *iIntBase; //jaki� int w fizyce
	};
	//void _fastcall Update(); //wska�nik do funkcji aktualizacji animacji
	int iFlags; //flagi animacji
	float fMaxDist; //do jakiej odleg�o�ci wykonywana jest animacja
	float fSpeed; //parametr szybko�ci animacji
	int iNumber; //numer kolejny obiektu
public:
	__fastcall TAnim();
	__fastcall ~TAnim();
	TUpdate yUpdate; //metoda TDynamicObject aktualizuj�ca animacj�
	int __fastcall TypeSet(int i, int fl = 0); //ustawienie typu
	void __fastcall Parovoz(); //wykonanie oblicze� animacji
};


// *************************************************************************************************************************************
class TDynamicObject
{
private:
    TTrackShape ts;
    TTrackParam tp;
    void ABuLittleUpdate(double ObjSqrDist);
    bool btnOn; //ABu: czy byly uzywane buttony, jesli tak, to po renderingu wylacz

                //bo ten sam model moze byc jeszcze wykorzystany przez inny obiekt!
 double ComputeRadius(vector3 p1,vector3 p2,vector3 p3,vector3 p4);
 //vector3 pOldPos1; //Ra: nie u�ywane
 //vector3 pOldPos4;
 vector3 vPosition; //Ra: pozycja pojazdu liczona zaraz po przesuni�ciu
//McZapkie-050402 - do krecenia kolami
    int iAnimatedAxles;
    int iAnimatedDoors;
    double DoorSpeedFactor[MaxAnimatedDoors];
    double tempdoorfactor;
    double tempdoorfactor2;
    double pantspeedfactor;
    TSubModel *smAnimatedWheel[MaxAnimatedAxles];
    TSubModel *smAnimatedDoor[MaxAnimatedDoors];
    TButton btCoupler1;     //sprzegi
    TButton btCoupler2;
    TAirCoupler btCPneumatic1;  //sprzegi powietrzne //yB - zmienione z Button na AirCoupler - krzyzyki
    TAirCoupler btCPneumatic2;
    TAirCoupler btCPneumatic1r; //ABu: to zeby nie bylo problemow przy laczeniu wagonow,
    TAirCoupler btCPneumatic2r; //     jesli beda polaczone sprzegami 1<->1 lub 0<->0
    TAirCoupler btPneumatic1;   //ABu: sprzegi powietrzne zolte
    TAirCoupler btPneumatic2;
    TAirCoupler btPneumatic1r;   //ABu: analogicznie jak 4 linijki wyzej
    TAirCoupler btPneumatic2r;

    TButton btCCtrl1;       //sprzegi sterowania
    TButton btCCtrl2;
    TButton btCPass1;       //mostki przejsciowe
    TButton btCPass2;
    TButton btEndSignals11;  //sygnalu konca pociagu
    TButton btEndSignals13;
    TButton btEndSignals21;
    TButton btEndSignals23;
    TButton btEndSignals1;   //zeby bylo kompatybilne ze starymi modelami...
    TButton btEndSignals2;
    TButton btEndSignalsTab1;  //sygnaly konca pociagu (blachy)
    TButton btEndSignalsTab2;
    TButton btHeadSignals11;  //oswietlenie czolowe - przod
    TButton btHeadSignals12;
    TButton btHeadSignals13;
    TButton btHeadSignals21;  //oswietlenie czolowe - tyl
    TButton btHeadSignals22;
    TButton btHeadSignals23;
//Winger 160204 - pantografy
    TSubModel *smPatykird1[2];
    TSubModel *smPatykird2[2];
    TSubModel *smPatykirg1[2];
    TSubModel *smPatykirg2[2];
    TSubModel *smPatykisl[2];
    TSubModel *smWiazary[2];
    TSubModel *smWahacze[4];
    double fWahaczeAmp;
    TSubModel *smMechanik;
    TSubModel *smBuforLewy[2];
    TSubModel *smBuforPrawy[2];
    double enginevolume; //MC: pomocnicze zeby gladziej silnik buczal

    int iAxles; //McZapkie: to potem mozna skasowac i zastapic iNumAxles
    double dRailLength;
    double dRailPosition[MaxAxles];   //licznik pozycji osi w/m szyny
    double dWheelsPosition[MaxAxles]; //pozycja osi w/m srodka pojazdu
//McZapkie-270202
    //TRealSound rsStukot[MaxAxles];   //dzwieki poszczegolnych osi
//McZapkie-010302 - silnik
    //TRealSound rsSilnik;
//McZapkie-030302
    //TRealSound rsWentylator;
//McZapkie-260302
    //TRealSound rsPisk;
//McZapkie-051202
    //TRealSound rsDerailment;

    //TAdvancedSound sHorn1;
    //TAdvancedSound sHorn2;
//NBMX wrzesien 2003
    //TAdvancedSound sCompressor;
    //TAdvancedSound sConverter;
    //TAdvancedSound sSmallCompressor;
    //TAdvancedSound sDepartureSignal;
    //TAdvancedSound sTurbo;
//Winger 010304
//    //TRealSound rsPanTup; //PSound sPantUp;
    //TRealSound sPantUp;
    //TRealSound sPantDown;

    double eng_vol_act;
    double eng_frq_act;
    double eng_dfrq;
    double eng_turbo;
    //ABuWozki 060504
    vector3 bogieRot[2];   //Obroty wozkow w/m korpusu
    TSubModel *smBogie[2]; //Wyszukiwanie max 2 wozkow
    void ABuBogies();
    void ABuModelRoll();
    vector3 modelShake;

    bool renderme; //yB - czy renderowac
    char cp1, sp1, cp2, sp2; //ustawienia wezy
    //TRealSound sBrakeAcc; //dzwiek przyspieszacza
 int iAxleFirst; //numer pierwszej o� w kierunku ruchu
 int iInventory; //flagi bitowe posiadanych submodeli (np. �wiate�)
 //TDynamicObject *NewDynamic; //Ra: nie u�ywane
 TDynamicObject* ABuFindNearestObject(TTrack *Track,TDynamicObject *MyPointer,int &CouplNr);
protected:
    bool bEnabled;

    //TTrackFollower Axle2; //dwie osie z czterech (te s� protected)
    //TTrackFollower Axle3; //Ra: wy��czy�em, bo k�ty s� liczone w Segment.cpp
    int iNumAxles; //ilo�� osi
    //Byte NextConnectedNo;
    //Byte PrevConnectedNo;
    int CouplCounter;
    std::string asModel;
    int iDirection; //kierunek wzgl�dem czo�a sk�adu (1=zgodny,0=przeciwny)
    void ABuScanObjects(int ScanDir,double ScanDist);
    void ABuCheckMyTrack();

public:
 int iLights[2]; //bity zapalonych �wiate�
 double fTrackBlock; //odleg�o�� do przeszkody do dalszego ruchu
 TDynamicObject* Prev();
 TDynamicObject* Next();
   //-- void SetdMoveLen(double dMoveLen) {MoverParameters->dMoveLen=dMoveLen;}
   //-- void ResetdMoveLen() {MoverParameters->dMoveLen=0;}
   //-- double GetdMoveLen() {return MoverParameters->dMoveLen;}

    //bool EndSignalsLight1Active() {return btEndSignals11.Active();};
    //bool EndSignalsLight2Active() {return btEndSignals21.Active();};
    //bool EndSignalsLight1oldActive() {return btEndSignals1.Active();};
    //bool EndSignalsLight2oldActive() {return btEndSignals2.Active();};
    int GetPneumatic(bool front, bool red);
    void SetPneumatic(bool front, bool red);
		std::string asName;
    std::string GetName()
       {
          return asName;
       };

//youBy
    //TRealSound rsDiesielInc;
//youBy
    //TRealSound rscurve;

//youBy - dym
    //TSmoke Smog;
    //float EmR;
    //vector3 smokeoffset;

    TDynamicObject *NextConnected; //pojazd pod��czony od strony sprz�gu 1 (kabina -1)
    TDynamicObject *PrevConnected; //pojazd pod��czony od strony sprz�gu 0 (kabina 1)
    int NextConnectedNo;
    int PrevConnectedNo;
    vector3 modelRot;      //Obrot pudla w/m swiata
    TDynamicObject* ABuScanNearestObject(TTrack *Track, double ScanDir, double ScanDist, int &CouplNr);
    TDynamicObject* GetFirstDynamic(int cpl_type);
    TDynamicObject* GetFirstCabDynamic(int cpl_type);
    TDynamicObject* GetLastDynamic(int cpl_type);

    TDynamicObject* GET_LIGHTR_P(TDynamicObject *vech);
    TDynamicObject* GET_LIGHTR_D(TDynamicObject *vech);
    TDynamicObject* RENDER_SENSORS(TDynamicObject *vech);

    void ABuSetModelShake(vector3 mShake);
    TModel3d *mdLoad;
    TModel3d *mdPrzedsionek;
    TModel3d *mdModel;
    TModel3d *mdKabina; //McZapkie-030303: to z train.h
	TModel3d *mdKabinb;
    TModel3d *mdLowPolyInt; //ABu 010305: wnetrze lowpoly
    bool pcabc1;        //Winger 040304 - zaleznosc pantografu od kabiny
    bool pcabc2;
    double pcabd1;
    double pcabd2;
    double pcabe1;
    double pcabe2;
    bool pcabc1x;
    double lastcabf;
    double dWheelAngle;
    double StartTime;
    double PantTraction1; //Winger 170204
    double PantTraction2; //Winger 170204
    double PantWysF;      //Winger 180204
    double PantWysR;      //Winger 180204
    double dPantAngleF;  //Winger 160204
    double dPantAngleR;  //Winger 160204
    double dPantAngleFT;  //Winger 170204
    double dPantAngleRT;  //Winger 170204
    double pant1x;      //Winger 010304
    double pant2x;      //Winger 010304
    double panty;       //Winger 010304
    double panth;       //Winger 010304
    double NoVoltTime;
    bool pcp1p;
    bool pcp2p;
    double dDoorMoveL; //NBMX
    double dDoorMoveR; //NBMX
    bool bcab1light;
    bool bcab2light;
    bool headlA_L;
    bool headlA_R;
    bool headlA_U;
    bool headlB_L;
    bool headlB_R;
    bool headlB_U;

    vector3 vLIGHT_R_POS;
    vector3 vLIGHT_R_END;
    vector3 vLIGHT_R_OFF;
    vector3 vLIGHT_R_MOV;
    vector3 vLIGHT_R_SHK;
    vector3 vLIGHT_R_VEL;
    float   vLIGHT_R_SPR;
    float   vLIGHT_R_ROT;


//McZapkie-010302
    TController *Mechanik;
    TTrainParameters *TrainParams;
    bool MechInside;
    TTrackFollower Axle0; //o� z przodu (od sprz�gu 0)
    TTrackFollower Axle1; //o� z ty�u (od sprz�gu 1)
//McZapkie-270202
    bool Controller;
    bool bDisplayCab; //czy wyswietlac kabine w train.cpp
    TTrack *MyTrack; //McZapkie-030303: tor na ktorym stoi, ABu
    std::string asBaseDir;
    GLuint ReplacableSkinID[5];  //McZapkie:zmienialne nadwozie
    int iAlpha; //czy tekstura przezroczysta
    TDynamicObject();
    ~TDynamicObject();
    double TDynamicObject::Init
    (//zwraca d�ugo�� pojazdu albo 0, je�li b��d
     std::string Name, std::string BaseDir, std::string asReplacableSkin, std::string Type_Name,
     TTrack *Track, double fDist, std::string DriverType, double fVel, std::string TrainName,
     float Load, std::string LoadType,bool Reversed
    );
    void AttachPrev(TDynamicObject *Object, int iType= 1);
    bool UpdateForce(double dt, double dt1, bool FullVer);
    void LoadUpdate();
    bool Update(double dt, double dt1);
    bool FastUpdate(double dt);
    void Move(double fDistance);
    void FastMove(double fDistance);
    bool Render();
    bool RenderAlpha();
    vector3 inline GetPosition();
    inline vector3 AxlePositionGet() { return iAxleFirst?Axle1.pPosition:Axle0.pPosition; };
    inline vector3 GetDirection() { return Axle0.pPosition-Axle1.pPosition; };
//--    inline double GetVelocity() { return MoverParameters->Vel; };
//--    inline double GetLength() { return MoverParameters->Dim.L; };
//--    inline double GetWidth() { return MoverParameters->Dim.W; };
    inline TTrack* GetTrack() { return (iAxleFirst?Axle1.GetTrack():Axle0.GetTrack()); };
    //void UpdatePos();
	bool setanimwheels(std::string prefix);
 TMoverParameters *MoverParameters;

 vector3 vUp,vFront,vLeft;
 matrix4x4 mMatrix;
 std::string asTrack;
 std::string asDestination; //dok�d pojazd ma by� kierowany "(stacja):(tor)"
 //McZapkie-260202
 void LoadMMediaFile(std::string BaseDir, std::string TypeName, std::string ReplacableSkin);

 inline double ABuGetDirection() //ABu.
 {
  return (Axle1.GetTrack()==MyTrack?Axle1.GetDirection():Axle0.GetDirection());
 };
 inline double ABuGetTranslation() //ABu.
 {//zwraca przesuni�cie w�zka wzgl�dem Point1 toru
  return (Axle1.GetTrack()==MyTrack?Axle1.GetTranslation():Axle0.GetTranslation());
 };
 inline double RaDirectionGet()
 {//zwraca kierunek pojazdu na torze z aktywn� os�
  return iAxleFirst?Axle1.GetDirection():Axle0.GetDirection();
 };
 inline double RaTranslationGet()
 {//zwraca przesuni�cie w�zka wzgl�dem Point1 toru z aktywn� osi�
  return iAxleFirst?Axle1.GetTranslation():Axle0.GetTranslation();
 };
 inline TTrack* RaTrackGet()
 {//zwraca tor z aktywn� osi�
  return iAxleFirst?Axle1.GetTrack():Axle0.GetTrack();
 };
 void CouplersDettach(double MinDist,int MyScanDir);
 void RadioStop();
 void RaLightsSet(int head,int rear);
 void RaAxleEvent(TEvent *e);
 TDynamicObject* FirstFind(int &coupler_nr);
 int DirectionSet(int d); //ustawienie kierunku w sk�adzie
 int DirectionGet() {return iDirection?1:-1;}; //ustawienie kierunku w sk�adzie
 bool DettachDistance(int dir);
 int Dettach(int dir,int cnt);
};


//---------------------------------------------------------------------------
#endif
