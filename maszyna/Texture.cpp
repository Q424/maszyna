//---------------------------------------------------------------------------

/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak and others

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
  

#include "commons_usr.h"

//#include <olectl.h>	


//AnsiString TexDir= "Textures\\";
const bool bGlobalOptimize= false;


TTexture *TTexturesManager::First;

/*
bool __fastcall TTexture::LoadJPG(char *szPathName)						// Load Image And Convert To A Texture
{
	HDC			hdcTemp;												// The DC To Hold Our Bitmap
	HBITMAP		hbmpTemp;												// Holds The Bitmap Temporarily
	IPicture	*pPicture;												// IPicture Interface
	OLECHAR		wszPath[MAX_PATH+1];									// Full Path To Picture (WCHAR)
	char		szPath[MAX_PATH+1];										// Full Path To Picture
	long		lWidth;													// Width In Logical Units
	long		lHeight;												// Height In Logical Units
	long		lWidthPixels;											// Width In Pixels
	long		lHeightPixels;											// Height In Pixels
	GLint		glMaxTexDim ;											// Holds Maximum Texture Size
	//WriteLog("OLE!!!");
	//WriteLog(szPathName);

	if (strstr(szPathName, "www"))									    // If PathName Contains http:// Then...
	{
		std::string tmp;
		tmp.append(szPathName);    // PRZEPISUJEMY Z char DO std::string
		//WriteLogSS("sss:", tmp);
		tmp = tmp.substr(8, 144);  //  USUWAMY tex/www.
		//WriteLogSS("xxx=", tmp);
		strcpy(szPath, "http://"); // NADPISUJEMY szPath prefixem protokolu
		strcat(szPath, tmp.c_str()); // DODAJEMY DO TEGO ADRES PLIKU
		WriteLog( szPath);
		//strcpy(szPath, szPathName);						        	// Append The PathName To szPath
	}
	else																// Otherwise... We Are Loading From A File
	{
		GetCurrentDirectory(MAX_PATH, szPath);							// Get Our Working Directory
		strcat(szPath, "\\");											// Append "\" After The Working Directory
		strcat(szPath, szPathName);										// Append The PathName
	}

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);		// Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if(FAILED(hr))														// If Loading Failed
		return FALSE;													// Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));								// Create The Windows Compatible Device Context
	if(!hdcTemp)														// Did Creation Fail?
	{
		pPicture->Release();											// Decrements IPicture Reference Count
		return FALSE;													// Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);					// Get Maximum Texture Size Supported
	
	pPicture->get_Width(&lWidth);										// Get IPicture Width (Convert To Pixels)
	lWidthPixels	= MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);										// Get IPicture Height (Convert To Pixels)
	lHeightPixels	= MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim) // Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
	else  // Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;
 
	if (lHeightPixels <= glMaxTexDim) // Is Image Height Greater Than Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
	else  // Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;
	
	//	Create A Temporary Bitmap
	BITMAPINFO	bi = {0};												// The Type Of Bitmap We Request
	DWORD		*pBits = 0;												// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);				// Set Structure Size
	bi.bmiHeader.biBitCount		= 32;									// 32 Bit
	bi.bmiHeader.biWidth		= lWidthPixels;							// Power Of Two Width
	bi.bmiHeader.biHeight		= lHeightPixels;						// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression	= BI_RGB;								// RGB Encoding
	bi.bmiHeader.biPlanes		= 1;									// 1 Bitplane

	//	Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
	
	if(!hbmpTemp)														// Did Creation Fail?
	{
		DeleteDC(hdcTemp);												// Delete The Device Context
		pPicture->Release();											// Decrements IPicture Reference Count
		return FALSE;													// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);									// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for(long i = 0; i < lWidthPixels * lHeightPixels; i++)				// Loop Through All Of The Pixels
	{
		BYTE* pPixel	= (BYTE*)(&pBits[i]);							// Grab The Current Pixel
		BYTE  temp		= pPixel[0];									// Store 1st Color In Temp Variable (Blue)
		pPixel[0]		= pPixel[2];									// Move Red Value To Correct Position (1st)
		pPixel[2]		= temp;											// Move Temp Value To Correct Blue Position (3rd)
		pPixel[3]		= 255;											// Set The Alpha Value To 255
	}

	glGenTextures(1, &ID);											    // Create The Texture

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, ID);								    // Bind To The Texture ID
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);		// (Modify This For The Type Of Filtering You Want)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);     // (Modify This For The Type Of Filtering You Want)

	// (Modify This If You Want Mipmaps)
	glTexImage2D(GL_TEXTURE_2D, 0, 3, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

	DeleteObject(hbmpTemp);												// Delete The Object
	DeleteDC(hdcTemp);													// Delete The Device Context

	pPicture->Release();												// Decrements IPicture Reference Count

	return TRUE;														// Return True (All Good)
}
*/

