#include "B2TrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "iostream"
#include "string.h"
#include "math.h"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2TrackerSD::B2TrackerSD(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2TrackerSD::~B2TrackerSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection 
    = new B2TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B2TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  //judge the boundary of geometry.
  G4StepPoint* prePoint = aStep->GetPreStepPoint();
  G4StepPoint* postPoint = aStep->GetPostStepPoint();
  int TrackID = aStep->GetTrack()->GetTrackID();
  
  if(TrackID==1 && prePoint->GetStepStatus()==fGeomBoundary && postPoint->GetStepStatus()==fGeomBoundary)
  {
    //G4Track* aTrack = aStep->GetTrack();
    //aTrack->SetTrackStatus(fStopAndKill);
    G4double StepLength = aStep->GetStepLength();
    G4TouchableHandle preTouch = prePoint ->GetTouchableHandle();
    int NW_BarIndex = preTouch->GetCopyNumber(2)*100+(preTouch->GetCopyNumber(1)-100);
    
    //cout<<"NW_BarIndex : "<<NW_BarIndex<<" StepLength: "<<StepLength<<endl;
    /*
    int CopyNum_0 = preTouch->GetCopyNumber(0);
    int CopyNum_1 = preTouch->GetCopyNumber(1);
    int CopyNum_2 = preTouch->GetCopyNumber(2);
    int CopyNum_3 = preTouch->GetCopyNumber(3);
    cout<<"CopyNum_0 : "<<CopyNum_0<<"CopyNum_1 : "<<CopyNum_1<<" CopyNum_2 : "<<CopyNum_2<<" CopyNum_3 : "<<CopyNum_3<<endl;
    */
    B2TrackerHit* newHit = new B2TrackerHit();
    newHit->Set_NW_BarIndex(NW_BarIndex);
    newHit->Set_StepLength(StepLength);
    fHitsCollection->insert( newHit );
    
    //newHit->Print();
    return 1;
  }
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2TrackerSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout << G4endl
            << "-------->Hits Collection: in this event they are " << nofHits 
            << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
