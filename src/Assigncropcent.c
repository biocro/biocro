#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "crocent.h"

void assignPools(struct cropcentlayer *CROPCENT, double *sompoolsfromR)
{
  //Rprintf("%f, %f, %f \n", *sompoolsfromR,*(sompoolsfromR+1),*(sompoolsfromR+2));
  CROPCENT->strucc1.C.totalC=28.4;
  CROPCENT->strucc1.C.unlablTOlabl=1.0;
  CROPCENT->strucc1.E.CN=200.0;
  CROPCENT->strucc1.E.CP=500.0;
  CROPCENT->strucc1.E.CS=500.0;
  CROPCENT->strucc1.E.CK=500.0;
  CROPCENT->strucc1.lignin=0.2;
  
  CROPCENT->strucc2.C.totalC=175.0;
  CROPCENT->strucc2.C.unlablTOlabl=1.0;
  CROPCENT->strucc2.E.CN=200.0;
  CROPCENT->strucc2.E.CP=500.0;
  CROPCENT->strucc2.E.CS=500.0;
  CROPCENT->strucc2.E.CK=500.0;
  CROPCENT->strucc2.lignin=0.2;
  
  CROPCENT->metabc1.C.totalC=37.5;
  CROPCENT->metabc1.C.unlablTOlabl=1.0;
  CROPCENT->metabc1.E.CN=18.2;
  CROPCENT->metabc1.E.CP=18.2;
  CROPCENT->metabc1.E.CS=18.2;
  CROPCENT->metabc1.E.CK=18.2;
  
  CROPCENT->metabc2.C.totalC=20.8;
  CROPCENT->metabc2.C.unlablTOlabl=1.0;
  CROPCENT->metabc2.E.CN=19.3;
  CROPCENT->metabc2.E.CP=19.3;
  CROPCENT->metabc2.E.CS=19.3;
  CROPCENT->metabc2.E.CK=19.3;
  
  CROPCENT->wood1.C.totalC=30.0;
  CROPCENT->wood1.C.unlablTOlabl=1.0;
  CROPCENT->wood1.E.CN=200.0;
  CROPCENT->wood1.E.CP=500.0;
  CROPCENT->wood1.E.CS=500.0;
  CROPCENT->wood1.E.CK=500.0;
  CROPCENT->wood1.lignin=0.1;
  
  CROPCENT->wood2.C.totalC=30.0;
  CROPCENT->wood2.C.unlablTOlabl=1.0;
  CROPCENT->wood2.E.CN=200.0;
  CROPCENT->wood2.E.CP=500.0;
  CROPCENT->wood2.E.CS=500.0;
  CROPCENT->wood2.E.CK=500.0;
  CROPCENT->wood2.lignin=0.1;
  
  CROPCENT->wood3.C.totalC=30.0;
  CROPCENT->wood3.C.unlablTOlabl=1.0;
  CROPCENT->wood3.E.CN=200.0;
  CROPCENT->wood3.E.CP=500.0;
  CROPCENT->wood3.E.CS=500.0;
  CROPCENT->wood3.E.CK=500.0;
  CROPCENT->wood3.lignin=0.1;
  
  CROPCENT->som1c1.C.totalC=14.6;
  CROPCENT->som1c1.C.unlablTOlabl=1.0;
  CROPCENT->som1c1.E.CN=6.7;
  CROPCENT->som1c1.E.CP=6.7;
  CROPCENT->som1c1.E.CS=6.7;
  CROPCENT->som1c1.E.CK=6.7;
  
  CROPCENT->som1c2.C.totalC=330.0;
  CROPCENT->som1c2.C.unlablTOlabl=1.0;
  CROPCENT->som1c2.E.CN=7.5;
  CROPCENT->som1c2.E.CP=7.5;
  CROPCENT->som1c2.E.CS=7.5;
  CROPCENT->som1c2.E.CK=7.5;
  
  CROPCENT->som2c1.C.totalC=6.4;
  CROPCENT->som2c1.C.unlablTOlabl=1.0;
  CROPCENT->som2c1.E.CN=13.0;
  CROPCENT->som2c1.E.CP=13.0;
  CROPCENT->som2c1.E.CS=13.0;
  CROPCENT->som2c1.E.CK=13.0;

  
  CROPCENT->som2c2.C.totalC=1647.0;
  CROPCENT->som2c2.C.unlablTOlabl=1.0;
  CROPCENT->som2c2.E.CN=15.4;
  CROPCENT->som2c2.E.CP=15.4;
  CROPCENT->som2c2.E.CS=15.4;
  CROPCENT->som2c2.E.CK=15.4;
 
  
  CROPCENT->som3c.C.totalC=3823.0;
  CROPCENT->som3c.C.unlablTOlabl=1.0;
  CROPCENT->som3c.E.CN=8.03;
  CROPCENT->som3c.E.CP=8.03;
  CROPCENT->som3c.E.CS=8.03;
  CROPCENT->som3c.E.CK=8.03;
  return;
}

