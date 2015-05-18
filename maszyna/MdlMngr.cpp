//---------------------------------------------------------------------------

/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak and others

*/
// mdlmngr.cpp is equal with 1166

#include "Texture.h"
#include "MdlMngr.h"
#include "Globals.h"
#include "object3d.h"

#define SeekFiles AnsiString("*.t3d")

TMdlContainer::TMdlContainer()
{
	Name = NULL; 
	Model3D = NULL;
}

TMdlContainer::~TMdlContainer()
{
 SafeDeleteArray(Name); 
 SafeDelete(Model3D); 
}

TModel3d* TMdlContainer::LoadModel( char *newName, bool dynamic)
{
if (Global::bLogFPHeaders) WriteLog("TModel3d* TMdlContainer::LoadModel(char *newName,bool dynamic)");
 SafeDeleteArray(Name);
 SafeDelete(Model3D);
 Name= new char[strlen(newName)+1];
 strcpy(Name, newName);
 Model3D = new TModel3d();
 if (!Model3D->LoadFromFile(Name, dynamic)) //np. "models\\pkp/head1-y.t3d"
  SafeDelete(Model3D);
 return Model3D;
};

TMdlContainer *TModelsManager::Models;
int TModelsManager::Count;
int MAX_MODELS = 1000;

void TModelsManager::Init()
{
	Models = new TMdlContainer[MAX_MODELS];
    Count = 0;
}

void TModelsManager::Free()
{
    SafeDeleteArray(Models);
}



double Radius;


TModel3d* TModelsManager::LoadModel(char *Name, bool dynamic)
{
 if (Global::bLogFPHeaders) WriteLog("TModel3d*  TModelsManager::LoadModel(char *Name, bool dynamic)");
 TModel3d *mdl=NULL;
 if (Count == MAX_MODELS) WriteLogSS("FIXME: Too many models, program will now crash :)", "ERROR");
 else
 {
  mdl=Models[Count].LoadModel(Name,dynamic);
  if (mdl) Count++; //jeœli b³¹d wczytania modelu, to go nie wliczamy
 }
 return mdl;
}

/*
TModel3d* TModelsManager::GetModel(static char *Name, bool dynamic)
{
 WriteLog("TModelsManager::GetModel()");

 char buf[255];
// std::string buftp=Global::asCurrentTexturePath;
 TModel3d* tmpModel; //tymczasowe zmienne

 
 //WriteLogSS("["+chartostdstr(Name)+"]", "INFO");
 if (strchr(Name,'\\')==NULL)
 {
  //WriteLog("ADD DEF PATH");

  strcpy(buf,"models\\"); //Ra: by³o by lepiej katalog dodaæ w parserze
  strcat(buf,Name);

  if (strchr(Name,'/')!=NULL)
  {
  //- Global::asCurrentTexturePath= Global::asCurrentTexturePath + chartostdstr(Name);
  //- Global::asCurrentTexturePath.erase(Global::asCurrentTexturePath.Pos("/")+1, Global::asCurrentTexturePath.length());
  }
 }
 else

 {
  strcpy(buf,Name);

  if (dynamic) //na razie tak, bo nie wiadomo, jaki mo¿e mieæ wp³yw na pozosta³e modele
   if (strchr(Name,'/')!=NULL)
   {//pobieranie tekstur z katalogu, w którym jest model
    //Global::asCurrentTexturePath= Global::asCurrentTexturePath+ Name;
    //Global::asCurrentTexturePath.erase(Global::asCurrentTexturePath.find("/")+1,Global::asCurrentTexturePath.length());
   }
 
 }

 
 ToLowerCase(buf);
 //WriteLog("CHECK MODELS TAB");
 for (int i=0; i<Count; i++)
 {
  if (strcmp(buf,Models[i].Name)==0)
  {
	  WriteLog("FROM MEMORY");
  // Global::asCurrentTexturePath= buftp;
   return (Models[i].Model3D);
  }
 };
 

 WriteLog("FIRST INSTANCE");
// WriteLog(buf);
 tmpModel = LoadModel(buf, dynamic);
// Global::asCurrentTexturePath=buftp;
 return (tmpModel); //NULL jeœli b³¹d

};
*/


