#ifndef B2PrimaryGeneratorAction_h
#define B2PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "Config.hh"
#include "Gun_Dir.hh"
#include "B2RunAction.hh"
#include "B2EventAction.hh"
#include "TRandom.h"
#include "TF1.h"
#include "Config.hh"
#include "TMath.h"
using namespace TMath;

class G4ParticleGun;
class G4Event;

class B2PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    B2PrimaryGeneratorAction(B2RunAction* m_Run_tem,B2EventAction* m_Evt_tem);
    virtual ~B2PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event* );
    G4ParticleGun* GetParticleGun() {return fParticleGun;}
    double Cal_Ekin(double Momentum,double Mass);
    
    // Set methods
    G4double Certain_Ekin;
    int EvtNum;
    G4double ParticleMass;
    TF1* f1_Sin;
    double SrcPos[3];
    string EmittedMod;
    double ThetaRange[2]; double PhiRange[2];
    
  private:
    B2RunAction* m_Run;
    B2EventAction* m_Evt;
    G4ParticleGun* fParticleGun; // G4 particle gun
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
