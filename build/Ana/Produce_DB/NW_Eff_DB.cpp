#include "NW_Eff_DB.hh"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

ClassImp(NW_Eff_DB);

NW_Eff_DB::NW_Eff_DB()
{
  Eff_DB = 0;
  Threshold = 3; //unit: MeVee, 3MeVee is a default value.
}

NW_Eff_DB::~NW_Eff_DB()
{;}

void NW_Eff_DB::InitialDB(string DBFile)
{
  TFile* f1_DB = new TFile(DBFile.c_str(),"read");
  Eff_DB = (TEfficiency*) f1_DB->Get("NW_DetEff_DB");
  if(Eff_DB==0) { cout<<"The efficiency reading failed, please check!"<<endl; }
}

double NW_Eff_DB::Get_Eff(double Ekin, double Theta, double Phi)
{
  int GlobalBinIndex = Eff_DB->FindFixBin(Ekin,Theta,Phi);
  double Eff = Eff_DB->GetEfficiency(GlobalBinIndex);
return Eff;
}

void NW_Eff_DB::Get_Eff(double Ekin, double Theta, double Phi, double* EffWithErr)
{
  int GlobalBinIndex = Eff_DB->FindFixBin(Ekin,Theta,Phi);
  EffWithErr[0] = Eff_DB->GetEfficiency(GlobalBinIndex);
  EffWithErr[1] = Eff_DB->GetEfficiencyErrorLow(GlobalBinIndex);
  EffWithErr[2] = Eff_DB->GetEfficiencyErrorUp(GlobalBinIndex);
}

void NW_Eff_DB::Produce_EffDB(int EkinNumTem, double* EkinValueTem,string* SimData_FileNameList, string DBFileName)
{
  //Attention : put the int energy in the center of the Efficiency; deltaTheta = 0.5Deg., deltaPhi = 0.5 Deg.
  Eff_DB = new TEfficiency("NW_DetEff_DB","NW_DetEff_DB",146,4.5,150.5,60,26,56,108,-27,27);
  
  for(int iE=0;iE<EkinNumTem;iE++)
  {
    TFile* f1_SimData = new TFile(SimData_FileNameList[iE].c_str(),"read");
    TTree* t1 = (TTree*) f1_SimData->Get("t1");
    
    double Src_Theta = 0; double Src_Phi = 0; 
    int BarNum = 0; double BarLength[20] = {0}; double LightYield[20] = {0};
    t1->SetBranchAddress("Src_Theta",&Src_Theta);
    t1->SetBranchAddress("Src_Phi",&Src_Phi);
    t1->SetBranchAddress("NW_BarNum",&BarNum);
    t1->SetBranchAddress("NW_StepLength",BarLength);
    t1->SetBranchAddress("NW_LightYield",LightYield);
  
    int EvtNum = t1->GetEntries();
    cout<<"Energy "<<EkinValueTem[iE]<<" EvtNum : "<<EvtNum<<endl;
    
    for(int iEvt=0;iEvt<EvtNum;iEvt++)
    {
      if(iEvt%100000==0) { printProgress (1.0*iEvt/EvtNum); }
      t1->GetEntry(iEvt);
      if(Src_Phi>180) { Src_Phi = Src_Phi-360; }
      bool IsTriggered = 0; //if the light output is larger than threshold, then this neutron will give a trigger.
      for(int iBar=0;iBar<BarNum;iBar++)
      {
        if( LightYield[iBar]>=Threshold ) { IsTriggered = 1; break; }
      }
      Eff_DB->Fill(IsTriggered,EkinValueTem[iE],Src_Theta,Src_Phi);
    }
    f1_SimData->Close();
  }
  
  TFile* f1_DB = new TFile(DBFileName.c_str(),"update");
  Eff_DB->Write("",TObject::kOverwrite);
  f1_DB->Close();
}

void NW_Eff_DB::printProgress (double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}