// CZY BEDZIE OK?
TModel3d *TModelsManager::GetModel(char *Name, bool dynamic)
{ //model mo¿e byæ we wpisie "node...model" albo "node...dynamic", a tak¿e byæ dodatkowym w dynamic (kabina, wnêtrze, ³adunek)
 //dla "node...dynamic" mamy podan¹ œcie¿kê w "\dynamic\" i musi byæ co najmniej 1 poziom, zwkle s¹ 2
 //dla "node...model" mo¿e byæ typowy model statyczny ze œcie¿k¹, domyœlnie w "\scenery\" albo "\models"
 //albo mo¿e byæ model z "\dynamic\", jeœli potrzebujemy wstawiæ auto czy wagon nieruchomo
 // - ze œcie¿ki z której jest wywo³any, np. dir="scenery\bud\" albo dir="dynamic\pkp\st44_v1\" plus name="model.t3d"
 // - z domyœlnej œcie¿ki dla modeli, np. "scenery\" albo "models\" plus name="bud\dombale.t3d" (dir="")
 // - konkretnie podanej œcie¿ki np. name="\scenery\bud\dombale.t3d" (dir="")
 //wywo³ania:
 // - konwersja wszystkiego do E3D, podawana dok³adna œcie¿ka, tekstury tam, gdzie plik
 // - wczytanie kabiny, dok³adna œcie¿ka, tekstury z katalogu modelu
 // - wczytanie ³adunku, œcie¿ka dok³adna, tekstury z katalogu modelu
 // - wczytanie modelu, œcie¿ka dok³adna, tekstury z katalogu modelu
 // - wczytanie przedsionków, œcie¿ka dok³adna, tekstury z katalogu modelu
 // - wczytanie uproszczonego wnêtrza, œcie¿ka dok³adna, tekstury z katalogu modelu
 // - niebo animowane, œcie¿ka brana ze wpisu, tekstury nieokreœlone
 // - wczytanie modelu animowanego - Init() - sprawdziæ

	if (Global::bLogFPHeaders) WriteLog("TModel3d *TModelsManager::GetModel(static char *Name, bool dynamic)");

	char buf[255];
	std::string buftp = Global::asCurrentTexturePath; // zapamiêtanie aktualnej œcie¿ki do
	// tekstur, bo bêdzie tyczmasowo zmieniana
	//WriteLogSS("INFO", buftp);

	if (strchr(Name, '\\') == NULL) 
	{
		strcpy(buf, "models\\"); // Ra: by³o by lepiej katalog dodaæ w parserze
		strcat(buf, Name);
		if (strchr(Name, '/') != NULL) 
		{
			Global::asCurrentTexturePath =	Global::asCurrentTexturePath + std::string(Name);
			Global::asCurrentTexturePath.erase(Global::asCurrentTexturePath.find("/") + 1,Global::asCurrentTexturePath.length());
		}
	}
	else 
	{strcpy(buf, Name);
		if (dynamic) // na razie tak, bo nie wiadomo, jaki mo¿e mieæ wp³yw na
			// pozosta³e modele
			if (strchr(Name, '/') != NULL) { // pobieranie tekstur z katalogu, w którym jest model
				Global::asCurrentTexturePath =Global::asCurrentTexturePath + std::string(Name);
				Global::asCurrentTexturePath.erase(Global::asCurrentTexturePath.find("/") + 1, Global::asCurrentTexturePath.length());
			}
	}
	ToLowerCase(buf);
	for (int i = 0; i < Count; i++) 
	{
		if (strcmp(buf, Models[i].Name) == 0) 
		{
			Global::asCurrentTexturePath = buftp;
			return (Models[i].Model3D);
		}
	};
	TModel3d *tmpModel = LoadModel(buf, dynamic); // model nie znaleziony, to wczytaæ
	Global::asCurrentTexturePath = buftp; // odtworzenie œcie¿ki do tekstur
	return (tmpModel); // NULL jeœli b³¹d
};



//---------------------------------------------------------------------------
#pragma package(smart_init)
