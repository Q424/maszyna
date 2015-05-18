//---------------------------------------------------------------------------

#ifndef VBOH
#define VBOH


//---------------------------------------------------------------------------
class CVertNormTex
{
public:
 float x;  //X wierzcho�ka
 float y;  //Y wierzcho�ka
 float z;  //Z wierzcho�ka
 float nx; //X wektora normalnego
 float ny; //Y wektora normalnego
 float nz; //Z wektora normalnego
 float u;  //U mapowania
 float v;  //V mapowania
};

class CMesh
{//wsparcie dla VBO
public:
 int m_nVertexCount; //liczba wierzcho�k�w
 CVertNormTex *m_pVNT;
 unsigned int m_nVBOVertices; //numer VBO z wierzcho�kami
 CMesh();
 ~CMesh();
 void MakeArray(int n); //tworzenie tablicy z elementami VNT
 void BuildVBOs(bool del=true); //zamiana tablic na VBO
 void Clear(); //zwolnienie zasob�w
 bool StartVBO();
 void EndVBO();
 bool StartColorVBO();
};

#endif

