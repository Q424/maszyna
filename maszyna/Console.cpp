#pragma hdrstop

#include "commons.h"
#include "commons_usr.h"
//#include "PoKeys55.h"
//#include "LPT.h"

//---------------------------------------------------------------------------

//Ra: klasa statyczna gromadz�ca sygna�y steruj�ce oraz informacje zwrotne
//Ra: stan wej�cia zmieniany klawiatur� albo dedykowanym urz�dzeniem
//Ra: stan wyj�cia zmieniany przez symulacj� (mierniki, kontrolki)

/*******************************
Do klawisza klawiatury przypisana jest maska bitowa oraz numer wej�cia.
Naci�ni�cie klawisza powoduje wywo�anie procedury ustawienia bitu o podanej
masce na podanym wej�ciu. Zwonienie klawisza analogicznie wywo�uje zerowanie
bitu wg maski. Zasadniczo w masce ustawiony jest jeden bit, ale w razie
potrzeby mo�e by� ich wi�cej.

Oddzielne wej�cia s� wprowadzone po to, by mo�na by�o u�ywa� wi�cej ni� 32
bity do sterowania. Podzia� na wej�cia jest r�wnie� ze wzgl�d�w organizacyjnych,
np. sterowanie �wiat�ami mo�e mie� oddzielny numer wej�cia ni� prze��czanie
radia, poniewa� nie ma potrzeby ich uzale�nia� (tzn. bada� wsp�ln� mask� bitow�).

Do ka�dego wej�cia podpi�ty jest skrypt binarny, charakterystyczny dla danej
konstrukcji pojazdu. Sprawdza on zale�no�ci (w tym uszkodzenia) za pomoc�
operacji logicznych na maskach bitowych. Do ka�dego wej�cia jest przypisana
jedna, oddzielna maska 32 bit, ale w razie potrzeby istnieje te� mo�liwo��
korzystania z masek innych wej��. Skrypt mo�e te� wysy�a� maski na inne wej�cia,
ale nale�y unika� rekurencji.

Definiowanie wej�� oraz przeznaczenia ich masek jest w gestii konstruktora
skryptu. Ka�dy pojazd mo�e mie� inny schemat wej�� i masek, ale w miar� mo�liwo�ci
nale�y d��y� do unifikacji. Skrypty mog� r�wnie� u�ywa� dodatkowych masek bitowych.
Maski bitowe odpowiadaj� stanom prze��cznik�w, czujnik�w, stycznik�w itd.

Dzia�anie jest nast�puj�ce:
- na klawiaturze konsoli naciskany jest przycisk
- naci�ni�cie przycisku zamieniane jest na mask� bitow� oraz numer wej�cia
- wywo�ywany jest skrypt danego wej�cia z ow� mask�
- skrypt sprawdza zale�no�ci i np. modyfikuje w�asno�ci fizyki albo inne maski
- ewentualnie do wyzwalacza czasowego dodana jest maska i numer wej�cia

/*******************************/

/* //kod do przetrawienia:
//aby si� nie w��czacz wygaszacz ekranu, co jaki� czas naciska si� wirtualnie ScrollLock

[DllImport("user32.dll")]
static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, UIntPtr dwExtraInfo);

private static void PressScrollLock()
{//przyciska i zwalnia ScrollLock
 const byte vkScroll = 0x91;
 const byte keyeventfKeyup = 0x2;
 keybd_event(vkScroll, 0x45, 0, (UIntPtr)0);
 keybd_event(vkScroll, 0x45, keyeventfKeyup, (UIntPtr)0);
};

[DllImport("user32.dll")]
private static extern bool SystemParametersInfo(int uAction,int uParam,int &lpvParam,int flags);

public static Int32 GetScreenSaverTimeout()
{
 Int32 value=0;
 SystemParametersInfo(14,0,&value,0);
 return value;
};
*/

//Ra: do poprawienia
void SetLedState(char Code,bool bOn)
{//Ra: bajer do migania LED-ami w klawiaturze

};

//---------------------------------------------------------------------------

int Console::iBits=0; //zmienna statyczna - obiekt Console jest jednen wsp�lny
int Console::iMode=0;
int Console::iConfig=0;
TPoKeys55 *Console::PoKeys55[2]={NULL,NULL};
TLPT *Console::LPT=NULL;
int Console::iSwitch[8]; //bistabilne w kabinie, za��czane z [Shift], wy��czane bez
int Console::iButton[8]; //monostabilne w kabinie, za��czane podczas trzymania klawisza

__fastcall Console::Console()
{
 PoKeys55[0]=PoKeys55[1]=NULL;
 for (int i=0;i<8;++i)
 {//zerowanie prze��cznik�w
  iSwitch[i]=0; //bity 0..127 - bez [Ctrl], 128..255 - z [Ctrl]
  iButton[i]=0; //bity 0..127 - bez [Shift], 128..255 - z [Shift]
 }
};

__fastcall Console::~Console()
{
//- delete PoKeys55[0];
//- delete PoKeys55[1];
};

void __fastcall Console::ModeSet(int m,int h)
{//ustawienie trybu pracy
 iMode=m;
 iConfig=h;
};

int __fastcall Console::On()
{//za��czenie konsoli (np. nawi�zanie komunikacji)
 iSwitch[0]=iSwitch[1]=iSwitch[2]=iSwitch[3]=0; //bity 0..127 - bez [Ctrl]
 iSwitch[4]=iSwitch[5]=iSwitch[6]=iSwitch[7]=0; //bity 128..255 - z [Ctrl]
 switch (iMode)
 {case 1: //kontrolki klawiatury
  case 2: //kontrolki klawiatury
   iConfig=0; //licznik u�ycia Scroll Lock
  break;
  case 3: //LPT

  break;
  case 4: //PoKeys

  break;
 }
 return 0;
};

