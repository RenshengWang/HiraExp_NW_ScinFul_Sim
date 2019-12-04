#include "ScinFul.hh"



ScinFul::ScinFul()
{
  f1_DB = 0;
  for(int i=0;i<500;i++)
  {
    EnergyValue[i] = 0;
    h1_LightYield[i] = 0;
    TotalProbability[i] = 0;
  }
  EnergyNum = 0;
  EnergyRange[0] = 0; EnergyRange[1] = 0;
  DeltaE = 0;
  gr1_TotalProbability = 0;
}

ScinFul::~ScinFul()
{
  
}

void ScinFul::Initial_ScinFul_DB(string FileName_WithPath)
{
  //the following histograms reading can be modified according to the actual case.
  f1_DB = new TFile(FileName_WithPath.c_str(),"read");
  EnergyNum = 146;
  EnergyRange[0] = 5; EnergyRange[1] = 150;
  DeltaE = 1;
  DB_Scintilator_Length = 57.5; //unit: mm.
  char NameTem[200];
  
  for(int i=0;i<EnergyNum;i++)
  {
    sprintf(NameTem,"h_%03dMeV",(int) (EnergyRange[0]+i*DeltaE));
    //cout<<NameTem<<endl;
    h1_LightYield[i] = (TH1D*) f1_DB->Get(NameTem);
    
    sprintf(NameTem,"h_%03dMeV_AreaProb",(int) (EnergyRange[0]+i*DeltaE));
    h1_LightYield_AreaProb[i] = (TH1D*) h1_LightYield[i]->Clone(NameTem);
    int NbinsX = h1_LightYield_AreaProb[i]->GetNbinsX();
    for(int iBin=1;iBin<=NbinsX;iBin++)
    {
      h1_LightYield_AreaProb[i]->SetBinContent(iBin,h1_LightYield_AreaProb[i]->GetBinContent(iBin)*h1_LightYield_AreaProb[i]->GetBinWidth(iBin));
    }
    
    EnergyValue[i] = EnergyRange[0]+i*DeltaE;
    //cout<<i<<"  "<<EnergyValue[i]<<endl;
    TotalProbability[i] = h1_LightYield[i]->Integral("width");
    Det_Lamda[i] = -DB_Scintilator_Length/Log(1-TotalProbability[i]); //TMath::Log() mean Ln().
    //cout<<"Read "<<NameTem<<"  Total Probability: "<<TotalProbability[i]<<" Lamda : "<<Det_Lamda[i]<<endl;
  }
  gr1_TotalProbability = new TGraph(EnergyNum,EnergyValue,TotalProbability);
}

double ScinFul::GetLightYield(double StepLength,double Src_Ekin)
{
  int EnergyIndex = Get_EnergyIndex(Src_Ekin);
  
  double Ab_Prob = 1-Exp(-StepLength/Det_Lamda[EnergyIndex]);
  double Rndm = gRandom->Uniform(0,1);
  double LightYield = 0;
  //cout<<"Ab_Prob : "<<Ab_Prob<<" Rndm : "<<Rndm<<endl;
  if(Rndm<=Ab_Prob)
  {
    LightYield = h1_LightYield_AreaProb[EnergyIndex]->GetRandom();
  }
  
return LightYield;
}

TH1D* ScinFul::Get_LightYieldSpec(double Src_Ekin)
{
  int EnergyIndex = Get_EnergyIndex(Src_Ekin);

return h1_LightYield[EnergyIndex];
}

//return the energy index that is the nearest point.
int ScinFul::Get_EnergyIndex(double Src_Ekin)
{
  int EnergyIndex = 0;
  
  if(Src_Ekin>=EnergyRange[1]) { EnergyIndex = EnergyNum-1; }
  else if(Src_Ekin<=EnergyRange[0]) { EnergyIndex = 0; }
  else
  {
    for(int i=0;i<EnergyNum;i++)
    {
      if(Src_Ekin>=EnergyValue[i] && Src_Ekin<EnergyValue[i+1])
      {
        if(Abs(Src_Ekin-EnergyValue[i])>Abs(Src_Ekin-EnergyValue[i+1])) { EnergyIndex=i+1; }
        else { EnergyIndex = i; }
        break;
      }
    }
  }
return EnergyIndex;
}

