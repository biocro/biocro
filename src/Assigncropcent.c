#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "crocent.h"

void assignPools(struct cropcentlayer *CROPCENT, double *sompoolsfromR)
{
  //Rprintf("%f, %f, %f \n", *sompoolsfromR,*(sompoolsfromR+1),*(sompoolsfromR+2));
  CROPCENT->strucc1.C.totalC=*sompoolsfromR;
  CROPCENT->strucc1.C.unlablTOlabl=*(sompoolsfromR+1);
  CROPCENT->strucc1.E.CN=*(sompoolsfromR+2);
  CROPCENT->strucc1.E.CP=*(sompoolsfromR+3);
  CROPCENT->strucc1.E.CS=*(sompoolsfromR+4);
  CROPCENT->strucc1.E.CK=*(sompoolsfromR+5);
  CROPCENT->strucc1.lignin=*(sompoolsfromR+6);
  
  CROPCENT->strucc2.C.totalC=*(sompoolsfromR+7);
  CROPCENT->strucc2.C.unlablTOlabl=*(sompoolsfromR+8);
  CROPCENT->strucc2.E.CN=*(sompoolsfromR+9);
  CROPCENT->strucc2.E.CP=*(sompoolsfromR+10);
  CROPCENT->strucc2.E.CS=*(sompoolsfromR+11);
  CROPCENT->strucc2.E.CK=*(sompoolsfromR+12);
  CROPCENT->strucc2.lignin=*(sompoolsfromR+13);
  
  CROPCENT->metabc1.C.totalC=*(sompoolsfromR+14);
  CROPCENT->metabc1.C.unlablTOlabl=*(sompoolsfromR+15);
  CROPCENT->metabc1.E.CN=*(sompoolsfromR+16);
  CROPCENT->metabc1.E.CP=*(sompoolsfromR+17);
  CROPCENT->metabc1.E.CS=*(sompoolsfromR+18);
  CROPCENT->metabc1.E.CK=*(sompoolsfromR+19);
  
  CROPCENT->metabc2.C.totalC=*(sompoolsfromR+20);
  CROPCENT->metabc2.C.unlablTOlabl=*(sompoolsfromR+21);
  CROPCENT->metabc2.E.CN=*(sompoolsfromR+22);
  CROPCENT->metabc2.E.CP=*(sompoolsfromR+23);
  CROPCENT->metabc2.E.CS=*(sompoolsfromR+24);
  CROPCENT->metabc2.E.CK=*(sompoolsfromR+25);
  
  CROPCENT->wood1.C.totalC=*(sompoolsfromR+26);
  CROPCENT->wood1.C.unlablTOlabl=*(sompoolsfromR+27);
  CROPCENT->wood1.E.CN=*(sompoolsfromR+28);
  CROPCENT->wood1.E.CP=*(sompoolsfromR+29);
  CROPCENT->wood1.E.CS=*(sompoolsfromR+30);
  CROPCENT->wood1.E.CK=*(sompoolsfromR+31);
  CROPCENT->wood1.lignin=*(sompoolsfromR+32);
  
  CROPCENT->wood2.C.totalC=*(sompoolsfromR+33);
  CROPCENT->wood2.C.unlablTOlabl=*(sompoolsfromR+34);
  CROPCENT->wood2.E.CN=*(sompoolsfromR+35);
  CROPCENT->wood2.E.CP=*(sompoolsfromR+36);
  CROPCENT->wood2.E.CS=*(sompoolsfromR+47);
  CROPCENT->wood2.E.CK=*(sompoolsfromR+48);
  CROPCENT->wood2.lignin=*(sompoolsfromR+49);
  
  CROPCENT->wood3.C.totalC=*(sompoolsfromR+50);
  CROPCENT->wood3.C.unlablTOlabl=*(sompoolsfromR+51);
  CROPCENT->wood3.E.CN=*(sompoolsfromR+52);
  CROPCENT->wood3.E.CP=*(sompoolsfromR+53);
  CROPCENT->wood3.E.CS=*(sompoolsfromR+54);
  CROPCENT->wood3.E.CK=*(sompoolsfromR+55);
  CROPCENT->wood3.lignin=*(sompoolsfromR+56);
  
  CROPCENT->som1c1.C.totalC=*(sompoolsfromR+57);
  CROPCENT->som1c1.C.unlablTOlabl=*(sompoolsfromR+58);
  CROPCENT->som1c1.E.CN=*(sompoolsfromR+59);
  CROPCENT->som1c1.E.CP=*(sompoolsfromR+60);
  CROPCENT->som1c1.E.CS=*(sompoolsfromR+61);
  CROPCENT->som1c1.E.CK=*(sompoolsfromR+62);
  
  CROPCENT->som1c2.C.totalC=*(sompoolsfromR+63);
  CROPCENT->som1c2.C.unlablTOlabl=*(sompoolsfromR+64);
  CROPCENT->som1c2.E.CN=*(sompoolsfromR+65);
  CROPCENT->som1c2.E.CP=*(sompoolsfromR+66);
  CROPCENT->som1c2.E.CS=*(sompoolsfromR+67);
  CROPCENT->som1c2.E.CK=*(sompoolsfromR+68);
  
  CROPCENT->som2c1.C.totalC=*(sompoolsfromR+69);
  CROPCENT->som2c1.C.unlablTOlabl=*(sompoolsfromR+70);
  CROPCENT->som2c1.E.CN=*(sompoolsfromR+71);
  CROPCENT->som2c1.E.CP=*(sompoolsfromR+72);
  CROPCENT->som2c1.E.CS=*(sompoolsfromR+73);
  CROPCENT->som2c1.E.CK=*(sompoolsfromR+74);

  
  CROPCENT->som2c2.C.totalC=*(sompoolsfromR+75);
  CROPCENT->som2c2.C.unlablTOlabl=*(sompoolsfromR+76);
  CROPCENT->som2c2.E.CN=*(sompoolsfromR+77);
  CROPCENT->som2c2.E.CP=*(sompoolsfromR+78);
  CROPCENT->som2c2.E.CS=*(sompoolsfromR+79);
  CROPCENT->som2c2.E.CK=*(sompoolsfromR+80);
 
  
  CROPCENT->som3c.C.totalC=*(sompoolsfromR+81);
  CROPCENT->som3c.C.unlablTOlabl=*(sompoolsfromR+82);
  CROPCENT->som3c.E.CN=*(sompoolsfromR+83);
  CROPCENT->som3c.E.CP=*(sompoolsfromR+84);
  CROPCENT->som3c.E.CS=*(sompoolsfromR+85);
  CROPCENT->som3c.E.CK=*(sompoolsfromR+86);
  return;
}

