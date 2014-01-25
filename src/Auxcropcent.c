#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "crocent.h"


void updatecropcentpools(struct cropcentlayer *CROPCENT)
{
  double delN;
  double out, in;
  
  delN=0.0;

  // updating strucc1 flux [only source is self-negative outflow]
  // step 1: chnage in mineral N of CropCENT layer due to flow from strucc1
  out=(CROPCENT->strucc1.Flux.strucc1TOstrucc1.C.totalC)/(CROPCENT->strucc1.Flux.strucc1TOstrucc1.E.CN); // This is always negative
  in=(CROPCENT->strucc1.Flux.strucc1TOmetabc1.C.totalC)/(CROPCENT->strucc1.Flux.strucc1TOmetabc1.E.CN)
      +(CROPCENT->strucc1.Flux.strucc1TOsom1c1.C.totalC)/(CROPCENT->strucc1.Flux.strucc1TOsom1c1.E.CN)
      +(CROPCENT->strucc1.Flux.strucc1TOsom2c1.C.totalC)/(CROPCENT->strucc1.Flux.strucc1TOsom2c1.E.CN); // This is always positive
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  // now update CN ratioi snot required as all outflow CE ratio is same as of source pool
  // finally update C structure of the pool
  updateCarbonStructure(&CROPCENT->strucc1.C, CROPCENT->strucc1.Flux.strucc1TOstrucc1.C);
  updateCarbonStructure(&CROPCENT->metabc1.C, CROPCENT->strucc1.Flux.strucc1TOmetabc1.C);
  updateMineralStructure(&CROPCENT->metabc1.C, &CROPCENT->metabc1.E, CROPCENT->strucc1.Flux.strucc1TOmetabc1.C, CROPCENT->strucc1.Flux.strucc1TOmetabc1.E);
  updateCarbonStructure(&CROPCENT->som1c1.C, CROPCENT->strucc1.Flux.strucc1TOsom1c1.C);
  updateMineralStructure(&CROPCENT->som1c1.C, &CROPCENT->som1c1.E, CROPCENT->strucc1.Flux.strucc1TOsom1c1.C, CROPCENT->strucc1.Flux.strucc1TOsom1c1.E);
  updateCarbonStructure(&CROPCENT->som2c1.C, CROPCENT->strucc1.Flux.strucc1TOsom2c1.C);
  updateMineralStructure(&CROPCENT->som2c1.C, &CROPCENT->som2c1.E, CROPCENT->strucc1.Flux.strucc1TOsom2c1.C, CROPCENT->strucc1.Flux.strucc1TOsom2c1.E);

  // updating strucc2 flux [ only source is self-negative outflow]
  out=(CROPCENT->strucc2.Flux.strucc2TOstrucc2.C.totalC)/(CROPCENT->strucc2.Flux.strucc2TOstrucc2.E.CN); // This is always negative
  in= (CROPCENT->strucc2.Flux.strucc2TOsom1c2.C.totalC)/(CROPCENT->strucc2.Flux.strucc2TOsom1c2.E.CN)
      +(CROPCENT->strucc2.Flux.strucc2TOsom2c2.C.totalC)/(CROPCENT->strucc2.Flux.strucc2TOsom2c2.E.CN); // This is always positive
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  // now update CN ratio, which does not change because outflow CE is same as CE of pool. Only CE ratio of receiving flux changes
  // finally update C structure of the pool
  updateCarbonStructure(&CROPCENT->strucc2.C, CROPCENT->strucc2.Flux.strucc2TOstrucc2.C);
  updateCarbonStructure(&CROPCENT->som1c2.C, CROPCENT->strucc2.Flux.strucc2TOsom1c2.C);
  updateMineralStructure(&CROPCENT->som1c2.C, &CROPCENT->som1c2.E, CROPCENT->strucc2.Flux.strucc2TOsom1c2.C, CROPCENT->strucc2.Flux.strucc2TOsom1c2.E);
  updateCarbonStructure(&CROPCENT->som2c2.C, CROPCENT->strucc2.Flux.strucc2TOsom2c2.C);
  updateMineralStructure(&CROPCENT->som2c2.C, &CROPCENT->som2c2.E, CROPCENT->strucc2.Flux.strucc2TOsom2c2.C, CROPCENT->strucc2.Flux.strucc2TOsom2c2.E);
  
  // updating metabc1 flux [source is self -negative outflow and from strucc1]
  out=(CROPCENT->metabc1.Flux.metabc1TOmetabc1.C.totalC)/(CROPCENT->metabc1.Flux.metabc1TOmetabc1.E.CN); 
  in=(CROPCENT->metabc1.Flux.metabc1TOsom1c1.C.totalC)/(CROPCENT->metabc1.Flux.metabc1TOsom1c1.E.CN); 
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->metabc1.C, CROPCENT->metabc1.Flux.metabc1TOmetabc1.C);
  updateCarbonStructure(&CROPCENT->som1c1.C, CROPCENT->metabc1.Flux.metabc1TOsom1c1.C);
  updateMineralStructure(&CROPCENT->som1c1.C, &CROPCENT->som1c1.E, CROPCENT->metabc1.Flux.metabc1TOsom1c1.C, CROPCENT->metabc1.Flux.metabc1TOsom1c1.E);
  
  // updating metabc2 flux [source is self -negative outflow and from strucc1]
  out=(CROPCENT->metabc2.Flux.metabc2TOmetabc2.C.totalC)/(CROPCENT->metabc2.Flux.metabc2TOmetabc2.E.CN); 
  in=(CROPCENT->metabc2.Flux.metabc2TOsom1c2.C.totalC)/(CROPCENT->metabc2.Flux.metabc2TOsom1c2.E.CN); 
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->metabc2.C, CROPCENT->metabc2.Flux.metabc2TOmetabc2.C);
  updateCarbonStructure(&CROPCENT->som1c2.C, CROPCENT->metabc2.Flux.metabc2TOsom1c2.C);
  updateMineralStructure(&CROPCENT->som1c2.C, &CROPCENT->som1c2.E, CROPCENT->metabc2.Flux.metabc2TOsom1c2.C, CROPCENT->metabc2.Flux.metabc2TOsom1c2.E);
  
  // updating som1c1 Fluxes
  out=(CROPCENT->som1c1.Flux.som1c1TOsom1c1.C.totalC)/(CROPCENT->som1c1.Flux.som1c1TOsom1c1.E.CN); 
  in=(CROPCENT->som1c1.Flux.som1c1TOsom2c1.C.totalC)/(CROPCENT->som1c1.Flux.som1c1TOsom2c1.E.CN); 
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->som1c1.C, CROPCENT->som1c1.Flux.som1c1TOsom1c1.C);
  updateCarbonStructure(&CROPCENT->som2c1.C, CROPCENT->som1c1.Flux.som1c1TOsom2c1.C);
  updateMineralStructure(&CROPCENT->som2c1.C, &CROPCENT->som2c1.E, CROPCENT->som1c1.Flux.som1c1TOsom2c1.C, CROPCENT->som1c1.Flux.som1c1TOsom2c1.E);
  
  
  // updating som2c1 Fluxes
  out=(CROPCENT->som2c1.Flux.som2c1TOsom2c1.C.totalC)/(CROPCENT->som2c1.Flux.som2c1TOsom2c1.E.CN); 
  in=(CROPCENT->som2c1.Flux.som2c1TOsom1c1.C.totalC)/(CROPCENT->som2c1.Flux.som2c1TOsom1c1.E.CN)
    +(CROPCENT->som2c1.Flux.som2c1TOsom2c2.C.totalC)/(CROPCENT->som2c1.Flux.som2c1TOsom2c2.E.CN); 
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->som2c1.C, CROPCENT->som2c1.Flux.som2c1TOsom2c1.C);
  updateCarbonStructure(&CROPCENT->som1c1.C, CROPCENT->som2c1.Flux.som2c1TOsom1c1.C);
  updateMineralStructure(&CROPCENT->som1c1.C, &CROPCENT->som1c1.E, CROPCENT->som2c1.Flux.som2c1TOsom1c1.C, CROPCENT->som2c1.Flux.som2c1TOsom1c1.E);
  updateCarbonStructure(&CROPCENT->som2c2.C, CROPCENT->som2c1.Flux.som2c1TOsom2c2.C);
  updateMineralStructure(&CROPCENT->som2c2.C, &CROPCENT->som2c2.E, CROPCENT->som2c1.Flux.som2c1TOsom2c2.C, CROPCENT->som2c1.Flux.som2c1TOsom2c2.E);
  
  
  // updating som1c2 Fluxes
  out=(CROPCENT->som1c2.Flux.som1c2TOsom1c2.C.totalC)/(CROPCENT->som1c2.Flux.som1c2TOsom1c2.E.CN); 
  in=(CROPCENT->som1c2.Flux.som1c2TOsom2c2.C.totalC)/(CROPCENT->som1c2.Flux.som1c2TOsom2c2.E.CN)
    +(CROPCENT->som1c2.Flux.som1c2TOsom3c.C.totalC)/(CROPCENT->som1c2.Flux.som1c2TOsom3c.E.CN)
    +(CROPCENT->som1c2.Flux.som1c2TOleachate.C.totalC)/(CROPCENT->som1c2.Flux.som1c2TOleachate.E.CN);
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->som1c2.C, CROPCENT->som1c2.Flux.som1c2TOsom1c2.C);
  // remember here we need to modify CN ratio of the som1c2 pool because CN ratio of leachate loss is much larger
  // This modified ratio can be obtained in th below function
  updateCEafterleachate(&CROPCENT->som1c2);
 
  updateCarbonStructure(&CROPCENT->som2c2.C, CROPCENT->som1c2.Flux.som1c2TOsom2c2.C);
  updateMineralStructure(&CROPCENT->som2c2.C, &CROPCENT->som2c2.E, CROPCENT->som1c2.Flux.som1c2TOsom2c2.C, CROPCENT->som1c2.Flux.som1c2TOsom2c2.E);
   
  updateCarbonStructure(&CROPCENT->som3c.C, CROPCENT->som1c2.Flux.som1c2TOsom3c.C);
  updateMineralStructure(&CROPCENT->som3c.C, &CROPCENT->som3c.E, CROPCENT->som1c2.Flux.som1c2TOsom3c.C, CROPCENT->som1c2.Flux.som1c2TOsom3c.E);
  
  
  // updating som2c2 Fluxes
  out=(CROPCENT->som2c2.Flux.som2c2TOsom2c2.C.totalC)/(CROPCENT->som2c2.Flux.som2c2TOsom2c2.E.CN); 
  in=(CROPCENT->som2c2.Flux.som2c2TOsom3c.C.totalC)/(CROPCENT->som2c2.Flux.som2c2TOsom3c.E.CN)
    +(CROPCENT->som2c2.Flux.som2c2TOsom1c2.C.totalC)/(CROPCENT->som2c2.Flux.som2c2TOsom1c2.E.CN); 
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->som2c2.C, CROPCENT->som2c2.Flux.som2c2TOsom2c2.C);
  updateCarbonStructure(&CROPCENT->som1c2.C, CROPCENT->som2c2.Flux.som2c2TOsom1c2.C);
  updateMineralStructure(&CROPCENT->som1c2.C, &CROPCENT->som1c2.E, CROPCENT->som2c2.Flux.som2c2TOsom1c2.C, CROPCENT->som2c2.Flux.som2c2TOsom1c2.E);
  updateCarbonStructure(&CROPCENT->som2c2.C, CROPCENT->som2c2.Flux.som2c2TOsom3c.C);
  updateMineralStructure(&CROPCENT->som2c2.C, &CROPCENT->som2c2.E, CROPCENT->som2c2.Flux.som2c2TOsom3c.C, CROPCENT->som2c2.Flux.som2c2TOsom3c.E);
 
 // updating som3c Fluxes
  out=(CROPCENT->som3c.Flux.som3cTOsom3c.C.totalC)/(CROPCENT->som3c.Flux.som3cTOsom3c.E.CN); 
  in=(CROPCENT->som3c.Flux.som3cTOsom1c2.C.totalC)/(CROPCENT->som3c.Flux.som3cTOsom1c2.E.CN);
   
  delN=out+in; // if this is negative then out > in or mineralization is occurin otherwise immobilization
  CROPCENT->ENV.minN = CROPCENT->ENV.minN-delN;
  updateCarbonStructure(&CROPCENT->som3c.C, CROPCENT->som3c.Flux.som3cTOsom3c.C);
  updateCarbonStructure(&CROPCENT->som1c2.C, CROPCENT->som3c.Flux.som3cTOsom1c2.C);
  updateMineralStructure(&CROPCENT->som1c2.C, &CROPCENT->som1c2.E, CROPCENT->som3c.Flux.som3cTOsom1c2.C, CROPCENT->som3c.Flux.som3cTOsom1c2.E);
  return;
}

