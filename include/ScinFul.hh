#ifndef ScinFul_h 
#define ScinFul_h 1

#include "iostream"
#include "fstream"
#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"

using namespace std;
using namespace TMath;

class ScinFul
{
public:
  ScinFul();
  ~ScinFul();

public:
  TFile* f1_DB;
  //the structure of database.
  int EnergyNum;
  double DB_Scintilator_Length; //this will be used to calculate the attenuation(absorb) parameters.
  double EnergyRange[2]; double DeltaE;
  double EnergyValue[500]; //I just first set 500 as the maximum number.
  
  TH1D* h1_LightYield[500];
  TH1D* h1_LightYield_AreaProb[500]; //the histogram that Fanurs give to me, the bin width is not uniform, but the GetRandom() is basing on the bin content.
  double TotalProbability[500];
  double Det_Lamda[500]; //the decay parameters, lamda.
  TGraph* gr1_TotalProbability;
  
  void Initial_ScinFul_DB(string FileName_WithPath);
  int Get_EnergyIndex(double Src_Ekin);
  double GetLightYield(double StepLength,double Src_Ekin);
  TH1D* Get_LightYieldSpec(double Src_Ekin);
  
  //the below is for checking the light yield.
  void Draw_LightYield(int ENum, double* Energy);
  void Draw_Eff(double threshold);
  void Draw_Eff(int ThresholdNum, double* threshold);
  
};

#endif
