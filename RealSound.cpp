//---------------------------------------------------------------------------
/*
    MaSzyna EU07 locomotive simulator
    Copyright (C) 2001-2004  Marcin Wozniak, Maciej Czapkiewicz and others

*/

#include "system.hpp"
#include "classes.hpp"
#pragma hdrstop

#include "math.h"
#include "RealSound.h"
#include "Globals.h"
#include "Timer.h"
#include "Logs.h"
#include "McZapkie\mctools.hpp"

__fastcall TRealSound::TRealSound()
{
    pSound = NULL;
    dSoundAtt = -1;
    AM = 0.0;
    AA = 0.0;
    FM = 0.0;
    FA = 0.0;
    vSoundPosition.x = 0;
    vSoundPosition.y = 0;
    vSoundPosition.z = 0;
    fDistance = fPreviousDistance = 0.0;
    fFrequency = 22050.0; // cz�stotliwo�� samplowania pliku
    iDoppler = 0; // normlanie jest za��czony; !=0 - modyfikacje
    bLoopPlay = false; // d�wi�k wy��czony
}

__fastcall TRealSound::~TRealSound()
{
    // if (this) if (pSound) pSound->Stop();
}

void TRealSound::Free() {}

void TRealSound::Init(char *SoundName, double DistanceAttenuation, double X, double Y,
                                 double Z, bool Dynamic, bool freqmod, double rmin)
{
    // Nazwa=SoundName; //to tak raczej nie zadzia�a, (SoundName) jest tymczasowe
    pSound = TSoundsManager::GetFromName(SoundName, Dynamic, &fFrequency);
    if (pSound)
    {
        if (freqmod)
            if (fFrequency != 22050.0)
            { // dla modulowanych nie mo�e by� zmiany mno�nika, bo cz�stotliwo�� w nag��wku by��
              // ignorowana, a mog�a by� inna ni� 22050
                fFrequency = 22050.0;
                ErrorLog("Bad sound: " + AnsiString(SoundName) +
                         ", as modulated, should have 22.05kHz in header");
            }
        AM = 1.0;
        pSound->SetVolume(DSBVOLUME_MIN);
    }
    else
    { // nie ma d�wi�ku, to jest wysyp
        AM = 0;
        ErrorLog("Missed sound: " + AnsiString(SoundName));
    }
    if (DistanceAttenuation > 0.0)
    {
        dSoundAtt = DistanceAttenuation * DistanceAttenuation;
        vSoundPosition.x = X;
        vSoundPosition.y = Y;
        vSoundPosition.z = Z;
        if (rmin < 0)
            iDoppler = 1; // wy��czenie efektu Dopplera, np. dla d�wi�ku ptak�w
    }
    else
        dSoundAtt = -1;
};

double TRealSound::ListenerDistance(vector3 ListenerPosition)
{
    if (dSoundAtt == -1)
    {
        return 0.0;
    }
    else
    {
        return SquareMagnitude(ListenerPosition - vSoundPosition);
    }
}

void TRealSound::Play(double Volume, int Looping, bool ListenerInside,
                                 vector3 NewPosition)
{
    if (!pSound)
        return;
    long int vol;
    double dS;
    // double Distance;
    DWORD stat;
    if ((Global::bSoundEnabled) && (AM != 0))
    {
        if (Volume > 1.0)
            Volume = 1.0;
        fPreviousDistance = fDistance;
        fDistance = 0.0; //??
        if (dSoundAtt > 0.0)
        {
            vSoundPosition = NewPosition;
            dS = dSoundAtt; //*dSoundAtt; //bo odleglosc podawana w kwadracie
            fDistance = ListenerDistance(Global::pCameraPosition);
            if (ListenerInside) // os�abianie d�wi�k�w z odleg�o�ci�
                Volume = Volume * dS / (dS + fDistance);
            else
                Volume = Volume * dS / (dS + 2 * fDistance); // podw�jne dla ListenerInside=false
        }
        if (iDoppler) //
        { // Ra 2014-07: efekt Dopplera nie zawsze jest wskazany
            // if (FreeFlyModeFlag) //gdy swobodne latanie - nie sprawdza si� to
            fPreviousDistance = fDistance; // to efektu Dopplera nie b�dzie
        }
        if (Looping) // d�wi�k zap�tlony mo�na wy��czy� i zostanie w��czony w miar� potrzeby
            bLoopPlay = true; // d�wi�k wy��czony
        // McZapkie-010302 - babranie tylko z niezbyt odleglymi d�wi�kami
        if ((dSoundAtt == -1) || (fDistance < 20.0 * dS))
        {
            //   vol=2*Volume+1;
            //   if (vol<1) vol=1;
            //   vol=10000*(log(vol)-1);
            //   vol=10000*(vol-1);
            // int glos=1;
            // Volume=Volume*glos; //Ra: whatta hella is this
            if (Volume < 0.0)
                Volume = 0.0;
            vol = -5000.0 + 5000.0 * Volume;
            if (vol >= 0)
                vol = -1;
            if (Timer::GetSoundTimer() || !Looping) // Ra: po co to jest?
                pSound->SetVolume(vol); // Attenuation, in hundredths of a decibel (dB).
            pSound->GetStatus(&stat);
            if (!(stat & DSBSTATUS_PLAYING))
                pSound->Play(0, 0, Looping);
        }
        else // wylacz dzwiek bo daleko
        { // Ra 2014-09: oddalanie si� nie mo�e by� powodem do wy��czenie d�wi�ku
            /*
            // Ra: stara wersja, ale podobno lepsza
               pSound->GetStatus(&stat);
               if (bLoopPlay) //je�li zap�tlony, to zostanie ponownie w��czony, o ile znajdzie si�
            bli�ej
                if (stat&DSBSTATUS_PLAYING)
                 pSound->Stop();
            // Ra: wy��czy�em, bo podobno jest gorzej ni� wcze�niej
               //ZiomalCl: d�wi�k po wy��czeniu sam si� nie w��czy, gdy wr�cimy w rejon odtwarzania
               pSound->SetVolume(DSBVOLUME_MIN); //dlatego lepiej go wyciszy� na czas oddalenia si�
               pSound->GetStatus(&stat);
               if (!(stat&DSBSTATUS_PLAYING))
                pSound->Play(0,0,Looping); //ZiomalCl: w��czenie odtwarzania rownie� i tu, gdy�
            jesli uruchamiamy d�wi�k poza promieniem, nie uruchomi si� on w og�le
            */
        }
    }
};

