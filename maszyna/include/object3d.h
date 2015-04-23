
#ifndef object3d_H
#define object3d_H

#include "../commons.h"
#include "../commons_usr.h"
#include "ResourceManager.h"
#include "dumb3d.h"
#include "float3d.h"
#include "VBO.h"



enum TAnimationType //rodzaj animacji
{
	qat_None, //brak
	qat_Rotate, //obr�t wzgl�dem wektora o k�t
	qat_RotateXYZ, //obr�t wzgl�dem osi o k�ty
	qat_Translate, //przesuni�cie
	qat_SecondsJump, //sekundy z przeskokiem
	qat_MinutesJump, //minuty z przeskokiem
	qat_HoursJump, //godziny z przeskokiem 12h/360�
	qat_Hours24Jump, //godziny z przeskokiem 24h/360�
	qat_Seconds, //sekundy p�ynnie
	qat_Minutes, //minuty p�ynnie
	qat_Hours, //godziny p�ynnie 12h/360�
	qat_Hours24, //godziny p�ynnie 24h/360�
	qat_Billboard, //obr�t w pionie do kamery
	qat_Wind, //ruch pod wp�ywem wiatru
	qat_Sky, //animacja nieba
	qat_IK = 0x100, //odwrotna kinematyka - submodel steruj�cy (np. staw skokowy)
	qat_IK11 = 0x101, //odwrotna kinematyka - submodel nadrz�dny do sterowango (np. stopa)
	qat_IK21 = 0x102, //odwrotna kinematyka - submodel nadrz�dny do sterowango (np. podudzie)
	qat_IK22 = 0x103, //odwrotna kinematyka - submodel nadrz�dny do nadrz�dnego sterowango (np. udo)
	qat_Undefined = 0x800000FF //animacja chwilowo nieokre�lona
};

class TObject3d;
class TSubObject;
class TSubObjectInfo;


class TQStringPack
{
	char *data;
	//+0 - 4 bajty: typ kromki
	//+4 - 4 bajty: d�ugo�� ��cznie z nag��wkiem
	//+8 - obszar �a�cuch�w znakowych, ka�dy zako�czony zerem
	int *index;
	//+0 - 4 bajty: typ kromki
	//+4 - 4 bajty: d�ugo�� ��cznie z nag��wkiem
	//+8 - tabela indeks�w
public:
	char* String(int n);
	char* StringAt(int n) { return data + 9 + n; };
	TQStringPack() { data = NULL; index = NULL; };
	void Init(char *d) { data = d; };
	void InitIndex(int *i) { index = i; };
};
class TSubObjectInfo
{//klasa z informacjami o submodelach, do tworzenia pliku binarnego
public:
	TSubObject *pSubModel; //wska�nik na submodel
	int iTransform; //numer transformu (-1 gdy brak)
	int iName; //numer nazwy
	int iTexture; //numer tekstury
	int iNameLen; //d�ugo�� nazwy
	int iTextureLen; //d�ugo�� tekstury
	int iNext, iChild; //numer nast�pnego i potomnego
	static int iTotalTransforms; //ilo�� transform�w
	static int iTotalNames; //ilo�� nazw
	static int iTotalTextures; //ilo�� tekstur
	static int iCurrent; //aktualny obiekt
	static TSubObjectInfo* pTable; //tabele obiekt�w pomocniczych
	TSubObjectInfo()
	{
		pSubModel = NULL;
		iTransform = iName = iTexture = iNext = iChild = -1; //nie ma
		iNameLen = iTextureLen = 0;
	}
	void Reset()
	{
		pTable = this; //ustawienie wska�nika tabeli obiekt�w
		iTotalTransforms = iTotalNames = iTotalTextures = iCurrent = 0; //zerowanie licznik�w
	}
	~TSubObjectInfo() {};
};