void assignParms(struct cropcentlayer *CROPCENT, double *somassignparmsfromR)
{//Rprintf("%f, %f, %f \n", *somassignparmsfromR,*(somassignparmsfromR+1),*(somassignparmsfromR+2));
  CROPCENT->strucc1.parms.k=3.9;
  CROPCENT->strucc1.parms.timestep=1440.0;
  CROPCENT->strucc1.parms.strmx=5000.0;
  CROPCENT->strucc1.parms.pligst=3.0;
  CROPCENT->strucc1.parms.bioabsorp=100.0;
  CROPCENT->strucc1.parms.rsplig=0.3;
  CROPCENT->strucc1.parms.ps1co2=0.45;
  CROPCENT->strucc1.parms.pmetabco2=0.48;
  CROPCENT->strucc1.parms.maxphoto=0.0;
  CROPCENT->strucc1.TEff.teff1=15.4;
  CROPCENT->strucc1.TEff.teff2=11.75;
  CROPCENT->strucc1.TEff.teff3=29.7;
  CROPCENT->strucc1.TEff.teff4=0.031;
  CROPCENT->strucc1.SWEFF.a=1.0;
  CROPCENT->strucc1.SWEFF.b=1.0;
  CROPCENT->strucc1.SWEFF.c=30.0;
  CROPCENT->strucc1.SWEFF.d=-9.0;
  CROPCENT->strucc1.PHEFF.a=4.0;
  CROPCENT->strucc1.PHEFF.b=0.5;
  CROPCENT->strucc1.PHEFF.c=1.1;
  CROPCENT->strucc1.PHEFF.d=0.7;
 

  
  
  CROPCENT->strucc2.parms.k=4.9;
  CROPCENT->strucc2.parms.timestep=1440.0;
  CROPCENT->strucc2.parms.strmx=5000.0;
  CROPCENT->strucc2.parms.pligst=3.0;
  CROPCENT->strucc2.parms.rsplig=0.3;
  CROPCENT->strucc2.parms.ps1co2=0.55;
  CROPCENT->strucc2.TEff.teff1=15.4;
  CROPCENT->strucc2.TEff.teff2=11.75;
  CROPCENT->strucc2.TEff.teff3=29.7;
  CROPCENT->strucc2.TEff.teff4=0.031;
  CROPCENT->strucc2.SWEFF.a=1.0;
  CROPCENT->strucc2.SWEFF.b=1.0;
  CROPCENT->strucc2.SWEFF.c=30.0;
  CROPCENT->strucc2.SWEFF.d=9.0;
  CROPCENT->strucc2.PHEFF.a=4.0;
  CROPCENT->strucc2.PHEFF.b=0.5;
  CROPCENT->strucc2.PHEFF.c=1.1;
  CROPCENT->strucc2.PHEFF.d=0.7;
  CROPCENT->strucc2.ANEREFF.ANEREF1=1.5;
  CROPCENT->strucc2.ANEREFF.ANEREF2=3.0;
  CROPCENT->strucc2.ANEREFF.ANEREF3=0.3;
  
  
  CROPCENT->metabc1.parms.k=14.8;
  CROPCENT->metabc1.parms.timestep=1440.0;
  CROPCENT->metabc1.parms.pmco2=0.55;
  CROPCENT->metabc1.parms.a=1.0;
  CROPCENT->metabc1.parms.b=0.2;
  CROPCENT->metabc1.parms.x1=0.0;
  CROPCENT->metabc1.parms.x2=30000.0;
  CROPCENT->metabc1.TEff.teff1=15.4;
  CROPCENT->metabc1.TEff.teff2=11.75;
  CROPCENT->metabc1.TEff.teff3=29.7;
  CROPCENT->metabc1.TEff.teff4=0.031;
  CROPCENT->metabc1.SWEFF.a=1.0;
  CROPCENT->metabc1.SWEFF.b=1.0;
  CROPCENT->metabc1.SWEFF.c=30.0;
  CROPCENT->metabc1.SWEFF.d=-9.0;
  CROPCENT->metabc1.PHEFF.a=4.8;
  CROPCENT->metabc1.PHEFF.b=0.5;
  CROPCENT->metabc1.PHEFF.c=1.14;
  CROPCENT->metabc1.PHEFF.d=0.7;
  
  
  CROPCENT->metabc2.parms.k=18.5;
  CROPCENT->metabc2.parms.timestep=1440.0;
  CROPCENT->metabc2.parms.pmco2=0.55;
  CROPCENT->metabc2.TEff.teff1=15.4;
  CROPCENT->metabc2.TEff.teff2=11.75;
  CROPCENT->metabc2.TEff.teff3=29.7;
  CROPCENT->metabc2.TEff.teff4=0.031;
  CROPCENT->metabc2.SWEFF.a=1.0;
  CROPCENT->metabc2.SWEFF.b=1.0;
  CROPCENT->metabc2.SWEFF.c=30.0;
  CROPCENT->metabc2.SWEFF.d=9.0;
  CROPCENT->metabc2.PHEFF.a=4.8;
  CROPCENT->metabc2.PHEFF.b=0.5;
  CROPCENT->metabc2.PHEFF.c=1.14;
  CROPCENT->metabc2.PHEFF.d=0.7;
  CROPCENT->metabc2.ANEREFF.ANEREF1=1.5;
  CROPCENT->metabc2.ANEREFF.ANEREF2=3.0;
  CROPCENT->metabc2.ANEREFF.ANEREF3=0.3;
  
  CROPCENT->wood1.parms.k=3.9;
  CROPCENT->wood1.parms.timestep=1440.0;
  CROPCENT->wood1.parms.pligst=3.0;
  CROPCENT->wood1.parms.rsplig=0.3;
  CROPCENT->wood1.parms.ps1co2=0.45;
  CROPCENT->wood1.TEff.teff1=15.4;
  CROPCENT->wood1.TEff.teff2=11.75;
  CROPCENT->wood1.TEff.teff3=29.7;
  CROPCENT->wood1.TEff.teff4=0.031;
  CROPCENT->wood1.SWEFF.a=1.0;
  CROPCENT->wood1.SWEFF.b=1.0;
  CROPCENT->wood1.SWEFF.c=30.0;
  CROPCENT->wood1.SWEFF.d=9.0;
  CROPCENT->wood1.PHEFF.a=4.0;
  CROPCENT->wood1.PHEFF.b=0.5;
  CROPCENT->wood1.PHEFF.c=1.1;
  CROPCENT->wood1.PHEFF.d=0.7;
  
  
  CROPCENT->wood2.parms.k=3.9;
  CROPCENT->wood2.parms.timestep=1440.0;
  CROPCENT->wood2.parms.pligst=3.0;
  CROPCENT->wood2.parms.rsplig=0.3;
  CROPCENT->wood2.parms.ps1co2=0.45;
  CROPCENT->wood2.TEff.teff1=15.4;
  CROPCENT->wood2.TEff.teff2=11.75;
  CROPCENT->wood2.TEff.teff3=29.7;
  CROPCENT->wood2.TEff.teff4=0.031;
  CROPCENT->wood2.SWEFF.a=1.0;
  CROPCENT->wood2.SWEFF.b=1.0;
  CROPCENT->wood2.SWEFF.c=30.0;
  CROPCENT->wood2.SWEFF.d=-9.0;
  CROPCENT->wood2.PHEFF.a=4.0;
  CROPCENT->wood2.PHEFF.b=0.5;
  CROPCENT->wood2.PHEFF.c=1.1;
  CROPCENT->wood2.PHEFF.d=0.7;
  
  
  CROPCENT->wood3.parms.k=4.9;
  CROPCENT->wood3.parms.timestep=1440.0;
  CROPCENT->wood3.parms.pligst=3.0;
  CROPCENT->wood3.parms.rsplig=0.3;
  CROPCENT->wood3.parms.ps1co2=0.55;
  CROPCENT->wood3.TEff.teff1=15.4;
  CROPCENT->wood3.TEff.teff2=11.75;
  CROPCENT->wood3.TEff.teff3=29.7;
  CROPCENT->wood3.TEff.teff4=0.031;
  CROPCENT->wood3.SWEFF.a=1.0;
  CROPCENT->wood3.SWEFF.b=1.0;
  CROPCENT->wood3.SWEFF.c=30.0;
  CROPCENT->wood3.SWEFF.d=-9.0;
  CROPCENT->wood3.PHEFF.a=4.0;
  CROPCENT->wood3.PHEFF.b=0.5;
  CROPCENT->wood3.PHEFF.c=1.1;
  CROPCENT->wood3.PHEFF.d=0.7;
  CROPCENT->wood3.ANEREFF.ANEREF1=1.5;
  CROPCENT->wood3.ANEREFF.ANEREF2=3.0;
  CROPCENT->wood3.ANEREFF.ANEREF3=0.3;
  
  
  CROPCENT->som1c1.parms.k=6.0;
  CROPCENT->som1c1.parms.timestep=1440.0;
  CROPCENT->som1c1.parms.p1co2a=0.6;
  CROPCENT->som1c1.parms.p1co2b=0.0;
  CROPCENT->som1c1.parms.a=1.0;
  CROPCENT->som1c1.parms.b=5.0;
  CROPCENT->som1c1.parms.x1=0.0;
  CROPCENT->som1c1.parms.x2=30000.0;
  CROPCENT->som1c1.TEff.teff1=15.4;
  CROPCENT->som1c1.TEff.teff2=11.75;
  CROPCENT->som1c1.TEff.teff3=29.7;
  CROPCENT->som1c1.TEff.teff4=0.031;
  CROPCENT->som1c1.SWEFF.a=1.0;
  CROPCENT->som1c1.SWEFF.b=1.0;
  CROPCENT->som1c1.SWEFF.c=30.0;
  CROPCENT->som1c1.SWEFF.d=-9.0;
  CROPCENT->som1c1.PHEFF.a=4.0;
  CROPCENT->som1c1.PHEFF.b=0.5;
  CROPCENT->som1c1.PHEFF.c=1.1;
  CROPCENT->som1c1.PHEFF.d=0.7;
  CROPCENT->som1c1.INFLOW.maxCN= 15.0;
  CROPCENT->som1c1.INFLOW.minCN=  6.0; 
  CROPCENT->som1c1.INFLOW.minN=  1.0; 
  CROPCENT->som1c1.INFLOW.maxCP=  150.0; 
  CROPCENT->som1c1.INFLOW.minCP=  30.0; 
  CROPCENT->som1c1.INFLOW.minP=  2.0; 
  CROPCENT->som1c1.INFLOW.maxCS=  200.0; 
  CROPCENT->som1c1.INFLOW.minCS=  50.0; 
  CROPCENT->som1c1.INFLOW.minS=  2.0; 
  CROPCENT->som1c1.INFLOW.maxCK=  0.0; 
  CROPCENT->som1c1.INFLOW.minCK=  0.0; 
  CROPCENT->som1c1.INFLOW.minK=  0.0; 
  

  
  CROPCENT->som2c1.parms.k=0.2;
  CROPCENT->som2c1.parms.timestep=1440.0;
  CROPCENT->som2c1.parms.p2co2=0.55;
  CROPCENT->som2c1.parms.a=1.0;
  CROPCENT->som2c1.parms.b=5.0;
  CROPCENT->som2c1.parms.x1=0.0;
  CROPCENT->som2c1.parms.x2=30000.0;
  CROPCENT->som2c1.parms.mix=0.5;
  CROPCENT->som2c1.TEff.teff1=15.4;
  CROPCENT->som2c1.TEff.teff2=11.75;
  CROPCENT->som2c1.TEff.teff3=29.7;
  CROPCENT->som2c1.TEff.teff4=0.031;
  CROPCENT->som2c1.SWEFF.a=1.0;
  CROPCENT->som2c1.SWEFF.b=1.0;
  CROPCENT->som2c1.SWEFF.c=30.0;
  CROPCENT->som2c1.SWEFF.d=-9.0;
  CROPCENT->som2c1.PHEFF.a=4.0;
  CROPCENT->som2c1.PHEFF.b=0.5;
  CROPCENT->som2c1.PHEFF.c=1.1;
  CROPCENT->som2c1.PHEFF.d=0.7;
  CROPCENT->som2c1.INFLOW .maxCN= 20.0;
  CROPCENT->som2c1.INFLOW .minCN=  12.0;
  CROPCENT->som2c1.INFLOW .minN=  1.0;
  CROPCENT->som2c1.INFLOW .maxCP=  300.0;
  CROPCENT->som2c1.INFLOW .minCP=  100.0;
  CROPCENT->som2c1.INFLOW .minP=  2.0;
  CROPCENT->som2c1.INFLOW .maxCS=  400.0; 
  CROPCENT->som2c1.INFLOW .minCS=  100.0; 
  CROPCENT->som2c1.INFLOW .minS=  2.0; 
  CROPCENT->som2c1.INFLOW .maxCK=  0.0; 
  CROPCENT->som2c1.INFLOW .minCK=  0.0; 
  CROPCENT->som2c1.INFLOW .minK=  0.0; 
  
  
  
 
  
  CROPCENT->som1c2.parms.k=7.3;
  CROPCENT->som1c2.parms.timestep=1440.0;
  CROPCENT->som1c2.parms.peftxa=0.25;
  CROPCENT->som1c2.parms.peftxb=0.75;
  CROPCENT->som1c2.parms.p1co2a=0.17;
  CROPCENT->som1c2.parms.p1co2b=0.68;
  CROPCENT->som1c2.parms.ps1s3[0]=0.003;
  CROPCENT->som1c2.parms.ps1s3[1]=0.032;
  CROPCENT->som1c2.parms.animpt=5.0;
  CROPCENT->som1c2.TEff.teff1=15.4;
  CROPCENT->som1c2.TEff.teff2=11.75;
  CROPCENT->som1c2.TEff.teff3=29.7;
  CROPCENT->som1c2.TEff.teff4=0.031;
  CROPCENT->som1c2.SWEFF.a=1.0;
  CROPCENT->som1c2.SWEFF.b=1.0;
  CROPCENT->som1c2.SWEFF.c=30.0;
  CROPCENT->som1c2.SWEFF.d=-9.0;
  CROPCENT->som1c2.PHEFF.a=4.0;
  CROPCENT->som1c2.PHEFF.b=0.5;
  CROPCENT->som1c2.PHEFF.c=1.1;
  CROPCENT->som1c2.PHEFF.d=0.7;
  CROPCENT->som1c2.ANEREFF.ANEREF1=1.5;
  CROPCENT->som1c2.ANEREFF.ANEREF2=3.0;
  CROPCENT->som1c2.ANEREFF.ANEREF3=0.3;
  CROPCENT->som1c2.INFLOW .maxCN= 14.0;
  CROPCENT->som1c2.INFLOW .minCN=  3.0; 
  CROPCENT->som1c2.INFLOW .minN=  2.0; 
  CROPCENT->som1c2.INFLOW .maxCP=  150.0; 
  CROPCENT->som1c2.INFLOW .minCP=  30.0; 
  CROPCENT->som1c2.INFLOW .minP=  2.0; 
  CROPCENT->som1c2.INFLOW .maxCS=  200.0; 
  CROPCENT->som1c2.INFLOW .minCS=  50.0; 
  CROPCENT->som1c2.INFLOW .minS=  2.0; 
  CROPCENT->som1c2.INFLOW .maxCK=  0.0; 
  CROPCENT->som1c2.INFLOW .minCK=  0.0; 
  CROPCENT->som1c2.INFLOW .minK=  0.0; 
  
  
   
  CROPCENT->som2c2.parms.k=0.25;
  CROPCENT->som2c2.parms.timestep=1440.0;
  CROPCENT->som2c2.parms.p2co2=0.55;
  CROPCENT->som2c2.parms.ps2s3[0]=0.003;
  CROPCENT->som2c2.parms.ps2s3[1]=0.009;
  CROPCENT->som2c2.parms.animpt=5.0;
  CROPCENT->som2c2.TEff.teff1=15.4;
  CROPCENT->som2c2.TEff.teff2=11.75;
  CROPCENT->som2c2.TEff.teff3=29.7;
  CROPCENT->som2c2.TEff.teff4=.031;
  CROPCENT->som2c2.SWEFF.a=1.0;
  CROPCENT->som2c2.SWEFF.b=1.0;
  CROPCENT->som2c2.SWEFF.c=30.0;
  CROPCENT->som2c2.SWEFF.d=-9.0;
  CROPCENT->som2c2.PHEFF.a=4.0;
  CROPCENT->som2c2.PHEFF.b=0.5;
  CROPCENT->som2c2.PHEFF.c=1.1;
  CROPCENT->som2c2.PHEFF.d=0.7;
  CROPCENT->som2c2.ANEREFF.ANEREF1=1.5;
  CROPCENT->som2c2.ANEREFF.ANEREF2=3.0;
  CROPCENT->som2c2.ANEREFF.ANEREF3=0.3;
  CROPCENT->som2c2.INFLOW .maxCN= 20.0;
  CROPCENT->som2c2.INFLOW .minCN=  12.0; 
  CROPCENT->som2c2.INFLOW .minN=  2.0; 
  CROPCENT->som2c2.INFLOW .maxCP=  400.0; 
  CROPCENT->som2c2.INFLOW .minCP=  100.0; 
  CROPCENT->som2c2.INFLOW .minP=  2.0; 
  CROPCENT->som2c2.INFLOW .maxCS=  400.0; 
  CROPCENT->som2c2.INFLOW .minCS=  100.0; 
  CROPCENT->som2c2.INFLOW .minS=  2.0; 
  CROPCENT->som2c2.INFLOW .maxCK=  0.0; 
  CROPCENT->som2c2.INFLOW .minCK=  0.0; 
  CROPCENT->som2c2.INFLOW .minK=  0.0; 
  
  
  
  CROPCENT->som3c.parms.k=0.0025;
  CROPCENT->som3c.parms.timestep=1440.0;
  CROPCENT->som3c.parms.p3co2=0.55;
  CROPCENT->som3c.TEff.teff1=15.4;
  CROPCENT->som3c.TEff.teff2=11.75;
  CROPCENT->som3c.TEff.teff3=29.7;
  CROPCENT->som3c.TEff.teff4=0.031;
  CROPCENT->som3c.SWEFF.a=1.0;
  CROPCENT->som3c.SWEFF.b=1.0;
  CROPCENT->som3c.SWEFF.c=30.0;
  CROPCENT->som3c.SWEFF.d=-9.0;
  CROPCENT->som3c.PHEFF.a=3.0;
  CROPCENT->som3c.PHEFF.b=0.5;
  CROPCENT->som3c.PHEFF.c=1.10;
  CROPCENT->som3c.PHEFF.d=0.7;
  CROPCENT->som3c.ANEREFF.ANEREF1=1.5;
  CROPCENT->som3c.ANEREFF.ANEREF2=3.0;
  CROPCENT->som3c.ANEREFF.ANEREF3=0.3;
  CROPCENT->som3c.INFLOW .maxCN= 8.0;
  CROPCENT->som3c.INFLOW .minCN=  6.0; 
  CROPCENT->som3c.INFLOW .minN=  2.0; 
  CROPCENT->som3c.INFLOW .maxCP=  200.0; 
  CROPCENT->som3c.INFLOW .minCP=  50.0; 
  CROPCENT->som3c.INFLOW .minP=  2.0; 
  CROPCENT->som3c.INFLOW .maxCS=  200.0; 
  CROPCENT->som3c.INFLOW .minCS=  50.0; 
  CROPCENT->som3c.INFLOW .minS=  2.0; 
  CROPCENT->som3c.INFLOW .maxCK=  0.0; 
  CROPCENT->som3c.INFLOW .minCK=  0.0; 
  CROPCENT->som3c.INFLOW .minK=  0.0; 
  return;
}