__fastcall TTexture::TTexture(char* szFileName, TTexture *NNext )
{
    bool ret;
    //char buf[255];
    ID=0;
	//WriteLog(szFileName);
    char buff[255]= "Loading texture: ";
    strcat_s(buff,szFileName);
    WriteLog(buff);
//    TWorld.UpdateWindow();

	// POBIERANIE ROZSZERZENIA, 4 ZNAKI OD KONCA. NP: .jpg
	std::string qext;
    qext.append(szFileName);
    int size = qext.length();
    qext = qext.substr(size-4,4);
    //WriteLogSS("QEXT=", qext);

    char ext[100]; //= strchr(szFileName,'.'); // ZLE POBIERANIE ROZSZERZENIA - BIERZE PO PIERWSZEJ KROPCE !!!!

	sprintf_s(ext,"%s", qext.c_str()); // NADPISANIE ZMIENNEJ EXT PRAWIDLOWYM ROZSZERZENIEM
	//WriteLog(ext);

    if (strcmp(ext,".tex")==0)
        ret= LoadTEX(szFileName);
//    else
//    if (strcmp(ext,".jpg")==0)
//        ret= LoadJPG(szFileName);
//    else
//    if (strcmp(ext,".gif")==0)
//        ret= LoadJPG(szFileName);
//    else
//    if (strcmp(ext,".png")==0)
//        ret= LoadJPG(szFileName);
    else
    if (strcmp(ext,".bmp")==0)
        ret= LoadBMP(szFileName);
    else
    if (strcmp(ext,".tga")==0)
        ret= LoadTargaFile(szFileName);
    else
	{
     char tolog[100];
     sprintf_s(tolog, "Failed, unknown extension: %s", ext);
     WriteLog(tolog);
	}
    if (ret)
        WriteLog("Success");
    else
        WriteLog("Failed");


    Name= new char[strlen(szFileName)+1];

    strcpy(Name,szFileName);

    Next= NNext;
};

///////////////////////////////////////////////////////////////////////////////
GLuint CreateTexture(byte *buff, int bpp, int width, int Height, bool bHasAlpha, bool bHash, bool bOptimize)
{
    WORD *tempBuff= NULL;
    GLuint ID;
    glGenTextures(1,&ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);


    if ( bHasAlpha || bHash )
     {
      if (bHasAlpha) // przezroczystosc: nie wlaczac mipmapingu
       {
         if (bHash) // #: calkowity brak filtracji
          {
           glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
           glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          }
         else
          {
           glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
           glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          }
       }
      else  // #: filtruj ale bez dalekich mipmap
       {
         glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       }
     }
    else // filtruj wszystko
     {
       glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
     }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);