void BiocroToCrocent(double *stdedbc, double fallrate, double lignin, struct minerals *E, double isotoperatio, int surface, int woody,struct InputToCropcent *INCROCENT) 
{
  /***************************************************************
  *   Purpose:
  *   Purpose of this function is to take the daily output of BioCro
  *   and create structure InputToCrocent that contains information
  *   about characteristics of the litter and this structure can be used to
  *   update Crocent pools
  *   Note that we are not reducing standing biomass due to photodecomposition
  *   we are only letting it fall based on fallrate to contribute to 
  *   appropriate soilpools
  *  
  * 
  *   Arguments
  *   (1)  *stdedbc - This is a pointe to standing dead biomass. We are using
  *         ponters here because we would like to update it after accouting for
  *         reduction in fall rate
  *   (2)  fallrate - This represents fraction of standing deadbiomasss
  *         that will contribute to soil pools
  *   (3)   lignin - This represents lignin content of the dead biomass
  *   (4)   E representes CN,CP,CS, CK- represents nutrient:C ratio for the dead biomass
  *   (5)   isotopratio - represents ratio of unlabled and labled C in 
  *          the dead biomass
  *   (6)   surface = 1 for surface input and 0 for below ground input 
  *   (7)   woody =0 for non woody material and
  *        woody =1 for branches
  *        woody =2 for large wood
  *        woody =3 for coarse roots
  * 
  *   We also need some environmental variables to evaluate photodegradation of
  *   the standing biomass
  * **********************************************************/
      double temp;
      if(fallrate>1.0) 
      {
        error("Fall rate is greather than 1.0");
      }
      
      temp=(*stdedbc)*fallrate;  // Find how much litter goes to ground
      (*stdedbc)=(*stdedbc)-temp; // update standing litter
       
       temp=temp*40; // unit conversion for Mg biomass/ha  to gram C/m2 for non woody
      
      INCROCENT->C.totalC =temp;
      INCROCENT->C.unlablTOlabl = isotoperatio;
      INCROCENT->E.CN= E->CN;
      INCROCENT->E.CP= E->CP;
      INCROCENT->E.CS= E->CS;
      INCROCENT->E.CK= E->CK;
      INCROCENT->lignin=lignin;
      INCROCENT->surface=surface;
      INCROCENT->woody=woody;
      return;                                  
}
void UpdateCropcentPoolsFromBioCro(struct cropcentlayer *CROPCENT, struct InputToCropcent *INCROPCENT)
{
  /**********************************************************************************
   * Purpose:
   * Structure INCROPCENT contains organic material with specified characteristics
   * We want to use content of the structure INCROPCENT and use it to update various
   * pools of CROCENT
   * which pool of crocent receives added organic matter depends upon type (surface or subsurface)
   * and ID (woody or non wody) of the INCROPCENT
   * Additional characteristics of INCROPCENT (lignin and minerals) also affects
   * partitioning of INCROPCENT into various pools of CROCENT
   * woody=1 non woody
   * woody=0 non-woody
   * surface =1 Surface Litter (leaf stem etc.)
   * surface = 0 subsurface (root, rhizome etc.)
   * 
   * Arguments:
   * pointer to a crocent layer CROCENT,only structual and metabolic pools are updated based on incoming litter amount and composition
   * pointer to a inputtocrocent INCROPCENT (Information about litter amout and composition), this information will be used to update CROCENT pools
   *
   * Strategy:
   * I can use N and lignin content of INCROPCENT to determine parameter frmet, which determines
   * how much goes to strucc and how much to metabc pool
   * All the required parameters are contained within CROPCENT structure
   ********************************************************************************/
  double lignintoN;
  double frmet;
  double caddm,cadds;
  struct minerals tometabc;
  double tmpligfrac,tE;
  
  // First I need to change the mineral N content of the incoming residues based on whether direct absorption occurs or not
  // this is equivalent to simulating immobilization 
  // immobilization depends current status of minerals therefore I need cropcent Environment (ENV)
//  Rprintf("before Direct Abs, total C - %f ,structligin=%f, structCN=%f\n",INCROPCENT->C.totalC,INCROPCENT->lignin,INCROPCENT->E.CN);
  UpdateDirectAbsorp(INCROPCENT,&CROPCENT->BcroTOCentParms, &CROPCENT->ENV); // all the arguments are actually pointer to respective sturcutres
//  Rprintf("after Direct Abs, total C = %f, structligin=%f, structCN=%f\n",INCROPCENT->C.totalC,INCROPCENT->lignin,INCROPCENT->E.CN);
  lignintoN=(INCROPCENT->lignin)*(INCROPCENT->E.CN);
  frmet=CROPCENT->BcroTOCentParms.structometaSLOPE*lignintoN+CROPCENT->BcroTOCentParms.structometaINTERCEP;
  caddm=frmet*(INCROPCENT->C.totalC); //carbon going to metabolic pool
  cadds=INCROPCENT->C.totalC-caddm; // remaining carbon going to structural pool
  tmpligfrac=(INCROPCENT->lignin)*(INCROPCENT->C.totalC)/cadds; // assuming all the lignin goes to structural pool
  tmpligfrac=(tmpligfrac<1.0)?tmpligfrac:1.0; // restricting lignin fraction of C flow to strucc1 by 1.0
//  Rprintf("slopeparm=%f, interceptparm=%f,structlignin=%f, structCN=%f,LigtoN=%f,Frmet = %f, Cadds=%f, Caddm=%f \n",CROPCENT->BcroTOCentParms.structometaSLOPE,CROPCENT->BcroTOCentParms.structometaINTERCEP,INCROPCENT->lignin,INCROPCENT->E.CN,lignintoN,frmet, cadds,caddm);
  if(INCROPCENT->surface==1)
  {
  CROPCENT->strucc1.lignin=weightavg(CROPCENT->strucc1.C.totalC, CROPCENT->strucc1.lignin,cadds, tmpligfrac); // updating lignin fraction 
  CROPCENT->strucc1.C.unlablTOlabl=weightavg(CROPCENT->strucc1.C.totalC, CROPCENT->strucc1.C.unlablTOlabl,cadds, INCROPCENT->C.unlablTOlabl); // updating isotope ratios
  CROPCENT->strucc1.C.totalC+=cadds; // Updating the pool 
  /************STRUCC1 mineral content does not change due to addition of residues, only metabolic pool E changes****/
  
  CROPCENT->metabc1.C.unlablTOlabl=weightavg(CROPCENT->metabc1.C.totalC, CROPCENT->metabc1.C.unlablTOlabl,caddm,INCROPCENT->C.unlablTOlabl);
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CN)-cadds/(CROPCENT->strucc1.E.CN);
  tometabc.CN=caddm/tE;
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CP)-cadds/(CROPCENT->strucc1.E.CP);
  tometabc.CP=caddm/tE;
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CS)-cadds/(CROPCENT->strucc1.E.CS);
  tometabc.CS=caddm/tE;
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CK)-cadds/(CROPCENT->strucc1.E.CK);
  tometabc.CK=caddm/tE;
  
  // updaing mineral concentration of metabolic pool
  CROPCENT->metabc1.E.CN=weightavg(CROPCENT->metabc1.C.totalC,CROPCENT->metabc1.E.CN,caddm,tometabc.CN);
  CROPCENT->metabc1.E.CN=weightavg(CROPCENT->metabc1.C.totalC,CROPCENT->metabc1.E.CP,caddm,tometabc.CP);
  CROPCENT->metabc1.E.CN=weightavg(CROPCENT->metabc1.C.totalC,CROPCENT->metabc1.E.CS,caddm,tometabc.CS);
  CROPCENT->metabc1.E.CN=weightavg(CROPCENT->metabc1.C.totalC,CROPCENT->metabc1.E.CK,caddm,tometabc.CK);
  CROPCENT->metabc1.C.totalC+=caddm; // updating metabolic C pool
  
  }
  else
  {
  CROPCENT->strucc2.lignin=weightavg(CROPCENT->strucc2.C.totalC, CROPCENT->strucc2.lignin,cadds, tmpligfrac); // updating lignin fraction 
  CROPCENT->strucc2.C.unlablTOlabl=weightavg(CROPCENT->strucc2.C.totalC, CROPCENT->strucc2.C.unlablTOlabl,cadds, INCROPCENT->C.unlablTOlabl); // updating isotope ratios
  CROPCENT->strucc2.C.totalC+=cadds;
  /************strucc2 mineral content does not change due to addition of residues, only metabolic pool E changes****/
  
  CROPCENT->metabc2.C.unlablTOlabl=weightavg(CROPCENT->metabc2.C.totalC, CROPCENT->metabc2.C.unlablTOlabl,caddm,INCROPCENT->C.unlablTOlabl);
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CN)-cadds/(CROPCENT->strucc2.E.CN);
  tometabc.CN=caddm/tE;
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CP)-cadds/(CROPCENT->strucc2.E.CP);
  tometabc.CP=caddm/tE;
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CS)-cadds/(CROPCENT->strucc2.E.CS);
  tometabc.CS=caddm/tE;
  tE=(INCROPCENT->C.totalC)/(INCROPCENT->E.CK)-cadds/(CROPCENT->strucc2.E.CK);
  tometabc.CK=caddm/tE;
  
  // updaing mineral concentration of metabolic pool
  CROPCENT->metabc2.E.CN=weightavg(CROPCENT->metabc2.C.totalC,CROPCENT->metabc2.E.CN,caddm,tometabc.CN);
  CROPCENT->metabc2.E.CN=weightavg(CROPCENT->metabc2.C.totalC,CROPCENT->metabc2.E.CP,caddm,tometabc.CP);
  CROPCENT->metabc2.E.CN=weightavg(CROPCENT->metabc2.C.totalC,CROPCENT->metabc2.E.CS,caddm,tometabc.CS);
  CROPCENT->metabc2.E.CN=weightavg(CROPCENT->metabc2.C.totalC,CROPCENT->metabc2.E.CK,caddm,tometabc.CK);
  CROPCENT->metabc2.C.totalC+=caddm; // updating metabolic C pool
    
  }
   return;
}