void CROPCENTTimescaling(struct cropcentlayer *CROPCENT){
CROPCENT->strucc1.parms.k= timescaling(CROPCENT->strucc1.parms.k,CROPCENT->strucc1.parms.timestep);
CROPCENT->strucc2.parms.k= timescaling(CROPCENT->strucc2.parms.k,CROPCENT->strucc2.parms.timestep);
CROPCENT->metabc1.parms.k= timescaling(CROPCENT->metabc1.parms.k,CROPCENT->metabc1.parms.timestep);
CROPCENT->metabc2.parms.k= timescaling(CROPCENT->metabc2.parms.k,CROPCENT->metabc2.parms.timestep);
CROPCENT->wood1.parms.k= timescaling(CROPCENT->wood1.parms.k,CROPCENT->wood1.parms.timestep);
CROPCENT->wood2.parms.k= timescaling(CROPCENT->wood2.parms.k,CROPCENT->wood2.parms.timestep);
CROPCENT->wood3.parms.k= timescaling(CROPCENT->wood3.parms.k,CROPCENT->wood3.parms.timestep);
CROPCENT->som1c1.parms.k= timescaling(CROPCENT->som1c1.parms.k,CROPCENT->som1c1.parms.timestep);
CROPCENT->som2c1.parms.k= timescaling(CROPCENT->som2c1.parms.k,CROPCENT->som2c1.parms.timestep);
CROPCENT->som1c2.parms.k= timescaling(CROPCENT->som1c2.parms.k,CROPCENT->som1c2.parms.timestep);
CROPCENT->som2c2.parms.k= timescaling(CROPCENT->som2c2.parms.k,CROPCENT->som2c2.parms.timestep);
CROPCENT->som3c.parms.k= timescaling(CROPCENT->som3c.parms.k,CROPCENT->som3c.parms.timestep);
  return;
}