void assignParms(struct cropcentlayer *CROPCENT, double *somassignparmsfromR)
{//Rprintf("%f, %f, %f \n", *somassignparmsfromR,*(somassignparmsfromR+1),*(somassignparmsfromR+2));
  CROPCENT->strucc1.parms.k=*somassignparmsfromR;
  CROPCENT->strucc1.parms.timestep=*(somassignparmsfromR+1);
  CROPCENT->strucc1.parms.strmx=*(somassignparmsfromR+2);
  CROPCENT->strucc1.parms.pligst=*(somassignparmsfromR+3);
  CROPCENT->strucc1.parms.bioabsorp=*(somassignparmsfromR+4);
  CROPCENT->strucc1.parms.rsplig=*(somassignparmsfromR+5);
  CROPCENT->strucc1.parms.ps1co2=*(somassignparmsfromR+6);
  CROPCENT->strucc1.parms.pmetabco2=*(somassignparmsfromR+7);
  CROPCENT->strucc1.parms.maxphoto=*(somassignparmsfromR+8);
  CROPCENT->strucc1.TEff.teff1=*(somassignparmsfromR+9);
  CROPCENT->strucc1.TEff.teff2=*(somassignparmsfromR+10);
  CROPCENT->strucc1.TEff.teff3=*(somassignparmsfromR+11);
  CROPCENT->strucc1.TEff.teff4=*(somassignparmsfromR+12);
  CROPCENT->strucc1.SWEFF.a=*(somassignparmsfromR+13);
  CROPCENT->strucc1.SWEFF.b=*(somassignparmsfromR+14);
  CROPCENT->strucc1.SWEFF.c=*(somassignparmsfromR+15);
  CROPCENT->strucc1.SWEFF.d=*(somassignparmsfromR+16);
  CROPCENT->strucc1.PHEFF.a=*(somassignparmsfromR+17);
  CROPCENT->strucc1.PHEFF.b=*(somassignparmsfromR+18);
  CROPCENT->strucc1.PHEFF.c=*(somassignparmsfromR+19);
  CROPCENT->strucc1.PHEFF.d=*(somassignparmsfromR+20);
 

  
  
  CROPCENT->strucc2.parms.k=*(somassignparmsfromR+21);
  CROPCENT->strucc2.parms.timestep=*(somassignparmsfromR+22);
  CROPCENT->strucc2.parms.strmx=*(somassignparmsfromR+23);
  CROPCENT->strucc2.parms.pligst=*(somassignparmsfromR+24);
  CROPCENT->strucc2.parms.rsplig=*(somassignparmsfromR+25);
  CROPCENT->strucc2.parms.ps1co2=*(somassignparmsfromR+26);
  CROPCENT->strucc2.TEff.teff1=*(somassignparmsfromR+27);
  CROPCENT->strucc2.TEff.teff2=*(somassignparmsfromR+28);
  CROPCENT->strucc2.TEff.teff3=*(somassignparmsfromR+29);
  CROPCENT->strucc2.TEff.teff4=*(somassignparmsfromR+30);
  CROPCENT->strucc2.SWEFF.a=*(somassignparmsfromR+31);
  CROPCENT->strucc2.SWEFF.b=*(somassignparmsfromR+32);
  CROPCENT->strucc2.SWEFF.c=*(somassignparmsfromR+33);
  CROPCENT->strucc2.SWEFF.d=*(somassignparmsfromR+34);
  CROPCENT->strucc2.PHEFF.a=*(somassignparmsfromR+35);
  CROPCENT->strucc2.PHEFF.b=*(somassignparmsfromR+36);
  CROPCENT->strucc2.PHEFF.c=*(somassignparmsfromR+37);
  CROPCENT->strucc2.PHEFF.d=*(somassignparmsfromR+38);
  CROPCENT->strucc2.ANEREFF.ANEREF1=*(somassignparmsfromR+39);
  CROPCENT->strucc2.ANEREFF.ANEREF2=*(somassignparmsfromR+40);
  CROPCENT->strucc2.ANEREFF.ANEREF3=*(somassignparmsfromR+41);
  
  
  CROPCENT->metabc1.parms.k=*(somassignparmsfromR+42);
  CROPCENT->metabc1.parms.timestep=*(somassignparmsfromR+43);
  CROPCENT->metabc1.parms.pmco2=*(somassignparmsfromR+44);
  CROPCENT->metabc1.parms.a=*(somassignparmsfromR+45);
  CROPCENT->metabc1.parms.b=*(somassignparmsfromR+46);
  CROPCENT->metabc1.parms.x1=*(somassignparmsfromR+47);
  CROPCENT->metabc1.parms.x2=*(somassignparmsfromR+48);
  CROPCENT->metabc1.TEff.teff1=*(somassignparmsfromR+49);
  CROPCENT->metabc1.TEff.teff2=*(somassignparmsfromR+50);
  CROPCENT->metabc1.TEff.teff3=*(somassignparmsfromR+51);
  CROPCENT->metabc1.TEff.teff4=*(somassignparmsfromR+52);
  CROPCENT->metabc1.SWEFF.a=*(somassignparmsfromR+53);
  CROPCENT->metabc1.SWEFF.b=*(somassignparmsfromR+54);
  CROPCENT->metabc1.SWEFF.c=*(somassignparmsfromR+55);
  CROPCENT->metabc1.SWEFF.d=*(somassignparmsfromR+56);
  CROPCENT->metabc1.PHEFF.a=*(somassignparmsfromR+57);
  CROPCENT->metabc1.PHEFF.b=*(somassignparmsfromR+58);
  CROPCENT->metabc1.PHEFF.c=*(somassignparmsfromR+59);
  CROPCENT->metabc1.PHEFF.d=*(somassignparmsfromR+60);

  
  
  CROPCENT->metabc2.parms.k=*(somassignparmsfromR+61);
  CROPCENT->metabc2.parms.timestep=*(somassignparmsfromR+62);
  CROPCENT->metabc2.parms.pmco2=*(somassignparmsfromR+63);
  CROPCENT->metabc2.TEff.teff1=*(somassignparmsfromR+64);
  CROPCENT->metabc2.TEff.teff2=*(somassignparmsfromR+65);
  CROPCENT->metabc2.TEff.teff3=*(somassignparmsfromR+66);
  CROPCENT->metabc2.TEff.teff4=*(somassignparmsfromR+67);
  CROPCENT->metabc2.SWEFF.a=*(somassignparmsfromR+68);
  CROPCENT->metabc2.SWEFF.b=*(somassignparmsfromR+69);
  CROPCENT->metabc2.SWEFF.c=*(somassignparmsfromR+70);
  CROPCENT->metabc2.SWEFF.d=*(somassignparmsfromR+71);
  CROPCENT->metabc2.PHEFF.a=*(somassignparmsfromR+72);
  CROPCENT->metabc2.PHEFF.b=*(somassignparmsfromR+73);
  CROPCENT->metabc2.PHEFF.c=*(somassignparmsfromR+74);
  CROPCENT->metabc2.PHEFF.d=*(somassignparmsfromR+75);
  CROPCENT->metabc2.ANEREFF.ANEREF1=*(somassignparmsfromR+76);
  CROPCENT->metabc2.ANEREFF.ANEREF2=*(somassignparmsfromR+77);
  CROPCENT->metabc2.ANEREFF.ANEREF3=*(somassignparmsfromR+78);
  
  CROPCENT->wood1.parms.k=*(somassignparmsfromR+79);
  CROPCENT->wood1.parms.timestep=*(somassignparmsfromR+80);
  CROPCENT->wood1.parms.pligst=*(somassignparmsfromR+81);
  CROPCENT->wood1.parms.rsplig=*(somassignparmsfromR+82);
  CROPCENT->wood1.parms.ps1co2=*(somassignparmsfromR+83);
  CROPCENT->wood1.TEff.teff1=*(somassignparmsfromR+84);
  CROPCENT->wood1.TEff.teff2=*(somassignparmsfromR+85);
  CROPCENT->wood1.TEff.teff3=*(somassignparmsfromR+86);
  CROPCENT->wood1.TEff.teff4=*(somassignparmsfromR+87);
  CROPCENT->wood1.SWEFF.a=*(somassignparmsfromR+88);
  CROPCENT->wood1.SWEFF.b=*(somassignparmsfromR+89);
  CROPCENT->wood1.SWEFF.c=*(somassignparmsfromR+90);
  CROPCENT->wood1.SWEFF.d=*(somassignparmsfromR+91);
  CROPCENT->wood1.PHEFF.a=*(somassignparmsfromR+92);
  CROPCENT->wood1.PHEFF.b=*(somassignparmsfromR+93);
  CROPCENT->wood1.PHEFF.c=*(somassignparmsfromR+94);
  CROPCENT->wood1.PHEFF.d=*(somassignparmsfromR+95);
  
  
  CROPCENT->wood2.parms.k=*(somassignparmsfromR+96);
  CROPCENT->wood2.parms.timestep=*(somassignparmsfromR+97);
  CROPCENT->wood2.parms.pligst=*(somassignparmsfromR+98);
  CROPCENT->wood2.parms.rsplig=*(somassignparmsfromR+99);
  CROPCENT->wood2.parms.ps1co2=*(somassignparmsfromR+100);
  CROPCENT->wood2.TEff.teff1=*(somassignparmsfromR+101);
  CROPCENT->wood2.TEff.teff2=*(somassignparmsfromR+102);
  CROPCENT->wood2.TEff.teff3=*(somassignparmsfromR+103);
  CROPCENT->wood2.TEff.teff4=*(somassignparmsfromR+104);
  CROPCENT->wood2.SWEFF.a=*(somassignparmsfromR+105);
  CROPCENT->wood2.SWEFF.b=*(somassignparmsfromR+106);
  CROPCENT->wood2.SWEFF.c=*(somassignparmsfromR+107);
  CROPCENT->wood2.SWEFF.d=*(somassignparmsfromR+108);
  CROPCENT->wood2.PHEFF.a=*(somassignparmsfromR+109);
  CROPCENT->wood2.PHEFF.b=*(somassignparmsfromR+110);
  CROPCENT->wood2.PHEFF.c=*(somassignparmsfromR+111);
  CROPCENT->wood2.PHEFF.d=*(somassignparmsfromR+112);
  
  
  CROPCENT->wood3.parms.k=*(somassignparmsfromR+1113);
  CROPCENT->wood3.parms.timestep=*(somassignparmsfromR+114);
  CROPCENT->wood3.parms.pligst=*(somassignparmsfromR+115);
  CROPCENT->wood3.parms.rsplig=*(somassignparmsfromR+116);
  CROPCENT->wood3.parms.ps1co2=*(somassignparmsfromR+117);
  CROPCENT->wood3.TEff.teff1=*(somassignparmsfromR+118);
  CROPCENT->wood3.TEff.teff2=*(somassignparmsfromR+119);
  CROPCENT->wood3.TEff.teff3=*(somassignparmsfromR+120);
  CROPCENT->wood3.TEff.teff4=*(somassignparmsfromR+121);
  CROPCENT->wood3.SWEFF.a=*(somassignparmsfromR+122);
  CROPCENT->wood3.SWEFF.b=*(somassignparmsfromR+123);
  CROPCENT->wood3.SWEFF.c=*(somassignparmsfromR+124);
  CROPCENT->wood3.SWEFF.d=*(somassignparmsfromR+125);
  CROPCENT->wood3.PHEFF.a=*(somassignparmsfromR+126);
  CROPCENT->wood3.PHEFF.b=*(somassignparmsfromR+127);
  CROPCENT->wood3.PHEFF.c=*(somassignparmsfromR+128);
  CROPCENT->wood3.PHEFF.d=*(somassignparmsfromR+129);
  CROPCENT->wood3.ANEREFF.ANEREF1=*(somassignparmsfromR+130);
  CROPCENT->wood3.ANEREFF.ANEREF2=*(somassignparmsfromR+131);
  CROPCENT->wood3.ANEREFF.ANEREF3=*(somassignparmsfromR+132);
  
  
  CROPCENT->som1c1.parms.k=*(somassignparmsfromR+133);
  CROPCENT->som1c1.parms.timestep=*(somassignparmsfromR+134);
  CROPCENT->som1c1.parms.p1co2a=*(somassignparmsfromR+135);
  CROPCENT->som1c1.parms.p1co2b=*(somassignparmsfromR+136);
  CROPCENT->som1c1.parms.a=*(somassignparmsfromR+137);
  CROPCENT->som1c1.parms.b=*(somassignparmsfromR+138);
  CROPCENT->som1c1.parms.x1=*(somassignparmsfromR+139);
  CROPCENT->som1c1.parms.x2=*(somassignparmsfromR+140);
  CROPCENT->som1c1.TEff.teff1=*(somassignparmsfromR+141);
  CROPCENT->som1c1.TEff.teff2=*(somassignparmsfromR+142);
  CROPCENT->som1c1.TEff.teff3=*(somassignparmsfromR+143);
  CROPCENT->som1c1.TEff.teff4=*(somassignparmsfromR+144);
  CROPCENT->som1c1.SWEFF.a=*(somassignparmsfromR+145);
  CROPCENT->som1c1.SWEFF.b=*(somassignparmsfromR+146);
  CROPCENT->som1c1.SWEFF.c=*(somassignparmsfromR+147);
  CROPCENT->som1c1.SWEFF.d=*(somassignparmsfromR+148);
  CROPCENT->som1c1.PHEFF.a=*(somassignparmsfromR+149);
  CROPCENT->som1c1.PHEFF.b=*(somassignparmsfromR+150);
  CROPCENT->som1c1.PHEFF.c=*(somassignparmsfromR+151);
  CROPCENT->som1c1.PHEFF.d=*(somassignparmsfromR+152);
  CROPCENT->som1c1.INFLOW.maxCN= *(somassignparmsfromR+153);
  CROPCENT->som1c1.INFLOW.minCN= *(somassignparmsfromR+154); 
  CROPCENT->som1c1.INFLOW.minN= *(somassignparmsfromR+155); 
  CROPCENT->som1c1.INFLOW.maxCP= *(somassignparmsfromR+156); 
  CROPCENT->som1c1.INFLOW.minCP=  *(somassignparmsfromR+157); 
  CROPCENT->som1c1.INFLOW.minP= *(somassignparmsfromR+158); 
  CROPCENT->som1c1.INFLOW.maxCS=  *(somassignparmsfromR+159); 
  CROPCENT->som1c1.INFLOW.minCS= *(somassignparmsfromR+160); 
  CROPCENT->som1c1.INFLOW.minS= *(somassignparmsfromR+161); 
  CROPCENT->som1c1.INFLOW.maxCK= *(somassignparmsfromR+162); 
  CROPCENT->som1c1.INFLOW.minCK= *(somassignparmsfromR+163); 
  CROPCENT->som1c1.INFLOW.minK= *(somassignparmsfromR+164); 
  

  
  CROPCENT->som2c1.parms.k=*(somassignparmsfromR+165);
  CROPCENT->som2c1.parms.timestep=*(somassignparmsfromR+166);
  CROPCENT->som2c1.parms.p2co2=*(somassignparmsfromR+167);
  CROPCENT->som2c1.parms.a=*(somassignparmsfromR+168);
  CROPCENT->som2c1.parms.b=*(somassignparmsfromR+169);
  CROPCENT->som2c1.parms.x1=*(somassignparmsfromR+170);
  CROPCENT->som2c1.parms.x2=*(somassignparmsfromR+171);
  CROPCENT->som2c1.parms.mix=*(somassignparmsfromR+172);
  CROPCENT->som2c1.TEff.teff1=*(somassignparmsfromR+173);
  CROPCENT->som2c1.TEff.teff2=*(somassignparmsfromR+174);
  CROPCENT->som2c1.TEff.teff3=*(somassignparmsfromR+175);
  CROPCENT->som2c1.TEff.teff4=*(somassignparmsfromR+176);
  CROPCENT->som2c1.SWEFF.a=*(somassignparmsfromR+177);
  CROPCENT->som2c1.SWEFF.b=*(somassignparmsfromR+178);
  CROPCENT->som2c1.SWEFF.c=*(somassignparmsfromR+179);
  CROPCENT->som2c1.SWEFF.d=*(somassignparmsfromR+180);
  CROPCENT->som2c1.PHEFF.a=*(somassignparmsfromR+181);
  CROPCENT->som2c1.PHEFF.b=*(somassignparmsfromR+182);
  CROPCENT->som2c1.PHEFF.c=*(somassignparmsfromR+183);
  CROPCENT->som2c1.PHEFF.d=*(somassignparmsfromR+184);
  CROPCENT->som2c1.INFLOW.maxCN= *(somassignparmsfromR+185);
  CROPCENT->som2c1.INFLOW.minCN= *(somassignparmsfromR+186);
  CROPCENT->som2c1.INFLOW.minN= *(somassignparmsfromR+187);
  CROPCENT->som2c1.INFLOW.maxCP= *(somassignparmsfromR+188);
  CROPCENT->som2c1.INFLOW.minCP= *(somassignparmsfromR+189);
  CROPCENT->som2c1.INFLOW.minP= *(somassignparmsfromR+190);
  CROPCENT->som2c1.INFLOW.maxCS= *(somassignparmsfromR+191); 
  CROPCENT->som2c1.INFLOW.minCS= *(somassignparmsfromR+192); 
  CROPCENT->som2c1.INFLOW.minS= *(somassignparmsfromR+193); 
  CROPCENT->som2c1.INFLOW.maxCK= *(somassignparmsfromR+194); 
  CROPCENT->som2c1.INFLOW.minCK=*(somassignparmsfromR+195); 
  CROPCENT->som2c1.INFLOW.minK= *(somassignparmsfromR+196); 
  
  
  
 
  
  CROPCENT->som1c2.parms.k=*(somassignparmsfromR+197);
  CROPCENT->som1c2.parms.timestep=*(somassignparmsfromR+198);
  CROPCENT->som1c2.parms.peftxa=*(somassignparmsfromR+199);
  CROPCENT->som1c2.parms.peftxb=*(somassignparmsfromR+200);
  CROPCENT->som1c2.parms.p1co2a=*(somassignparmsfromR+201);
  CROPCENT->som1c2.parms.p1co2b=*(somassignparmsfromR+202);
  CROPCENT->som1c2.parms.ps1s3[0]=*(somassignparmsfromR+203);
  CROPCENT->som1c2.parms.ps1s3[1]=*(somassignparmsfromR+204);
  CROPCENT->som1c2.parms.animpt=*(somassignparmsfromR+205);
  CROPCENT->som1c2.TEff.teff1=*(somassignparmsfromR+206);
  CROPCENT->som1c2.TEff.teff2=*(somassignparmsfromR+207);
  CROPCENT->som1c2.TEff.teff3=*(somassignparmsfromR+208);
  CROPCENT->som1c2.TEff.teff4=*(somassignparmsfromR+209);
  CROPCENT->som1c2.SWEFF.a=*(somassignparmsfromR+210);
  CROPCENT->som1c2.SWEFF.b=*(somassignparmsfromR+211);
  CROPCENT->som1c2.SWEFF.c=*(somassignparmsfromR+212);
  CROPCENT->som1c2.SWEFF.d=*(somassignparmsfromR+213);
  CROPCENT->som1c2.PHEFF.a=*(somassignparmsfromR+214);
  CROPCENT->som1c2.PHEFF.b=*(somassignparmsfromR+215);
  CROPCENT->som1c2.PHEFF.c=*(somassignparmsfromR+216);
  CROPCENT->som1c2.PHEFF.d=*(somassignparmsfromR+217);
  CROPCENT->som1c2.ANEREFF.ANEREF1=*(somassignparmsfromR+218);
  CROPCENT->som1c2.ANEREFF.ANEREF2=*(somassignparmsfromR+219);
  CROPCENT->som1c2.ANEREFF.ANEREF3=*(somassignparmsfromR+220);
  CROPCENT->som1c2.INFLOW.maxCN= *(somassignparmsfromR+221);
  CROPCENT->som1c2.INFLOW.minCN= *(somassignparmsfromR+222); 
  CROPCENT->som1c2.INFLOW.minN= *(somassignparmsfromR+223); 
  CROPCENT->som1c2.INFLOW.maxCP= *(somassignparmsfromR+224); 
  CROPCENT->som1c2.INFLOW.minCP= *(somassignparmsfromR+225); 
  CROPCENT->som1c2.INFLOW.minP= *(somassignparmsfromR+226); 
  CROPCENT->som1c2.INFLOW.maxCS= *(somassignparmsfromR+227); 
  CROPCENT->som1c2.INFLOW.minCS= *(somassignparmsfromR+228); 
  CROPCENT->som1c2.INFLOW.minS= *(somassignparmsfromR+229); 
  CROPCENT->som1c2.INFLOW.maxCK= *(somassignparmsfromR+230); 
  CROPCENT->som1c2.INFLOW.minCK= *(somassignparmsfromR+231); 
  CROPCENT->som1c2.INFLOW.minK=  *(somassignparmsfromR+232); 
  
  
   
  CROPCENT->som2c2.parms.k=*(somassignparmsfromR+233);
  CROPCENT->som2c2.parms.timestep=*(somassignparmsfromR+234);
  CROPCENT->som2c2.parms.p2co2=*(somassignparmsfromR+235);
  CROPCENT->som2c2.parms.ps2s3[0]=*(somassignparmsfromR+236);
  CROPCENT->som2c2.parms.ps2s3[1]=*(somassignparmsfromR+237);
  CROPCENT->som2c2.parms.animpt=*(somassignparmsfromR+238);
  CROPCENT->som2c2.TEff.teff1=*(somassignparmsfromR+239);
  CROPCENT->som2c2.TEff.teff2=*(somassignparmsfromR+240);
  CROPCENT->som2c2.TEff.teff3=*(somassignparmsfromR+241);
  CROPCENT->som2c2.TEff.teff4=*(somassignparmsfromR+242);
  CROPCENT->som2c2.SWEFF.a=*(somassignparmsfromR+243);
  CROPCENT->som2c2.SWEFF.b=*(somassignparmsfromR+244);
  CROPCENT->som2c2.SWEFF.c=*(somassignparmsfromR+245);
  CROPCENT->som2c2.SWEFF.d=*(somassignparmsfromR+246);
  CROPCENT->som2c2.PHEFF.a=*(somassignparmsfromR+247);
  CROPCENT->som2c2.PHEFF.b=*(somassignparmsfromR+248);
  CROPCENT->som2c2.PHEFF.c=*(somassignparmsfromR+249);
  CROPCENT->som2c2.PHEFF.d=*(somassignparmsfromR+250);
  CROPCENT->som2c2.ANEREFF.ANEREF1=*(somassignparmsfromR+251);
  CROPCENT->som2c2.ANEREFF.ANEREF2=*(somassignparmsfromR+252);
  CROPCENT->som2c2.ANEREFF.ANEREF3=*(somassignparmsfromR+253);
  CROPCENT->som2c2.INFLOW.maxCN= *(somassignparmsfromR+254);
  CROPCENT->som2c2.INFLOW.minCN= *(somassignparmsfromR+255); 
  CROPCENT->som2c2.INFLOW.minN= *(somassignparmsfromR+256); 
  CROPCENT->som2c2.INFLOW.maxCP= *(somassignparmsfromR+257); 
  CROPCENT->som2c2.INFLOW.minCP= *(somassignparmsfromR+258); 
  CROPCENT->som2c2.INFLOW.minP= *(somassignparmsfromR+259); 
  CROPCENT->som2c2.INFLOW.maxCS= *(somassignparmsfromR+260); 
  CROPCENT->som2c2.INFLOW.minCS= *(somassignparmsfromR+261); 
  CROPCENT->som2c2.INFLOW.minS= *(somassignparmsfromR+262); 
  CROPCENT->som2c2.INFLOW.maxCK= *(somassignparmsfromR+263); 
  CROPCENT->som2c2.INFLOW.minCK= *(somassignparmsfromR+264); 
  CROPCENT->som2c2.INFLOW.minK= *(somassignparmsfromR+265); 
  
  
  
  CROPCENT->som3c.parms.k=*(somassignparmsfromR+266);
  CROPCENT->som3c.parms.timestep=*(somassignparmsfromR+267);
  CROPCENT->som3c.parms.p3co2=*(somassignparmsfromR+268);
  CROPCENT->som3c.TEff.teff1=*(somassignparmsfromR+269);
  CROPCENT->som3c.TEff.teff2=*(somassignparmsfromR+270);
  CROPCENT->som3c.TEff.teff3=*(somassignparmsfromR+271);
  CROPCENT->som3c.TEff.teff4=*(somassignparmsfromR+272);
  CROPCENT->som3c.SWEFF.a=*(somassignparmsfromR+273);
  CROPCENT->som3c.SWEFF.b=*(somassignparmsfromR+274);
  CROPCENT->som3c.SWEFF.c=*(somassignparmsfromR+275);
  CROPCENT->som3c.SWEFF.d=*(somassignparmsfromR+276);
  CROPCENT->som3c.PHEFF.a=*(somassignparmsfromR+277);
  CROPCENT->som3c.PHEFF.b=*(somassignparmsfromR+278);
  CROPCENT->som3c.PHEFF.c=*(somassignparmsfromR+279);
  CROPCENT->som3c.PHEFF.d=*(somassignparmsfromR+280);
  CROPCENT->som3c.ANEREFF.ANEREF1=*(somassignparmsfromR+281);
  CROPCENT->som3c.ANEREFF.ANEREF2=*(somassignparmsfromR+282);
  CROPCENT->som3c.ANEREFF.ANEREF3=*(somassignparmsfromR+283);
  CROPCENT->som3c.INFLOW.maxCN= *(somassignparmsfromR+284);
  CROPCENT->som3c.INFLOW.minCN= *(somassignparmsfromR+285); 
  CROPCENT->som3c.INFLOW.minN= *(somassignparmsfromR+286); 
  CROPCENT->som3c.INFLOW.maxCP= *(somassignparmsfromR+287); 
  CROPCENT->som3c.INFLOW.minCP= *(somassignparmsfromR+288); 
  CROPCENT->som3c.INFLOW.minP= *(somassignparmsfromR+289); 
  CROPCENT->som3c.INFLOW.maxCS= *(somassignparmsfromR+290); 
  CROPCENT->som3c.INFLOW.minCS= *(somassignparmsfromR+291); 
  CROPCENT->som3c.INFLOW.minS= *(somassignparmsfromR+292); 
  CROPCENT->som3c.INFLOW.maxCK= *(somassignparmsfromR+293); 
  CROPCENT->som3c.INFLOW.minCK= *(somassignparmsfromR+294); 
  CROPCENT->som3c.INFLOW.minK= *(somassignparmsfromR+295); 
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
  //Rprintf("%f, %f, %f \n", *getcropcentstatevarfromR,*(getcropcentstatevarfromR+1),*(getcropcentstatevarfromR+2));
  ENV->minN=*getcropcentstatevarfromR;
  ENV->minP=*(getcropcentstatevarfromR+1);
  ENV->minS=*(getcropcentstatevarfromR+2);
  ENV->minK=*(getcropcentstatevarfromR+3);
  ENV->surfaceTEMP=*(getcropcentstatevarfromR+4);
  ENV->soilTEMP=*(getcropcentstatevarfromR+5);
  ENV->surfaceRELWC=*(getcropcentstatevarfromR+6);
  ENV->soilRELWC=*(getcropcentstatevarfromR+7);
  ENV->leachedWATER=*(getcropcentstatevarfromR+8);
  ENV->PET=*(getcropcentstatevarfromR+9);
  ENV->AWC=*(getcropcentstatevarfromR+10);
  ENV->soilrad=*(getcropcentstatevarfromR+11); // check unit for this variable
  ENV->pH=*(getcropcentstatevarfromR+12);
  ENV->drainage=*(getcropcentstatevarfromR+13);
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
   
    replaceMineralStruct(&CROPCENT->metabc1.E,&CROPCENT->metabc1.Flux.metabc1TOmetabc1.E);
    replaceMineralStruct(&CROPCENT->metabc2.E,&CROPCENT->metabc2.Flux.metabc2TOmetabc2.E);
    
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
  temp->FXMCA=*getsymbnfixationparmsfromR;
  temp->FXMCB=*(getsymbnfixationparmsfromR+1);
  temp->FXMXS=*(getsymbnfixationparmsfromR+2);
  temp->FXNPB=*(getsymbnfixationparmsfromR+3);
  temp->NSNFIX=*(getsymbnfixationparmsfromR+4);
  temp->NTSPM=*(getsymbnfixationparmsfromR+5);
  return;
}