void TRealSound::Start(){// w��czenie d�wi�ku

};

void TRealSound::Stop()
{
    DWORD stat;
    if (pSound)
        if ((Global::bSoundEnabled) && (AM != 0))
        {
            bLoopPlay = false; // d�wi�k wy��czony
            pSound->GetStatus(&stat);
            if (stat & DSBSTATUS_PLAYING)
                pSound->Stop();
        }
};

void TRealSound::AdjFreq(double Freq,
                                    double dt) // McZapkie TODO: dorobic tu efekt Dopplera
// Freq moze byc liczba dodatnia mniejsza od 1 lub wieksza od 1
{
    float df, Vlist, Vsrc;
    if ((Global::bSoundEnabled) && (AM != 0))
    {
        if (dt > 0)
        // efekt Dopplera
        {
            Vlist = (sqrt(fPreviousDistance) - sqrt(fDistance)) / dt;
            df = Freq * (1 + Vlist / 299.8);
        }
        else
            df = Freq;
        if (Timer::GetSoundTimer())
        {
            df = fFrequency * df; // TODO - brac czestotliwosc probkowania z wav
            pSound->SetFrequency((df < DSBFREQUENCY_MIN ?
                                      DSBFREQUENCY_MIN :
                                      (df > DSBFREQUENCY_MAX ? DSBFREQUENCY_MAX : df)));
        }
    }
}

double TRealSound::GetWaveTime() // McZapkie: na razie tylko dla 22KHz/8bps
{ // u�ywana do pomiaru czasu dla d�wi�k�w z pocz�tkiem i ko�cem
    if (!pSound)
        return 0.0;
    double WaveTime;
    DSBCAPS caps;
    caps.dwSize = sizeof(caps);
    pSound->GetCaps(&caps);
    WaveTime = caps.dwBufferBytes;
    return WaveTime /
           fFrequency; //(pSound->);  // wielkosc w bajtach przez czestotliwosc probkowania
}

void TRealSound::SetPan(int Pan) { pSound->SetPan(Pan); }

int TRealSound::GetStatus()
{
    DWORD stat;
    if ((Global::bSoundEnabled) && (AM != 0))
    {
        pSound->GetStatus(&stat);
        return stat;
    }
    else
        return 0;
}

void TRealSound::ResetPosition()
{
    if (pSound) // Ra: znowu jaki� badziew!
        pSound->SetCurrentPosition(0);
}

void TTextSound::Init(char *SoundName, double SoundAttenuation, double X, double Y,
                                 double Z, bool Dynamic, bool freqmod, double rmin)
{ // dodatkowo doczytuje plik tekstowy
    TRealSound::Init(SoundName, SoundAttenuation, X, Y, Z, Dynamic, freqmod, rmin);
    fTime = GetWaveTime();
    AnsiString txt = AnsiString(SoundName);
    txt.Delete(txt.Length() - 3, 4); // obci�cie rozszerzenia
    for (int i = txt.Length(); i > 0; --i)
        if (txt[i] == '/')
            txt[i] = '\\'; // bo nie rozumi
    txt += "-" + Global::asLang + ".txt"; // ju� mo�e by� w r�nych j�zykach
    if (!FileExists(txt))
        txt = "sounds\\" + txt; //�cie�ka mo�e nie by� podana
    if (FileExists(txt))
    { // wczytanie
        TFileStream *ts = new TFileStream(txt, fmOpenRead);
        asText = AnsiString::StringOfChar(' ', ts->Size);
        ts->Read(asText.c_str(), ts->Size);
        delete ts;
    }
};
void TTextSound::Play(double Volume, int Looping, bool ListenerInside,
                                 vector3 NewPosition)
{
    if (!asText.IsEmpty())
    { // je�li ma powi�zany tekst
        DWORD stat;
        pSound->GetStatus(&stat);
        if (!(stat & DSBSTATUS_PLAYING)) // je�li nie jest aktualnie odgrywany
        {
            int i;
            AnsiString t = asText;
            do
            { // na razie zrobione jakkolwiek, docelowo przenie�� teksty do tablicy nazw
                i = t.Pos("\r"); // znak nowej linii
                if (!i)
                    Global::tranTexts.Add(t.c_str(), fTime, true);
                else
                {
                    Global::tranTexts.Add(t.SubString(1, i - 1).c_str(), fTime, true);
                    t.Delete(1, i);
                    while (t.IsEmpty() ? false : (unsigned char)(t[1]) < 33)
                        t.Delete(1, 1);
                }
            } while (i > 0);
        }
    }
    TRealSound::Play(Volume, Looping, ListenerInside, NewPosition);
};

//---------------------------------------------------------------------------
#pragma package(smart_init)
