//---------------------------------------------------------------------------
#ifndef MdlMngrH
#define MdlMngrH


#include "Model3d.h"
#include "Object3d.h"
#include "usefull.h"

class TMdlContainer
{
public:
	TModel3d *Model3D;
    //friend class TModelsManager;
    TMdlContainer();
	~TMdlContainer();
	TModel3d* LoadModel(static char *newName, bool dynamic);
    
    char *Name;
};

class TModelsManager
{
	
public:
	static int Count;
	static TModel3d* LoadModel(static char *Name, bool dynamic);
	static TMdlContainer* Models;
    static void Init();
    static void Free();
//McZapkie: dodalem sciezke, notabene Path!=Patch :)
	static int LoadModels(std::string asModelsPathC);
	static TModel3d* GetModel(static char *Name, bool dynamic = false);
};
//---------------------------------------------------------------------------
#endif
