#include "Gun_Dir.hh"

Gun_Dir::Gun_Dir()
{
  f1_sin_theta = new TF1("f1_sin_theta","sin(x)",0,Pi());
  
  WorkMode_Allowed[0] = "FixedDir";
  WorkMode_Allowed[1] = "4pi_Iso";
  WorkMode_Allowed[2] = "Const_Dir";
  WorkMode_Allowed[2] = "Uniform_ThetaPhi";
  //default state
  Work_Mode = "FixedDir"; 
  double Dir_tem[3] = {0,0,1};
  Init_FixedDir(Dir_tem);
}

bool Gun_Dir::Set_WorkMode(string Mode_tem)
{
  for(int i=0;i<Gun_Dir_Work_Mode_Num;i++)
  {
    if(Mode_tem==WorkMode_Allowed[i]) { Work_Mode = Mode_tem; return 1;}
  }
  Show_Defined_WorkMode();
  return 0;
}

Gun_Dir::~Gun_Dir()
{
  if(f1_sin_theta!=0){ f1_sin_theta->Delete(); f1_sin_theta=0; }
}

void Gun_Dir::Get_Gun_Dir(double* Dir_tem)
{
  if(Work_Mode=="FixedDir")
  {
    for(int i=0;i<3;i++)
    { Dir_tem[i] = FixedDir[i]; }
  }
  else if(Work_Mode=="4pi_Iso")
  {
    Get_4pi_Iso_Dir(Dir_tem);
  }
  else if(Work_Mode=="Const_Dir")
  {
    Get_Const_Theta_Dir(Dir_tem);
  }
  else if(Work_Mode=="Uniform_ThetaPhi")
  {
    Get_Uniform_ThetaPhi_Dir(Dir_tem);
  }
  else
  {
    cout<<"Now this class does not include the work mode that you desired!"<<endl; getchar();
  }
}

bool Gun_Dir::Init_FixedDir(double* Dir_tem)
{
  double Amplitude_tem = sqrt(Dir_tem[0]*Dir_tem[0]+Dir_tem[1]*Dir_tem[1]+Dir_tem[2]*Dir_tem[2]);
  if(Amplitude_tem==0)
  {
    cout<<"The vector that you input is non-meaning, Pay attention."<<endl; getchar();
    return 0;
  }
  else
  {
    for(int i=0;i<3;i++)
    { FixedDir[i] = Dir_tem[i]/Amplitude_tem; }
    return 1;
  }
}

void Gun_Dir::Get_4pi_Iso_Dir(double* Dir_tem)
{
  double phi_tem   = gRandom->Uniform(0,2*Pi());
  double theta_tem = f1_sin_theta->GetRandom(0,Pi());
  
  Dir_tem[0] = Sin(theta_tem)*Cos(phi_tem);
  Dir_tem[1] = Sin(theta_tem)*Sin(phi_tem);
  Dir_tem[2] = Cos(theta_tem);
}

void Gun_Dir::Get_Uniform_ThetaPhi_Dir(double* Dir_tem)
{
  double phi_tem   = gRandom->Uniform(0,2*Pi());
  double theta_tem = gRandom->Uniform(0,Pi());
  
  Dir_tem[0] = Sin(theta_tem)*Cos(phi_tem);
  Dir_tem[1] = Sin(theta_tem)*Sin(phi_tem);
  Dir_tem[2] = Cos(theta_tem);
}

void Gun_Dir::Get_Const_Theta_Dir(double* Random_Dir_tem)
{
  double phi_tem   = gRandom->Uniform(Const_Phi[0],Const_Phi[1]);
  double theta_tem = f1_sin_theta->GetRandom(Const_Theta[0],Const_Theta[1]);
  
  double Dir_tem[3] = {0};
  Dir_tem[0] = Sin(theta_tem)*Cos(phi_tem);
  Dir_tem[1] = Sin(theta_tem)*Sin(phi_tem);
  Dir_tem[2] = Cos(theta_tem);
  
  for(int i=0;i<3;i++)
  {
    Random_Dir_tem[i] = Dir_tem[0]*xp[i]+Dir_tem[1]*yp[i]+Dir_tem[2]*zp[i];
  }
}

void Gun_Dir::Cal_Trans_Matrix(double* Const_Dir_tem)
{
  for(int i=0;i<3;i++)
  {
    Const_Dir[i] = Const_Dir_tem[i];
  }
  double z[3] = {0,0,1}; double x[3] = {1,0,0}; double y[3] = {0,1,0}; 
  for(int i=0;i<3;i++) { xp[i] = 0; yp[i] = 0; zp[i] = 0; }
  for(int i=0;i<3;i++)
  {
    zp[i] = Const_Dir[i]/sqrt(Const_Dir[0]*Const_Dir[0] + Const_Dir[1]*Const_Dir[1] + Const_Dir[2]*Const_Dir[2]);
  }

  if(zp[2]==1)
  { xp[0] = 1; yp[1] = 1; }
  else if(zp[2]==-1)
  { xp[0] = -1; yp[1] = -1; }
  else
  {
    Cal_Cross_Product(zp,z,xp);
    Cal_Cross_Product(zp,xp,yp);
  }
}

void Gun_Dir::Cal_Cross_Product(double* V1_tem,double* V2_tem,double* Results)
{
  Results[0] = V1_tem[1]*V2_tem[2]-V1_tem[2]*V2_tem[1];
  Results[1] = V1_tem[2]*V2_tem[0]-V1_tem[0]*V2_tem[2];
  Results[2] = V1_tem[0]*V2_tem[1]-V1_tem[1]*V2_tem[0];
}

void Gun_Dir::Show_Defined_WorkMode()
{
  cout<<"There are only "<<Gun_Dir_Work_Mode_Num<<" defined:"<<endl;
  for(int i=0;i<Gun_Dir_Work_Mode_Num;i++)
  {
    cout<<WorkMode_Allowed[i]<<"  ";
  }
  cout<<endl;
}
