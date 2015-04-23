//---------------------------------------------------------------------------

#ifndef TrackH
#define TrackH


#include "../commons.h"
#include "../commons_usr.h"

//#include "segment.h"
//#include "parser.h"
//#include "Event.h"
//#include "Flags.h"
//#include "ResourceManager.h"

class TEvent;
//class TSegment;

typedef enum { tt_Unknown, tt_Normal, tt_Switch, tt_Turn, tt_Cross } TTrackType;
typedef enum { e_unknown, e_flat, e_mountains, e_canyon, e_tunnel, e_bridge, e_bank } TEnvironmentType;

class TTrack;
class TGroundNode;

static const double fMaxOffset=0.1f;

class TSwitchExtension
{//dodatkowe dane do toru, kt�ry jest zwrotnic�
public:
    TSwitchExtension();
    ~TSwitchExtension();
    
    TTrack *pNexts[2];
    TTrack *pPrevs[2];
	TSegment Segments[4]; //dwa tory od punktu 1, pozosta�e dwa od 2?
    bool bNextSwitchDirection[2];
    bool bPrevSwitchDirection[2];
    bool iNextDirection[2]; // 291
    bool iPrevDirection[2]; // 291

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

// TSubRect *pOwner; //sektor, kt�remu trzeba zg�osi� animacj�
 TTrack *pNextAnim; //nast�pny tor do animowania
 TEvent *EventPlus,*EventMinus; //zdarzenia sygnalizacji rozprucia
private:
};

const int iMaxNumDynamics= 40; //McZapkie-100303

const int NextMask[4]= {0,1,0,1}; //tor nast�pny dla stan�w 0, 1, 2, 3
const int PrevMask[4]= {0,0,1,1}; //tor poprzedni dla stan�w 0, 1, 2, 3


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
  TIsolated(const std::string &n,TIsolated *i);
  ~TIsolated();
 static TIsolated*  Find(const std::string &n); //znalezienie obiektu albo utworzenie nowego
 void  Modify(int i,TDynamicObject *o); //dodanie lub odj�cie osi
};


class TTrack: public Resource
{
private:
    TSwitchExtension *SwitchExtension; //dodatkowe dane do toru, kt�ry jest zwrotnic�
    TSegment *Segment;
    TTrack *pNext; //odcinek od strony punktu 2
    TTrack *pPrev; //odcinek od strony punktu 1

//McZapkie-070402: dodalem zmienne opisujace rozmiary tekstur
    GLuint TextureID1; //tekstura szyn
    float fTexLength;
    GLuint TextureID2; //tekstura automatycznej podsypki
    float fTexHeight; //wysoko� brzegu wzgl�dem trajektorii
    float fTexWidth;
    float fTexSlope;
    //vector3 *HelperPts; //Ra: nie u�ywane, na razie niech zostanie
    double fRadiusTable[2]; //dwa promienie, drugi dla zwrotnicy
    int iTrapezoid; //0-standard, 1-przechy�ka, 2-trapez, 3-oba
private:
    GLuint DisplayListID;
public:
    int iNumDynamics;
	int iRAILTYPE;
	std::string asRAILTYPE;
    TDynamicObject *Dynamics[iMaxNumDynamics];
    TEvent *Eventall0;  //McZapkie-140302: wyzwalany gdy pojazd stoi
    TEvent *Eventall1;
    TEvent *Eventall2;
    TEvent *Event0;  //McZapkie-280503: wyzwalany tylko gdy headdriver
    TEvent *Event1;
    TEvent *Event2;
    TEvent *EventBusy; //Ra: wyzwalane, gdy zajmowany; nazwa automatyczna
    TEvent *EventFree; //Ra: wyzwalane, gdy zwalniany; nazwa automatyczna
	std::string asName;
	std::string asEventall0Name; //nazwy event�w
    std::string asEventall1Name;
    std::string asEventall2Name;
    std::string asEvent0Name;
    std::string asEvent1Name;
    std::string asEvent2Name;
    bool bNextSwitchDirection;
    bool bPrevSwitchDirection;
 int iNextDirection; //0:Point1, 1:Point2, 3:do odchylonego na zwrotnicy
 int iPrevDirection;
    TTrackType eType;
    int iCategoryFlag;
    float fTrackWidth; //szeroko�� w punkcie 1
    float fTrackWidth2; //szeroko�� w punkcie 2 (g��wnie drogi i rzeki)
    float fFriction; //wsp�czynnik tarcia
    float fSoundDistance;
    int iQualityFlag;
    int iDamageFlag;
    TEnvironmentType eEnvironment; //d�wi�k i o�wietlenie
    bool bVisible; //czy rysowany
    double fVelocity; //pr�dko�� dla AI (powy�ej ro�nie prawdopowobie�stwo wykolejenia)
//McZapkie-100502:
    double fTrackLength; //d�ugo�� z wpisu, nigdzie nie u�ywana
    double fRadius; //promie�, dla zwrotnicy kopiowany z tabeli
    bool ScannedFlag; //McZapkie: to dla testu
     TTrack();
     ~TTrack();
    void  Init();
    bool  IsEmpty() { return (iNumDynamics<=0); };
    void  ConnectPrevPrev(TTrack *pNewPrev);
    void  ConnectPrevNext(TTrack *pNewPrev);
    void  ConnectNextPrev(TTrack *pNewNext);
    void  ConnectNextNext(TTrack *pNewNext);
//-    inline double  Length() { return Segment->GetLength(); };
    inline TSegment*  CurrentSegment() { return Segment; };
    inline TTrack*  CurrentNext() { return (pNext); };
    inline TTrack*  CurrentPrev() { return (pPrev); };
    inline bool  SetConnections(int i)
    {
        if (SwitchExtension)
        {
            SwitchExtension->pNexts[NextMask[i]]= pNext;
            SwitchExtension->pPrevs[PrevMask[i]]= pPrev;
            SwitchExtension->bNextSwitchDirection[NextMask[i]]= bNextSwitchDirection;
            SwitchExtension->bPrevSwitchDirection[PrevMask[i]]= bPrevSwitchDirection;
            if (eType==tt_Switch)
            {
                SwitchExtension->pPrevs[PrevMask[i+2]]= pPrev;
                SwitchExtension->bPrevSwitchDirection[PrevMask[i+2]]= bPrevSwitchDirection;
            }
            Switch(0);
            return true;
        }
        //Error("Cannot set connections");
		MessageBox(0,"Cannot set connections","Error",MB_OK);
        return false;
    }
    bool  Switch(int i);
    inline int  GetSwitchState() { return (SwitchExtension?SwitchExtension->CurrentIndex:-1); };
	void  Load(cParser *parser, vector3 pOrigin, std::string name);
    bool  AssignEvents(TEvent *NewEvent0, TEvent *NewEvent1, TEvent *NewEvent2);
    bool  AssignallEvents(TEvent *NewEvent0, TEvent *NewEvent1, TEvent *NewEvent2);
    bool  CheckDynamicObject(TDynamicObject *Dynamic);
    bool  AddDynamicObject(TDynamicObject *Dynamic);
    bool  RemoveDynamicObject(TDynamicObject *Dynamic);
    void  MoveMe(vector3 pPosition);

    void Release();
    void Compile();

    bool  Render();
    bool  RenderAlpha();
    bool  InMovement(); //czy w trakcie animacji?
	double  WidthTotal();

    void  Assign(TGroundNode *gn,TAnimContainer *ac);
    void  Assign(TGroundNode *gn,TAnimModel *am);
	char* stdstrtochar(std::string var);
};

//---------------------------------------------------------------------------
#endif
