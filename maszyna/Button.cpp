//---------------------------------------------------------------------------
// button.cpp is equal with 1166

#pragma hdrstop

#include "commons.h"
#include "commons_usr.h"

//#include "button.h"
//#include "Console.h"

//---------------------------------------------------------------------------

TButton::TButton()
{
 iFeedbackBit=0;
 Clear();
};

TButton::~TButton()
{
};

void TButton::Clear(int i)
{
 pModelOn=NULL;
 pModelOff=NULL;
 bOn=false;
 if (i>=0) FeedbackBitSet(i);
 Update(); //kasowanie bitu Feedback, o ile jaki� ustawiony
};

void TButton::Init(std::string asName,TModel3d *pModel,bool bNewOn)
{
 if (!pModel) return; //nie ma w czym szuka�
 pModelOn=pModel->GetFromName(std::string(asName+"_on").c_str());
 pModelOff=pModel->GetFromName(std::string(asName+"_off").c_str());
 bOn=bNewOn;
 Update();
};

void TButton::Load(cParser *Parser,TModel3d *pModel1,TModel3d *pModel2)
{
 std::string str;
 Parser->getToken(str);

 ToLowerCase(str);

 if (pModel1)
 {//poszukiwanie submodeli w modelu
  Init(str,pModel1,false);
  if (pModel2)
   if (!pModelOn&&!pModelOff)
    Init(str,pModel2,false); //mo�e w drugim b�dzie (jak nie w kabinie, to w zewn�trznym)
 }
 else
 {
  pModelOn=NULL;
  pModelOff=NULL;
 }
};

void TButton::Update()
{
 if (pModelOn) pModelOn->iVisible=bOn;
 if (pModelOff) pModelOff->iVisible=!bOn;
 if (iFeedbackBit) //je�eli generuje informacj� zwrotn�
 {if (bOn) //zapalenie
   Console::BitsSet(iFeedbackBit);
  else
   Console::BitsClear(iFeedbackBit);
 }
};

