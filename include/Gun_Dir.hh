#ifndef Gun_Dir_h 
#define Gun_Dir_h 1

#include "iostream"
#include "fstream"
#include "TRandom.h"
#include "TMath.h"
#include "TF1.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;
using namespace TMath;

#define Gun_Dir_Work_Mode_Num 4

class Gun_Dir
{
public:
  Gun_Dir();
  ~Gun_Dir();
public:
  string WorkMode_Allowed[Gun_Dir_Work_Mode_Num];
  string Work_Mode;//Fixed_Dir, 4pi_iso, Constrained_Dir
  bool Set_WorkMode(string Mode_tem);
  void Show_Defined_WorkMode();
  void Get_Gun_Dir(double* Dir_tem);
  void Get_Uniform_ThetaPhi_Dir(double* Dir_tem);
  
public://for Fixed Direction
  double FixedDir[4];
  bool Init_FixedDir(double* Dir_tem);//this function will do the normalize for the vector
  void GetFixedDir(double* Dir_tem){ for(int i=0;i<3;i++){ Dir_tem[i] = FixedDir[i]; } }
  
public://for 4pi isotrope 
  TF1* f1_sin_theta;
  void Get_4pi_Iso_Dir(double* Dir_tem);

public://for constrained angle
  double Const_Dir[3]; //the random vector should coming out around this vector
  double Const_Theta[2]; //the open angle from the Const_Dir
  double Const_Phi[2];
  double zp[3]; double xp[3]; double yp[3];
  
  void Set_Const_Theta_Phi(double* Theta_tem,double* Phi_tem)
  { for(int i=0;i<2;i++){ Const_Theta[i] = Theta_tem[i];Const_Phi[i] = Phi_tem[i]; } }
  void Get_Const_Theta_Dir(double* Random_Dir_tem);
  void Cal_Cross_Product(double* V1_tem,double* V2_tem,double* Results);
  void Cal_Trans_Matrix(double* Const_Dir_tem);
};

#endif
