#include "B2PrimaryGeneratorAction.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonTable.hh"
#include "Randomize.hh"
#include "G4EventManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2PrimaryGeneratorAction::B2PrimaryGeneratorAction(B2RunAction* m_Run_tem,B2EventAction* m_Evt_tem)
 : G4VUserPrimaryGeneratorAction()
{
  m_Run = m_Run_tem;
  m_Evt = m_Evt_tem;
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);
  
  Config* configure = Config::Get_Config_Man();
  G4String ParticleName = configure->Get_ConfigValue_string("Particle");
  Certain_Ekin = configure->Get_ConfigValue_double("Certain_Ekin")*MeV;
  
  G4ParticleDefinition* particleDefinition = 0;
  
  if(ParticleName=="proton" || ParticleName=="neutron" || ParticleName=="pi+" || ParticleName=="pi-"|| ParticleName=="pi0"|| ParticleName=="e+" || ParticleName=="e-" || ParticleName=="mu+" || ParticleName=="mu-" || ParticleName=="geantino")
  {
    particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle(ParticleName.c_str());
  }
  else if(ParticleName=="heavyion")
  {
    G4double Particle_A = configure->Get_ConfigValue_double("Particle_A");
    G4double Particle_Z = configure->Get_ConfigValue_double("Particle_Z");
    particleDefinition = G4IonTable::GetIonTable()->GetIon(Particle_Z,Particle_A,0);//[1]
  }
  ParticleMass = particleDefinition->GetPDGMass();
  fParticleGun->SetParticleDefinition(particleDefinition);
  
  configure->Get_ConfigValue_doubleArray("SrcPos",0,2,SrcPos);
  EmittedMod = configure->Get_ConfigValue_string("EmittedMod");
  configure->Get_ConfigValue_doubleArray("ThetaRange",0,1,ThetaRange);
  configure->Get_ConfigValue_doubleArray("PhiRange",0,1,PhiRange);
  f1_Sin = new TF1("f1_Sin","TMath::Sin(x)",0,Pi());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2PrimaryGeneratorAction::~B2PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void B2PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  fParticleGun->SetParticleEnergy(Certain_Ekin);
  fParticleGun->SetParticlePosition(G4ThreeVector(SrcPos[0]*mm, SrcPos[1]*mm, SrcPos[2]*mm));
  
  double Phi = 0; double Theta = 0;
  Phi = gRandom->Uniform(PhiRange[0],PhiRange[1])*DegToRad();
  if(EmittedMod=="Isotopic")
  { 
    Theta = f1_Sin->GetRandom(ThetaRange[0]*DegToRad(),ThetaRange[1]*DegToRad());
  }
  else if(EmittedMod=="Uniform")
  {
    Theta = gRandom->Uniform(ThetaRange[0]*DegToRad(),ThetaRange[1]*DegToRad());
  }
  
  double Dir_XYZ_tem[3] = {Sin(Theta)*Cos(Phi),Sin(Theta)*Sin(Phi),Cos(Theta)};
  G4ThreeVector MomentumDir(Dir_XYZ_tem[0],Dir_XYZ_tem[1],Dir_XYZ_tem[2]);
  fParticleGun->SetParticleMomentumDirection(MomentumDir);
  
  m_Run->Set_SrcInfo(Certain_Ekin, Theta*RadToDeg(), Phi*RadToDeg());
  m_Evt->Set_SrcInfo(Certain_Ekin, Theta*RadToDeg(), Phi*RadToDeg());
  
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

double B2PrimaryGeneratorAction::Cal_Ekin(double Momentum,double Mass)
{
  return (Sqrt(Momentum*Momentum+Mass*Mass)-Mass);
}