/*
    if (bOptimize)
    {
        if (bHasAlpha)
        {
            tempBuff= new WORD[width*Height];
            for (int i=0; i<width*Height; i++)
            {
                tempBuff[i]= (buff[i*4  ]/31) << 11+
                             (buff[i*4+1]/31) << 6+
                             (buff[i*4+2]/31) << 1+
                             (buff[i*4+3]/255);
            }
        }
        else
        {

        }
    }
  */

    if (bOptimize)
        if (bHasAlpha || bHash )
            glTexImage2D(GL_TEXTURE_2D, 0, ( bHasAlpha ? GL_RGBA2 : GL_R3_G3_B2 ), width, Height,
                 0, ( bHasAlpha ? GL_RGBA : GL_RGB ), GL_UNSIGNED_BYTE, buff);
        else
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_R3_G3_B2, width, Height,
                 GL_RGB, GL_UNSIGNED_BYTE, buff);
    else
        if (bHasAlpha || bHash )
            glTexImage2D(GL_TEXTURE_2D, 0, ( bHasAlpha ? GL_RGBA : GL_RGB ), width, Height, 0,
                 ( bHasAlpha ? GL_RGBA : GL_RGB ), GL_UNSIGNED_BYTE, buff);
        else
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, Height,
                 GL_RGB, GL_UNSIGNED_BYTE, buff);
    SafeDelete(tempBuff);
    return ID;
}
///////////////////////////////////////////////////////////////////////////////

bool __fastcall TTexture::LoadTEX(char* szFileName)
{
	/*
    int width= -1;
    int Height= -1;

//	FileStream^ fs = gcnew FileStream( "c:\\Variables.txt",FileMode::Append,FileAccess::Write,FileShare::Write );
    FileStream *fs= new FileStream(szFileName,fmOpenRead | fmShareCompat);


    ID= 0;

//    bool Alpha;
    AnsiString head;
    head= "    ";
    fs->Read(head.c_str(),4);
    if (head==AnsiString("RGB "))
        HasAlpha= false;
    else
        if (head==AnsiString("RGBA"))
            HasAlpha= true;
        else
        {
            Error(AnsiString("Unrecognized texture format: "+head).c_str());
            return false;
        }
    int bpp= ( HasAlpha ? 4 : 3 );
    HasHash= strchr(szFileName,'#')!=NULL;

    fs->Read(&width,sizeof(int));
    fs->Read(&Height,sizeof(int));

    char *buff= new char[width*Height*bpp];

    fs->Read(buff,width*Height*bpp);

    ID= CreateTexture(buff,bpp,width,Height,HasAlpha,HasHash,bGlobalOptimize);

//    GLuint CreateTexture(void *buff, int bpp, int width, int Height, bool bHasAlpha, bool bMipMap, bool bOptimize)

    delete[] buff;

    delete fs;
*/
	return true;
}

bool __fastcall TTexture::LoadBMP(char *szFileName)
{
	HANDLE hFileHandle;
	BITMAPINFO *pBitmapInfo = NULL;
	unsigned long lInfoSize = 0;
	unsigned long lBitSize = 0;
	int nTexturewidth;
	int nTextureHeight;
	BYTE *pBitmapData;

	// Open the Bitmap file
	hFileHandle = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,
		NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	// Check for open failure (most likely file does not exist).
	if(hFileHandle == INVALID_HANDLE_VALUE)
		return FALSE;

    ID= 0;
//    Name= AnsiString(ExtractFileName(szFileName)).LowerCase();
    //int i= Name.Pos(".");
  //  if (i!=0)
//        Name= Name.SubString(1,i-1);

	// File is Open. Read in bitmap header information
	BITMAPFILEHEADER	bitmapHeader;
	DWORD dwBytes;
	ReadFile(hFileHandle,&bitmapHeader,sizeof(BITMAPFILEHEADER),
		&dwBytes,NULL);

	__try {
		if(dwBytes != sizeof(BITMAPFILEHEADER))
			return FALSE;

		// Check format of bitmap file
//		if(bitmapHeader.bfType != 'MB')
  //			return FALSE;

		// Read in bitmap information structure
		lInfoSize = bitmapHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
		pBitmapInfo = (BITMAPINFO *) new BYTE[lInfoSize];

		ReadFile(hFileHandle,pBitmapInfo,lInfoSize,&dwBytes,NULL);

		if(dwBytes != lInfoSize)
			return FALSE;


		nTexturewidth = pBitmapInfo->bmiHeader.biWidth;
		nTextureHeight = pBitmapInfo->bmiHeader.biHeight;
		lBitSize = pBitmapInfo->bmiHeader.biSizeImage;

		if(lBitSize == 0)
			lBitSize = (nTexturewidth *
               pBitmapInfo->bmiHeader.biBitCount + 7) / 8 *
  			  abs(nTextureHeight);

		// Allocate space for the actual bitmap
		pBitmapData = new BYTE[lBitSize];

		// Read in the bitmap bits
		ReadFile(hFileHandle,pBitmapData,lBitSize,&dwBytes,NULL);

		if(lBitSize != dwBytes)
			{
			if(pBitmapData)
				delete [] (BYTE *) pBitmapData;
			pBitmapData = NULL;

			return FALSE;
			}
		}
	__finally // Fail or success, close file and free working memory
		{
		CloseHandle(hFileHandle);

		if(pBitmapInfo != NULL)
			delete [] (BYTE *)pBitmapInfo;
		}


    glGenTextures(3,&ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);    //    GL_BGR_EXT



	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, nTexturewidth, nTextureHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBitmapData);

	// This is specific to the binary format of the data read in.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, nTexturewidth, nTextureHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBitmapData);


	if(pBitmapData)
		delete [] (BYTE *) pBitmapData;

    HasAlpha= false;
    HasHash= strchr(szFileName,'#')!=NULL;
  //  width= nTexturewidth;