void UpdateDirectAbsorp(struct InputToCropcent *INCROCENT, struct BioCroToCropcentParms *parms, struct cropcentEnvironment *ENV){
  double Eabs, totalE;// Temporary variable declaration
  double amtFactor; // This factor limits maximum absoprtion based on amount of litter fall
  double damr,temp;
  
  /***********************************************************
   * Purpose:
   * To simulate direction absorption of nutrients from residues (immobilization)
   * 
   * Input:
   * Incoming Plant Litter Structure (INCROCENT)
   * Pointer to Structure containing Parameters govenring immobilization (parms)
   * CopCent Environmnet (ENV)
   * 
   * Output
   * Updated values of CE (C to mineral ratio) of plant residues within INCROCENT
   * **********************************************************/
  
if(INCROCENT->woody==1)
{
return;// no direct absorption by woody species
}
else
{
          amtFactor=(INCROCENT->C.totalC)/(parms->pabres);
          amtFactor=(amtFactor<1.0)?amtFactor:1.0;
         
          damr=(INCROCENT->surface==1)?parms->surfacedamrN:parms->soildamrN; // ecide whether surface or soil parameters are to be used
          Eabs=damr*(ENV->minN)*amtFactor; // Amount of Nitrogen Absorbed
          totalE=((INCROCENT->C.totalC)/(INCROCENT->E.CN))+Eabs; //Total N in g/cm2 of the INCROCENT residue
          temp=INCROCENT->C.totalC/totalE; //updated CN Ratio of the residue INCROCENT
          if(temp<(parms->mindamrN)) // checking and updating if updated CN of residue is lower than allowed value
          {
            Eabs=(INCROCENT->C.totalC)/(parms->mindamrN)-(INCROCENT->C.totalC)/(INCROCENT->E.CN);
            Eabs=(Eabs>0.0)?Eabs:0.0;
            totalE=((INCROCENT->C.totalC)/(INCROCENT->E.CN))+Eabs; //New value of Total N in g/cm2 of the INCROCENT residue
            temp=INCROCENT->C.totalC/totalE; //updated CN Ratio of the residue INCROCENT
          }
//          Rprintf("returniig CN from absor function is %f \n",temp);
//          Rprintf("totalC=%f, totalE=%f \n",INCROCENT->C.totalC,totalE);
          INCROCENT->E.CN=temp;
          ENV->minN=ENV->minN-Eabs; //updating mineral N pool of the crocent environment
          
          // Updating P concentration fo residues
          //based on PABRES parameters base don Nitrogen
          // does it have to be P specific ??
          damr=(INCROCENT->surface==1)?parms->surfacedamrP:parms->soildamrP;
          Eabs=damr*(ENV->minP)*amtFactor; // Amount of P Absorbed
          totalE=((INCROCENT->C.totalC)/(INCROCENT->E.CP))+Eabs; //Total P in g/cm2 of the INCROCENT residue
          INCROCENT->E.CP=INCROCENT->C.totalC/totalE; //updated CP Ratio of the residue INCROCENT
          ENV->minP=ENV->minP-Eabs; //updating mineral N pool of the crocent environment
          
          //Updating CS ratio of the incoming material
          damr=(INCROCENT->surface==1)?parms->surfacedamrS:parms->soildamrS;
          Eabs=damr*(ENV->minS)*amtFactor; // Amount of S Absorbed
          totalE=((INCROCENT->C.totalC)/(INCROCENT->E.CS))+Eabs; //Total S in g/cm2 of the INCROCENT residue
          INCROCENT->E.CS=INCROCENT->C.totalC/totalE; //updated CS Ratio of the residue INCROCENT
          ENV->minS=ENV->minS-Eabs; //updating mineral S pool of the crocent environment
          
          
          //Updating CK ratio of the incoming material
          damr=(INCROCENT->surface==1)?parms->surfacedamrK:parms->soildamrK;
          Eabs=damr*(ENV->minK)*amtFactor; // Amount of K Absorbed
          totalE=((INCROCENT->C.totalC)/(INCROCENT->E.CK))+Eabs; //Total K in g/cm2 of the INCROCENT residue
          INCROCENT->E.CK=INCROCENT->C.totalC/totalE; //updated CK Ratio of the residue INCROCENT
          ENV->minK=ENV->minK-Eabs; //updating mineral K pool of the crocent environment    
}
  return;
}