void assignENV(struct cropcentlayer *CROPCENT,double *getsoiltexturefromR,double *getcropcentstatevarfromR, 
double *getbiocrotocropcentparmsfromR, double *geterosionparmsfromR,double *getc13parmsfromR,double *getleachingparmsfromR,double *getsymbnfixationparmsfromR){
 
  GetBioCroToCropcentParms(&CROPCENT->BcroTOCentParms,getbiocrotocropcentparmsfromR);
  GetSymbNFixationParms(&CROPCENT->ENV.NFIX,getsymbnfixationparmsfromR);
  GetLeachingParms(&CROPCENT->ENV.ORGLECH,getleachingparmsfromR);
  GetC13Parms(&CROPCENT->ENV.C13,getc13parmsfromR);
  GetErosionParms(&CROPCENT->ENV.EROSION,geterosionparmsfromR);
  GetSoilTexture(&CROPCENT->ENV.SOILTEX,getsoiltexturefromR);
  GetCropCentStateVar(&CROPCENT->ENV,getcropcentstatevarfromR);
  return;
}

void GetCropCentStateVar(struct cropcentEnvironment *ENV, double *getcropcentstatevarfromR){
  
  ENV->minN=1e-6;
  ENV->minP=5.0;
  ENV->minS=5.0;
  ENV->minK=5.0;
  ENV->surfaceTEMP=25.0;
  ENV->soilTEMP=25.0;
  ENV->surfaceRELWC=0.3;
  ENV->soilRELWC=0.3;
  ENV->leachedWATER=0.0;
  ENV->PET=0.8;
  ENV->AWC=0.8;
  ENV->soilrad=100.0; // check unit for this variable
  ENV->pH=7.0;
  ENV->drainage=0.5;
  /***************************/
  if(ENV->minN <1e-6) ENV->minN=1e-6;
  /*****************************/
  return;
}


