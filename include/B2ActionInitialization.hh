#ifndef B2ActionInitialization_h
#define B2ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class B4DetectorConstruction;

/// Action initialization class.
///

class B2ActionInitialization : public G4VUserActionInitialization
{
  public:
    B2ActionInitialization();
    virtual ~B2ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif

    
