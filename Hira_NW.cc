
#include "B2aDetectorConstruction.hh"
#include "B2ActionInitialization.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "QGSP_BERT_HP.hh"
//#include "QGSP_BERT_HP_EMY.hh"
#include "Shielding.hh"
#include "G4StepLimiterPhysics.hh"

#include "Randomize.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "Config.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  Config* configure = Config::Get_Config_Man();
  int Is_Batch_mode = configure->Get_ConfigValue_int("Is_Batch_mode");
  
  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = 0;
  if(Is_Batch_mode==0) { ui = new G4UIExecutive(argc, argv); }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED  
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  //
  runManager->SetUserInitialization(new B2aDetectorConstruction());
  string PhysicsListName = configure->Get_ConfigValue_string("PhysicsList");
  G4VModularPhysicsList* physicsList = 0;
  
  if(PhysicsListName=="FTFP_BERT")
  {
    physicsList = new FTFP_BERT();
    physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  }
  else if(PhysicsListName=="QGSP_BERT_HP")
  {
    physicsList = new QGSP_BERT_HP();
    physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  }
  else if(PhysicsListName=="Shielding")
  {
    physicsList = new Shielding();
    physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  }
  
  if(physicsList!=0) { runManager->SetUserInitialization(physicsList); }
  else {  cout<<"The PhysicsList is Null ptr;"<<endl; return 0; }
  
  // Set user action classes
  runManager->SetUserInitialization(new B2ActionInitialization());
  
  // Initialize visualization
  //
  G4VisManager* visManager;
  if(Is_Batch_mode==0)
  {
    visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();
  }
  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Process macro or start UI session
  //
  if (! ui) 
  {// batch mode
    UImanager->ApplyCommand("/control/execute ./macro/init_BatchMode.mac");
    int Batch_mode_run_beamOn = configure->Get_ConfigValue_int("Batch_mode_run_beamOn");
    char Command_tem[200];
    sprintf(Command_tem,"/run/beamOn %d",Batch_mode_run_beamOn);
    UImanager->ApplyCommand(Command_tem);
  }
  else 
  {// interactive mode
    UImanager->ApplyCommand("/control/execute ./macro/init_InteractiveMode.mac");
    if(configure->Get_ConfigValue_int("Is_Geo_Tracking")!=0)
    {
      UImanager->ApplyCommand("/control/execute ./macro/vis.mac");
      if (ui->IsGUI()) 
      { UImanager->ApplyCommand("/control/execute ./macro/gui.mac"); }
    }
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !
  //
  if(Is_Batch_mode==0) { delete visManager; }
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
