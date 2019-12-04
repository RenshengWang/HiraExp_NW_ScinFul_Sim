#include "B2RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "Config.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2RunAction::B2RunAction()
 : G4UserRunAction()
{ 
  // set printing event number per each 100 events
  Config* configure = Config::Get_Config_Man();
  int ShowEvt_Num  = configure->Get_ConfigValue_int("ShowEvt_Num");
  G4RunManager::GetRunManager()->SetPrintProgress(ShowEvt_Num);
//  f1_data = 0;
//  t1_data = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2RunAction::~B2RunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::BeginOfRunAction(const G4Run*)
{ 
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  Config* configure = Config::Get_Config_Man();
  G4String Output_FileName = configure->Get_ConfigValue_string("Output_FileName");
  
  f1_data = new TFile(Output_FileName.c_str(),"recreate");
  t1_data = new TTree("t1","decay particles");
  
  t1_data->Branch("Src_Ekin",&Src_Ekin,"Src_Ekin/D");
  t1_data->Branch("Src_Theta",&Src_Theta,"Src_Theta/D");
  t1_data->Branch("Src_Phi",&Src_Phi,"Src_Phi/D");
  
  t1_data->Branch("NW_BarNum",&NW_BarNum,"NW_BarNum/I");
  t1_data->Branch("NW_BarIndex",NW_BarIndex,"NW_BarIndex[NW_BarNum]/I");
  t1_data->Branch("NW_StepLength",NW_StepLength,"NW_StepLength[NW_BarNum]/D");
  t1_data->Branch("NW_LightYield",NW_LightYield,"NW_LightYield[NW_BarNum]/D");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::EndOfRunAction(const G4Run* )
{
  if(t1_data!=0) { t1_data->Write(); }
  if(t1_data!=0) { t1_data->Delete(); t1_data=0; }
  if(f1_data!=0)
  {
    cout<<f1_data->GetName()<<" Closed!"<<endl;
    f1_data->Close(); f1_data->Delete(); f1_data=0; 
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