//    Height= nTextureHeight;
//    Name= AnsiString(ExtractFileName(szFileName)).LowerCase();

	return TRUE;
}



bool __fastcall TTexture::LoadBUM(char *szFileName)
{
	GLfloat Gray[]= {0.5f,0.5f,0.5f,1.0f};
	HANDLE hFileHandle;
	BITMAPINFO *pBitmapInfo = NULL;
	unsigned long lInfoSize = 0;
	unsigned long lBitSize = 0;
	int nTexturewidth;
	int nTextureHeight;
	BYTE *pBitmapData;

	// Open the Bitmap file
	hFileHandle = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,
		NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	// Check for open failure (most likely file does not exist).
	if(hFileHandle == INVALID_HANDLE_VALUE)
		return FALSE;

    ID= 0;
//    Name= AnsiString(ExtractFileName(szFileName)).LowerCase();
    //int i= Name.Pos(".");
  //  if (i!=0)
//        Name= Name.SubString(1,i-1);

	// File is Open. Read in bitmap header information
	BITMAPFILEHEADER	bitmapHeader;
	DWORD dwBytes;
	ReadFile(hFileHandle,&bitmapHeader,sizeof(BITMAPFILEHEADER),
		&dwBytes,NULL);

	__try {
		if(dwBytes != sizeof(BITMAPFILEHEADER))
			return FALSE;

		// Check format of bitmap file
//		if(bitmapHeader.bfType != 'MB')
  //			return FALSE;

		// Read in bitmap information structure
		lInfoSize = bitmapHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
		pBitmapInfo = (BITMAPINFO *) new BYTE[lInfoSize];

		ReadFile(hFileHandle,pBitmapInfo,lInfoSize,&dwBytes,NULL);

		if(dwBytes != lInfoSize)
			return FALSE;


		nTexturewidth = pBitmapInfo->bmiHeader.biWidth;
		nTextureHeight = pBitmapInfo->bmiHeader.biHeight;
		lBitSize = pBitmapInfo->bmiHeader.biSizeImage;

		if(lBitSize == 0)
			lBitSize = (nTexturewidth *
               pBitmapInfo->bmiHeader.biBitCount + 7) / 8 *
  			  abs(nTextureHeight);

		// Allocate space for the actual bitmap
		pBitmapData = new BYTE[lBitSize];

		// Read in the bitmap bits
		ReadFile(hFileHandle,pBitmapData,lBitSize,&dwBytes,NULL);

		if(lBitSize != dwBytes)
			{
			if(pBitmapData)
				delete [] (BYTE *) pBitmapData;
			pBitmapData = NULL;

			return FALSE;
			}
		}
	__finally // Fail or success, close file and free working memory
		{
		CloseHandle(hFileHandle);

		if(pBitmapInfo != NULL)
			delete [] (BYTE *)pBitmapInfo;
		}

		glPixelTransferf(GL_RED_SCALE,0.5f);						// Scale RGB By 50%, So That We Have Only			
		glPixelTransferf(GL_GREEN_SCALE,0.5f);						// Half Intenstity
		glPixelTransferf(GL_BLUE_SCALE,0.5f);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);	// No Wrapping, Please!
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,Gray);

		glGenTextures(3, &ID);										// Create Three Textures

	// This is specific to the binary format of the data read in.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, nTexturewidth, nTextureHeight, 0,
                 GL_BGR_EXT, GL_UNSIGNED_BYTE, pBitmapData);


	if(pBitmapData)
		delete [] (BYTE *) pBitmapData;

    HasAlpha= false;
    HasHash= strchr(szFileName,'#')!=NULL;
  //  width= nTexturewidth;
