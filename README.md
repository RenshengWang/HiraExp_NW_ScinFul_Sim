# HiraExp_NW_ScinFul_Sim
This is a simulation code for calculating the neutron efficiency/NE213 neutron wall/NSCL.

The Detector Responding Function(DRF) is hard to get for the high energetic neutron when it get through a rectangular
bar shape. Without a accurate DRF, we cannot get a accurate neutron yield, energy spectrum.

In response to this situation, we use a ScinFul-QMD code to produce the DRF. Then put this DRF into Geant4 code. In this case,
the Geant4 is just used as a geometry calculator: when the neutron get through the NW bar, what is the interaction length.