void decomposeCROPCENT(struct cropcentlayer *CROPCENT, int woody, int Eflag)
{
  /*********************************************************************
        Purpose:
        (1) Update Crocent SOC Pools and Mineral N contained within CROPCENT structure
        (2) Calculate leachate losses (C and nutrients)
        (3) Calculate heterotrophic respiration

        Note: struct  crocentEnvironment within crocentlayer must be updated before calling this function
        as this function uses the climate contained within to determine decomposition rates, flow,
        and updation of different pools
	*********************************************************************/

        int flag; // Flag Variable to Indicate whether decomposition will occur or not
     
         if (woody ==1) // for woody species, decompose woody components
         {
         // Decomposition of wood1 (large wood)
        flag = CheckDecomposition(&CROPCENT->wood1.E,&CROPCENT->wood1.Flux.wood1TOsom1c1,&CROPCENT->ENV,Eflag);
        decomposeWOOD1(&CROPCENT->wood1, &CROPCENT->ENV, flag,Eflag);
        
        // Decomposition of wood2 (branches)
        flag = CheckDecomposition(&CROPCENT->wood2.E,&CROPCENT->wood2.Flux.wood2TOsom1c1,&CROPCENT->ENV,Eflag);
        decomposeWOOD2(&CROPCENT->wood2, &CROPCENT->ENV, flag,Eflag);
       
       // Deomposition of wood3 (coarse roots)
         flag = CheckDecomposition(&CROPCENT->wood3.E,&CROPCENT->wood3.Flux.wood3TOsom1c2,&CROPCENT->ENV,Eflag);
          decomposeWOOD2(&CROPCENT->wood3, &CROPCENT->ENV, flag,Eflag);
        }
   
        // step 1: check if decomposition will occur based on CE ratio of limiting Flux and current CE of source and CROPCENT ENVIRONMENT
        flag = CheckDecomposition(&CROPCENT->strucc1.E,&CROPCENT->strucc1.Flux.strucc1TOsom2c1.E,&CROPCENT->ENV,Eflag);
        decomposeSTRUCC1(&CROPCENT->strucc1,&CROPCENT->ENV, flag,Eflag);
      
      	// Decomposition of strucc2
        
        flag = CheckDecomposition(&CROPCENT->strucc2.E,&CROPCENT->strucc2.Flux.strucc2TOsom2c2.E,&CROPCENT->ENV,Eflag);
        decomposeSTRUCC2(&CROPCENT->strucc2,&CROPCENT->ENV,flag ,Eflag);
      	
 	       // Decomposition of metabc1
        
        flag = CheckDecomposition(&CROPCENT->metabc1.E,&CROPCENT->metabc1.Flux.metabc1TOsom1c1,&CROPCENT->ENV,Eflag);
        decomposeMETABC1(&CROPCENT->metabc1, &CROPCENT->ENV, flag,Eflag);
	  
        	// Decomposition of metabc2
        flag = CheckDecomposition(&CROPCENT->metabc2.E,&CROPCENT->metabc2.Flux.metabc2TOsom1c2,&CROPCENT->ENV,Eflag);
        decomposeMETABC2(&CROPCENT->metabc2, &CROPCENT->ENV, flag,Eflag);
      
           // Decomposition of som1c1
    
         flag = CheckDecomposition(&CROPCENT->som1c1.E,&CROPCENT->som1c1.Flux.som1c1TOsom2c1,&CROPCENT->ENV,Eflag);
        decomposeSOM1C1(&CROPCENT->som1c1, &CROPCENT->ENV, flag,Eflag);
        
        // Decomposition of som1c2
        flag = CheckDecomposition(&CROPCENT->som1c2.E,&CROPCENT->som1c2.Flux.som1c2TOsom2c2,&CROPCENT->ENV,Eflag);
        decomposeSOM1C2(&CROPCENT->som1c2, &CROPCENT->ENV, flag,Eflag);

        // Decomposition of som2c1
         flag = CheckDecomposition(&CROPCENT->som2c1.E,&CROPCENT->som2c1.Flux.som2c1TOsom1c1,&CROPCENT->ENV,Eflag);
        decomposeSOM2C1(&CROPCENT->som2c1, &CROPCENT->ENV, flag,Eflag);

        // decompose som2c2
        flag = CheckDecomposition(&CROPCENT->som2c2.E,&CROPCENT->som2c2.Flux.som2c2TOsom1c2,&CROPCENT->ENV,Eflag);
        decomposeSOM2C2(&CROPCENT->som2c2, &CROPCENT->ENV, flag,Eflag);

        // Decomposition of som3c
        flag = CheckDecomposition(&CROPCENT->som3c.E,&CROPCENT->som3c.Flux.som3cTOsom1c2,&CROPCENT->ENV,Eflag);
        decomposeSOM3C(&CROPCENT->som3c, &CROPCENT->ENV, flag,Eflag);

	return;
}


