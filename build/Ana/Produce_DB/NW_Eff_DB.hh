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
  TEfficiency* Eff_DB;
  
  void InitialDB(string DBFile);
  double Get_Eff(double Ekin, double Theta, double Phi);
  void Get_Eff(double Ekin, double Theta, double Phi, double* EffWithErr);
  
  //the below is for prodcuing the DB.
  double Threshold;
  void SetThreshold(double tem) { Threshold = tem; }
  void Produce_EffDB(int EkinNumTem, double* EkinValueTem,string* SimData_FileNameList,string DBFileName);
  void printProgress (double percentage);
  
  ClassDef(NW_Eff_DB,1)
};

#endif
