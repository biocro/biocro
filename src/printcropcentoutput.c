#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "crocent.h"
  
  void printcropcentout(struct cropcentlayer CROPCENT, double *totalSOC, double *strucc1, double *strucc2, 
                        double *metabc1, double *metabc2, double *som1c1, double *som1c2, double *som2c1, 
                        double *som2c2, double *som3c, double *minN){
  double cbalance;
  
  *totalSOC = CROPCENT.strucc1.C.totalC +CROPCENT.strucc2.C.totalC+CROPCENT.metabc1.C.totalC+CROPCENT.metabc2.C.totalC
              +CROPCENT.som1c1.C.totalC+CROPCENT.som1c2.C.totalC + CROPCENT.som2c1.C.totalC+CROPCENT.som2c2.C.totalC
              +CROPCENT.som3c.C.totalC;
              
      *strucc1 = CROPCENT.strucc1.C.totalC;
      *strucc2 = CROPCENT.strucc2.C.totalC;
      *metabc1 = CROPCENT.metabc1.C.totalC;
      *metabc2 = CROPCENT.metabc2.C.totalC;
      *som1c1 = CROPCENT.som1c1.C.totalC;
      *som1c2 = CROPCENT.som1c2.C.totalC;
       *som2c1 = CROPCENT.som2c1.C.totalC;
      *som2c2 = CROPCENT.som2c2.C.totalC;
       *som3c = CROPCENT.som3c.C.totalC;
       *minN=CROPCENT.ENV.minN;
     
              
  /*
  Rprintf("SOM1c1\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.som1c1.Flux.hetresp);
  cbalance=CROPCENT.som1c1.Flux.som1c1TOsom1c1.C.totalC+CROPCENT.som1c1.Flux.som1c1TOsom2c1.C.totalC+CROPCENT.som1c1.Flux.hetresp;
  Rprintf("Cbalance=%f\n",cbalance);
  
  Rprintf("SOM1c2\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.som1c2.Flux.hetresp);
   cbalance=CROPCENT.som1c2.Flux.som1c2TOsom1c2.C.totalC+CROPCENT.som1c2.Flux.som1c2TOsom2c2.C.totalC+CROPCENT.som1c2.Flux.hetresp
  +CROPCENT.som1c2.Flux.som1c2TOsom3c.C.totalC+CROPCENT.som1c2.Flux.som1c2TOleachate.C.totalC;
  Rprintf("Cbalance=%f\n",cbalance);
  Rprintf("leachate Losses = %f\n",CROPCENT.som1c2.Flux.som1c2TOleachate.C.totalC);
  Rprintf("leachate CN = %f, som1c2 CN= %f\n",CROPCENT.som1c2.Flux.som1c2TOleachate.E.CP,CROPCENT.som1c2.E.CP);
  
  Rprintf("SOM2c1\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.som2c1.Flux.hetresp);
  cbalance=CROPCENT.som2c1.Flux.som2c1TOsom2c1.C.totalC+CROPCENT.som2c1.Flux.som2c1TOsom1c1.C.totalC+CROPCENT.som2c1.Flux.hetresp
  +CROPCENT.som2c1.Flux.som2c1TOsom2c2.C.totalC;
  Rprintf("Cbalance=%f\n",cbalance);
  
  
  Rprintf("SOM2c2\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.som2c2.Flux.hetresp);
  cbalance=CROPCENT.som3c.Flux.som3cTOsom3c.C.totalC+CROPCENT.som3c.Flux.som3cTOsom1c2.C.totalC+CROPCENT.som3c.Flux.hetresp;
  Rprintf("Cbalance=%f\n",cbalance);
  
  
  Rprintf("SOM3c\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.som3c.Flux.hetresp);
  cbalance=CROPCENT.som3c.Flux.som3cTOsom3c.C.totalC+CROPCENT.som3c.Flux.som3cTOsom1c2.C.totalC+CROPCENT.som3c.Flux.hetresp;
  Rprintf("Cbalance=%f\n",cbalance);
  
  
   //  Printing wood1
    Rprintf("WOOD1\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.wood1.Flux.hetresp);
  Rprintf("wood1TOSOM1C1, totalC=%f \n",CROPCENT.wood1.Flux.wood1TOsom1c1.C.totalC);
  Rprintf("wood1TOSOM2C1, totalC=%f \n",CROPCENT.wood1.Flux.wood1TOsom2c1.C.totalC);
  Rprintf("wood1TOwood1, totalC=%f \n",CROPCENT.wood1.Flux.wood1TOwood1.C.totalC);
  
  cbalance=CROPCENT.wood1.Flux.wood1TOwood1.C.totalC+CROPCENT.wood1.Flux.wood1TOsom2c1.C.totalC
  +CROPCENT.wood1.Flux.wood1TOsom1c1.C.totalC+CROPCENT.wood1.Flux.hetresp;
  
  Rprintf("Flux Balance of strucc1=%f\n",cbalance);
  Rprintf("Environmental Mineral N= %f\n",CROPCENT.ENV.minN);
  
  Rprintf("wood1TOSOM1C2, CN=%f \n",CROPCENT.wood1.Flux.wood1TOsom1c1.E.CN);
  Rprintf("wood1TOSOM2C2, CN=%f \n",CROPCENT.wood1.Flux.wood1TOsom2c1.E.CN);
  Rprintf("wood1TOwood1, CN=%f \n",CROPCENT.wood1.Flux.wood1TOwood1.E.CN);
  
 Rprintf("METABC2\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.metabc2.Flux.hetresp);
  Rprintf("METABC2TOSOM1C2, totalC=%f \n",CROPCENT.metabc2.Flux.metabc2TOsom1c2.C.totalC);
  Rprintf("METABC2TOMETABC2, totalC=%f \n",CROPCENT.metabc2.Flux.metabc2TOmetabc2.C.totalC);
  
  cbalance=CROPCENT.metabc2.Flux.metabc2TOmetabc2.C.totalC+CROPCENT.metabc2.Flux.metabc2TOsom1c2.C.totalC
  +CROPCENT.metabc2.Flux.hetresp;
  Rprintf("Flux Balance of metabc2=%f\n",cbalance);
  Rprintf("Environmental Mineral N= %f\n",CROPCENT.ENV.minN);
  Rprintf("METABC2TOMETABC2, CN=%f \n",CROPCENT.metabc2.Flux.metabc2TOmetabc2.E.CN);
  Rprintf("METABC2TOSOM1C2, CN=%f \n",CROPCENT.metabc2.Flux.metabc2TOsom1c2.E.CN);

  
 Rprintf("METABC1\n");
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.metabc1.Flux.hetresp);
  Rprintf("METABC1TOSOM1C1, totalC=%f \n",CROPCENT.metabc1.Flux.metabc1TOsom1c1.C.totalC);
  Rprintf("METABC1TOMETABC1, totalC=%f \n",CROPCENT.metabc1.Flux.metabc1TOmetabc1.C.totalC);
  
  cbalance=CROPCENT.metabc1.Flux.metabc1TOmetabc1.C.totalC+CROPCENT.metabc1.Flux.metabc1TOsom1c1.C.totalC
  +CROPCENT.metabc1.Flux.hetresp;
  Rprintf("Flux Balance of strucc1=%f\n",cbalance);

  
  Rprintf("Environmental Mineral N= %f\n",CROPCENT.ENV.minN);
  
  Rprintf("METABC1TOMETABC1, CN=%f \n",CROPCENT.metabc1.Flux.metabc1TOmetabc1.E.CN);
  Rprintf("METABC1TOSOM1C1, CN=%f \n",CROPCENT.metabc1.Flux.metabc1TOsom1c1.E.CN);

  //  Printing strucc2
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.strucc2.Flux.hetresp);
  Rprintf("STRUCC2TOSOM1C2, totalC=%f \n",CROPCENT.strucc2.Flux.strucc2TOsom1c2.C.totalC);
  Rprintf("STRUCC2TOSOM2C2, totalC=%f \n",CROPCENT.strucc2.Flux.strucc2TOsom2c2.C.totalC);
  Rprintf("STRUCC2TOSTRUCC2, totalC=%f \n",CROPCENT.strucc2.Flux.strucc2TOstrucc2.C.totalC);
  
  cbalance=CROPCENT.strucc2.Flux.strucc2TOstrucc2.C.totalC+CROPCENT.strucc2.Flux.strucc2TOsom2c2.C.totalC
  +CROPCENT.strucc2.Flux.strucc2TOsom1c2.C.totalC+CROPCENT.strucc2.Flux.hetresp;
  
  Rprintf("Flux Balance of strucc1=%f\n",cbalance);
  Rprintf("Environmental Mineral N= %f\n",CROPCENT.ENV.minN);
  
  Rprintf("STRUCC2TOSOM1C2, CN=%f \n",CROPCENT.strucc2.Flux.strucc2TOsom1c2.E.CN);
  Rprintf("STRUCC2TOSOM2C2, CN=%f \n",CROPCENT.strucc2.Flux.strucc2TOsom2c2.E.CN);
  Rprintf("STRUCC2TOSTRUCC2, CN=%f \n",CROPCENT.strucc2.Flux.strucc2TOstrucc2.E.CN);
*/
 // Printing STRUCC1
 /*
   Rprintf("C Pool = %f\n", CROPCENT.strucc1.C.totalC);
   Rprintf("C Pool unlablto labl Ratio= %f \n", CROPCENT.strucc1.C.unlablTOlabl);
  Rprintf("heterotrophic Respiration = %f\n",CROPCENT.strucc1.Flux.hetresp);
  Rprintf("STRUCC1TOMETABC1, totalC=%f \n",CROPCENT.strucc1.Flux.strucc1TOmetabc1.C.totalC);
  Rprintf("STRUCC1TOSOM1C1, totalC=%f \n",CROPCENT.strucc1.Flux.strucc1TOsom1c1.C.totalC);
  Rprintf("STRUCC1TOSOM2C1, totalC=%f \n",CROPCENT.strucc1.Flux.strucc1TOsom2c1.C.totalC);
  Rprintf("STRUCC1TOSTRUCC1, totalC=%f \n",CROPCENT.strucc1.Flux.strucc1TOstrucc1.C.totalC);
  
  cbalance=CROPCENT.strucc1.Flux.strucc1TOstrucc1.C.totalC+CROPCENT.strucc1.Flux.strucc1TOsom2c1.C.totalC
  +CROPCENT.strucc1.Flux.strucc1TOsom1c1.C.totalC+CROPCENT.strucc1.Flux.strucc1TOmetabc1.C.totalC
  +CROPCENT.strucc1.Flux.hetresp;
  
  Rprintf("Flux Balance of strucc1=%f\n",cbalance);
  Rprintf("Environmental Mineral N= %f\n",CROPCENT.ENV.minN);
  
  Rprintf("STRUCC1TOMETABC1, CN=%f \n",CROPCENT.strucc1.Flux.strucc1TOmetabc1.E.CN);
  Rprintf("STRUCC1TOSOM1C1, CN=%f \n",CROPCENT.strucc1.Flux.strucc1TOsom1c1.E.CN);
  Rprintf("STRUCC1TOSOM2C1, CN=%f \n",CROPCENT.strucc1.Flux.strucc1TOsom2c1.E.CN);
  Rprintf("STRUCC1TOSTRUCC1, CN=%f \n",CROPCENT.strucc1.Flux.strucc1TOstrucc1.E.CN);
  */
  return;
  }