void GetLeachingParms (struct OrgLeachParms *temp,double *getleachingparmsfromR)
{//Rprintf("%f, %f, %f \n", *getleachingparmsfromR,*(getleachingparmsfromR+1),*(getleachingparmsfromR+2));
  temp->DailyThresholdFlow=*getleachingparmsfromR;
  temp->OMLEACH[0]=*(getleachingparmsfromR+1);
  temp->OMLEACH[1]=*(getleachingparmsfromR+2);
  temp->OMLEACH[2]=*(getleachingparmsfromR+3); // is this same as threshold value of daily flow?
  temp->som1c2toleach.CN=*(getleachingparmsfromR+4);
  temp->som1c2toleach.CP=*(getleachingparmsfromR+5);
  temp->som1c2toleach.CS=*(getleachingparmsfromR+6);
  temp->som1c2toleach.CK=*(getleachingparmsfromR+7);// This need to be verified
  return;
}

void GetC13Parms(struct C13Parms *temp,double *getc13parmsfromR)
{ //Rprintf("%f, %f \n", *getc13parmsfromR,*(getc13parmsfromR+1));
  temp->DRESP=*getc13parmsfromR; 
  temp->DLIGDF=*(getc13parmsfromR+1);
  return;
}

void GetErosionParms(struct ErosionParms *temp,double *geterosionparmsfromR){
  //Rprintf("%f, %f, %f \n", *geterosionparmsfromR,*(geterosionparmsfromR+1),*(geterosionparmsfromR+2));
  temp->LHZF[0]=*geterosionparmsfromR; 
  temp->LHZF[1]=*(geterosionparmsfromR+1);
  temp->LHZF[2]=*(geterosionparmsfromR+2);
  temp->EDEPTH=*(geterosionparmsfromR+3); 
  temp->ENRICH=*(geterosionparmsfromR+4);
}