void decomposeSOM3C(struct som3c *som3c, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow;
        double pheff,bgdefac,anerb;
     
  // initialize flux to zero
  // CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	som3c->Flux.som3cTOsom3c.C.totalC=0.0;
    som3c->Flux.som3cTOsom1c2.C.totalC=0.0;
    respiration=0.0;
   // actual decomposition based on intrinsic rate of decomposition
		 if(flag ==1){
		  pheff=GetPHfac(&som3c->PHEFF,ENV->pH);
			bgdefac=Getdefac(&som3c->TEff,&som3c->SWEFF, ENV->soilRELWC,ENV->soilTEMP);
      anerb=GetAnerbFac(&som3c->ANEREFF,ENV->PET, ENV->AWC,ENV->drainage);
     
		 // calulate total flow from decomposing strucc1 pool
		  tcflow=som3c->C.totalC*bgdefac*(som3c->parms.k)*pheff*anerb;
		 // Associated Respiration Losses
		  tempResp=tcflow*som3c->parms.p3co2;
      respiration+=tempResp;    
		  som3c->Flux.som3cTOsom3c.C.totalC-=tcflow;
      som3c->Flux.som3cTOsom1c2.C.totalC=tcflow-tempResp;
		}
    som3c->Flux.hetresp=respiration;
		return;
}

void decomposeSOM2C2(struct som2c2 *som2c2, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow;
        double tosom3c;
        double pheff,bgdefac,anerb;
        double fps2s3;
     
  // initialize flux to zero
	// CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	som2c2->Flux.som2c2TOsom2c2.C.totalC=0.0;
    som2c2->Flux.som2c2TOsom1c2.C.totalC=0.0;
		som2c2->Flux.som2c2TOsom3c.C.totalC=0.0;
    respiration=0.0;
   // actual decomposition based on intrinsic rate of decomposition
		 if(flag ==1){
		  pheff=GetPHfac(&som2c2->PHEFF,ENV->pH);
			bgdefac=Getdefac(&som2c2->TEff,&som2c2->SWEFF, ENV->soilRELWC,ENV->soilTEMP);
      anerb=GetAnerbFac(&som2c2->ANEREFF,ENV->PET, ENV->AWC,ENV->drainage);
     
		 // calulate total flow from decomposing strucc1 pool
		  tcflow=som2c2->C.totalC*bgdefac*(som2c2->parms.k)*pheff*anerb;
       som2c2->Flux.som2c2TOsom2c2.C.totalC-=tcflow;
		 // Associated Respiration Losses
		  tempResp=tcflow*som2c2->parms.p2co2;
      respiration+=tempResp;  
      // Flux from som2c2 to som3c
      fps2s3=som2c2->parms.ps2s3[0]+som2c2->parms.ps2s3[1]*ENV->SOILTEX.clay;
      tosom3c=tcflow*fps2s3*(1.0+som2c2->parms.animpt*(1.0-anerb));
  	  som2c2->Flux.som2c2TOsom3c.C.totalC=tosom3c;  
		  // flux from som2c2 to som1c2
		  som2c2->Flux.som2c2TOsom1c2.C.totalC=tcflow-tempResp-tosom3c;
		}
    som2c2->Flux.hetresp=respiration;
		return;
}


void decomposeSOM2C1(struct som2c1 *som2c1, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow, tcmix;
        double pheff,agdefac;
        double mti;
 
  // initialize flux to zero
	// CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	som2c1->Flux.som2c1TOsom2c1.C.totalC=0.0;
    som2c1->Flux.som2c1TOsom1c1.C.totalC=0.0;
    som2c1->Flux.som2c1TOsom2c2.C.totalC=0.0;
    respiration=0.0;

      // actual decomposition based on intrinsic rate of decomposition
		 if(flag ==1){
		  mti= GetMTI(som2c1->parms.a,som2c1->parms.b,som2c1->parms.x1,som2c1->parms.x2,ENV->soilrad);
		  pheff=GetPHfac(&som2c1->PHEFF,ENV->pH);
		  agdefac=Getdefac(&som2c1->TEff,&som2c1->SWEFF, ENV->surfaceRELWC,ENV->surfaceTEMP);
		  // calulate total flow from decomposing strucc1 pool
		  tcflow=som2c1->C.totalC*agdefac*(som2c1->parms.k)*pheff*mti;
      tcmix=som2c1->C.totalC*agdefac*som2c1->parms.mix;
      // Respiration Losses associated with TOTAL FLUX  
      tempResp=tcflow*som2c1->parms.p2co2;
  		respiration+=tempResp;
			 // update flux from som2c1 to som1c1
			 som2c1->Flux.som2c1TOsom1c1.C.totalC=tcflow-tempResp;
        // Updating Flux out of som1c1 pool
       tcflow=tcflow+tcmix;
  		 som2c1->Flux.som2c1TOsom2c1.C.totalC-=tcflow;
       //updating flow to som2c2 due to mixing
       som2c1->Flux.som2c1TOsom2c2.C.totalC=tcmix;
		 }
     som2c1->Flux.hetresp=respiration;
		return;
}

void decomposeSOM1C2(struct som1c2 *som1c2, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow;
        double pheff,bgdefac,anerb;
        double p1co2;
        double eftext,fps1s3;
     
	// initialize flux to zero
	// CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	som1c2->Flux.som1c2TOsom1c2.C.totalC=0.0;
    som1c2->Flux.som1c2TOsom2c2.C.totalC=0.0;
		som1c2->Flux.som1c2TOsom3c.C.totalC=0.0;
		som1c2->Flux.som1c2TOleachate.C.totalC=0.0;
    respiration=0.0;
   // actual decomposition based on intrinsic rate of decomposition
		 if(flag ==1){
		  pheff=GetPHfac(&som1c2->PHEFF,ENV->pH);
			bgdefac=Getdefac(&som1c2->TEff,&som1c2->SWEFF, ENV->soilRELWC,ENV->soilTEMP);
      anerb=GetAnerbFac(&som1c2->ANEREFF,ENV->PET, ENV->AWC,ENV->drainage);
		  eftext=som1c2->parms.peftxa + (som1c2->parms.peftxb)*ENV->SOILTEX.sand;
		 // calulate total flow from decomposing strucc1 pool
		  tcflow=som1c2->C.totalC*bgdefac*(som1c2->parms.k)*pheff*eftext*anerb;
		 // Updating Flux out of som1c2 pool
		  som1c2->Flux.som1c2TOsom1c2.C.totalC-=tcflow;
		 // Respiration Losses associated with TOTAL FLUX  
      p1co2=som1c2->parms.p1co2a+(som1c2->parms.p1co2b)*ENV->SOILTEX.sand;
		  tempResp=tcflow*p1co2;
      respiration+=tempResp;
		  // update flux from som1c2 to som3c
		  fps1s3=som1c2->parms.ps1s3[0]+som1c2->parms.ps1s3[1]*ENV->SOILTEX.clay;
		  som1c2->Flux.som1c2TOsom3c.C.totalC=tcflow*fps1s3*(1.0+som1c2->parms.animpt*(1.0-anerb));
      
		  // Flux to leachate
      som1c2->Flux.som1c2TOleachate=GetLeachate (tcflow,&som1c2->E,ENV->leachedWATER,ENV->SOILTEX.sand,&ENV->ORGLECH);


		 
		  // Flux to som2c2 pool
		  som1c2->Flux.som1c2TOsom2c2.C.totalC=tcflow-tempResp-som1c2->Flux.som1c2TOsom3c.C.totalC-som1c2->Flux.som1c2TOleachate.C.totalC;
		  // No need to updateCE pool here
		}
    som1c2->Flux.hetresp=respiration;
		return;
}



