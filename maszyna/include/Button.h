//---------------------------------------------------------------------------

#ifndef ButtonH
#define ButtonH

#include "Model3d.h"
#include "parser.h"

class TButton
{//animacja dwustanowa, w��cza jeden z dw�ch submodeli (jednego z nich mo�e nie by�)
private:
 TSubModel *pModelOn,*pModelOff; //submodel dla stanu za��czonego i wy��czonego
 bool bOn;
 int iFeedbackBit; //Ra: bit informacji zwrotnej, do wyprowadzenia na pulpit
 void  Update();
public:
  TButton();
  ~TButton();
 void  Clear(int i=-1);
 inline void FeedbackBitSet(int i) {iFeedbackBit=1<<i;};
 inline void Turn(bool to) { bOn=to; Update(); };
 inline void TurnOn() { bOn=true; Update(); };
 inline void TurnOff() { bOn=false; Update(); };
 inline void Switch() { bOn=!bOn; Update(); };
 inline bool Active() { return (pModelOn)||(pModelOff);};
 void  Init(std::string asName,TModel3d *pModel,bool bNewOn=false);
 void  Load(cParser *Parser,TModel3d *pModel1,TModel3d *pModel2=NULL);
};

//---------------------------------------------------------------------------
#endif