void assignFluxRatios(struct cropcentlayer *CROPCENT)
{
  struct minerals *tmp;
  // Updating unlabl and labl C ratio in each flux component based on  source
  CROPCENT->strucc1.Flux.strucc1TOsom1c1.C.unlablTOlabl = CROPCENT->strucc1.C.unlablTOlabl;
  CROPCENT->strucc1.Flux.strucc1TOsom2c1.C.unlablTOlabl = CROPCENT->strucc1.C.unlablTOlabl;
  CROPCENT->strucc1.Flux.strucc1TOstrucc1.C.unlablTOlabl = CROPCENT->strucc1.C.unlablTOlabl;
  CROPCENT->strucc1.Flux.strucc1TOmetabc1.C.unlablTOlabl = CROPCENT->strucc1.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->strucc1.E,&CROPCENT->strucc1.Flux.strucc1TOstrucc1.E);
  
  CROPCENT->strucc2.Flux.strucc2TOsom1c2.C.unlablTOlabl = CROPCENT->strucc2.C.unlablTOlabl;
  CROPCENT->strucc2.Flux.strucc2TOsom2c2.C.unlablTOlabl = CROPCENT->strucc2.C.unlablTOlabl;
  CROPCENT->strucc2.Flux.strucc2TOstrucc2.C.unlablTOlabl = CROPCENT->strucc2.C.unlablTOlabl;
   replaceMineralStruct(&CROPCENT->strucc2.E,&CROPCENT->strucc2.Flux.strucc2TOstrucc2.E);
   
