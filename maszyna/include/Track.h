//---------------------------------------------------------------------------

#ifndef TrackH
#define TrackH

#include "Segment.h"
#include "ResourceManager.h"
#include "Classes.h"


class TEvent;

typedef enum { tt_Unknown, tt_Normal, tt_Switch, tt_Turn, tt_Cross, tt_Tributary } TTrackType;
//McZapkie-100502
typedef enum {e_unknown=-1, e_flat=0, e_mountains, e_canyon, e_tunnel, e_bridge, e_bank} TEnvironmentType;
//Ra: opracowa� alternatywny system cieni/�wiate� z definiowaniem koloru o�wietlenia w halach

class TTrack;
class TGroundNode;
class TSubRect;
class TTraction;

class TSwitchExtension
{//dodatkowe dane do toru, kt�ry jest zwrotnic�
public:
 TSwitchExtension(TTrack *owner);
 ~TSwitchExtension();
 TSegment *Segments[4]; //dwa tory od punktu 1, pozosta�e dwa od 2?
 TTrack *pNexts[2];
 TTrack *pPrevs[2];
 bool iNextDirection[2];
 bool iPrevDirection[2];
 int CurrentIndex; //dla zwrotnicy
 double fOffset1, fDesiredOffset1; //ruch od strony punktu 1
 union
 {double fOffset2, fDesiredOffset2; //ruch od strony punktu 2 nie obs�ugiwany
  TGroundNode *pMyNode; //dla obrotnicy do wt�rnego pod��czania tor�w
 };
 union
 {bool RightSwitch; //czy zwrotnica w prawo
  //TAnimContainer *pAnim; //animator modelu dla obrotnicy
  TAnimModel *pModel; //na razie model
 };
 bool bMovement; //czy w trakcie animacji
 int iLeftVBO,iRightVBO; //indeksy iglic w VBO
 TSubRect *pOwner; //sektor, kt�remu trzeba zg�osi� animacj�
 TTrack *pNextAnim; //nast�pny tor do animowania
 TEvent *EventPlus,*EventMinus; //zdarzenia sygnalizacji rozprucia
 float fVelocity; //maksymalne ograniczenie pr�dko�ci (ustawianej eventem)
private:
};

const int iMaxNumDynamics=40; //McZapkie-100303

class TIsolated
{//obiekt zbieraj�cy zaj�to�ci z kilku odcink�w
 int iAxles; //ilo�� osi na odcinkach obs�ugiwanych przez obiekt
 TIsolated *pNext;
 static TIsolated *pRoot;
public:
 std::string asName; //nazwa obiektu, baza do nazw event�w
 TEvent *eBusy; //zdarzenie wyzwalane po zaj�ciu grupy
 TEvent *eFree; //zdarzenie wyzwalane po ca�kowitym zwolnieniu zaj�to�ci grupy
 TIsolated();
 TIsolated(const std::string &n, TIsolated *i);
 ~TIsolated();
 static TIsolated* Find(const std::string &n); //znalezienie obiektu albo utworzenie nowego
 void Modify(int i,TDynamicObject *o); //dodanie lub odj�cie osi
};

class TTrack : public Resource
{//trajektoria ruchu - opakowanie
private:
 TSwitchExtension *SwitchExtension; //dodatkowe dane do toru, kt�ry jest zwrotnic�
 TSegment *Segment;
 TTrack *pNext; //odcinek od strony punktu 2 - to powinno by� w segmencie
 TTrack *pPrev; //odcinek od strony punktu 1
 //McZapkie-070402: dodalem zmienne opisujace rozmiary tekstur
 GLuint TextureID1; //tekstura szyn albo nawierzchni
 GLuint TextureID2; //tekstura automatycznej podsypki albo pobocza
 float fTexLength; //d�ugo�� powtarzania tekstury w metrach
 float fTexRatio1; //proporcja rozmiar�w tekstury dla nawierzchni drogi
 float fTexRatio2; //proporcja rozmiar�w tekstury dla chodnika
 float fTexHeight1; //wysoko�� brzegu wzgl�dem trajektorii
 float fTexWidth; //szeroko�� boku
 float fTexSlope;
 double fRadiusTable[2]; //dwa promienie, drugi dla zwrotnicy
 int iTrapezoid; //0-standard, 1-przechy�ka, 2-trapez, 3-oba
 GLuint DisplayListID;
 TIsolated *pIsolated; //obw�d izolowany obs�uguj�cy zaj�cia/zwolnienia grupy tor�w
 TGroundNode *pMyNode; //Ra: proteza, �eby tor zna� swoj� nazw� TODO: odziedziczy� TTrack z TGroundNode
public:
 int iNumDynamics;
 TDynamicObject *Dynamics[iMaxNumDynamics];
 int iEvents; //Ra: flaga informuj�ca o obecno�ci event�w
 TEvent *Eventall0;  //McZapkie-140302: wyzwalany gdy pojazd stoi
 TEvent *Eventall1;
 TEvent *Eventall2;
 TEvent *Event0;  //McZapkie-280503: wyzwalany tylko gdy headdriver
 TEvent *Event1;
 TEvent *Event2;
 TEvent *EventBusy; //Ra: wyzwalane, gdy zajmowany; nazwa automatyczna
 TEvent *EventFree; //Ra: wyzwalane, gdy zwalniany; nazwa automatyczna
 std::string asEventall0Name; //nazwy event�w
 std::string asEventall1Name;
 std::string asEventall2Name;
 std::string asEvent0Name;
 std::string asEvent1Name;
 std::string asEvent2Name;
 int iNextDirection; //0:Point1, 1:Point2, 3:do odchylonego na zwrotnicy
 int iPrevDirection;
 TTrackType eType;
 int iCategoryFlag; //0x100 - usuwanie pojaz�w
 float fTrackWidth; //szeroko�� w punkcie 1
 float fTrackWidth2; //szeroko�� w punkcie 2 (g��wnie drogi i rzeki)
 float fFriction; //wsp�czynnik tarcia
 float fSoundDistance;
 int iQualityFlag;
 int iDamageFlag;
 TEnvironmentType eEnvironment; //d�wi�k i o�wietlenie
 bool bVisible; //czy rysowany
private:
 double fVelocity; //pr�dko�� dla AI (powy�ej ro�nie prawdopowobie�stwo wykolejenia)
public:
 //McZapkie-100502:
 double fTrackLength; //d�ugo�� z wpisu, nigdzie nie u�ywana
 double fRadius; //promie�, dla zwrotnicy kopiowany z tabeli
 bool ScannedFlag; //McZapkie: do zaznaczania kolorem tor�w skanowanych przez AI
 TTraction *pTraction; //drut zasilaj�cy

