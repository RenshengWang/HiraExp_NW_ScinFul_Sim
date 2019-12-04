
#include "B2ActionInitialization.hh"
#include "B2PrimaryGeneratorAction.hh"
#include "B2RunAction.hh"
#include "B2EventAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2ActionInitialization::B2ActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2ActionInitialization::~B2ActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2ActionInitialization::BuildForMaster() const
{
  SetUserAction(new B2RunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2ActionInitialization::Build() const
{
  B2RunAction* m_Run = new B2RunAction();
  SetUserAction(m_Run);
  B2EventAction* m_Evt = new B2EventAction(m_Run);
  SetUserAction(m_Evt);
  SetUserAction(new B2PrimaryGeneratorAction(m_Run,m_Evt));
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