void decomposeSOM1C1(struct som1c1 *som1c1, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow;
        double pheff,agdefac;
        double mti;
        double p1co2;
 
     
	// initialize flux to zero
	// CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	som1c1->Flux.som1c1TOsom1c1.C.totalC=0.0;
    som1c1->Flux.som1c1TOsom2c1.C.totalC=0.0;
    respiration=0.0;

    // actual decomposition based on intrinsic rate of decomposition
		 if(flag ==1){
		  mti= GetMTI(som1c1->parms.a,som1c1->parms.b,som1c1->parms.x1,som1c1->parms.x2,ENV->soilrad);
		  pheff=GetPHfac(&som1c1->PHEFF,ENV->pH);
		  agdefac=Getdefac(&som1c1->TEff,&som1c1->SWEFF, ENV->surfaceRELWC,ENV->surfaceTEMP);
		  // calulate total flow from decomposing strucc1 pool
		  tcflow=som1c1->C.totalC*agdefac*(som1c1->parms.k)*pheff*mti;
			 // Updating Flux out of som1c1 pool
			 som1c1->Flux.som1c1TOsom1c1.C.totalC-=tcflow;
			 // Respiration Losses associated with TOTAL FLUX  
       p1co2=(som1c1->parms.p1co2a)+(som1c1->parms.p1co2b)*(ENV->SOILTEX.sand);
       tempResp=tcflow*p1co2;
			 respiration+=tempResp;
			 // update flux from som1c1 to som2c1
			 som1c1->Flux.som1c1TOsom2c1.C.totalC=tcflow-tempResp;
		 }
     som1c1->Flux.hetresp=respiration;
		return;
}


void decomposeWOOD3(struct wood3 *wood3, struct cropcentEnvironment *ENV, int flag,int Eflag)
{
/**********************************************
 *Purpose:
  Decompose wood3 pool and update flows

 if flag =1 then only decomposition occurs otherwise not
*****************************************************/
        double tmp,tempResp,respiration;
        double tcflow;
        double tosom2c2, tosom1c2;
        double pheff,bgdefac;
        double lignindecomrate;
        
        wood3->Flux.wood3TOsom1c2.C.totalC=0.0;
        wood3->Flux.wood3TOsom2c2.C.totalC=0.0;
        wood3->Flux.wood3TOwood3.C.totalC=0;
        respiration=0.0;
      // actual decomposition based on intrinsic rate of decomposition
      if(flag ==1)
      {
  		tmp=wood3->C.totalC;
			// Find decomposition parameters
			pheff=GetPHfac(&wood3->PHEFF,ENV->pH);
			bgdefac=Getdefac(&wood3->TEff,&wood3->SWEFF, ENV->soilRELWC,ENV->soilTEMP);
			lignindecomrate=exp((-1)*(wood3->parms.pligst)*(wood3->lignin));
			// calulate total flow from decomposing strucc1 pool
			tcflow=tmp*bgdefac*(wood3->parms.k)*lignindecomrate*pheff;
			 // update  total flow out from strucc1
			wood3->Flux.wood3TOwood3.C.totalC=(-1)*tcflow;
			 // flow to som2c2 with respiration						
			 tosom2c2 =tcflow*wood3->lignin;
		        // associated respiration losses
		         tempResp=tosom2c2*wood3->parms.rsplig;
			       respiration+=tempResp;
			 // update flux to som2c2
			 wood3->Flux.wood3TOsom2c2.C.totalC=tosom2c2-tempResp; // actual updating of total C pool, E pool is already updated
			 //flow to som1c2 with respiration
			 tosom1c2=tcflow-tosom2c2;
			 // associated respiration losses
		    tempResp=tosom1c2*wood3->parms.ps1co2;
        respiration+=tempResp;
			 // update flux to som1c1
			wood3->Flux.wood3TOsom1c2.C.totalC=tosom1c2-tempResp; // actual updating of total C pool, E pool is already updated							   
		 }
	    wood3->Flux.hetresp=respiration;
		return;
}

void decomposeWOOD2(struct wood2 *wood2, struct cropcentEnvironment *ENV, int flag,int Eflag)
{
/**********************************************
 *Purpose:
  Decompose wood2 pool and update flows

 if flag =1 then only decomposition occurs otherwise not
*****************************************************/
        double tmp,tempResp,respiration;
        double tcflow;
        double tosom2c1, tosom1c1;
        double pheff,agdefac;
        double lignindecomrate;
        
        wood2->Flux.wood2TOsom1c1.C.totalC=0.0;
        wood2->Flux.wood2TOsom2c1.C.totalC=0.0;
        wood2->Flux.wood2TOwood2.C.totalC=0;
        respiration=0.0;
      // actual decomposition based on intrinsic rate of decomposition
      if(flag ==1)
      {
			tmp=wood2->C.totalC;
			// Find decomposition parameters
			pheff=GetPHfac(&wood2->PHEFF,ENV->pH);
			agdefac=Getdefac(&wood2->TEff,&wood2->SWEFF, ENV->surfaceRELWC,ENV->surfaceTEMP);
			lignindecomrate=exp((-1)*(wood2->parms.pligst)*(wood2->lignin));
			// calulate total flow from decomposing strucc1 pool
			tcflow=tmp*agdefac*(wood2->parms.k)*lignindecomrate*pheff;
			 // update  total flow out from strucc1
			wood2->Flux.wood2TOwood2.C.totalC=(-1)*tcflow;
			 // flow to som2c2 with respiration						
			 tosom2c1 =tcflow*wood2->lignin;
		        // associated respiration losses
		         tempResp=tosom2c1*wood2->parms.rsplig;
			       respiration+=tempResp;
			 // update flux to som2c2
			 wood2->Flux.wood2TOsom2c1.C.totalC=tosom2c1-tempResp; // actual updating of total C pool, E pool is already updated
			 //flow to som1c2 with respiration
			 tosom1c1=tcflow-tosom2c1;
			 // associated respiration losses
		    tempResp=tosom1c1*wood2->parms.ps1co2;
        respiration+=tempResp;
			 // update flux to som1c1
			wood2->Flux.wood2TOsom1c1.C.totalC=tosom1c1-tempResp; // actual updating of total C pool, E pool is already updated							   
		 }
	    wood2->Flux.hetresp=respiration;
		return;
}

void decomposeWOOD1(struct wood1 *wood1, struct cropcentEnvironment *ENV, int flag,int Eflag)
{
/**********************************************
 *Purpose:
  Decompose wood1 pool and update flows

 if flag =1 then only decomposition occurs otherwise not
*****************************************************/
        double tmp,tempResp,respiration;
        double tcflow;
        double tosom2c1, tosom1c1;
        double pheff,agdefac;
        double lignindecomrate;
        
        wood1->Flux.wood1TOsom1c1.C.totalC=0.0;
        wood1->Flux.wood1TOsom2c1.C.totalC=0.0;
        wood1->Flux.wood1TOwood1.C.totalC=0;
        respiration=0.0;
      // actual decomposition based on intrinsic rate of decomposition
  	  if(flag ==1)
      {
			tmp=wood1->C.totalC;
			// Find decomposition parameters
			pheff=GetPHfac(&wood1->PHEFF,ENV->pH);
			agdefac=Getdefac(&wood1->TEff,&wood1->SWEFF, ENV->surfaceRELWC,ENV->surfaceTEMP);
			lignindecomrate=exp((-1)*(wood1->parms.pligst)*(wood1->lignin));
			// calulate total flow from decomposing strucc1 pool
			tcflow=tmp*agdefac*(wood1->parms.k)*lignindecomrate*pheff;
			 // update  total flow out from strucc1
			wood1->Flux.wood1TOwood1.C.totalC=(-1)*tcflow;
			 // flow to som2c2 with respiration						
			 tosom2c1 =tcflow*wood1->lignin;
		        // associated respiration losses
		         tempResp=tosom2c1*wood1->parms.rsplig;
			       respiration+=tempResp;
			 // update flux to som2c2
			 wood1->Flux.wood1TOsom2c1.C.totalC=tosom2c1-tempResp; // actual updating of total C pool, E pool is already updated
			 //flow to som1c2 with respiration
			 tosom1c1=tcflow-tosom2c1;
			 // associated respiration losses
		    tempResp=tosom1c1*wood1->parms.ps1co2;
        respiration+=tempResp;
			 // update flux to som1c1
			wood1->Flux.wood1TOsom1c1.C.totalC=tosom1c1-tempResp; // actual updating of total C pool, E pool is already updated							   
		 }
	    wood1->Flux.hetresp=respiration;
		return;
}