//    replaceMineralStruct(&CROPCENT->metabc1.E,&CROPCENT->metabc1.Flux.metabc1TOmetabc1.E); this is repeated
//    replaceMineralStruct(&CROPCENT->metabc2.E,&CROPCENT->metabc2.Flux.metabc2TOmetabc2.E); this is repeated
    
  CROPCENT->wood1.Flux.wood1TOsom1c1.C.unlablTOlabl = CROPCENT->wood1.C.unlablTOlabl;
  CROPCENT->wood1.Flux.wood1TOsom2c1.C.unlablTOlabl = CROPCENT->wood1.C.unlablTOlabl;
  CROPCENT->wood1.Flux.wood1TOwood1.C.unlablTOlabl = CROPCENT->wood1.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->wood1.E,&CROPCENT->wood1.Flux.wood1TOwood1.E);
  
  CROPCENT->wood2.Flux.wood2TOsom1c1.C.unlablTOlabl = CROPCENT->wood2.C.unlablTOlabl;
  CROPCENT->wood2.Flux.wood2TOsom2c1.C.unlablTOlabl = CROPCENT->wood2.C.unlablTOlabl;
  CROPCENT->wood2.Flux.wood2TOwood2.C.unlablTOlabl = CROPCENT->wood2.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->wood2.E,&CROPCENT->wood2.Flux.wood2TOwood2.E);

  
  CROPCENT->wood3.Flux.wood3TOsom1c2.C.unlablTOlabl = CROPCENT->wood3.C.unlablTOlabl;
  CROPCENT->wood3.Flux.wood3TOsom2c2.C.unlablTOlabl = CROPCENT->wood3.C.unlablTOlabl;
  CROPCENT->wood3.Flux.wood3TOwood3.C.unlablTOlabl = CROPCENT->wood3.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->wood3.E,&CROPCENT->wood3.Flux.wood3TOwood3.E);
  
  CROPCENT->metabc1.Flux.metabc1TOsom1c1.C.unlablTOlabl = CROPCENT->metabc1.C.unlablTOlabl;
  CROPCENT->metabc1.Flux.metabc1TOmetabc1.C.unlablTOlabl = CROPCENT->metabc1.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->metabc1.E,&CROPCENT->metabc1.Flux.metabc1TOmetabc1.E);
  
  
  CROPCENT->metabc2.Flux.metabc2TOsom1c2.C.unlablTOlabl = CROPCENT->metabc2.C.unlablTOlabl;
  CROPCENT->metabc2.Flux.metabc2TOmetabc2.C.unlablTOlabl = CROPCENT->metabc2.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->metabc2.E,&CROPCENT->metabc2.Flux.metabc2TOmetabc2.E);
  
  CROPCENT->som1c1.Flux.som1c1TOsom2c1.C.unlablTOlabl = CROPCENT->som1c1.C.unlablTOlabl;
  CROPCENT->som1c1.Flux.som1c1TOsom1c1.C.unlablTOlabl = CROPCENT->som1c1.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->som1c1.E,&CROPCENT->som1c1.Flux.som1c1TOsom1c1.E);
  
  CROPCENT->som2c1.Flux.som2c1TOsom1c1.C.unlablTOlabl = CROPCENT->som2c1.C.unlablTOlabl;
  CROPCENT->som2c1.Flux.som2c1TOsom2c2.C.unlablTOlabl = CROPCENT->som2c1.C.unlablTOlabl; // due to mixing
  CROPCENT->som2c1.Flux.som2c1TOsom2c1.C.unlablTOlabl = CROPCENT->som2c1.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->som2c1.E,&CROPCENT->som2c1.Flux.som2c1TOsom2c1.E);
  
  CROPCENT->som1c2.Flux.som1c2TOsom2c2.C.unlablTOlabl = CROPCENT->som1c2.C.unlablTOlabl;
  CROPCENT->som1c2.Flux.som1c2TOsom3c.C.unlablTOlabl = CROPCENT->som1c2.C.unlablTOlabl;
  CROPCENT->som1c2.Flux.som1c2TOleachate.C.unlablTOlabl = CROPCENT->som1c2.C.unlablTOlabl;
  CROPCENT->som1c2.Flux.som1c2TOsom1c2.C.unlablTOlabl = CROPCENT->som1c2.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->som1c2.E,&CROPCENT->som1c2.Flux.som1c2TOsom1c2.E);
   
  CROPCENT->som2c2.Flux.som2c2TOsom1c2.C.unlablTOlabl = CROPCENT->som2c2.C.unlablTOlabl;
  CROPCENT->som2c2.Flux.som2c2TOsom3c.C.unlablTOlabl = CROPCENT->som2c2.C.unlablTOlabl;
  CROPCENT->som2c2.Flux.som2c2TOsom2c2.C.unlablTOlabl = CROPCENT->som2c2.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->som2c2.E,&CROPCENT->som2c2.Flux.som2c2TOsom2c2.E);
  
  CROPCENT->som3c.Flux.som3cTOsom1c2.C.unlablTOlabl = CROPCENT->som3c.C.unlablTOlabl;
  CROPCENT->som3c.Flux.som3cTOsom3c.C.unlablTOlabl = CROPCENT->som3c.C.unlablTOlabl;
  replaceMineralStruct(&CROPCENT->som3c.E,&CROPCENT->som3c.Flux.som3cTOsom3c.E);
  
  // updating CE ratio of each Flux based on Environmental condition and Restriction criteria
  
  // Cflow from strucc1 to metabc1 due to photodegradation
  tmp=&CROPCENT->strucc1.E;
  replaceMineralStruct(tmp,&CROPCENT->strucc1.Flux.strucc1TOmetabc1.E);
  
  
  // Lets focus on all the fluxes to pool som1c1
  GetInFlowCERatio(&CROPCENT->som1c1.INFLOW, tmp, &CROPCENT->ENV);
  replaceMineralStruct(tmp,&CROPCENT->strucc1.Flux.strucc1TOsom1c1.E);
  replaceMineralStruct(tmp,&CROPCENT->wood1.Flux.wood1TOsom1c1.E);
  replaceMineralStruct(tmp,&CROPCENT->wood2.Flux.wood2TOsom1c1.E);
  replaceMineralStruct(tmp,&CROPCENT->metabc1.Flux.metabc1TOsom1c1.E);
  replaceMineralStruct(tmp,&CROPCENT->som2c1.Flux.som2c1TOsom1c1.E);
   
  // Get CE ratio of inflow to som2c1 pool and update all the fluxes E 
  GetInFlowCERatio(&CROPCENT->som2c1.INFLOW, tmp, &CROPCENT->ENV);
  replaceMineralStruct(tmp,&CROPCENT->strucc1.Flux.strucc1TOsom2c1.E);
  replaceMineralStruct(tmp,&CROPCENT->wood1.Flux.wood1TOsom2c1.E);
  replaceMineralStruct(tmp,&CROPCENT->wood2.Flux.wood2TOsom2c1.E);
  replaceMineralStruct(tmp,&CROPCENT->som1c1.Flux.som1c1TOsom2c1.E);
  
  // Get CE ratio of inflow to som1c2 and update all the fluxes
  GetInFlowCERatio(&CROPCENT->som1c2.INFLOW, tmp, &CROPCENT->ENV);
  replaceMineralStruct(tmp,&CROPCENT->strucc2.Flux.strucc2TOsom1c2.E);
  replaceMineralStruct(tmp,&CROPCENT->wood3.Flux.wood3TOsom1c2.E);
  replaceMineralStruct(tmp,&CROPCENT->som2c2.Flux.som2c2TOsom1c2.E);
  replaceMineralStruct(tmp,&CROPCENT->som3c.Flux.som3cTOsom1c2.E);
  replaceMineralStruct(tmp,&CROPCENT->metabc2.Flux.metabc2TOsom1c2.E);
  
  // CE ratio of inflow to som2c2 and update all the fluxes
  GetInFlowCERatio(&CROPCENT->som2c2.INFLOW, tmp, &CROPCENT->ENV);
  replaceMineralStruct(tmp,&CROPCENT->strucc2.Flux.strucc2TOsom2c2.E);
  replaceMineralStruct(tmp,&CROPCENT->wood3.Flux.wood3TOsom2c2.E);
  replaceMineralStruct(tmp,&CROPCENT->som2c1.Flux.som2c1TOsom2c2.E); // due to mixing
  replaceMineralStruct(tmp,&CROPCENT->som1c2.Flux.som1c2TOsom2c2.E); 
  
  // CE ratio of inflow to som3c and update all the fluxes
  GetInFlowCERatio(&CROPCENT->som3c.INFLOW, tmp, &CROPCENT->ENV);
  replaceMineralStruct(tmp,&CROPCENT->som2c2.Flux.som2c2TOsom3c.E); 
  replaceMineralStruct(tmp,&CROPCENT->som1c2.Flux.som1c2TOsom3c.E); 

  return;
}