void __fastcall Console::Off()
{//wy��czenie informacji zwrotnych (reset pulpitu)
 BitsClear(-1);
 if ((iMode==1)||(iMode==2))
  if (iConfig&1) //licznik u�ycia Scroll Lock
  {//bez sensu to jest, ale mi si� samo w��cza
   //-SetLedState(VK_SCROLL,true); //przyci�ni�ty
   //-SetLedState(VK_SCROLL,false); //zwolniony
  }
 //-delete PoKeys55[0]; PoKeys55[0]=NULL;
 //-delete PoKeys55[1]; PoKeys55[1]=NULL;
 //-delete LPT; LPT=NULL;
};

void __fastcall Console::BitsSet(int mask,int entry)
{//ustawienie bit�w o podanej masce (mask) na wej�ciu (entry)
 if ((iBits&mask)!=mask) //je�eli zmiana
 {iBits|=mask;
  BitsUpdate(mask);
 }
};

void __fastcall Console::BitsClear(int mask,int entry)
{//zerowanie bit�w o podanej masce (mask) na wej�ciu (entry)
 if (iBits&mask) //je�eli zmiana
 {iBits&=~mask;
  BitsUpdate(mask);
 }
};

void __fastcall Console::BitsUpdate(int mask)
{//aktualizacja stanu interfejsu informacji zwrotnej; (mask) - zakres zmienianych bit�w
 switch (iMode)
 {case 1: //sterowanie �wiate�kami klawiatury: CA/SHP+opory
   if (mask&3) //gdy SHP albo CA
    SetLedState(VK_CAPITAL,iBits&3);
   if (mask&4) //gdy jazda na oporach
   {//Scroll Lock ma jako� dziwnie... zmiana stanu na przeciwny
    //-SetLedState(VK_SCROLL,true); //przyci�ni�ty
    //-SetLedState(VK_SCROLL,false); //zwolniony
    ++iConfig; //licznik u�ycia Scroll Lock
   }
   break;
  case 2: //sterowanie �wiate�kami klawiatury: CA+SHP
	  /*
   if (mask&2) //gdy CA
    SetLedState(VK_CAPITAL,iBits&2);
   if (mask&1) //gdy SHP
   {//Scroll Lock ma jako� dziwnie... zmiana stanu na przeciwny
    SetLedState(VK_SCROLL,true); //przyci�ni�ty
    SetLedState(VK_SCROLL,false); //zwolniony
    ++iConfig; //licznik u�ycia Scroll Lock
	
   }
   */
   break;
  case 3: //LPT Marcela z modyfikacj� (jazda na oporach zamiast brz�czyka)

   break;
  case 4: //PoKeys55 wg Marcela - wersja druga z ko�ca 2012
   if (PoKeys55[0])
   {//pewnie trzeba b�dzie to dodatkowo buforowa� i oczekiwa� na potwierdzenie

   }
   break;
 }
};

bool __fastcall Console::Pressed(int x)
{//na razie tak - czyta si� tylko klawiatura
 return Global::bActive&&(GetKeyState(x)<0);
};

void __fastcall Console::ValueSet(int x,double y)
{//ustawienie warto�ci (y) na kanale analogowym (x)

};

void __fastcall Console::Update()
{//funkcja powinna by� wywo�ywana regularnie, np. raz w ka�dej ramce ekranowej

};

float __fastcall Console::AnalogGet(int x)
{//pobranie warto�ci analogowej

	return 0.0;
};

unsigned char __fastcall Console::DigitalGet(int x)
{//pobranie warto�ci cyfrowej

	return '0';
};

void __fastcall Console::OnKeyDown(int k)
{//naci�ni�cie klawisza z powoduje wy��czenie, a
 if (k&0x10000) //je�li [Shift]
 {//ustawienie bitu w tabeli prze��cznik�w bistabilnych
  if (k&0x20000) //je�li [Ctrl], to zestaw dodatkowy
   iSwitch[4+(char(k)>>5)]|=1<<(k&31); //za��cz bistabliny dodatkowy
  else
  {//z [Shift] w��czenie bitu bistabilnego i dodatkowego monostabilnego
   iSwitch[char(k)>>5]|=1<<(k&31); //za��cz bistabliny podstawowy
   iButton[4+(char(k)>>5)]|=(1<<(k&31)); //za��cz monostabilny dodatkowy
  }
 }
 else
 {//zerowanie bitu w tabeli prze��cznik�w bistabilnych
  if (k&0x20000) //je�li [Ctrl], to zestaw dodatkowy
   iSwitch[4+(char(k)>>5)]&=~(1<<(k&31)); //wy��cz bistabilny dodatkowy
  else
  {iSwitch[char(k)>>5]&=~(1<<(k&31)); //wy��cz bistabilny podstawowy
   iButton[char(k)>>5]|=1<<(k&31); //za��cz monostabilny podstawowy
  }
 }
};
void __fastcall Console::OnKeyUp(int k)
{//puszczenie klawisza w zasadzie nie ma znaczenia dla iSwitch, ale zeruje iButton
 if ((k&0x20000)==0) //monostabilne tylko bez [Ctrl]
  if (k&0x10000) //je�li [Shift]
   iButton[4+(char(k)>>5)]&=~(1<<(k&31)); //wy��cz monostabilny dodatkowy
  else
   iButton[char(k)>>5]&=~(1<<(k&31)); //wy��cz monostabilny podstawowy
};