void decomposeMETABC2(struct metabc2 *metabc2, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow;
        double pheff,bgdefac,anerb;
     
  // initialize flux to zero
	// CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	metabc2->Flux.metabc2TOsom1c2.C.totalC=0.0;
    metabc2->Flux.metabc2TOmetabc2.C.totalC=0.0;
    respiration=0.0;
    // actual decomposition based on intrinsic rate of decomposition
		if(flag ==1){
		  pheff=GetPHfac(&metabc2->PHEFF,ENV->pH);
		  bgdefac=Getdefac(&metabc2->TEff,&metabc2->SWEFF, ENV->soilRELWC,ENV->soilTEMP);
      anerb=GetAnerbFac(&metabc2->ANEREFF, ENV->PET, ENV->AWC,ENV->drainage);
		  // calulate total flow from decomposing strucc1 pool
		  tcflow=metabc2->C.totalC*bgdefac*(metabc2->parms.k)*pheff*anerb;
			 // update  total flow out from strucc1
			 metabc2->Flux.metabc2TOmetabc2.C.totalC-=tcflow;
			 // flow to som1c1 lost via respiration					       
		   tempResp=tcflow*(metabc2->parms.pmco2);
			 respiration+=tempResp;
			 metabc2->Flux.metabc2TOsom1c2.C.totalC=tcflow-tempResp;
       }
       metabc2->Flux.hetresp=respiration;
		return;
}

void decomposeMETABC1(struct metabc1 *metabc1, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/
        double tempResp,respiration;
        double tcflow;
        double pheff,agdefac;
        double mdr;
     
	// initialize flux to zero
	// CE ratio of the flux is already updated
	// labl and unlabl C fraction of the flux is already updated

	 	metabc1->Flux.metabc1TOsom1c1.C.totalC=0.0;
    metabc1->Flux.metabc1TOmetabc1.C.totalC=0.0;
    respiration=0.0;
    // actual decomposition based on intrinsic rate of decomposition
		if(flag ==1){
		  mdr= GetMDR(metabc1->parms.a,metabc1->parms.b,metabc1->parms.x1,metabc1->parms.x2,ENV->soilrad);
		  pheff=GetPHfac(&metabc1->PHEFF,ENV->pH);
		  agdefac=Getdefac(&metabc1->TEff,&metabc1->SWEFF, ENV->surfaceRELWC,ENV->surfaceTEMP);
		  // calulate total flow from decomposing strucc1 pool
		  tcflow=metabc1->C.totalC*agdefac*(metabc1->parms.k)*pheff*mdr;
			 // update  total flow out from strucc1
			 metabc1->Flux.metabc1TOmetabc1.C.totalC-=tcflow;
			 // flow to som1c1 lost via respiration					       
		   tempResp=tcflow*(metabc1->parms.pmco2);
			 respiration+=tempResp;
			 metabc1->Flux.metabc1TOsom1c1.C.totalC=tcflow-tempResp;
       }
       metabc1->Flux.hetresp=respiration;
		return;
}

void decomposeSTRUCC2(struct strucc2 *strucc2, struct cropcentEnvironment *ENV, int flag,int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc2 pool and update flows

 if flag =1 then only decomposition occurs otherwise not
*****************************************************/
        double tmp,tempResp,respiration;
        double tcflow;
        double tosom2c2, tosom1c2;
        double pheff,bgdefac,anerb;
        double lignindecomrate;
        
        strucc2->Flux.strucc2TOsom1c2.C.totalC=0.0;
        strucc2->Flux.strucc2TOsom2c2.C.totalC=0.0;
        strucc2->Flux.strucc2TOstrucc2.C.totalC=0;
        respiration=0.0;
      // actual decomposition based on intrinsic rate of decomposition
		  if(flag ==1)
      {
			tmp=((strucc2->C.totalC)>(strucc2->parms.strmx))?strucc2->parms.strmx:strucc2->C.totalC;
			// Find decomposition parameters
			pheff=GetPHfac(&strucc2->PHEFF,ENV->pH);
			bgdefac=Getdefac(&strucc2->TEff,&strucc2->SWEFF, ENV->soilRELWC,ENV->soilTEMP);
      anerb=GetAnerbFac(&strucc2->ANEREFF, ENV->PET, ENV->AWC,ENV->drainage);
			lignindecomrate=exp((-1)*(strucc2->parms.pligst)*(strucc2->lignin));
			// calulate total flow from decomposing strucc1 pool
			tcflow=tmp*bgdefac*(strucc2->parms.k)*lignindecomrate*pheff*anerb;
			 // update  total flow out from strucc1
			strucc2->Flux.strucc2TOstrucc2.C.totalC=(-1)*tcflow;
			 // flow to som2c2 with respiration						
			 tosom2c2 =tcflow*strucc2->lignin;
		        // associated respiration losses
		         tempResp=tosom2c2*strucc2->parms.rsplig;
			       respiration+=tempResp;
			 // update flux to som2c2
			 strucc2->Flux.strucc2TOsom2c2.C.totalC=tosom2c2-tempResp; // actual updating of total C pool, E pool is already updated
			 //flow to som1c2 with respiration
			 tosom1c2=tcflow-tosom2c2;
			 // associated respiration losses
		    tempResp=tosom1c2*strucc2->parms.ps1co2;
        respiration+=tempResp;
			 // update flux to som1c1
			strucc2->Flux.strucc2TOsom1c2.C.totalC=tosom1c2-tempResp; // actual updating of total C pool, E pool is already updated							   
		 }
	    strucc2->Flux.hetresp=respiration;
		return;
}

void decomposeSTRUCC1(struct strucc1 *strucc1, struct cropcentEnvironment *ENV, int flag, int Eflag)
{
/**********************************************
 *Purpose:
  Decompose strucc1 pool and update flows

 if flag =1 then only decomposition due to solar radiation take place
 else strucc1 decomposes to som2c1 (slow surface pool)
 *********************************************/

        double tmp,tmp1,tempResp,respiration;
        double tcflow;
        double tosom2c1, tosom1c1;
        double pheff,agdefac;
        double lignindecomrate;
        
        tmp1=(strucc1->C.totalC)*2.5; 
        tmp =line(tmp1,0.0,0.0,strucc1->parms.bioabsorp,1.0); 
        tmp=(tmp>1.0)?1.0:tmp;         
        tcflow=tmp*(ENV->soilrad)*(strucc1->parms.maxphoto)*1e-6; 
        respiration=tcflow*strucc1->parms.pmetabco2; 
       	strucc1->Flux.strucc1TOmetabc1.C.totalC=tcflow-respiration;
        strucc1->Flux.strucc1TOsom1c1.C.totalC=0.0;
        strucc1->Flux.strucc1TOsom2c1.C.totalC=0.0;
        strucc1->Flux.strucc1TOstrucc1.C.totalC=-tcflow;
		if(flag ==1){
			tmp=((strucc1->C.totalC)>(strucc1->parms.strmx))?strucc1->parms.strmx:strucc1->C.totalC;
			// Find decomposition parameters
			pheff=GetPHfac(&strucc1->PHEFF,ENV->pH);
			agdefac=Getdefac(&strucc1->TEff,&strucc1->SWEFF, ENV->surfaceRELWC,ENV->surfaceTEMP);
			lignindecomrate=exp((-1)*(strucc1->parms.pligst)*(strucc1->lignin));
			// calulate total flow from decomposing strucc1 pool
			tcflow=tmp*agdefac*(strucc1->parms.k)*lignindecomrate*pheff;
			 // update  total flow out from strucc1
			 strucc1->Flux.strucc1TOstrucc1.C.totalC-=tcflow;
			 // flow to som2c1 with respiration						
			 tosom2c1 =tcflow*strucc1->lignin;
		        // associated respiration losses
		         tempResp=tosom2c1*strucc1->parms.rsplig;
			 respiration+=tempResp;
			 // update flux to som2c1
			 strucc1->Flux.strucc1TOsom2c1.C.totalC=tosom2c1-tempResp; // actual updating of total C pool, E pool is already updated
			 //flow to som1c1 with respiration
			 tosom1c1=tcflow-tosom2c1;
			 // associated respiration losses
		         tempResp=tosom1c1*strucc1->parms.ps1co2;
			 respiration+=tempResp;
			 // update flux to som1c1
			strucc1->Flux.strucc1TOsom1c1.C.totalC=tosom1c1-tempResp; // actual updating of total C pool, E pool is already updated							   
		}
      strucc1->Flux.hetresp=respiration;
		return;
}