//    Height= nTextureHeight;
//    Name= AnsiString(ExtractFileName(szFileName)).LowerCase();

	return TRUE;
}


bool __fastcall TTexture::LoadTargaFile( char* strPathname )
{
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGACompheader[12]={0,0,10,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)
    GLuint      width;
    GLuint      height;
    GLuint      bpp;
    GLubyte	    *imageData;
    bool        IsCompressed;

	FILE *file = fopen(strPathname, "rb");						// Open The TGA File

	if(	file==NULL ||										// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
//		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

        if(memcmp(TGAheader,TGAcompare,sizeof(TGAheader))== 0)  //Uncompressed?
          IsCompressed=false;

        if(memcmp(TGACompheader,TGAcompare,sizeof(TGACompheader))== 0)  //Compressed?
          IsCompressed=true;

if (!IsCompressed)
{
	width  = header[1] * 256 + header[0];			// Determine The TGA width	(highbyte*256+lowbyte)
	height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)

 	if(	width	<=0	||								// Is The width Less Than Or Equal To Zero
		height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		return false;										// Return False
	}

	bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= width*height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	imageData=new GLubyte[imageSize];		// Reserve Memory To Hold The TGA Data

	if(	imageData==NULL ||							// Does The Storage Memory Exist?
		fread(imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(imageData!=NULL)						// Was Image Data Loaded
			delete[] imageData;						// If So, Release The Image Data

		fclose(file);										// Close The File
		return false;										// Return False
	}

    if (bytesPerPixel>3)
        for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)				// Loop Through The Image Data
	    {										// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		    temp=imageData[i];						// Temporarily Store The Value At Image Data 'i'
    		imageData[i] = imageData[i + 2];			// Set The 1st Byte To The Value Of The 3rd Byte
	    	imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
    	}
    else
        for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)				// Loop Through The Image Data
	    {										// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		    temp=imageData[i];						// Temporarily Store The Value At Image Data 'i'
    		imageData[i] = imageData[i + 2];			// Set The 1st Byte To The Value Of The 3rd Byte
	    	imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
    	}


	fclose (file);											// Close The File
    HasAlpha= (bpp==32);
    bpp/= 8;
    HasHash= strchr(strPathname,'#')!=NULL;
    ID= CreateTexture(imageData,bpp,width,height,HasAlpha,HasHash,bGlobalOptimize);

 }
