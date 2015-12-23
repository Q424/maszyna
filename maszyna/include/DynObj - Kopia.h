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
const int ANIM_TYPES = 7; //Ra: iloœæ typów animacji
const int ANIM_WHEELS = 0; //ko³a
const int ANIM_DOORS = 1; //drzwi
const int ANIM_LEVERS = 2; //elementy obracane (wycieraczki, ko³a skrêtne, przestawiacze, klocki ham.)
const int ANIM_BUFFERS = 3; //elementy przesuwane (zderzaki)
const int ANIM_BOOGIES = 4; //wózki (s¹ skrêcane w dwóch osiach)
const int ANIM_PANTS = 5; //pantografy
const int ANIM_STEAMS = 6; //napêd parowozu

class TAnim;
typedef void ( *TUpdate)(TAnim *pAnim); //typ funkcji aktualizuj¹cej po³o¿enie submodeli

//McZapkie-250202
  const int MaxAxles=16; //ABu 280105: zmienione z 8 na 16
  const int MaxAnimatedAxles=16; //i to tez.
  const int MaxAnimatedDoors=8;  //NBMX  wrzesien 2003
  /*
  Ra: Utworzyæ klasê wyposa¿enia opcjonalnego, z której bêd¹ dziedziczyæ klasy drzwi,
  pantografów, napêdu parowozu i innych ruchomych czêœci pojazdów. Klasy powinny byæ
  pseudo-wirtualne, bo wirtualne mog¹ obni¿aæ wydajnosœæ.
  Przy wczytywaniu MMD utworzyæ tabelê wskaŸnikow na te dodatki. Przy wyœwietlaniu
  pojazdu wykonywaæ Update() na kolejnych obiektach wyposa¿enia.
  Rozwa¿yæ u¿ycie oddzielnych modeli dla niektórych pojazdów (np. lokomotywy), co
  zaoszczêdzi³o by czas ustawiania animacji na modelu wspólnym dla kilku pojazdów,
  szczególnie dla pojazdów w danej chwili nieruchomych (przy du¿ym zagêszczeniu
  modeli na stacjach na ogó³ przewaga jest tych nieruchomych).
  */
class TController;

class TAnimValveGear
{//wspó³czynniki do animacji parowozu (wartoœci przyk³adowe dla Pt47)
	int iValues; //iloœæ liczb (wersja):
	float fKorbowodR; //d³ugoœæ korby (pó³ skoku t³oka) [m]: 0.35
	float fKorbowodL; //d³ugoœæ korbowodu [m]: 3.8
	float fDrazekR;   //promieñ mimoœrodu (dr¹¿ka) [m]: 0.18
	float fDrazekL;   //d³. dr¹¿ka mimoœrodowego [m]: 2.55889
	float fJarzmoV;   //wysokoœæ w pionie osi jarzma od osi ko³a [m]: 0.751
	float fJarzmoH;   //odleg³oœæ w poziomie osi jarzma od osi ko³a [m]: 2.550
	float fJarzmoR;   //promieñ jarzma do styku z dr¹¿kiem [m]: 0.450
	float fJarzmoA;   //k¹t mimoœrodu wzglêdem k¹ta ko³a [°]: -96.77416667
	float fWdzidloL;  //d³ugoœæ wodzid³a [m]: 2.0
	float fWahaczH;   //d³ugoœæ wahacza (góra) [m]: 0.14
	float fSuwakH;    //wysokoœæ osi suwaka ponad osi¹ ko³a [m]: 0.62
	float fWahaczL;   //d³ugoœæ wahacza (dó³) [m]: 0.84
	float fLacznikL;  //d³ugoœæ ³¹cznika wahacza [m]: 0.75072
	float fRamieL;    //odleg³oœæ ramienia krzy¿ulca od osi ko³a [m]: 0.192
	float fSuwakL;    //odleg³oœæ œrodka t³oka/suwaka od osi ko³a [m]: 5.650
	//do³o¿yæ parametry dr¹¿ka nastawnicy
	//albo nawet zrobiæ dynamiczn¹ tablicê float[] i w ni¹ pakowaæ wszelkie wspó³czynniki, potem u¿ywaæ indeksów
	//wspó³czynniki mog¹ byæ wspólne dla 2-4 t³oków, albo ka¿dy t³ok mo¿e mieæ odrêbne
};

