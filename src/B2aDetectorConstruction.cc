#include "B2aDetectorConstruction.hh"
#include "B2aDetectorMessenger.hh"
#include "B2TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Ellipsoid.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"
#include "Config.hh"

#include "TMath.h"
using namespace TMath;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4ThreadLocal 
G4GlobalMagFieldMessenger* B2aDetectorConstruction::fMagFieldMessenger = 0;

B2aDetectorConstruction::B2aDetectorConstruction()
:G4VUserDetectorConstruction(), 
 fStepLimit(NULL),
 fCheckOverlaps(true)
{
  fMessenger = new B2aDetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
B2aDetectorConstruction::~B2aDetectorConstruction()
{
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4VPhysicalVolume* B2aDetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::DefineMaterials()
{
  // Material definition 
  G4NistManager* nistManager = G4NistManager::Instance();
  Config* configure = Config::Get_Config_Man();
  
  // Air defined using NIST Manager
  Air = nistManager->FindOrBuildMaterial("G4_AIR");
  vaccum = nistManager->FindOrBuildMaterial("G4_Galactic");
  //elements
  G4Element* H = nistManager->FindOrBuildElement("H",false);//1
  G4Element* C = nistManager->FindOrBuildElement("C",false);//6
  //Materials for the NW bar
  // LiquidScint(NE213) EJ-301
  NE213 = new G4Material("NE213",0.874*g/cm3,2);
  NE213->AddElement(H,10);
  NE213->AddElement(C,8);
  Pyrex_Glass = nistManager->FindOrBuildMaterial("G4_Pyrex_Glass"); //bar material and light guide
  
  BC412 = new G4Material("BC412",1.023*g/cm3,2);
  BC412->AddElement(H,0.0842);
  BC412->AddElement(C,0.9158);
  
  string Scint_Material_Name = configure->Get_ConfigValue_string("Scint_Material");
  if(Scint_Material_Name=="NE213") { Scint_Material = NE213; }
  else if(Scint_Material_Name=="BC412") { Scint_Material = BC412; }
  else if(Scint_Material_Name=="vaccum") { Scint_Material = vaccum; }
  
  string world_Material_Name = configure->Get_ConfigValue_string("world_Material");
  if(world_Material_Name=="Air") { world_Material = Air; }
  else if(world_Material_Name=="vaccum") { world_Material = vaccum; }
  
  string BarCase_Material_Name = configure->Get_ConfigValue_string("BarCase_Material");
  if(BarCase_Material_Name=="Pyrex_Glass") { BarCase_Material = Pyrex_Glass; }
  else if(BarCase_Material_Name=="vaccum") { BarCase_Material = vaccum; }
  
  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2aDetectorConstruction::DefineVolumes()
{
  Config* configure = Config::Get_Config_Man();
  
  double worldLength[3] = {0,0,0};
  configure->Get_ConfigValue_doubleArray("worldLength",0,2,worldLength);
  
  G4Box* worldS = new G4Box("world",0.5*worldLength[0]*mm,0.5*worldLength[1]*mm,0.5*worldLength[2]*mm); //its size
  G4LogicalVolume* worldLV = new G4LogicalVolume( worldS, world_Material, "World");
  G4VPhysicalVolume* worldPV  = new G4PVPlacement(0,
                 G4ThreeVector(0,0,0), // at (0,0,0)
                 worldLV,         // its logical volume
                 "World",         // its name
                 0,               // its mother  volume
                 false,           // no boolean operations
                 0,               // copy number
                 fCheckOverlaps); // checking overlaps 
  /*
  					Y
  					^
  	|				|				| 24 
  	|				|				| 23
  	|				|				| 22
  	|				|				|  *
  	|				|     Z				|  *
  	|				|    /				|  *
  	|				|   /				| 15
  	|				|  /				| 14
  	|				| /      			| 13
   X <--|---------------------------------------------------------------|-12
  	|				|				| 11 
  	|				|				| 10
  	|				|				| 09
  	|				|				|  *
  	|				|				|  *
  	|				|				|  *
  	|				|				| 02
  	|				|				| 01
  	|				|				| 00  					|  
  
  */
  // NW Bar
  G4double NW_Unit_X = configure->Get_ConfigValue_double("NW_Unit_X")*mm;
  G4double NW_Unit_Y = configure->Get_ConfigValue_double("NW_Unit_Y")*mm;
  G4double NW_Unit_Z = configure->Get_ConfigValue_double("NW_Unit_Z")*mm;
  G4double NW_Unit_Pyrex_Thickness = configure->Get_ConfigValue_double("NW_Unit_Pyrex_Thickness")*mm;
  G4double NW_Unit_SurfaceDist = configure->Get_ConfigValue_double("NW_Unit_SurfaceDist")*mm;
  
  // Scintillating Material Volume
  G4double Scintillator_X = NW_Unit_X-0.1; //avoiding the overlapping geometry boundary.
  G4double Scintillator_Y = NW_Unit_Y-2*NW_Unit_Pyrex_Thickness;
  G4double Scintillator_Z = NW_Unit_Z-2*NW_Unit_Pyrex_Thickness;
  
  //the below is for the case of Scintillator-NE213, material:pyrex_glass.
  G4Box* solidBarCase =new G4Box("solidBarCase",0.5*NW_Unit_X, 0.5*NW_Unit_Y, 0.5*NW_Unit_Z);
  G4LogicalVolume* logicBarCase = new G4LogicalVolume(solidBarCase, BarCase_Material,"logicBarCase");
  
  //the below is for the NE213.
  G4Box* solidBarScint = new G4Box("solidBarScint",0.5*Scintillator_X, 0.5*Scintillator_Y, 0.5*Scintillator_Z);
  G4LogicalVolume* logicBarScint = new G4LogicalVolume(solidBarScint, Scint_Material,"logicBarScint");
  new G4PVPlacement(0,               // no rotation
                    G4ThreeVector(0*mm,0*mm,0*mm),  // at (x,y,z)
                    logicBarScint,   // its logical volume
                    "Phy_BarScint",  // its name
                    logicBarCase,    // its mother volume
                    false,           // no boolean operations
                    1,               // copy number
                    fCheckOverlaps); // checking overlaps
  
  //the below is a definition of neutron wall.
  G4Box* solid_NW = new G4Box("solid_NW",0.5*(NW_Unit_X+10), 0.5*(25*(NW_Unit_Y+NW_Unit_SurfaceDist)+10), 0.5*(Scintillator_Z+10));
  G4LogicalVolume* logic_NW = new G4LogicalVolume(solid_NW, Air,"logic_NW");
  for(int i=0;i<25;i++)
  {
    //put the NW unit in the World.
    new G4PVPlacement(0,               // no rotation
                    G4ThreeVector(0*mm,(-79.2*12+i*79.2)*mm,0*mm),  // at (x,y,z)
                    logicBarCase,    // its logical volume
                    "Phy_NW",        // its name
                    logic_NW,         // its mother volume
                    false,           // no boolean operations
                    100+i,               // copy number
                    fCheckOverlaps); // checking overlaps
  }
  
  //put the NW unit in the World.
  double NWa_Pos[3]; double NWb_Pos[3];
  
  double NWb_Dist = configure->Get_ConfigValue_double("NWb_Dist");
  double NWa_Dist = configure->Get_ConfigValue_double("NWa_Dist");
  
  double NWa_RotY = configure->Get_ConfigValue_double("NWa_RotY");
  double NWb_RotY = configure->Get_ConfigValue_double("NWb_RotY");
  
  double NWb_X_offset = configure->Get_ConfigValue_double("NWb_X_offset");
  double NWa_X_offset = configure->Get_ConfigValue_double("NWa_X_offset");
  
  //the below is for the position calculation.
  NWa_Pos[0] = NWa_Dist*Sin(NWa_RotY*DegToRad())+NWa_X_offset*Cos(NWa_RotY*DegToRad());
  NWa_Pos[1] = 0;
  NWa_Pos[2] = NWa_Dist*Cos(NWa_RotY*DegToRad())-NWa_X_offset*Sin(NWa_RotY*DegToRad());
  
  NWb_Pos[0] = NWb_Dist*Sin(NWb_RotY*DegToRad())+NWb_X_offset*Cos(NWb_RotY*DegToRad());
  NWb_Pos[1] = 0;
  NWb_Pos[2] = NWb_Dist*Cos(NWb_RotY*DegToRad())-NWb_X_offset*Sin(NWb_RotY*DegToRad());
  
  //the below is for the rotation matrix.
  G4RotationMatrix* NWa_Rot = new G4RotationMatrix();
  NWa_Rot->rotateY(-NWa_RotY*deg);
  G4RotationMatrix* NWb_Rot = new G4RotationMatrix();
  NWb_Rot->rotateY(-NWb_RotY*deg);
  
  int IsSet_NWa = configure->Get_ConfigValue_int("IsSet_NWa");
  int IsSet_NWb = configure->Get_ConfigValue_int("IsSet_NWb");
  
  if(IsSet_NWa!=0)
  { 
    new G4PVPlacement(NWa_Rot,               // no rotation
                    G4ThreeVector(NWa_Pos[0]*mm,NWa_Pos[1]*mm,NWa_Pos[2]*mm),  // at (x,y,z)
                    logic_NW,    // its logical volume
                    "Phy_NWa",        // its name
                    worldLV,         // its mother volume
                    false,           // no boolean operations
                    2,               // copy number
                    fCheckOverlaps); // checking overlaps
  }
  if(IsSet_NWb!=0)
  {
    new G4PVPlacement(NWb_Rot,               // no rotation
                    G4ThreeVector(NWb_Pos[0]*mm,NWb_Pos[1]*mm,NWb_Pos[2]*mm),  // at (x,y,z)
                    logic_NW,    // its logical volume
                    "Phy_NWb",        // its name
                    worldLV,         // its mother volume
                    false,           // no boolean operations
                    3,               // copy number
                    fCheckOverlaps); // checking overlaps
  }
  // Visualization attributes
  G4VisAttributes* boxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
  
  logicBarCase      ->SetVisAttributes(boxVisAtt);
  logicBarScint     ->SetVisAttributes(chamberVisAtt);
  
  G4double Scint_MaxStep = configure->Get_ConfigValue_double("Scint_MaxStep")*mm;
  fStepLimit = new G4UserLimits(Scint_MaxStep);
  logicBarScint->SetUserLimits(fStepLimit);

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void B2aDetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors
  G4String trackerChamberSDname = "B2/TrackerChamberSD";
  B2TrackerSD* aTrackerSD = new B2TrackerSD(trackerChamberSDname,"NW_HitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name 
  // of "Chamber_LV".
  SetSensitiveDetector("logicBarScint", aTrackerSD, true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps;
}  