if (IsCompressed)
{
	width  = header[1] * 256 + header[0];					// Determine The TGA width	(highbyte*256+lowbyte)
	height = header[3] * 256 + header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	bpp	= header[4];										// Determine Bits Per Pixel

	if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp !=32)))	//Make sure all texture info is ok
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);	// If it isnt...Display error
		if(file != NULL)													// Check if file is open
  		 fclose(file);													// Ifit is, close it
		return false;														// Return failed
	}

	bytesPerPixel	= (bpp / 8);									// Compute BYTES per pixel
	imageSize	= (bytesPerPixel * width * height);		// Compute amout of memory needed to store image
	imageData	= (GLubyte *)malloc(imageSize);					// Allocate that much memory

	if(imageData == NULL)											// If it wasnt allocated correctly..
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);	// Display Error
		fclose(file);														// Close file
		return false;														// Return failed
	}

	GLuint pixelcount	= height * width;							// Nuber of pixels in the image
	GLuint currentpixel	= 0;												// Current pixel being read
	GLuint currentbyte	= 0;												// Current byte 
	GLubyte * colorbuffer = (GLubyte *)malloc(bytesPerPixel);			// Storage for 1 pixel

	do
	{
		GLubyte chunkheader = 0;											// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(GLubyte), 1, file) == 0)				// Read in the 1 byte header
		{
			MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);	// Display Error
			if(file != NULL)												// If file is open
				fclose(file);												// Close file
			if(imageData != NULL)									// If there is stored image data
				free(imageData);									// Delete image data
			return false;													// Return failed
		}

		if(chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																	// that follow the header
			chunkheader++;													// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
			{
				if(fread(colorbuffer, 1, bytesPerPixel, file) != bytesPerPixel) // Try to read 1 pixel
				{
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// IF we cant, display an error

					if(file != NULL)													// See if file is open
						fclose(file);													// If so, close file
               				if(colorbuffer != NULL)												// See if colorbuffer has data in it
						free(colorbuffer);												// If so, delete it
               				if(imageData != NULL)										// See if there is stored Image data
						free(imageData);										// If so, delete it too
					return false;														// Return failed
				}
																						// write to memory
				imageData[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process
				imageData[currentbyte + 1	] = colorbuffer[1];
				imageData[currentbyte + 2	] = colorbuffer[0];

				if(bytesPerPixel == 4)												// if its a 32 bpp image
				{
					imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
				}

				currentbyte += bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!
					if(file != NULL)													// If there is a file open
						fclose(file);													// Close file
					if(colorbuffer != NULL)												// If there is data in colorbuffer
						free(colorbuffer);												// Delete it
					if(imageData != NULL)										// If there is Image data
						free(imageData);										// delete it
					return false;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, bytesPerPixel, file) != bytesPerPixel)		// Attempt to read following color values
			{	
				MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);			// If attempt fails.. Display error (again)

				if(file != NULL)														// If thereis a file open
					fclose(file);														// Close it
				if(colorbuffer != NULL)													// If there is data in the colorbuffer
					free(colorbuffer);													// delete it
				if(imageData != NULL)											// If thereis image data
					free(imageData);											// delete it
				return false;															// return failed
			}

			for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
			{																			// by the header
				imageData[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				imageData[currentbyte + 1	] = colorbuffer[1];
				imageData[currentbyte + 2	] = colorbuffer[0];

				if(bytesPerPixel == 4)												// If TGA images is 32 bpp
				{
					imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
				}

				currentbyte += bytesPerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(file != NULL)													// If there is a file open
						fclose(file);													// Close file
               				if(colorbuffer != NULL)												// If there is data in colorbuffer
						free(colorbuffer);												// Delete it
					if(imageData != NULL)										// If there is Image data
						free(imageData);										// delete it
					return false;														// Return failed
				}
			}
		}
	}

	while(currentpixel < pixelcount);													// Loop while there are still pixels left
	fclose(file);																		// Close the file
        HasAlpha= (bpp==32);
        bpp/= 8;
        HasHash= strchr(strPathname,'#')!=NULL;
        ID= CreateTexture(imageData,bpp,width,height,HasAlpha,HasHash,bGlobalOptimize);
	return true;																		// return success
}


//}

    delete[] imageData;

	return true;											// Texture Building Went Ok, Return True
}


//---------------------------------------------------------------------------

__fastcall TTexturesManager::~TTexturesManager()
{
    Free();
}

