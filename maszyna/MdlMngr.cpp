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
  if (mdl) Count++; //je�li b��d wczytania modelu, to go nie wliczamy
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

  strcpy(buf,"models\\"); //Ra: by�o by lepiej katalog doda� w parserze
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

  if (dynamic) //na razie tak, bo nie wiadomo, jaki mo�e mie� wp�yw na pozosta�e modele
   if (strchr(Name,'/')!=NULL)
   {//pobieranie tekstur z katalogu, w kt�rym jest model
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
 return (tmpModel); //NULL je�li b��d

};
*/


// CZY BEDZIE OK?
TModel3d *TModelsManager::GetModel(char *Name, bool dynamic)
{ //model mo�e by� we wpisie "node...model" albo "node...dynamic", a tak�e by� dodatkowym w dynamic (kabina, wn�trze, �adunek)
 //dla "node...dynamic" mamy podan� �cie�k� w "\dynamic\" i musi by� co najmniej 1 poziom, zwkle s� 2
 //dla "node...model" mo�e by� typowy model statyczny ze �cie�k�, domy�lnie w "\scenery\" albo "\models"
 //albo mo�e by� model z "\dynamic\", je�li potrzebujemy wstawi� auto czy wagon nieruchomo
 // - ze �cie�ki z kt�rej jest wywo�any, np. dir="scenery\bud\" albo dir="dynamic\pkp\st44_v1\" plus name="model.t3d"
 // - z domy�lnej �cie�ki dla modeli, np. "scenery\" albo "models\" plus name="bud\dombale.t3d" (dir="")
 // - konkretnie podanej �cie�ki np. name="\scenery\bud\dombale.t3d" (dir="")
 //wywo�ania:
 // - konwersja wszystkiego do E3D, podawana dok�adna �cie�ka, tekstury tam, gdzie plik
 // - wczytanie kabiny, dok�adna �cie�ka, tekstury z katalogu modelu
 // - wczytanie �adunku, �cie�ka dok�adna, tekstury z katalogu modelu
 // - wczytanie modelu, �cie�ka dok�adna, tekstury z katalogu modelu
 // - wczytanie przedsionk�w, �cie�ka dok�adna, tekstury z katalogu modelu
 // - wczytanie uproszczonego wn�trza, �cie�ka dok�adna, tekstury z katalogu modelu
 // - niebo animowane, �cie�ka brana ze wpisu, tekstury nieokre�lone
 // - wczytanie modelu animowanego - Init() - sprawdzi�

	if (Global::bLogFPHeaders) WriteLog("TModel3d *TModelsManager::GetModel(static char *Name, bool dynamic)");

	char buf[255];
	std::string buftp = Global::asCurrentTexturePath; // zapami�tanie aktualnej �cie�ki do
	// tekstur, bo b�dzie tyczmasowo zmieniana
	//WriteLogSS("INFO", buftp);

	if (strchr(Name, '\\') == NULL) 
	{
		strcpy(buf, "models\\"); // Ra: by�o by lepiej katalog doda� w parserze
		strcat(buf, Name);
		if (strchr(Name, '/') != NULL) 
		{
			Global::asCurrentTexturePath =	Global::asCurrentTexturePath + std::string(Name);
			Global::asCurrentTexturePath.erase(Global::asCurrentTexturePath.find("/") + 1,Global::asCurrentTexturePath.length());
		}
	}
	else 
	{strcpy(buf, Name);
		if (dynamic) // na razie tak, bo nie wiadomo, jaki mo�e mie� wp�yw na
			// pozosta�e modele
			if (strchr(Name, '/') != NULL) { // pobieranie tekstur z katalogu, w kt�rym jest model
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
	TModel3d *tmpModel = LoadModel(buf, dynamic); // model nie znaleziony, to wczyta�
	Global::asCurrentTexturePath = buftp; // odtworzenie �cie�ki do tekstur
	return (tmpModel); // NULL je�li b��d
};



//---------------------------------------------------------------------------
#pragma package(smart_init)