double Getdefac(struct TempEffectParms *Temp, struct SoilWaterEffectParms *swc, double RELWC, double TEMP)
{
/*****************************************************************
 * This function returns agdefac factor based on Crocent layer Environment
 * and Parameters related to Temperature and Moisture
 * I am yet to implement anaerobic effect
 */
double tmp1,tmp2,tmp;
// Calculating Effect of Soil Temperature
tmp=Temp->teff2+(Temp->teff3/3.14)*atan(3.14*Temp->teff4*(30.0-Temp->teff1)); //normalizer
tmp1=Temp->teff2+(Temp->teff3/3.14)*atan(3.14*Temp->teff4*(TEMP-Temp->teff1));// using soil temp
tmp1=tmp1/tmp; // actual factor
tmp1=(tmp1>0.01)?tmp1:0.01;//constraint
// Calculating Effect of soil Water
tmp2=swc->a/(swc->b+swc->c*exp(swc->d*RELWC));
//combined factor
tmp=tmp1*tmp2;
return(tmp);
}

double GetAnerbFac(struct AnaerobicParms *ANEREFF, double PET, double AWC,double drainage)
{
  double slope,anerb,rprpet;
  slope=(1.0-ANEREFF->ANEREF3)/(ANEREFF->ANEREF1-ANEREFF->ANEREF2);
  rprpet=AWC/PET;
  
  if(rprpet<ANEREFF->ANEREF1){
    anerb=1.0;
  }
  else{
    anerb=1.0+slope*(rprpet-ANEREFF->ANEREF1)*(1.0-drainage);
    anerb=(anerb>ANEREFF->ANEREF3)?anerb:ANEREFF->ANEREF3;
  }
  return(anerb);
}


double GetPHfac(struct PHParms *PHEFF,double pH){
  double tmp;
  tmp=(PHEFF->b)+(PHEFF->c)*atan(3.14*(PHEFF->d)*(pH-PHEFF->a))/(3.14);
    return(tmp);
}

int CheckDecomposition(struct minerals *source, struct minerals *flow,struct cropcentEnvironment *ENV, int Eflag)
{
  /*****************************************************
   * This function checks whether decomposition of pool will occur or not based on requirement in flow and enviornment ENV
   * 
   * returns 1 if decomposition will occur
   * return 0 if decomposition does not occur
   * *******************************************************/
   int check;
   switch(Eflag)
   {
   case 1:    
           if((ENV->minN>1e-7)||((source->CN)<=(flow->CN)))
           {
             check =1;
           }
           else
           {
             check=0;
           }
           break;
    case 2:
    case 3:
    case 4:
    Rprintf("Decomposition for P,S and K is not implemented");
    break;
   }
   return(check);
}

double line(double x,double x1,double y1,double x2,double y2){
  double y,m,c;
  m=(y2-y1)/(x2-x1);
  c=y1-m*x1;
  y=m*x+c;
  return(y);
}

double timescaling (double k ,double t)
{
  /*************************************************************
   * Purpose: Calculate Decomposition Rates for different Time Steps
   * 
   * Arguments: 
   * (1)double k Annual Decomposition Rate
   * (2)double t Desired Time Step in minutes
   * 
   * Output:
   * (1)double rate constant for time step t
   * 
   *  Error Checking
   *  t must be larger than 525600.0 minutes
   *  i.e. time step cant be larger than one year
   * 
   * Reference
   * Page 8 of Daycent documentation
   * Olson (1963) as cited by melanie in Daycent documentation
   * *********************************************************/
  double kcont;
  
  if (t >525600.0)
  {
    Rprintf("Time Step is >525600.0 min ( 1 yr)");
  }
  
   if (k <=1.0) 
      {
        kcont=(-1)*log(1.0-k);
        k=1.0-exp((-1)*kcont*t/525600.0);
      }
    else
      {
       k=(k)*(t)/(525600.0);
      }
      return k;
}

double GetMDR(double a,double b,double x1,double x2,double soilrad){
  double output;
  if(soilrad<=x1)
  {
      output=a;
  }
      else
      {
          if(soilrad>=x2)
          {
            output=b;
          }
          else
          {
            output=(x2-soilrad)*(b-a)/(x2-x1);
            output=(output>0.0)?output:((-1)*output);
            output=output+b;
          }
      }
  return(output);
}

double GetMTI(double a,double b,double x1,double x2,double soilrad){
  double output;
  if(soilrad<=x1)
  {
      output=a;
  }
      else
      {
          if(soilrad>=x2)
          {
            output=b;
          }
          else
          {
            output=(soilrad-x1)*(b-a)/(x2-x1);
            output=output+a;
          }
      }
  return(output);
}

// struct flow GetLeachate(double tcflow,double *omlech, double leachedWATER, double sand,struct minerals *E, double rCN, 
// double rCP, double rCS, double rCK)
struct flow GetLeachate (double tcflow, struct minerals *E, double leachedWATER, double sand, struct OrgLeachParms *temp)
{
  struct flow tmp;
  double linten,orglch;
  
  orglch=temp->OMLEACH[0]+temp->OMLEACH[1]*sand;
  linten=leachedWATER/temp->OMLEACH[2];// simplication of Equation 2.141 in the document prepared by Melanie
  linten=(linten<1.0)? linten:1.0;
  tmp.C.totalC=tcflow*orglch*linten;
  tmp.E.CN=E->CN*temp->som1c2toleach.CN;
  tmp.E.CP=E->CP*temp->som1c2toleach.CP;
  tmp.E.CS=E->CS*temp->som1c2toleach.CS;
  tmp.E.CK=E->CK*temp->som1c2toleach.CK;
  return(tmp);
}

double weightavg(double W1,double v1,double W2,double v2)
{
  double avg;
  avg = (W1*v1+W2*v2)/(W1+W2);
  return (avg);
}

double updateCEratio(double C1, double CE1, double C2, double CE2)
{
  double totalC, totalE, tmp;
  totalC=C1+C2;
  totalE=(C1/CE1)+(C2/CE2);
  tmp=totalC/totalE;
  return(tmp);
}

void updateMineralStructure(struct carbon *toupdateC, struct minerals *toupdateE, struct carbon flowC, struct minerals flowE)
{
  toupdateE->CN =updateCEratio(toupdateC->totalC,toupdateE->CN,flowC.totalC,flowE.CN);
  toupdateE->CP =updateCEratio(toupdateC->totalC,toupdateE->CP,flowC.totalC,flowE.CP);
  toupdateE->CS =updateCEratio(toupdateC->totalC,toupdateE->CS,flowC.totalC,flowE.CS);
  toupdateE->CK =updateCEratio(toupdateC->totalC,toupdateE->CK,flowC.totalC,flowE.CK);
  return;
}
void updateCarbonStructure(struct carbon *toupdateC,struct carbon flow)
{
  // first we need to update ratio of unlabl to labl pool
  toupdateC->unlablTOlabl= weightavg(toupdateC->totalC,toupdateC->unlablTOlabl,flow.totalC,flow.unlablTOlabl);
  // Then total C pool
  toupdateC->totalC=toupdateC->totalC+flow.totalC;
  return;
}

void updateCEafterleachate(struct som1c2 *som1c2)
{
  struct carbon *tmpC;
 
  tmpC= &som1c2->C;
  tmpC->totalC+=som1c2->Flux.som1c2TOleachate.C.totalC;
  updateMineralStructure(tmpC, &som1c2->E, som1c2->Flux.som1c2TOleachate.C, som1c2->Flux.som1c2TOleachate.E);
  return;  
}
