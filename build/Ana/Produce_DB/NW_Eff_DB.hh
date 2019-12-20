#ifndef NW_Eff_DB_h
#define NW_Eff_DB_h 1

#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "iostream"
#include "TMath.h"
#include "TF1.h"
#include "TCanvas.h"
#include "fstream"
#include "TCutG.h"
#include "TEfficiency.h"

using namespace std;
using namespace TMath;

class NW_Eff_DB : public TObject
{
public:
  NW_Eff_DB();
  ~NW_Eff_DB();
  
  //the below is for setting up the DB.
  int  EkinNum; double EkinRange[2]; double Ekin_BinSize;
  int ThetaNum; double ThetaRange[2]; double Theta_BinSize;
  int PhiNum; double PhiRange[2];  double Phi_BinSize;
  TEfficiency* Eff_DB;
  
  void InitialDB(string DBFile);
  double Get_Eff(double Ekin, double Theta, double Phi);
  void Get_Eff(double Ekin, double Theta, double Phi, double* EffWithErr);
  
  //the below is for prodcuing the DB.
  double Threshold;
  void SetThreshold(double tem) { Threshold = tem; }
  void Produce_EffDB(int EkinNumTem, double* EkinValueTem,string* SimData_FileNameList,string DBFileName);
  
  //the below is only for drawing the Efficiency database.
  // the TEfficiency only support 1D, 2D efficiency database, so 3D has to be drawn by ourself.
  TH2D* DrawEff_withFixEkin(double Ekin);
  TH1D* DrawEff_withFixAngle(double Theta,double Phi);
  
  //calculate the Geometry Efficiency. This geometry efficiency is relay on the DB, so initialDB first.
  TH1D* h1_GeoEff[200];
  void Initial_GeoEff(double DetEff_Cut);
  double Get_GeoEff(double Ekin, double Theta);
  bool Draw_GeoEff_Histo(double Ekin);
  
  //the below is only for showing info better.
  void Set_1DHisto_Style(TH1D* h1_tem);
  void printProgress (double percentage);
  
  ClassDef(NW_Eff_DB,1)
};

#endif