void GetInFlowCERatio(struct flowrestriction *INFLOWRESTRICT, struct minerals *output, struct cropcentEnvironment *ENV){
  /****************************************
   * Purpose:
   * This function calculates CE ratio of the flow structure flow nased on cropcent environment and restriction critera
   * Input
   * cropcent Environment ENV
   * flow restriction criteria input
   * OUTPUT
   * flow structure output
   * **************************************/
   double temp;

        if(ENV->minN <= 0.0) // when mineral N is too low in the crocent environment
        {
          output->CN=INFLOWRESTRICT->maxCN;
        }
        if((ENV->minN > 0.0)&&(ENV->minN < INFLOWRESTRICT->minN)) // when mineral N is sufficient but lower than required to obtain MINIMUM CN ratio of flow
        {
          temp=(1.0-ENV->minN)/INFLOWRESTRICT->minN;
          output->CN=temp*(INFLOWRESTRICT->maxCN-INFLOWRESTRICT->minCN)+INFLOWRESTRICT->minCN;
        }
        if(ENV->minN >= INFLOWRESTRICT->minN)// when mineral N of crocent environment is greater than what is required for min CN ratio of INFLOWRESTRICT
        {
          output->CN=INFLOWRESTRICT->minCN;
        }
  return;
}
void replaceMineralStruct(struct minerals *FROM,struct minerals *TO)
{
  TO->CN=FROM->CN;
  TO->CP=FROM->CP;
  TO->CS=FROM->CS;
  TO->CK=FROM->CK;
  return;
}