void ScinFul::Draw_LightYield(int ENum, double* Energy)
{
  string NameTem = "c1_LightYield";
  int LineColor[] = {1, 2, 3, 4, 6, 7, 9, 10, 38, 49};
  
  for(int i=0;i<ENum;i++)
  {
    NameTem += ("_E"+std::to_string((int)Energy[i])+"MeV");
  }
  TCanvas* c1_LightYield = new TCanvas(NameTem.c_str(),NameTem.c_str(),1);
  
  TLegend* legend = new TLegend(0.6,0.6,0.8,0.8);
  
  for(int i=0;i<ENum;i++)
  {
    int EnergyIndex = Get_EnergyIndex(Energy[i]);
    
    if(i==0) { h1_LightYield[EnergyIndex]->Draw("hist"); }
    else { h1_LightYield[EnergyIndex]->Draw("histsame"); }
    if(i<10) { h1_LightYield[EnergyIndex]->SetLineColor(LineColor[i]); }
    h1_LightYield[EnergyIndex]->SetLineWidth(3);
    
    cout<<" Input: "<<Energy[i]<<" , the Index in the DB : "<<EnergyIndex<<" , the nearest energy in DB: "<<EnergyValue[EnergyIndex]<<endl;
    
    legend->AddEntry(h1_LightYield[EnergyIndex],(std::to_string((int) EnergyValue[EnergyIndex])+"MeV").c_str(),"l");
  }
  legend->Draw();
}

void ScinFul::Draw_Eff(double threshold)
{
  TCanvas* c1_DetProbability = new TCanvas("c1_DetProbability","c1_DetProbability",1);
  gr1_TotalProbability->Draw("AL*");
  gr1_TotalProbability->SetMarkerStyle(2);
  gr1_TotalProbability->SetLineWidth(2);
  gr1_TotalProbability->SetLineColor(2);
  gr1_TotalProbability->GetXaxis()->SetTitle("E(MeV)");
  gr1_TotalProbability->GetXaxis()->CenterTitle();
  gr1_TotalProbability->GetYaxis()->SetTitle("Eff");
  gr1_TotalProbability->GetYaxis()->CenterTitle();
  
  double Prob_withThreshold[200] = {0};
  
  for(int i=0;i<EnergyNum;i++)
  {
    int Bin1 = h1_LightYield[i]->FindBin(threshold);
    int Bin2 = h1_LightYield[i]->GetNbinsX();
    Prob_withThreshold[i] = h1_LightYield[i]->Integral(Bin1,Bin2,"width");
  }
  TGraph* gr1_TotalProbability_withThrehold = new TGraph(EnergyNum,EnergyValue,Prob_withThreshold);
  gr1_TotalProbability_withThrehold->Draw("L*same");
  gr1_TotalProbability_withThrehold->SetLineWidth(2);
  gr1_TotalProbability_withThrehold->SetMarkerStyle(5);
  gr1_TotalProbability_withThrehold->SetLineColor(1);
}

void ScinFul::Draw_Eff(int ThresholdNum, double* threshold)
{
  TLegend* legend = new TLegend(0.6,0.6,0.8,0.8);
  
  TCanvas* c1_DetProbability = new TCanvas("c1_DetProbability","c1_DetProbability",1);
  gr1_TotalProbability->Draw("AL*");
  gr1_TotalProbability->SetMarkerStyle(2);
  gr1_TotalProbability->SetLineWidth(2);
  gr1_TotalProbability->SetLineColor(2);
  gr1_TotalProbability->GetXaxis()->SetTitle("E(MeV)");
  gr1_TotalProbability->GetXaxis()->CenterTitle();
  gr1_TotalProbability->GetYaxis()->SetTitle("Eff");
  gr1_TotalProbability->GetYaxis()->CenterTitle();
  
  legend->AddEntry(gr1_TotalProbability,"Thres = 0.008MeVee", "l");
  
  double Prob_withThreshold[20][200] = {{0}};
  TGraph* gr1_TotalProbability_withThrehold[20];
  int LineColor[] = {1, 3, 4, 6, 7, 9, 10, 38, 49};
  
  char NameTem[200];
  for(int iT=0;iT<ThresholdNum;iT++)
  {
    for(int i=0;i<EnergyNum;i++)
    {
      int Bin1 = h1_LightYield[i]->FindBin(threshold[iT]);
      int Bin2 = h1_LightYield[i]->GetNbinsX();
      Prob_withThreshold[iT][i] = h1_LightYield[i]->Integral(Bin1,Bin2,"width");
    }
    gr1_TotalProbability_withThrehold[iT] = new TGraph(EnergyNum,EnergyValue,Prob_withThreshold[iT]);
    gr1_TotalProbability_withThrehold[iT]->Draw("L*same");
    gr1_TotalProbability_withThrehold[iT]->SetLineWidth(2);
    gr1_TotalProbability_withThrehold[iT]->SetMarkerStyle(5);
    gr1_TotalProbability_withThrehold[iT]->SetLineColor(LineColor[iT]);
    
    sprintf(NameTem,"Thres = %.1fMeVee",threshold[iT]);
    legend->AddEntry(gr1_TotalProbability_withThrehold[iT],NameTem, "l");
  }
  legend->Draw();
  
}
