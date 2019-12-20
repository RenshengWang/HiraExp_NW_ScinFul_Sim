void Test_Eff()
{
  NW_Eff_DB* NW_Eff = new NW_Eff_DB();
  double Threshold = 3.0; //Unit: MeVee.
  
  char NameTem[200];
  const int EkinNum = 146;
  double EkinValue[EkinNum] = {0};
  sprintf(NameTem,"f1_Eff_DB_%.1fMeVee.root",Threshold);
  string DBFileName = NameTem;
  
  string SimData_FileNameList[EkinNum];
  
  for(int iE=0;iE<EkinNum;iE++)
  {
    EkinValue[iE] = iE+5;
    sprintf(NameTem,"./SimData/Neutron_%dMeV_1E7.root",(int) EkinValue[iE]); //the EvtNum 1E8, but I make a writtern mistake.
    SimData_FileNameList[iE] = NameTem;
  }
  NW_Eff -> SetThreshold(Threshold); //Unit: MeVee.
  //NW_Eff -> Produce_EffDB(EkinNum, EkinValue,SimData_FileNameList, DBFileName);
  NW_Eff->InitialDB(DBFileName);
  NW_Eff->Initial_GeoEff(0.01);
  
  //the below will draw the Geometry efficiency histogram for different Ekin.
  NW_Eff->Draw_GeoEff_Histo(5);
  NW_Eff->Draw_GeoEff_Histo(15);
  NW_Eff->Draw_GeoEff_Histo(20);
  NW_Eff->Draw_GeoEff_Histo(145);
  double GeoEff = NW_Eff->Get_GeoEff(20, 30);
  cout<<"Ekin="<<20<<" MeV, Theta="<<30<<" , GeoEff = "<<GeoEff<<endl;
  
  //Draw the Detect efficiency for a fix Ekin.
  NW_Eff -> DrawEff_withFixEkin(15);
  NW_Eff -> DrawEff_withFixEkin(45);
  NW_Eff -> DrawEff_withFixEkin(65);
  NW_Eff -> DrawEff_withFixEkin(142);
  
  //Draw the Detect efficiency for a fix angle.
  NW_Eff -> DrawEff_withFixAngle(31.6, -2.9);
  NW_Eff -> DrawEff_withFixAngle(31.6, -2.2);
  
  double TestEkin = 45; double EffWithErr[3];
  NW_Eff -> Get_Eff(TestEkin, 31.6, -2.9,EffWithErr);
  cout<<EffWithErr[0]<<"  "<<EffWithErr[1]<<"  "<<EffWithErr[2]
      <<" RelErr: "<<(EffWithErr[1]+EffWithErr[2])/EffWithErr[0]<<endl;
}
