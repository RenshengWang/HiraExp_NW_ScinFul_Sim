void ReadGeoEff()
{
  TFile* f1_GeoEff = new TFile("f1_NW_GeoEff.root","read");
  
  TH1D* h1_NWa_GeoEff = (TH1D*) f1_GeoEff->Get("h1_NWa_GeoEff");
  TH1D* h1_NWb_GeoEff = (TH1D*) f1_GeoEff->Get("h1_NWb_GeoEff");
  
  double ThetaValue = 35;
  double GeoEff = h1_NWa_GeoEff->GetBinContent(h1_NWa_GeoEff->FindBin(ThetaValue));
  
  cout<<"GeoEff : "<<GeoEff<<endl;
}
