#ifndef B2RunAction_h
#define B2RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "iostream"
#include "G4ThreeVector.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;

/// Run action class

class B2RunAction : public G4UserRunAction
{
  public:
    B2RunAction();
    virtual ~B2RunAction();

    virtual void BeginOfRunAction(const G4Run* run);
    virtual void EndOfRunAction(const G4Run* run);
    
public:
  double Src_Theta; double Src_Phi; double Src_Ekin;
  void Set_SrcInfo(double E, double Theta, double Phi) { Src_Ekin = E; Src_Theta = Theta; Src_Phi = Phi; }
  
  int NW_BarNum; int NW_BarIndex[100]; double NW_LightYield[100]; double NW_StepLength[100];
  void FillData(int Num, int* Index, double* Length, double* LightYield)
  {
    NW_BarNum = Num;
    for(int i=0;i<Num;i++)
    {
      NW_BarIndex[i] = Index[i];
      NW_StepLength[i] = Length[i];
      NW_LightYield[i] = LightYield[i];
    }
    t1_data->Fill(); 
  }
  
public:
  TFile* f1_data;
  TTree* t1_data;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