 TTrack(TGroundNode *g);
 ~TTrack();
 void Init();
 static TTrack* Create400m(int what,double dx);
 TTrack* NullCreate(int dir);
 inline bool IsEmpty() { return (iNumDynamics<=0); };
 void ConnectPrevPrev(TTrack *pNewPrev,int typ);
 void ConnectPrevNext(TTrack *pNewPrev,int typ);
 void ConnectNextPrev(TTrack *pNewNext,int typ);
 void ConnectNextNext(TTrack *pNewNext,int typ);
 inline double Length() { return Segment->GetLength(); };
 inline TSegment* CurrentSegment() { return Segment; };
 inline TTrack* CurrentNext() {return (pNext);};
 inline TTrack* CurrentPrev() {return (pPrev);};
 bool SetConnections(int i);
 bool Switch(int i);
 bool SwitchForced(int i,TDynamicObject *o);
 inline int GetSwitchState() { return (SwitchExtension?SwitchExtension->CurrentIndex:-1); };
 void Load(cParser *parser, vector3 pOrigin, std::string name);
 bool AssignEvents(TEvent *NewEvent0, TEvent *NewEvent1, TEvent *NewEvent2);
 bool AssignallEvents(TEvent *NewEvent0, TEvent *NewEvent1, TEvent *NewEvent2);
 bool AssignForcedEvents(TEvent *NewEventPlus, TEvent *NewEventMinus);
 bool CheckDynamicObject(TDynamicObject *Dynamic);
 bool AddDynamicObject(TDynamicObject *Dynamic);
 bool RemoveDynamicObject(TDynamicObject *Dynamic);
 void MoveMe(vector3 pPosition);

 void Release();
 void Compile(GLuint tex=0);

 void Render(); //renderowanie z Display Lists
 int RaArrayPrepare(); //zliczanie rozmiaru dla VBO sektroa
 void  RaArrayFill(CVertNormTex *Vert,const CVertNormTex *Start); //wype�nianie VBO
 void  RaRenderVBO(int iPtr); //renderowanie z VBO sektora
 void RenderDyn(); //renderowanie nieprzezroczystych pojazd�w (oba tryby)
 void RenderDynAlpha(); //renderowanie przezroczystych pojazd�w (oba tryby)

 void RaOwnerSet(TSubRect *o)
 {if (SwitchExtension) SwitchExtension->pOwner=o;};
 bool InMovement(); //czy w trakcie animacji?
 void RaAssign(TGroundNode *gn,TAnimContainer *ac);
 void RaAssign(TGroundNode *gn,TAnimModel *am);
 void RaAnimListAdd(TTrack *t);
 TTrack* RaAnimate();

 void RadioStop();
 void AxleCounter(int i,TDynamicObject *o)
 {if (pIsolated) pIsolated->Modify(i,o);}; //dodanie lub odj�cie osi
 std::string IsolatedName();
 bool IsolatedEventsAssign(TEvent *busy, TEvent *free);
 double WidthTotal();
 GLuint TextureGet(int i) {return i?TextureID1:TextureID2;};
 bool IsGroupable();
 int TestPoint(vector3 *Point);
 void MovedUp1(double dh);
 std::string NameGet();
 void VelocitySet(float v);
 inline float VelocityGet() {return fVelocity;};
private:
 void EnvironmentSet();
 void EnvironmentReset();
};

//---------------------------------------------------------------------------
#endif
