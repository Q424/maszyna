//---------------------------------------------------------------------------

/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak and others

*/
// trackfollower.cpp is equal with 1166

#pragma hdrstop
#include "commons.h"
#include "commons_usr.h"
#include "segment.h" 
#include "trackfollower.h"
#include "Globals.h"

TTrackFollower::TTrackFollower()
{
 pCurrentTrack=NULL;
 pCurrentSegment=NULL;
 fCurrentDistance=0;
 pPosition=vAngles=vector3(0,0,0);
 fDirection=1; //jest przodem do Point2
 fOffsetH=0.0; //na starcie stoi na �rodku
}

TTrackFollower::~TTrackFollower()
{
}

bool TTrackFollower::Init(TTrack *pTrack,TDynamicObject *NewOwner,double fDir)
{
 fDirection=fDir;
 Owner=NewOwner;
 SetCurrentTrack(pTrack,0);
 iEventFlag=3; //na torze startowym r�wnie� wykona� eventy 1/2
 iEventallFlag=3;
 if ((pCurrentSegment))// && (pCurrentSegment->GetLength()<fFirstDistance))
  return false;
 return true;
}

TTrack* TTrackFollower::SetCurrentTrack(TTrack *pTrack, int end)
{//przejechanie na inny odcinkek toru, z ewentualnym rozpruciem
	if (pTrack)
		switch (pTrack->eType)
	{
		case tt_Switch: //je�li zwrotnica, to przek�adamy j�, aby uzyska� dobry segment
		{int i = (end ? pCurrentTrack->iNextDirection : pCurrentTrack->iPrevDirection);
		if (i>0) //je�eli wjazd z ostrza
			pTrack->SwitchForced(i >> 1, Owner); //to prze�o�enie zwrotnicy - rozprucie!
		}
		break;
		case tt_Cross: //skrzy�owanie trzeba tymczasowo prze��czy�, aby wjecha� na w�a�ciwy tor
		{
	//TODO:--iSegment = Owner->RouteWish(pTrack); //nr segmentu zosta� ustalony podczas skanowania
		//Ra 2014-08: aby ustali� dalsz� tras�, nale�y zapyta� AI - trasa jest ustalana podczas skanowania
		//Ra 15-01: zapytanie AI nie wybiera segmentu - kolejny skanuj�cy mo�e przestawi�
		//pTrack->CrossSegment(end?pCurrentTrack->iNextDirection:pCurrentTrack->iPrevDirection,i); //ustawienie w�a�ciwego wska�nika
		//powinno zwraca� kierunek do zapami�tania, bo segmenty mog� mie� r�ny kierunek
		//if fDirection=(iSegment>0)?1.0:-1.0; //kierunek na tym segmencie jest ustalany bezpo�rednio
		if (iSegment == 0)
		{//to jest b��dna sytuacja - generuje p�tle zawracaj�ce za skrzy�owaniem - ustali�, kiedy powstaje!
			iSegment = 1; //dora�na poprawka
		}
		if ((end ? iSegment : -iSegment)<0)
			fDirection = -fDirection; //wt�rna zmiana
		pTrack->SwitchForced(abs(iSegment) - 1, NULL); //wyb�r zapami�tanego segmentu
		}
		break;
	}
	if (!pTrack)
	{//gdy nie ma toru w kierunku jazdy
		pTrack = pCurrentTrack->NullCreate(end); //tworzenie toru wykolej�cego na przed�u�eniu pCurrentTrack
		if (!end) //je�li dodana od strony zero, to zmiana kierunku
			fDirection = -fDirection; //wt�rna zmiana
		//if (pTrack->iCategoryFlag&2)
		//{//je�li samoch�d, zepsu� na miejscu
		// Owner->MoverParameters->V=0; //zatrzyma�
		// Owner->MoverParameters->Power=0; //ukra�� silnik
		// Owner->MoverParameters->AccS=0; //wch�on�� moc
		// Global::iPause|=1; //zapauzowanie symulacji
		//}
	}
	else
	{//najpierw +1, p�niej -1, aby odcinek izolowany wsp�lny dla tych tor�w nie wykry� zera
		pTrack->AxleCounter(+1, Owner); //zaj�cie nowego toru
		if (pCurrentTrack) pCurrentTrack->AxleCounter(-1, Owner); //opuszczenie tamtego toru
	}
	pCurrentTrack = pTrack;
	pCurrentSegment = (pCurrentTrack ? pCurrentTrack->CurrentSegment() : NULL);
	if (!pCurrentTrack)
		Error(Owner->MoverParameters->Name + " at NULL track");
	return pCurrentTrack;
};