void GetSymbNFixationParms(struct SymbNFixationParms *temp,double *getsymbnfixationparmsfromR)
{//Rprintf("%f, %f, %f \n", *getsymbnfixationparmsfromR,*(getsymbnfixationparmsfromR+1),*(getsymbnfixationparmsfromR+2));
  temp->FXMCA=-0.125;
  temp->FXMCB=0.005;
  temp->FXMXS=0.35;
  temp->FXNPB=7.0;
  temp->NSNFIX=0;
  temp->NTSPM=4.0;
  return;
}

void GetLeachingParms (struct OrgLeachParms *temp,double *getleachingparmsfromR)
{
  temp->DailyThresholdFlow=1.5;
  temp->OMLEACH[0]=0.03;
  temp->OMLEACH[1]=0.12;
  temp->OMLEACH[2]=1.90; // is this same as threshold value of daily flow?
  temp->som1c2toleach.CN=2.0;
  temp->som1c2toleach.CP=35.0;
  temp->som1c2toleach.CS=2.0;
  temp->som1c2toleach.CK=2.0;// This need to be verified
  return;
}

void GetC13Parms(struct C13Parms *temp,double *getc13parmsfromR)
{ 
  temp->DRESP=0.999; 
  temp->DLIGDF=-4.0;
  return;
}

void GetErosionParms(struct ErosionParms *temp,double *geterosionparmsfromR){
  
  temp->LHZF[0]=0.2; 
  temp->LHZF[1]=0.4;
  temp->LHZF[2]=0.8;
  temp->EDEPTH=0.2; 
  temp->ENRICH=2.0;
  return;
}

void GetSoilTexture( struct SoilTexture *temp, double *getsoiltexturefromR){
/**************************************************************************
//  I am chaning it back to constant value to avoid error now as ther eis not pointer passed from R for now
  temp->sand=*getsoiltexturefromR;
  temp->silt=*(getsoiltexturefromR+1);
  temp->clay=*(getsoiltexturefromR+2);
  *************************************************************************/
  temp->sand=0.33;
  temp->silt=0.33;
  temp->clay=0.34;
  return;
}

void GetBioCroToCropcentParms(struct BioCroToCropcentParms  *temp,double *getbiocrotocropcentparmsfromR ){
  
  temp->structometaSLOPE=-0.013;
  temp->structometaINTERCEP=0.85;
  temp->surfacedamrN=0.0;
  temp->surfacedamrP=0.0;
  temp->surfacedamrS=0.01;
  temp->surfacedamrK=0.0;
  temp->soildamrN=0.02;
  temp->soildamrP=0.02;
  temp->soildamrS=0.04;
  temp->soildamrK=0.0;
  temp->mindamrN=15.0;
  temp->mindamrP=150.0;
  temp->mindamrS=150.0;
  temp->mindamrK=0.0;
  temp->pabres=100;
  return;
}