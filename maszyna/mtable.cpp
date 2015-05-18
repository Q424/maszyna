// mtable.cpp is equal with 1166
#include "commons.h"
#include "commons_usr.h"
#include <io.h>
#include <windows.h>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <cmath>
#pragma hdrstop

#include "mtable.h"

TMTableTime* GlobalTime;

void TMTableTime::Init(int InitH, int InitM, int InitSRH, int InitSRM, int InitSSH, int InitSSM)
{
  GameTime =0.0;
  mr = 0.0;
  dd =0;
  hh =InitH;
  mm =InitM;
  srh =InitSRH;
  srm =InitSRM;
  ssh =InitSSH;
  ssm =InitSSM;
  shh = "10";
  smm = "30";
  sss = "00";
  bysec = 0.0;
}


void TMTableTime::UpdateMTableTime(double deltaT)
{
int xss;    //dodanie czasu (deltaT) w sekundach, z przeliczeniem godziny

mr = mr + deltaT; //dodawanie sekund

while (mr>60.0)  //przeliczenie sekund do w³aœciwego przedzia³u
{
     mr =mr-60.0;
	 mm++;
}
  while (mm>59) //przeliczenie minut do w³aœciwego przedzia³u
  {
     mm =0;
     hh++;
  }
  while (hh>23) //przeliczenie godzin do w³aœciwego przedzia³u
  {
     hh=0;
     dd++; //zwiêkszenie numeru dnia
  }
  
   xss = glm::round(mr);
   ss=xss;

   //xh := hh;
   //xm := mm;
   //xs := ss;

   if (hh < 10)  shh = "0" + itoss(hh); else shh = itoss(hh);
   if (mm < 10)  smm = "0" + itoss(mm); else smm = itoss(mm);
   if (ss < 10)  sss = "0" + itoss(ss); else sss = itoss(ss);

   bysec =bysec+ deltaT;
 
  GameTime = GameTime + deltaT;
}

std::string TMTableTime::GetTimeStr()
{
	std::string outtext;
	return outtext = GlobalTime->shh + ":" + GlobalTime->smm + ":" + GlobalTime->sss;
}

bool TTrainParameters::UpdateMTable(double hh, double mm, std::string NewName)
{
 return true;
}
TTrainParameters::TTrainParameters(std::string NewTrainName)
{
//
}

void TTrainParameters::NewName(std::string NewTrainName)
{
//
}

bool TTrainParameters::LoadTTfile(std::string scnpath)
{
 return true;
}

bool TTrainParameters::DirectionChange(void)
{
 return true;
}

void TTrainParameters::StationIndexInc(void)
{
//
}

std::string TTrainParameters::NextStop()
{
 return "xxx";
}