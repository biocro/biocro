#include "CanA_3D_Structure.h"
#include "AuxBioCro.h"
  void getIdirIdiff(double *Idir, double *Idiff,double *cosTh,double solarR,double lat,int DOY, int hr)
  {
  lightME(lat,DOY,hr);
  *Idir = tmp1[0] * solarR;
  *Idiff = tmp1[1] * solarR;
  *cosTh = tmp1[2];
  return;
  }