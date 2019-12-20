#include "NW_Eff_DB.hh"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

ClassImp(NW_Eff_DB);

NW_Eff_DB::NW_Eff_DB()
{
  Eff_DB = 0;
  Threshold = 3; //unit: MeVee, 3MeVee is a default value.
  
  //the below is the parameters of DB, normally, this parameters should not be modified.
  EkinNum = 146;
  EkinRange[0] = 4.5; EkinRange[1] = 150.5; 
  Ekin_BinSize = (EkinRange[1]-EkinRange[0])/EkinNum;
  ThetaNum = 60;
  ThetaRange[0] = 26; ThetaRange[1] = 56;
  Theta_BinSize = (ThetaRange[1]-ThetaRange[0])/ThetaNum;
  PhiNum = 108;
  PhiRange[0] = -27; PhiRange[1] = 27;
  Phi_BinSize = (PhiRange[1]-PhiRange[0])/PhiNum;
  
  cout<<"---> Neutron Wall Efficiency Info:"<<endl;
  cout<<"  Ekin  : ("<<EkinRange[0]<<","<<EkinRange[1]<<"), BinNum: "<<EkinNum<<", BinSize: "<<Ekin_BinSize<<" MeV"<<endl;
  cout<<"  Theta : ("<<ThetaRange[0]<<","<<ThetaRange[1]<<"), BinNum: "<<ThetaNum<<", BinSize: "<<Theta_BinSize<<" Deg."<<endl;
  cout<<"  Phi   : ("<<PhiRange[0]<<","<<PhiRange[1]<<"), BinNum: "<<PhiNum<<", BinSize: "<<Phi_BinSize<<" Deg."<<endl;
  cout<<"-----------------------------------"<<endl;
}

NW_Eff_DB::~NW_Eff_DB()
{;}

void NW_Eff_DB::InitialDB(string DBFile)
{
  TFile* f1_DB = new TFile(DBFile.c_str(),"read");
  Eff_DB = (TEfficiency*) f1_DB->Get("NW_DetEff_DB");
  if(Eff_DB==0) { cout<<"--->The efficiency reading failed, please check!"<<endl; }
  else { cout<<"--->Efficiency DB load successfully!"<<endl; }
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
  /*
  EkinNum = 146;
  EkinRange[0] = 4.5; EkinRange[1] = 150.5;
  ThetaNum = 60;
  ThetaRange[0] = 26; ThetaRange[1] = 56;
  PhiNum = 108;
  PhiRange[0] = -27; PhiRange[1] = 27; 
  */
  Eff_DB = new TEfficiency("NW_DetEff_DB","NW_DetEff_DB",EkinNum,EkinRange[0],EkinRange[1],ThetaNum,ThetaRange[0],ThetaRange[1],PhiNum,PhiRange[0],PhiRange[1]);
  
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
  TH2D* h2_Eff = new TH2D(NameTem,";Theta(Deg.);Phi(Deg.)",ThetaNum,ThetaRange[0],ThetaRange[1],PhiNum,PhiRange[0],PhiRange[1]);
  
  for(int iTheta=1;iTheta<=ThetaNum;iTheta++)
  {
    for(int iPhi=1;iPhi<=PhiNum;iPhi++)
    {
      double Eff = Get_Eff(Ekin, ThetaRange[0]+iTheta*0.5, PhiRange[0]+iPhi*0.5);//,EffWithErr);
      h2_Eff->Fill(ThetaRange[0]+iTheta*0.5,PhiRange[0]+iPhi*0.5,Eff);
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
  TH1D* h1_Eff = new TH1D(NameTem,";Ekin(MeV);DetEff(Prob.)",EkinNum,EkinRange[0],EkinRange[1]);
  
  for(int i=0;i<EkinNum;i++)
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
  Set_1DHisto_Style(h1_Eff);
  
return h1_Eff;
}

//the geometry efficiency can be calculated with the efficiency DB quickly. 
//This value also is related to DetEff_Cut.
// So, produce the geometry efficiency histogram each time.
void NW_Eff_DB::Initial_GeoEff(double DetEff_Cut)
{
  cout<<"---> Initial Geometry Efficiency histogram!"<<endl;
  
  char NameTem[200];
  for(int iE=0;iE<EkinNum;iE++)
  {
    sprintf(NameTem,"h1_GeoEff_Theta_Ekin_%.1f_%.1f",EkinRange[0]+iE*Ekin_BinSize,EkinRange[0]+(iE+1)*Ekin_BinSize);
    h1_GeoEff[iE] = new TH1D(NameTem,";Theta(Deg.);GeoEf",ThetaNum,ThetaRange[0],ThetaRange[1]);
    for(int iTheta=0;iTheta<ThetaNum;iTheta++)
    {
      for(int iPhi=0;iPhi<PhiNum;iPhi++)
      {
        double Eff = Get_Eff(EkinRange[0]+(iE+0.5)*Ekin_BinSize, ThetaRange[0]+(iTheta+0.5)*Theta_BinSize, PhiRange[0]+(iPhi+0.5)*Phi_BinSize);
        if(Eff>=DetEff_Cut)
        {
          h1_GeoEff[iE]->Fill(ThetaRange[0]+(iTheta+0.5)*Theta_BinSize,Phi_BinSize);
        }
      }
    }
    h1_GeoEff[iE]->Scale(1.0/360.0);
  }
}

bool NW_Eff_DB::Draw_GeoEff_Histo(double Ekin)
{
  if(Ekin<=EkinRange[0] || Ekin>=EkinRange[1]) { return 0; }
  int Ekin_Index = (int) (Ekin-EkinRange[0])/Ekin_BinSize;
  char NameTem[200];
  sprintf(NameTem,"c1_GeoEff_Ekin_%.1f",Ekin);
  TCanvas* c1_Ekin_GeoEff = new TCanvas(NameTem,NameTem,1);
  h1_GeoEff[Ekin_Index]->Draw("hist");
  
  Set_1DHisto_Style(h1_GeoEff[Ekin_Index]);
  h1_GeoEff[Ekin_Index]->GetXaxis()->SetRangeUser(25,55);
  h1_GeoEff[Ekin_Index]->GetYaxis()->SetRangeUser(0,0.15);
  
return 1;
}

double NW_Eff_DB::Get_GeoEff(double Ekin, double Theta)
{
  if(Ekin<=EkinRange[0] || Ekin>=EkinRange[1]) { return 0; }
  if(Theta<=ThetaRange[0] || Theta>=ThetaRange[1]) { return 0; }
  
  int Ekin_Index = (int) (Ekin-EkinRange[0])/Ekin_BinSize;
  int Theta_Index = h1_GeoEff[Ekin_Index]->FindBin(Theta);
  
return h1_GeoEff[Ekin_Index]->GetBinContent(Theta_Index);
}

void NW_Eff_DB::printProgress (double percentage)
{
  int val = (int) (percentage * 100);
  int lpad = (int) (percentage * PBWIDTH);
  int rpad = PBWIDTH - lpad;
  printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
  fflush (stdout);
}

void NW_Eff_DB::Set_1DHisto_Style(TH1D* h1_tem)
{
  h1_tem->SetLineWidth(2);
  h1_tem->GetXaxis()->CenterTitle(1);
  h1_tem->GetYaxis()->CenterTitle(1);
  h1_tem->GetXaxis()->SetTitleSize(0.05);
  h1_tem->GetYaxis()->SetTitleSize(0.05);
  h1_tem->GetXaxis()->SetLabelSize(0.05);
  h1_tem->GetYaxis()->SetLabelSize(0.05);
}