//---------------------------------------------------------------------------
bool __fastcall TTexturesManager::Free()
{
    TTexture *tmp1;
    for (TTexture *tmp= First; tmp!=NULL; )
    {
        tmp1= tmp;
        tmp= tmp->Next;
        delete tmp1;

    }
	return true;
}

//---------------------------------------------------------------------------


GLuint __fastcall TTexturesManager::GetTextureID( char *Name, bool dynamic )  // char *Name
{
    char buf[255]= "";
    if ((strchr(Name,'\\')==NULL) && (!dynamic))
     {
        strcat_s(buf, szDefaultTexturePath);        //jesli biezaca sciezka do tekstur nie zostala dodana to dodajemy defaultowa
        strcat_s(buf, Name);
     }
    else
      strcat_s(buf,Name);

    if (strchr(Name,'.')==NULL)  // JESLI BRAK ROZSZERZENIA TO DODAJ DEFAULTOWE
		strcat_s(buf, Global::szDefaultExt.c_str());  // szDefaultTextureExt, Global::szDefaultExt

	WriteLog(buf);
/*
    if (i!=0)
        buff= Name.SubString(1,i-1);
    else
        buff= Name;
/*
    int i= Name.AnsiPos(".")-1;
    if (i>0)
        Name= Name.SubString(1,i);
    Name+= AnsiString(".tex");*/

    for (TTexture *tmp= First; tmp!=NULL; tmp= tmp->Next)
    {
        if (stricmp(tmp->Name,buf)==0)
            return (tmp->ID);
    }
WriteLog(buf);
    return (LoadFromFile( buf ));
}


GLuint __fastcall TTexturesManager::GetTextureIDB( char *Name )  // char *Name
{
    char buf[255]= "";
    if (strchr(Name,'\\')==NULL)
     {
        strcat_s(buf, szDefaultTexturePath);        //jesli biezaca sciezka do tekstur nie zostala dodana to dodajemy defaultowa
        strcat_s(buf, Name);
     }
    else
      strcat_s(buf,Name);
    if (strchr(Name,'.')==NULL)  // JESLI BRAK ROZSZERZENIA TO DODAJ DEFAULTOWE
		strcat_s(buf, Global::szDefaultExt.c_str());  // szDefaultTextureExt, Global::szDefaultExt


    for (TTexture *tmp= First; tmp!=NULL; tmp= tmp->Next)
    {
        if (stricmp(tmp->Name,buf)==0)
            return (tmp->ID);
    }

    return (LoadFromFile( buf ));
}

bool __fastcall TTexturesManager::GetAlpha( GLuint ID )
{
    for (TTexture *tmp= First; tmp!=NULL; tmp= tmp->Next)
    {
        if (tmp->ID==ID)
            return (tmp->HasAlpha && !tmp->HasHash);
    }
    return (false);
}
/*
bool __fastcall TTexturesManager::GetHash( GLuint ID )
{
    for (TTexture *tmp= First; tmp!=NULL; tmp= tmp->Next)
    {
        if (tmp->ID==ID)
            return (tmp->HasHash);
    }
    return (false);
}
*/

//---------------------------------------------------------------------------

GLuint __fastcall TTexturesManager::LoadFromFile( char *FileName )
{
//    char buf[255];

    TTexture *tmp= First;
    First= new TTexture(FileName,tmp);
    return (First->ID);
/*
    if (FileExists(buf))
    {
        TTexture *tmp= First;
        First= new TTexture(buf.c_str(),tmp);
        return (First->ID);
    }
    else
    {
        buf= FileName+AnsiString(".bmp");
        if (FileExists(buf))
        {
            TTexture *tmp= First;
            First= new TTexture(buf.c_str(),tmp);
            return (First->ID);
        }
        else
        {
            WriteLog(AnsiString("Texture "+FileName+AnsiString(" does not exist !")).c_str());
//        MessageBox(0,AnsiString("Texture "+FileName+AnsiString(" does not exist !")).c_str(),"Error",MB_OK);
            return (0);
        }
    }*/
//    First->Next= tmp;
}

//---------------------------------------------------------------------------