class TSubObject
{//klasa submodelu - pojedyncza siatka, punkt �wietlny albo grupa punkt�w
	//Ra: ta klasa ma mie� wielko�� 256 bajt�w, aby pokry�a si� z formatem binarnym
	//Ra: nie przestawia� zmiennych, bo wczytuj� si� z pliku binarnego!
private:
	TSubObject *Next;
	TSubObject *Child;
	int eType; //Ra: modele binarne daj� wi�cej mo�liwo�ci ni� mesh z�o�ony z tr�jk�t�w
	int iName; //numer �a�cucha z nazw� submodelu, albo -1 gdy anonimowy
public: //chwilowo
	TAnimationType b_Anim;
private:
	int iFlags; //flagi informacyjne:
	//bit  0: =1 faza rysowania zale�y od wymiennej tekstury 0
	//bit  1: =1 faza rysowania zale�y od wymiennej tekstury 1
	//bit  2: =1 faza rysowania zale�y od wymiennej tekstury 2
	//bit  3: =1 faza rysowania zale�y od wymiennej tekstury 3
	//bit  4: =1 rysowany w fazie nieprzezroczystych (sta�a tekstura albo brak)
	//bit  5: =1 rysowany w fazie przezroczystych (sta�a tekstura)
	//bit  7: =1 ta sama tekstura, co poprzedni albo nadrz�dny
	//bit  8: =1 wierzcho�ki wy�wietlane z indeks�w
	//bit  9: =1 wczytano z pliku tekstowego (jest w�a�cicielem tablic)
	//bit 13: =1 wystarczy przesuni�cie zamiast mno�enia macierzy (trzy jedynki)
	//bit 14: =1 wymagane przechowanie macierzy (animacje)
	//bit 15: =1 wymagane przechowanie macierzy (transform niejedynkowy)
	union
	{//transform, nie ka�dy submodel musi mie�
		float4x4 *fMatrix; //pojedyncza precyzja wystarcza
		//matrix4x4 *dMatrix; //do testu macierz podw�jnej precyzji
		int iMatrix; //w pliku binarnym jest numer matrycy
	};
	int iNumVerts; //ilo�� wierzcho�k�w (1 dla FreeSpotLight)
	int iVboPtr; //pocz�tek na li�cie wierzcho�k�w albo indeks�w
	int iTexture; //numer nazwy tekstury, -1 wymienna, 0 brak
	float fVisible; //pr�g jasno�ci �wiat�a do za��czenia submodelu
	float fLight; //pr�g jasno�ci �wiat�a do zadzia�ania selfillum
	float f4Ambient[4];
	float f4Diffuse[4]; //float ze wzgl�du na glMaterialfv()
	float f4Specular[4];
	float f4Emision[4];
	float fWireSize; //nie u�ywane, ale wczytywane
	float fSquareMaxDist;
	float fSquareMinDist;
	//McZapkie-050702: parametry dla swiatla:
	float fNearAttenStart;
	float fNearAttenEnd;
	int bUseNearAtten;      //te 3 zmienne okreslaja rysowanie aureoli wokol zrodla swiatla
	int iFarAttenDecay;      //ta zmienna okresla typ zaniku natezenia swiatla (0:brak, 1,2: potega 1/R)
	float fFarDecayRadius;  //normalizacja j.w.
	float fCosFalloffAngle; //cosinus k�ta sto�ka pod kt�rym wida� �wiat�o
	float fCosHotspotAngle; //cosinus k�ta sto�ka pod kt�rym wida� aureol� i zwi�kszone nat�enie �wiat�a
	float fCosViewAngle;    //cos kata pod jakim sie teraz patrzy
	//Ra: dalej s� zmienne robocze, mo�na je przestawia� z zachowaniem rozmiaru klasy
	int TextureID; //numer tekstury, -1 wymienna, 0 brak
	int bWire; //nie u�ywane, ale wczytywane
	//short TexAlpha;  //Ra: nie u�ywane ju�
	GLuint uiDisplayList; //roboczy numer listy wy�wietlania
	float Opacity; //nie u�ywane, ale wczytywane
	//ABu: te same zmienne, ale zdublowane dla Render i RenderAlpha,
	//bo sie chrzanilo przemieszczanie obiektow.
	//Ra: ju� si� nie chrzani
	float f_Angle;
	float3 v_RotateAxis;
	float3 v_Angles;
public: //chwilowo
	float3 v_TransVector;
	float8 *Vertices; //roboczy wska�nik - wczytanie T3D do VBO
	int iAnimOwner; //roboczy numer egzemplarza, kt�ry ustawi� animacj�
	TAnimationType b_aAnim; //kody animacji oddzielnie, bo zerowane
public:
	float4x4 *mAnimMatrix; //macierz do animacji kwaternionowych (nale�y do AnimContainer)
	char space[8]; //wolne miejsce na przysz�e zmienne (zmniejszy� w miar� potrzeby)
public:
	TSubObject **smLetter; //wska�nik na tablic� submdeli do generoania tekstu (docelowo zapisa� do E3D)
	TSubObject *Parent; //nadrz�dny, np. do wymna�ania macierzy
	int iVisible; //roboczy stan widoczno�ci
	//std::string asTexture; //robocza nazwa tekstury do zapisania w pliku binarnym
	//std::string asName; //robocza nazwa
	char *pTexture; //robocza nazwa tekstury do zapisania w pliku binarnym
	char *pName; //robocza nazwa
private:
	//int SeekFaceNormal(DWORD *Masks, int f,DWORD dwMask,vector3 *pt,GLVERTEX *Vertices);
	int SeekFaceNormal(DWORD *Masks, int f, DWORD dwMask, float3 *pt, float8 *Vertices);
	void RaAnimation(TAnimationType a);
public:
	static int iInstance; //identyfikator egzemplarza, kt�ry aktualnie renderuje model
	static GLuint *ReplacableSkinId;
	static int iAlpha; //maska bitowa dla danego przebiegu
	static double fSquareDist;
	static TModel3d* pRoot;
	static std::string* pasText; //tekst dla wy�wietlacza (!!!! do przemy�lenia)
	TSubObject();
	~TSubObject();
	void FirstInit();
	int Load(cParser& Parser, TModel3d *Model, int Pos, bool dynamic);
	void ChildAdd(TSubObject *SubModel);
	void NextAdd(TSubObject *SubModel);
	TSubObject* NextGet() { return Next; };
	TSubObject* ChildGet() { return Child; };
	int TriangleAdd(TModel3d *m, int tex, int tri);
	float8* TrianglePtr(int tex, int pos, int *la, int *ld, int*ls);
	//float8* TrianglePtr(const char *tex,int tri);
	//void SetRotate(vector3 vNewRotateAxis,float fNewAngle);
	void SetRotate(float3 vNewRotateAxis, float fNewAngle);
	void SetRotateXYZ(vector3 vNewAngles);
	void SetRotateXYZ(float3 vNewAngles);
	void SetTranslate(vector3 vNewTransVector);
	void SetTranslate(float3 vNewTransVector);
	void SetRotateIK1(float3 vNewAngles);
  //TSubObject* GetFromName(std::string search, bool i = true);
	TSubObject* GetFromNameQ(char *search, bool i = true);
	void RenderDL();
	void RenderAlphaDL();
	void RenderVBO();
	void RenderAlphaVBO();
	//inline matrix4x4* GetMatrix() {return dMatrix;};
	inline float4x4* GetMatrix() { return fMatrix; };
	//matrix4x4* GetTransform() {return Matrix;};
	inline void Hide() { iVisible = 0; };
	void RaArrayFill(CVertNormTex *Vert);
	//void Render();
	int FlagsCheck();
	void WillBeAnimated() { if (this) iFlags |= 0x4000; };
	void InitialRotate(bool doit);
	void DisplayLists();
	void Info();
	void InfoSet(TSubObjectInfo *info);
	void BinInit(TSubObject *s, float4x4 *m, float8 *v, TQStringPack *t, TQStringPack *n = NULL, bool dynamic = false);
	void ReplacableSet(GLuint *r, int a)
	{
		ReplacableSkinId = r; iAlpha = a;
	};
	void TextureNameSet(const char *n);
	void NameSet(const char *n);
	//Ra: funkcje do budowania terenu z E3D
	int Flags() { return iFlags; };
	void UnFlagNext() { iFlags &= 0x00FFFFFF; };
	void ColorsSet(int *a, int *d, int*s);
	inline float3 Translation1Get()
	{
		return fMatrix ? *(fMatrix->TranslationGet()) + v_TransVector : v_TransVector;
	}
	inline float3 Translation2Get()
	{
		return *(fMatrix->TranslationGet()) + Child->Translation1Get();
	}
	void ParentMatrix(float4x4 *m);
	float MaxY(const float4x4 &m);
	void AdjustDist();
};