bool TTrackFollower::Move(double fDistance,bool bPrimary)
{//przesuwanie w�zka po torach o odleg�o�� (fDistance), z wyzwoleniem event�w
 //bPrimary=true - jest pierwsz� osi� w poje�dzie, czyli generuje eventy i przepisuje pojazd
 //Ra: zwraca false, je�li pojazd ma by� usuni�ty
 fDistance*=fDirection; //dystans mno�nony przez kierunek
 double s;
 bool bCanSkip; //czy przemie�ci� pojazd na inny tor
 while (true)  //p�tla wychodzi, gdy przesuni�cie wyjdzie zerowe
 {//p�tla przesuwaj�ca w�zek przez kolejne tory, a� do trafienia w jaki�
  if (!pCurrentTrack) return false; //nie ma toru, to nie ma przesuwania
  if (pCurrentTrack->iEvents) //sumaryczna informacja o eventach
  {//omijamy ca�y ten blok, gdy tor nie ma on �adnych event�w (wi�kszo�c nie ma)
   if (fDistance<0)
   {

  //-  if (iSetFlag(iEventFlag,-1)) //zawsze zeruje flag� sprawdzenia, jak mechanik dosi�dzie, to si� nie wykona
  //-   if (Owner->Mechanik) //tylko dla jednego cz�onu
  //-    if (bPrimary && pCurrentTrack->Event1 && (!pCurrentTrack->Event1->iQueued))
  //-     Global::AddToQuery(pCurrentTrack->Event1,Owner); //dodanie do kolejki

  //-  if (iSetFlag(iEventallFlag,-1)) //McZapkie-280503: wyzwalanie eventall dla wszystkich pojazdow
  //-   if (bPrimary && pCurrentTrack->Eventall1 && (!pCurrentTrack->Eventall1->iQueued))
  //-    Global::AddToQuery(pCurrentTrack->Eventall1,Owner); //dodanie do kolejki

   }
   else if (fDistance>0)
   {
   //- if (iSetFlag(iEventFlag,-2)) //zawsze ustawia flag� sprawdzenia, jak mechanik dosi�dzie, to si� nie wykona
   //-  if (Owner->Mechanik) //tylko dla jednego cz�onu
   //-
   //-   if (bPrimary && pCurrentTrack->Event2 && (!pCurrentTrack->Event2->iQueued))
   //-    Global::AddToQuery(pCurrentTrack->Event2,Owner);

   //- if (iSetFlag(iEventallFlag,-2)) //sprawdza i zeruje na przysz�o��, true je�li zmieni z 2 na 0
   //-  if (bPrimary && pCurrentTrack->Eventall2 && (!pCurrentTrack->Eventall2->iQueued))
   //-   Global::AddToQuery(pCurrentTrack->Eventall2,Owner);

   }
   else //if (fDistance==0) //McZapkie-140602: wyzwalanie zdarzenia gdy pojazd stoi
   {
  //-  if (Owner->Mechanik) //tylko dla jednego cz�onu
   //-  if (pCurrentTrack->Event0)
   //-   if (!pCurrentTrack->Event0->iQueued)
   //-    Global::AddToQuery(pCurrentTrack->Event0,Owner);

   //- if (pCurrentTrack->Eventall0)
  //-   if (!pCurrentTrack->Eventall0->iQueued)
   //-   Global::AddToQuery(pCurrentTrack->Eventall0,Owner);

   }
 
  }

  if (!pCurrentSegment) //je�eli nie ma powi�zanego segmentu toru?
   return false;
  //if (fDistance==0.0) return true; //Ra: jak stoi, to chyba dalej nie ma co kombinowa�?
  s=fCurrentDistance+fDistance; //doliczenie przesuni�cia
  //Ra: W Point1 toru mo�e znajdowa� si� "dziura", kt�ra zamieni energi� kinetyczn�
  // ruchu wzd�u�nego na energi� potencjaln�, zamieniaj�c� si� potem na energi�
  // spr�ysto�ci na amortyzatorach. Nale�a�oby we wpisie toru umie�ci� wsp�czynnik
  // podzia�u energii kinetycznej.
  if (s<0)
  {//je�li przekroczenie toru od strony Point1
   bCanSkip=bPrimary?pCurrentTrack->CheckDynamicObject(Owner):false;
   if (bCanSkip) //tylko g��wna o� przenosi pojazd do innego toru
    Owner->MyTrack->RemoveDynamicObject(Owner); //zdejmujemy pojazd z dotychczasowego toru
   if (pCurrentTrack->iPrevDirection)
   {//gdy kierunek bez zmiany (Point1->Point2)
    SetCurrentTrack(pCurrentTrack->CurrentPrev(),0);
    if (pCurrentTrack==NULL)
    {
     WriteLogSS(Owner->MoverParameters->Name+" at NULL track", "ERROR");
     return false; //wyj�cie z b��dem
    }
    fCurrentDistance=pCurrentSegment->GetLength();
    fDistance=s;
   }
   else
   {//gdy zmiana kierunku toru (Point1->Point1)
    SetCurrentTrack(pCurrentTrack->CurrentPrev(),0); //ustawienie (pCurrentTrack)
    fCurrentDistance=0;
    fDistance=-s;
    fDirection=-fDirection;
    if (pCurrentTrack==NULL)
    {
     WriteLogSS(Owner->MoverParameters->Name+" at NULL track", "ERROR");
     return false; //wyj�cie z b��dem
    }
   }
   if (bCanSkip)
   {//jak g��wna o�, to dodanie pojazdu do nowego toru
    pCurrentTrack->AddDynamicObject(Owner);
    iEventFlag=3; //McZapkie-020602: umozliwienie uruchamiania event1,2 po zmianie toru
    iEventallFlag=3; //McZapkie-280503: jw, dla eventall1,2
    if (!Owner->MyTrack) return false;
   }
   continue;
  }
  else if (s>pCurrentSegment->GetLength())
  {//je�li przekroczenie toru od strony Point2
   bCanSkip=bPrimary?pCurrentTrack->CheckDynamicObject(Owner):false;
   if (bCanSkip) //tylko g��wna o� przenosi pojazd do innego toru
    Owner->MyTrack->RemoveDynamicObject(Owner); //zdejmujemy pojazd z dotychczasowego toru
   if (pCurrentTrack->iNextDirection)
   {//gdy zmiana kierunku toru (Point2->Point2)
    fDistance=-(s-pCurrentSegment->GetLength());
    SetCurrentTrack(pCurrentTrack->CurrentNext(),1);
    if (pCurrentTrack==NULL)
    {
     WriteLogSS(Owner->MoverParameters->Name + " at NULL track", "ERROR");
     return false; //wyj�cie z b��dem
    }
    fCurrentDistance=pCurrentSegment->GetLength();
    fDirection=-fDirection;
   }
   else
   {//gdy kierunek bez zmiany (Point2->Point1)
    fDistance=s-pCurrentSegment->GetLength();
    SetCurrentTrack(pCurrentTrack->CurrentNext(),1);
    fCurrentDistance=0;
    if (pCurrentTrack==NULL)
    {
	 WriteLogSS(Owner->MoverParameters->Name + " at NULL track", "ERROR");
     return false; //wyj�cie z b��dem
    }
   }
   if (bCanSkip)
   {//jak g��wna o�, to dodanie pojazdu do nowego toru
    pCurrentTrack->AddDynamicObject(Owner);
    iEventFlag=3; //McZapkie-020602: umozliwienie uruchamiania event1,2 po zmianie toru
    iEventallFlag=3;
    if (!Owner->MyTrack) return false;
   }
   continue;
  }
  else
  {//gdy zostaje na tym samym torze (przesuwanie ju� nie zmienia toru)
   if (bPrimary)
   {//tylko gdy pocz�tkowe ustawienie, dodajemy eventy stania do kolejki
    if (Owner->MoverParameters->ActiveCab!=0)
    //if (Owner->MoverParameters->CabNo!=0)
    {
    //- if (pCurrentTrack->Event1 && pCurrentTrack->Event1->fDelay<=-1.0f)
    //-  Global::AddToQuery(pCurrentTrack->Event1,Owner);
    //- if (pCurrentTrack->Event2 && pCurrentTrack->Event2->fDelay<=-1.0f)
    //-  Global::AddToQuery(pCurrentTrack->Event2,Owner);
    }
    //-if (pCurrentTrack->Eventall1 && pCurrentTrack->Eventall1->fDelay<=-1.0f)
    //- Global::AddToQuery(pCurrentTrack->Eventall1,Owner);
    //-if (pCurrentTrack->Eventall2 && pCurrentTrack->Eventall2->fDelay<=-1.0f)
   //-  Global::AddToQuery(pCurrentTrack->Eventall2,Owner);
   }
   fCurrentDistance=s;
   //fDistance=0;
   return ComputatePosition(); //przeliczenie XYZ, true o ile nie wyjecha� na NULL
  }
 }
};