void GetSoilTexture( struct SoilTexture *temp, double *getsoiltexturefromR){
//Rprintf("%f, %f, %f \n", *getsoiltexturefromR,*(getsoiltexturefromR+1),*(getsoiltexturefromR+2));
  temp->sand=*getsoiltexturefromR;
  temp->silt=*(getsoiltexturefromR+1);
  temp->clay=*(getsoiltexturefromR+2);
}

void GetBioCroToCropcentParms(struct BioCroToCropcentParms  *temp,double *getbiocrotocropcentparmsfromR ){
  //Rprintf("%f, %f, %f \n", *getbiocrotocropcentparmsfromR,*(getbiocrotocropcentparmsfromR+1),*(getbiocrotocropcentparmsfromR+2));
  temp->structometaSLOPE=*getbiocrotocropcentparmsfromR;
  temp->structometaINTERCEP=*(getbiocrotocropcentparmsfromR+1);
  temp->surfacedamrN=*(getbiocrotocropcentparmsfromR+2);
  temp->surfacedamrP=*(getbiocrotocropcentparmsfromR+3);
  temp->surfacedamrS=*(getbiocrotocropcentparmsfromR+4);
  temp->surfacedamrK=*(getbiocrotocropcentparmsfromR+5);
  temp->soildamrN=*(getbiocrotocropcentparmsfromR+6);
  temp->soildamrP=*(getbiocrotocropcentparmsfromR+7);
  temp->soildamrS=*(getbiocrotocropcentparmsfromR+8);
  temp->soildamrK=*(getbiocrotocropcentparmsfromR+9);
  temp->mindamrN=*(getbiocrotocropcentparmsfromR+10);
  temp->mindamrP=*(getbiocrotocropcentparmsfromR+11);
  temp->mindamrS=*(getbiocrotocropcentparmsfromR+12);
  temp->mindamrK=*(getbiocrotocropcentparmsfromR+13);
  temp->pabres=*(getbiocrotocropcentparmsfromR+14);
  return;
}