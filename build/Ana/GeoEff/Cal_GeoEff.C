#include "TMath.h"
using namespace TMath;

void Cal_Eff(int TotalEvtNum, double* ThetaRange, double* PhiRange, TH1D* h1_GeoEff);

void Cal_GeoEff()
{
  string Det_Tag = "NWb";
  
  int TotalEvtNum = 10000000;
  double ThetaRange[2] = {26,56};
  double PhiRange[2] = {-27,27};
  
  TFile* f1_SimData = new TFile((Det_Tag+"_NE213_GeoEff_1E7.root").c_str(),"read");
  TTree* t1 = (TTree*) f1_SimData->Get("t1");
  
  double Src_Theta = 0; double Src_Phi = 0; 
  int BarNum = 0; double BarLength[20] = {0}; double LightYield[20] = {0};
  
  t1->SetBranchAddress("Src_Theta",&Src_Theta);
  t1->SetBranchAddress("Src_Phi",&Src_Phi);
  t1->SetBranchAddress("NW_BarNum",&BarNum);
  t1->SetBranchAddress("NW_StepLength",BarLength);
  t1->SetBranchAddress("NW_LightYield",LightYield);
  
  int EvtNum = t1->GetEntries();
  cout<<"EvtNum : "<<EvtNum<<endl;
  
  TH2D* h2_Theta_Phi_Dis = new TH2D(("h2_"+Det_Tag+"_Theta_Phi_Dis").c_str(),";Theta(Deg.);Phi(Deg.)",40*10,20,60,60*10,-30,30);
  TH1D* h1_Theta_Dis = new TH1D(("h1_"+Det_Tag+"_Theta_Dis").c_str(),";Theta(Deg.);Count",200,20,60);
  TH1D* h1_EffStepLength = new TH1D("h1_EffStepLength",";Eff Step Length(mm);Prob.(Arb.)",700,0,70);
  TH1D* h1_LightYield = new TH1D("h1_LightYield",";LightYield(MeVee);Count",1500,0,150);
  
  for(int iEvt=0;iEvt<EvtNum;iEvt++)
  {
    if(iEvt%10000==0) { cout<<"iEvt : "<<iEvt<<endl; }
    t1->GetEntry(iEvt);
    
    if(BarNum==0) { continue; }
    
    if(Src_Phi>180) { Src_Phi = Src_Phi-360; }
    h2_Theta_Phi_Dis->Fill(Src_Theta,Src_Phi);
    
    h1_Theta_Dis->Fill(Src_Theta);
    
    for(int iBar=0;iBar<BarNum;iBar++)
    {
      h1_EffStepLength->Fill(BarLength[iBar]);
      h1_LightYield->Fill(LightYield[iBar]);
    }
  }
  
  //cal the Geo Eff
  TH1D* h1_GeoEff = (TH1D*) h1_Theta_Dis->Clone(("h1_"+Det_Tag+"_GeoEff").c_str());
  h1_GeoEff->GetYaxis()->SetTitle("Prob.(Arb.)");
  Cal_Eff(TotalEvtNum, ThetaRange, PhiRange,h1_GeoEff);
  
  TCanvas* c1_Theta_Phi_Dis = new TCanvas(("c1_"+Det_Tag+"_Theta_Phi_Dis").c_str(),("c1_"+Det_Tag+"_Theta_Phi_Dis").c_str(),1);
  c1_Theta_Phi_Dis->cd();
  h2_Theta_Phi_Dis->Draw("colz");
  
  TCanvas* c1_Theta_Dis = new TCanvas(("c1_"+Det_Tag+"_Theta_Dis").c_str(),("c1_"+Det_Tag+"_Theta_Dis").c_str(),1);
  h1_Theta_Dis->Draw("hist");

  TCanvas* c1_GeoEff = new TCanvas(("c1_"+Det_Tag+"_GeoEff").c_str(),("c1_"+Det_Tag+"_GeoEff").c_str(),1);
  h1_GeoEff->Draw("hist");
  
  TCanvas* c1_EffStepLength = new TCanvas("c1_EffStepLength","c1_EffStepLength",1);
  c1_EffStepLength->Divide(2,1);
  c1_EffStepLength->cd(1); h1_EffStepLength->Draw("hist");
  h1_EffStepLength->Scale(1.0/TotalEvtNum);
  TH1D* h1_EffStepLength_Cumulative = (TH1D*) h1_EffStepLength->GetCumulative(1);
  c1_EffStepLength->cd(2); h1_EffStepLength_Cumulative->Draw("hist");
  h1_EffStepLength_Cumulative->Scale(1.0/h1_EffStepLength_Cumulative->GetMaximum());
  
  TCanvas* c1_LightYield = new TCanvas("c1_LightYield","c1_LightYield",1);
  h1_LightYield->Draw("hist");
  h1_LightYield->Scale(1.0/h1_LightYield->Integral("width"));
  
  TFile* f1_NW_GeoEff = new TFile("f1_NW_GeoEff.root","update");
  h2_Theta_Phi_Dis->Write("",TObject::kOverwrite);
  h1_GeoEff->Write("",TObject::kOverwrite);
  h1_Theta_Dis->Write("",TObject::kOverwrite);
  f1_NW_GeoEff->Close();
}

void Cal_Eff(int TotalEvtNum, double* ThetaRange, double* PhiRange, TH1D* h1_GeoEff)
{
  double DeltaTheta = h1_GeoEff->GetBinCenter(2)-h1_GeoEff->GetBinCenter(1);
  double PhiEff = 360.0/(PhiRange[1]-PhiRange[0]);
  double ThetaEff = TotalEvtNum/(ThetaRange[1]-ThetaRange[0]);
  
  double GeoEff = (1.0/DeltaTheta)*(1.0/PhiEff)*(1.0/ThetaEff);
  h1_GeoEff->Scale(GeoEff);
}
