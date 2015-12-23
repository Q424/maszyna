//---------------------------------------------------------------------------
// aircoupler.cpp is equal with 1166

#pragma hdrstop
#include "commons.h"
#include "commons_usr.h"
#include "AirCoupler.h"
#include "Timer.h"

TAirCoupler::TAirCoupler()
{
 Clear();
}

TAirCoupler::~TAirCoupler()
{
}

int TAirCoupler::GetStatus()
{//zwraca 1, jeœli istnieje model prosty, 2 gdy skoœny
 int x=0;
 if (pModelOn) x=1;
 if (pModelxOn) x=2;
 return x;
}

void TAirCoupler::Clear()
{//zerowanie wskaŸników
 pModelOn=NULL;
 pModelOff=NULL;
 pModelxOn=NULL;
 bOn=false;
 bxOn=false;
}


void TAirCoupler::Init(std::string asName, TModel3d *pModel)
{//wyszukanie submodeli
 if (!pModel) return; //nie ma w czym szukaæ
 pModelOn=pModel->GetFromName(std::string(asName+"_on").c_str()); //po³¹czony na wprost
 pModelOff=pModel->GetFromName(std::string(asName+"_off").c_str()); //odwieszony
 pModelxOn=pModel->GetFromName(std::string(asName+"_xon").c_str()); //po³¹czony na skos
}

void TAirCoupler::Load(cParser *Parser, TModel3d *pModel)
{
 std::string str;
 Parser->getToken(str);
 ToLowerCase(str);

 if (pModel)
  Init(str,pModel);
 else
 {
  pModelOn=NULL;
  pModelxOn=NULL;
  pModelOff=NULL;
 }
}

void TAirCoupler::Update()
{
//  if ((pModelOn!=NULL) && (pModelOn!=NULL))
   {
    if (pModelOn)
        pModelOn->iVisible=bOn;
    if (pModelOff)
        pModelOff->iVisible=!(bOn||bxOn);
    if (pModelxOn)
        pModelxOn->iVisible=bxOn;
   }
}

//---------------------------------------------------------------------------

#pragma package(smart_init)
