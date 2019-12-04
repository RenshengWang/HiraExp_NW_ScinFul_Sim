#include "B2EventAction.hh"
#include "TMath.h"
#include "Config.hh"

using namespace TMath;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2EventAction::B2EventAction(B2RunAction* p_Run_tem)
: G4UserEventAction()
{
  m_DM = G4DigiManager::GetDMpointer();
  m_Run = p_Run_tem;
  ScinFul_LightYield = new ScinFul();
  Config* configure = Config::Get_Config_Man();
  string ScinFul_DRF_withPath = configure->Get_ConfigValue_string("ScinFul_DRF_withPath");
  ScinFul_LightYield->Initial_ScinFul_DB(ScinFul_DRF_withPath);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2EventAction::~B2EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2EventAction::BeginOfEventAction(const G4Event* evt)
{
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2EventAction::EndOfEventAction(const G4Event* evt)
{
//  G4int event_id = evt->GetEventID();
  
  G4int HCI = -1;
  HCI = m_DM->GetHitsCollectionID("NW_HitsCollection");
  
  if(HCI>=0)
  {
    B2TrackerHitsCollection* HC = 0;
    HC = (B2TrackerHitsCollection*) (m_DM->GetHitsCollection(HCI));
    G4int n_hit = HC->entries();
    int BarNum = 0;
    int NW_BarIndex[10] = {0};
    double LightYield[10] = {0};
    double Length[10] = {0};
    
    for(int i=0;i<n_hit;i++)
    {
      double StepLength = (*HC)[i]->Get_StepLength();
      int BarIndex = (*HC)[i]->Get_NW_BarIndex();
      
      double LY = ScinFul_LightYield->GetLightYield(StepLength,Src_Ekin);
      //cout<<" event_id: "<<event_id<<" Src_Ekin: "<<Src_Ekin<<"  LY : "<<LY<<endl;
      LightYield[BarNum] = LY; 
      NW_BarIndex[BarNum] = BarIndex; 
      Length[BarNum] = StepLength; 
      BarNum++;
    }
    //if(n_hit>0) 
    m_Run->FillData(BarNum, NW_BarIndex, Length, LightYield); //no matter the hit number, we need to store the data.
  }
  
}