bool TTrackFollower::ComputatePosition()
{//ustalenie wsp�rz�dnych XYZ
 if (pCurrentSegment) //o ile jest tor
 {
  //pPosition=pCurrentSegment->GetPoint(fCurrentDistance); //wyliczenie z dystansu od Point1
  pCurrentSegment->RaPositionGet(fCurrentDistance,pPosition,vAngles);
  if (fDirection<0) //k�ty zale�� jeszcze od zwrotu na torze
  {vAngles.x=-vAngles.x; //przechy�ka jest w przecinw� stron�
   vAngles.y=-vAngles.y; //pochylenie jest w przecinw� stron�
   vAngles.z+=M_PI; //ale kierunek w planie jest obr�cony o 180� 
  }
  if (fOffsetH!=0.0)
  {//je�li przesuni�cie wzgl�dem osi toru, to je doliczy�

  }
  return true;
 }
 return false;
}

void TTrackFollower::Render(float fNr)
{//funkcja rysuj�ca sto�ek w miejscu osi
 glPushMatrix(); //matryca kamery
  glTranslatef(pPosition.x,pPosition.y+6,pPosition.z); //6m ponad
  glRotated(RadToDeg(-vAngles.z),0,1,0); //obr�t wzgl�dem osi OY
  //glRotated(RadToDeg(vAngles.z),0,1,0); //obr�t wzgl�dem osi OY
  glDisable(GL_LIGHTING);
  glColor3f(1.0-fNr,fNr,0); //czerwone dla 0, zielone dla 1
  //glutWireCone(promie� podstawy,wysoko��,k�tno�� podstawy,ilo�� segment�w na wysoko��)
  //-glutWireCone(0.5,2,4,1); //rysowanie sto�ka (ostros�upa o podstawie wieloboka)
  glEnable(GL_LIGHTING);
 glPopMatrix();
}

//---------------------------------------------------------------------------