class TAnimPant
{//wspó³czynniki do animacji pantografu
public:
	vector3 vPos; //Ra: wspó³rzêdne punktu zerowego pantografu (X dodatnie dla przedniego)
	double fLenL1; //d³ugoœæ dolnego ramienia 1, odczytana z modelu
	double fLenU1; //d³ugoœæ górnego ramienia 1, odczytana z modelu
	double fLenL2; //d³ugoœæ dolnego ramienia 2, odczytana z modelu
	double fLenU2; //d³ugoœæ górnego ramienia 2, odczytana z modelu
	double fHoriz; //przesuniêcie œlizgu w d³ugoœci pojazdu wzglêdem osi obrotu dolnego ramienia
	double fHeight; //wysokoœæ œlizgu ponad oœ obrotu, odejmowana od wysokoœci drutu
	double fWidth; //po³owa szerokoœci roboczej œlizgu, do wykrycia zeœlizgniêcia siê drutu
	double fAngleL0; //Ra: pocz¹tkowy k¹t dolnego ramienia (odejmowany przy animacji)
	double fAngleU0; //Ra: pocz¹tkowy k¹t górnego ramienia (odejmowany przy animacji)
	double PantTraction; //Winger 170204: wysokoœæ drutu ponad punktem na wysokoœci vPos.y p.g.s.
	double PantWys; //Ra: aktualna wysokoœæ uniesienia œlizgu do porównania z wysokoœci¹ drutu
	double fAngleL; //Winger 160204: aktualny k¹t ramienia dolnego
	double fAngleU; //Ra: aktualny k¹t ramienia górnego
	double NoVoltTime; //czas od utraty kontaktu z drutem
	TTraction *hvPowerWire; //aktualnie podczepione druty, na razie tu
	float fWidthExtra; //dodatkowy rozmiar poziomy poza czêœæ robocz¹ (fWidth)
	float fHeightExtra[5]; //³amana symuluj¹ca kszta³t nabie¿nika
	//double fHorizontal; //Ra 2015-01: po³o¿enie drutu wzglêdem osi pantografu 
	void __fastcall AKP_4E();
};

class TAnim
{//klasa animowanej czêœci pojazdu (ko³a, drzwi, pantografy, burty, napêd parowozu, si³owniki itd.)
public:
	union
	{
		TSubModel *smAnimated; //animowany submodel (jeœli tylko jeden, np. oœ)
		TSubModel **smElement; //jeœli animowanych elementów jest wiêcej (pantograf, napêd parowozu)
		int iShift; //przesuniêcie przed przydzieleniem wskaŸnika
	};
	union
	{//parametry animacji
		TAnimValveGear *pValveGear; //wspó³czynniki do animacji parowozu
		double *dWheelAngle; //wskaŸnik na k¹t obrotu osi
		float *fParam; //ró¿ne parametry dla animacji
		TAnimPant *fParamPants; //ró¿ne parametry dla animacji
	};
	union
	{//wskaŸnik na obiekt odniesienia
		double *fDoubleBase; //jakiœ double w fizyce
		float *fFloatBase; //jakiœ float w fizyce
		int *iIntBase; //jakiœ int w fizyce
	};
	//void _fastcall Update(); //wskaŸnik do funkcji aktualizacji animacji
	int iFlags; //flagi animacji
	float fMaxDist; //do jakiej odleg³oœci wykonywana jest animacja
	float fSpeed; //parametr szybkoœci animacji
	int iNumber; //numer kolejny obiektu
public:
	__fastcall TAnim();
	__fastcall ~TAnim();
	TUpdate yUpdate; //metoda TDynamicObject aktualizuj¹ca animacjê
	int __fastcall TypeSet(int i, int fl = 0); //ustawienie typu
	void __fastcall Parovoz(); //wykonanie obliczeñ animacji
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
 //vector3 pOldPos1; //Ra: nie u¿ywane
 //vector3 pOldPos4;
 vector3 vPosition; //Ra: pozycja pojazdu liczona zaraz po przesuniêciu
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
 int iAxleFirst; //numer pierwszej oœ w kierunku ruchu
 int iInventory; //flagi bitowe posiadanych submodeli (np. œwiate³)
 //TDynamicObject *NewDynamic; //Ra: nie u¿ywane
 TDynamicObject* ABuFindNearestObject(TTrack *Track,TDynamicObject *MyPointer,int &CouplNr);
protected:
    bool bEnabled;

