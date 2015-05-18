//---------------------------------------------------------------------------

#ifndef GaugeH
#define GaugeH

#include "../commons.h"
#include "../commons_usr.h"
#include "Classes.h"
#include "parser.h"
//class Queryparsercomp::TQueryParserComp;
class TSubModel;
class TModel3d;
      
typedef enum
{//typ ruchu
 gt_Unknown, //na razie nie znany
 gt_Rotate,  //obr�t
 gt_Move,    //przesuni�cie r�wnoleg�e
 gt_Wiper,   //obr�t trzech kolejnych submodeli o ten sam k�t (np. wycieraczka, drzwi harmonijkowe)
 gt_Digital  //licznik cyfrowy, np. kilometr�w
} TGaugeType;

class TGauge //zmienne "gg"
{//animowany wska�nik, mog�cy przyjmowa� wiele stan�w po�rednich
private:
 TGaugeType eType; //typ ruchu
 double fFriction; //hamowanie przy zli�aniu si� do zadanej warto�ci
 double fDesiredValue; //warto�� docelowa
 double fValue; //warto�� obecna
 double fOffset; //warto�� pocz�tkowa ("0")
 double fScale; //warto�� ko�cowa ("1")
 double fStepSize; //nie u�ywane
 char cDataType; //typ zmiennej parametru: f-float, d-double, i-int
 union
 {//wska�nik na parametr pokazywany przez animacj�
  float* fData;
  double* dData;
  int* iData;
 };
public:
 TGauge();
 ~TGauge();
 void Clear();
 void Init(TSubModel *NewSubModel,TGaugeType eNewTyp,double fNewScale=1,double fNewOffset=0,double fNewFriction=0,double fNewValue=0);
 bool Load(cParser *Parser,TModel3d *md1,TModel3d *md2=NULL,double mul=1.0);
 void PermIncValue(double fNewDesired);
 void IncValue(double fNewDesired);
 void DecValue(double fNewDesired);
 void UpdateValue(double fNewDesired);
 void PutValue(double fNewDesired);
 double GetValue() {return fValue;};
 void Update();
 void Render();
 void AssignFloat(float* fValue);
 void AssignDouble(double* dValue);
 void AssignInt(int* iValue);
 void UpdateValue();
 TSubModel *SubModel; //McZapkie-310302: zeby mozna bylo sprawdzac czy zainicjowany poprawnie
};

//---------------------------------------------------------------------------
#endif

