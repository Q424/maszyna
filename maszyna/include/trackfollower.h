//---------------------------------------------------------------------------

#ifndef trackfollowerH
#define trackfollowerH

#include "Track.h"
#include "segment.h"

inline bool iSetFlag(int Flag, int Value)
{
bool ret = false;
	if (Value > 0) 
		if ((Flag && Value) == 0)
		{
		Flag = Flag + Value;
		ret = true; //true, gdy by�o wcze�niej 0 i zosta�o ustawione
		}
	if (Value < 0 )
		if ((Flag && abs(Value)) == abs(Value))
		{
		Flag = Flag + Value; //Value jest ujemne, czyli zerowanie flagi
		ret = true; //true, gdy by�o wcze�niej 1 i zosta�o wyzerowane
		}
	return ret;
}


class TDynamicObject;

class TTrackFollower
{//o� poruszaj�ca si� po torze
private:
 TTrack *pCurrentTrack; //na kt�rym torze si� znajduje
 TSegment *pCurrentSegment; //zwrotnice mog� mie� dwa segmenty
 double fCurrentDistance; //przesuni�cie wzgl�dem Point1 w stron� Point2
 double fDirection; //ustawienie wzgl�dem toru: -1.0 albo 1.0, mno�one przez dystans
 bool ComputatePosition(); //przeliczenie pozycji na torze
 TDynamicObject *Owner; //pojazd posiadaj�cy
 int iEventFlag; //McZapkie-020602: informacja o tym czy wyzwalac zdarzenie: 0,1,2,3
 int iEventallFlag;

public:
 double fOffsetH; //Ra: odleg�o�� �rodka osi od osi toru (dla samochod�w)
 vector3 pPosition; //wsp�rz�dne XYZ w uk�adzie scenerii
 vector3 vAngles; //x:przechy�ka, y:pochylenie, z:kierunek w planie (w radianach)
 TTrackFollower();
 ~TTrackFollower();
 void SetCurrentTrack(TTrack *pTrack,int end);
 bool Move(double fDistance,bool bPrimary);
 inline TTrack* GetTrack() {return pCurrentTrack;};
 inline double GetRoll() {return vAngles.x;}; //przechy�ka policzona przy ustalaniu pozycji
 //{return pCurrentSegment->GetRoll(fCurrentDistance)*fDirection;}; //zamiast liczy� mo�na pobra�
 inline double GetDirection() {return fDirection;}; //zwrot na torze
 inline double GetTranslation() {return fCurrentDistance;};  //ABu-030403
 //inline double GetLength(vector3 p1, vector3 cp1, vector3 cp2, vector3 p2)
 //{ return pCurrentSegment->ComputeLength(p1,cp1,cp2,p2); };
 //inline double GetRadius(double L, double d);  //McZapkie-150503
 bool Init(TTrack *pTrack,TDynamicObject *NewOwner,double fDir);
 void Render(float fNr);
};
//---------------------------------------------------------------------------
#endif