class TObject3d : public CMesh
{
private:
	TSubObject *Root; //drzewo submodeli
	int iFlags; //Ra: czy submodele maj� przezroczyste tekstury
public: 
	int iNumVerts; //ilo�� wierzcho�k�w (gdy nie ma VBO, to m_nVertexCount=0)
private:
	TQStringPack Textures; //nazwy tekstur
	TQStringPack Names; //nazwy submodeli
	int *iModel; //zawarto�� pliku binarnego
	int iSubModelsCount; //Ra: u�ywane do tworzenia binarnych
	std::string asBinary; //nazwa pod kt�r� zapisa� model binarny
public:
	inline TSubObject* GetSMRoot() { return(Root); };
	TObject3d();
    TObject3d(char *FileName);
	~TObject3d();
	TSubObject* GetFromNameQ(char *sName);
	TSubObject* AddToNamed(const char *Name, TSubObject *SubModel);
	void AddTo(TSubObject *tmp, TSubObject *SubModel);
	void LoadFromTextFile(std::string FileName, bool dynamic);
	void LoadFromBinFile(char *FileName, bool dynamic);
	bool LoadFromFile(std::string FileName, bool dynamic);
	void SaveToBinFile(char *FileName);
	void BreakHierarhy();
	//renderowanie specjalne
	void Render(double fSquareDistance, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RenderAlpha(double fSquareDistance, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RaRender(double fSquareDistance, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RaRenderAlpha(double fSquareDistance, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	//jeden k�t obrotu
	void Render(vector3 pPosition, double fAngle = 0, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RenderAlpha(vector3 pPosition, double fAngle = 0, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RaRender(vector3 pPosition, double fAngle = 0, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RaRenderAlpha(vector3 pPosition, double fAngle = 0, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	//trzy k�ty obrotu
	void Render(vector3* vPosition, vector3* vAngle, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RenderAlpha(vector3* vPosition, vector3* vAngle, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RaRender(vector3* vPosition, vector3* vAngle, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	void RaRenderAlpha(vector3* vPosition, vector3* vAngle, GLuint *ReplacableSkinId = NULL, int iAlpha = 0x30300030);
	//inline int GetSubModelsCount() { return (SubModelsCount); };
	int Flags() { return iFlags; };
	void Init();
	char* NameGet() { return Root ? Root->pName : NULL; };
	int TerrainCount();
	TSubObject* TerrainSquare(int n);
	void TerrainRenderVBO(int n);
};




//---------------------------------------------------------------------------
#endif
