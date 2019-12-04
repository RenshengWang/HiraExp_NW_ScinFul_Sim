#ifndef B2EventAction_h
#define B2EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4DigiManager.hh"

#include "B2RunAction.hh"
#include "Config.hh"
#include "iostream"
#include "B2TrackerHit.hh"
#include "math.h"
#include "ScinFul.hh"

using namespace std;
class G4DigiManager;
/// Event action class

class B2EventAction : public G4UserEventAction
{
public:
  B2EventAction(B2RunAction* p_Run_tem);
  virtual ~B2EventAction();
  G4DigiManager* m_DM;
  
  ScinFul* ScinFul_LightYield;
  
  virtual void  BeginOfEventAction(const G4Event* );
  virtual void  EndOfEventAction(const G4Event* );
  
  double Src_Theta; double Src_Phi; double Src_Ekin;
  void Set_SrcInfo(double E, double Theta, double Phi) { Src_Ekin = E; Src_Theta = Theta; Src_Phi = Phi; }
  
  
  
public:
  B2RunAction* m_Run;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