    //TTrackFollower Axle2; //dwie osie z czterech (te s¹ protected)
    //TTrackFollower Axle3; //Ra: wy³¹czy³em, bo k¹ty s¹ liczone w Segment.cpp
    int iNumAxles; //iloœæ osi
    //Byte NextConnectedNo;
    //Byte PrevConnectedNo;
    int CouplCounter;
    std::string asModel;
    int iDirection; //kierunek wzglêdem czo³a sk³adu (1=zgodny,0=przeciwny)
    void ABuScanObjects(int ScanDir,double ScanDist);
    void ABuCheckMyTrack();

public:
 int iLights[2]; //bity zapalonych œwiate³
 double fTrackBlock; //odleg³oœæ do przeszkody do dalszego ruchu
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

    TDynamicObject *NextConnected; //pojazd pod³¹czony od strony sprzêgu 1 (kabina -1)
    TDynamicObject *PrevConnected; //pojazd pod³¹czony od strony sprzêgu 0 (kabina 1)
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
    TTrackFollower Axle0; //oœ z przodu (od sprzêgu 0)
    TTrackFollower Axle1; //oœ z ty³u (od sprzêgu 1)
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
    (//zwraca d³ugoœæ pojazdu albo 0, jeœli b³¹d
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
 std::string asDestination; //dok¹d pojazd ma byæ kierowany "(stacja):(tor)"
 //McZapkie-260202
 void LoadMMediaFile(std::string BaseDir, std::string TypeName, std::string ReplacableSkin);

 inline double ABuGetDirection() //ABu.
 {
  return (Axle1.GetTrack()==MyTrack?Axle1.GetDirection():Axle0.GetDirection());
 };
 inline double ABuGetTranslation() //ABu.
 {//zwraca przesuniêcie wózka wzglêdem Point1 toru
  return (Axle1.GetTrack()==MyTrack?Axle1.GetTranslation():Axle0.GetTranslation());
 };
 inline double RaDirectionGet()
 {//zwraca kierunek pojazdu na torze z aktywn¹ os¹
  return iAxleFirst?Axle1.GetDirection():Axle0.GetDirection();
 };
 inline double RaTranslationGet()
 {//zwraca przesuniêcie wózka wzglêdem Point1 toru z aktywn¹ osi¹
  return iAxleFirst?Axle1.GetTranslation():Axle0.GetTranslation();
 };
 inline TTrack* RaTrackGet()
 {//zwraca tor z aktywn¹ osi¹
  return iAxleFirst?Axle1.GetTrack():Axle0.GetTrack();
 };
 void CouplersDettach(double MinDist,int MyScanDir);
 void RadioStop();
 void RaLightsSet(int head,int rear);
 void RaAxleEvent(TEvent *e);
 TDynamicObject* FirstFind(int &coupler_nr);
 int DirectionSet(int d); //ustawienie kierunku w sk³adzie
 int DirectionGet() {return iDirection?1:-1;}; //ustawienie kierunku w sk³adzie
 bool DettachDistance(int dir);
 int Dettach(int dir,int cnt);
};


//---------------------------------------------------------------------------
#endif
