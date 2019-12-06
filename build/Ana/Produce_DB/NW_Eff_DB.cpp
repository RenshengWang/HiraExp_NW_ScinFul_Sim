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
    cout<<endl<<" ---> Energy "<<EkinValueTem[iE]<<" EvtNum : "<<EvtNum<<endl;
    
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

TH2D* NW_Eff_DB::DrawEff_withFixEkin(double Ekin)
{
  if(Eff_DB==0) { cout<<"Eff_DB==null pointer!"<<endl; return 0; }
  
  char NameTem[200];
  sprintf(NameTem,"c1_DetEff_Ekin_%.1fMeV",Ekin);
  TCanvas* c1_Eff = new TCanvas(NameTem,NameTem,1);
  
  sprintf(NameTem,"h2_DetEff_Ekin_%.1fMeV",Ekin);
  TH2D* h2_Eff = new TH2D(NameTem,";Theta(Deg.);Phi(Deg.)",60,26,56,108,-27,27);
  
  for(int iTheta=1;iTheta<=60;iTheta++)
  {
    for(int iPhi=1;iPhi<=108;iPhi++)
    {
      double Eff = Get_Eff(Ekin, 26+iTheta*0.5, -27+iPhi*0.5);//,EffWithErr);
      h2_Eff->Fill(26+iTheta*0.5,-27+iPhi*0.5,Eff);
      //cout<<Eff<<"  ";
    }
  }
  //cout<<endl;
  h2_Eff->Draw("colz");
  
return h2_Eff;
}

TH1D* NW_Eff_DB::DrawEff_withFixAngle(double Theta,double Phi)
{
  if(Eff_DB==0) { cout<<"Eff_DB==null pointer!"<<endl; return 0; }
  char NameTem[200];
  sprintf(NameTem,"c1_DetEff_Theta%.1f_Phi%.1f",Theta,Phi);
  TCanvas* c1_Eff = new TCanvas(NameTem,NameTem,1);
  
  sprintf(NameTem,"h2_DetEff_Theta%.1f_Phi%.1f",Theta,Phi);
  TH1D* h1_Eff = new TH1D(NameTem,";Ekin(MeV);DetEff(Prob.)",160,0,160);
  
  for(int i=0;i<160;i++)
  {
    double Ekin = i; //unit:MeV
    int GlobalBinIndex = Eff_DB->FindFixBin(Ekin,Theta,Phi);
    double EffWithErr[3] = {0,0,0};
    EffWithErr[0] = Eff_DB->GetEfficiency(GlobalBinIndex);
    EffWithErr[1] = Eff_DB->GetEfficiencyErrorLow(GlobalBinIndex);
    EffWithErr[2] = Eff_DB->GetEfficiencyErrorUp(GlobalBinIndex);
    
    int BinIndex = h1_Eff->FindBin(Ekin);
    
    if(EffWithErr[0]>0)
    {
      h1_Eff->Fill(Ekin,EffWithErr[0]);
      h1_Eff->SetBinError(BinIndex,0.5*(EffWithErr[1]+EffWithErr[2])); //set the responding error bar.
    }
  }
  h1_Eff->Draw();
  
return h1_Eff;
}

void NW_Eff_DB::printProgress (double percentage)
{
  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush (stdout);
}
