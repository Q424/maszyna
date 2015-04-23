// BUTTON.H 291 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef ButtonH
#define ButtonH

#include "../commons.h"
#include "../commons_usr.h"
#include "Model3d.h"
//#include "QueryParserComp.hpp"

class TButton
{
private:
 //TButtonType eType;
 TSubModel *pModelOn,*pModelOff;
 bool bOn;
 int iFeedbackBit; //Ra: informacja zwrotna: 0=SHP, 1=CA, 256-na oporach
 void __fastcall Update();
public:
 TButton();
 ~TButton();
 void __fastcall Clear();
 inline void FeedbackBitSet(int i) {iFeedbackBit=1<<i;};
 inline void TurnOn() { bOn= true; Update(); };
 inline void TurnOff() { bOn= false; Update(); };
 inline void Switch() { bOn= !bOn; Update(); };
 inline bool Active() { return (pModelOn)||(pModelOff);};
 void Init(std::string asName, TModel3d *pModel, bool bNewOn=false);
// void __fastcall Load(TQueryParserComp *Parser, TModel3d *pModel);
};

//---------------------------------------------------------------------------
#endif
