void Test_Eff()
{
  NW_Eff_DB* NW_Eff = new NW_Eff_DB();
  
  const int EkinNum = 1;
  double EkinValue[] = {100};
  string DBFileName = "f1_Eff_DB.root";
  
  string SimData_FileNameList[EkinNum];
  
  char NameTem[200];
  
  for(int iE=0;iE<EkinNum;iE++)
  {
    sprintf(NameTem,"f1_SimData_NWb_E%dMeV_1E8.root",(int) EkinValue[iE]);
    SimData_FileNameList[iE] = NameTem;
  }
  NW_Eff -> Produce_EffDB(EkinNum, EkinValue,SimData_FileNameList, DBFileName);
  //NW_Eff->InitialDB(DBFileName);
  
  TH2D* h2_Eff_Test = new TH2D("h2_Eff_Test",";Theta;Phi",80,20,60,120,-30,30);
  for(int iTheta=1;iTheta<=60;iTheta++)
  {
    for(int iPhi=1;iPhi<=108;iPhi++)
    {
      double Eff = NW_Eff -> Get_Eff(100.0, 26+iTheta*0.5, -27+iPhi*0.5);//,EffWithErr);
      h2_Eff_Test->Fill(26+iTheta*0.5,-27+iPhi*0.5,Eff);
    }
  }
  h2_Eff_Test->Draw("colz");
  
  
  double EffWithErr[3];
  NW_Eff -> Get_Eff(100.499, 31.6, -2.9,EffWithErr);
  cout<<EffWithErr[0]<<"  "<<EffWithErr[1]<<"  "<<EffWithErr[2]
      <<" RelErr: "<<(EffWithErr[1]+EffWithErr[2])/EffWithErr[0]<<endl;
}
