#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "AuxcaneGro.h"
#include "crocent.h"

void createNULLmiscanthus(struct miscanthus *miscanthus,int vecsize)
{
   int i;
   miscanthus->leafvec=malloc((vecsize+1)*sizeof(*miscanthus->leafvec));
   miscanthus->stemvec=malloc((vecsize+1)*sizeof(*miscanthus->stemvec));
   miscanthus->rootvec=malloc((vecsize+1)*sizeof(*miscanthus->rootvec));
   miscanthus->rhizomevec=malloc((vecsize+1)*sizeof(*miscanthus->rhizomevec));
  
   for (i = 0; i<=vecsize;i++)
   {
      miscanthus->leafvec[i].newbiomass=0.0;
      miscanthus->leafvec[i].newlitter=0.0;
       miscanthus->leafvec[i].ageinTT=0.0;
       miscanthus->stemvec[i].newbiomass=0.0;
      miscanthus->stemvec[i].newlitter=0.0;
       miscanthus->stemvec[i].ageinTT=0.0;
       miscanthus->rootvec[i].newbiomass=0.0;
      miscanthus->rootvec[i].newlitter=0.0;
       miscanthus->rootvec[i].ageinTT=0.0;
       miscanthus->rhizomevec[i].newbiomass=0.0;
      miscanthus->rhizomevec[i].newlitter=0.0;
       miscanthus->rhizomevec[i].ageinTT=0.0;
       
   }
   
   miscanthus->leaf.biomass=0.0;
   miscanthus->stem.biomass=0.0;
   miscanthus->root.biomass=0.0;
   miscanthus->rhizome.biomass=0.0;
   miscanthus->leaf.litter=0.0;
   miscanthus->stem.litter=0.0;
   miscanthus->root.litter=0.0;
   miscanthus->rhizome.litter=0.0;
   miscanthus->rhizome.carbohydratefraction=0.6; // Lets assume that in the beginning carbohydrate fraction of dry biomass is  60%
   return;
}


/*

double canopyFrostsenescence(struct leaf *leaf, double T0,double T100, double T)
{
  double temp,fraction, senesced;
  fraction = (T0-T)/(T0-T100);
  temp = fraction>1.0? 1.0:(fraction <0.0? 0.0:fraction);
  senesced=(leaf->biomass)*fraction;
  return(senesced);
}







double getDailyRemobilization(double current, double k, double senesced, double remobFac)
{
  double tmp;
  tmp=0.0;
  if(k<0.0)
  {
    tmp+=current*k;
    tmp=fabs(tmp);
    current-=tmp; // here i am up
    if(tmp>=current)error("One of the remobilization organ has become zero\n");
  }
  if(senesced>0.0)
  {
    tmp+=senesced*remobFac;
  }
}

double getDailyGrowth(double current, double netDailyCanopyAssim, double mResp, double k, double TotalRemobilize)
{
  double tmp;
  if(k>=0)
  {
    tmp=(netDailyCanopyAssim+TotalRemobilize)*k;
  }
  else
  {
    tmp=current*k;
    if(fabs(tmp)>=currrent) error("One of the remobilization organ has become zero\n");
  }
  tmp-=mResp;
  return (tmp);
}
// Function to calculate  complete structure of the new addition for a one particular day
struct plant DailyPlantBiomassGrowth(struct plant *current, struct litter *leaflitter, int Nday, int TTc, struct crop_phenology DBP)
{
  struct plant *tmp;
  int tmpN;
  tmpN=Nday*24;
  // Leaves
   tmp->leaf.litter=getDailySenescence(leaflitter,tmpN, current->leaf.parms.criticalTT, current->leaf.parms.remobFac);
   current->leaf.parms.k=getDBP(TTc);
   Leafremobilized=getDailyRemobilization(current->leaf.biomass, current->leaf.parms.k,newsenesced, current->leaf.parms.remobFac);
   // Stem
   tmp->stem.litter=getDailySenescence(stemlitter,tmpN, current->stem.parms.criticalTT, current->stem.parms.remobFac);
   current->stem.parms.k=getDBP(TTc);
   Stemremobilized=getDailyRemobilization(current->stem.biomass, current->stem.parms.k,newsenesced, current->stem.parms.remobFac);
   // Root
   tmp->root.litter=getDailySenescence(rootlitter,tmpN, current->root.parms.criticalTT, current->root.parms.remobFac);
   current->root.parms.k=getDBP(TTc);
   Rootremobilized=getDailyRemobilization(current->root.biomass, current->root.parms.k,newsenesced, current->root.parms.remobFac);
    // Rhizome
   tmp->rhizome.litter=getDailySenescence(rhizomelitter,tmpN, current->rhizome.parms.criticalTT, current->rhizome.parms.remobFac);
   current->rhizome.parms.k=getDBP(TTc);
   Rhizomeremobilized=getDailyRemobilization(current->rhizome.biomass, current->rhizome.parms.k,newsenesced, current->rhizome.parms.remobFac);
   
   TotalRemob=Leafremobilized+Stemremobilized+Rootremobilized+Rhizomeremobilized;
   
   tmp->leaf.biomass=getDailyGrowth(current->leaf.biomass,dailynetassim, leafMresp,current->leaf.parms.k, TotalRemob);
   tmp->stem.biomass=getDailyGrowth(current->stem.biomass,dailynetassim, stemMresp,current->stem.parms.k, TotalRemob);
   tmp->root.biomass=getDailyGrowth(current->root.biomass,dailynetassim, rootMresp,current->root.parms.k, TotalRemob);
   tmp->rhizome.biomass=getDailyGrowth(current->rhizome.biomass,dailynetassim, rhizomeMresp,current->rhizome.parms.k, TotalRemob);
   
  // here I am updating CE content of the daily plant material
  // Get minimum CE ratios as suggested by partitionin coefficients
  tmp->leaf.E = DBP->leaf.E;
  tmp->stem.E = DBP->stem.E;
  tmp->root.E = DBP ->root.E;
  tmp->rhizome.E = DBP ->rhizome.E;
  // DO I need to do above isnt it same aq equal to litter CE content?
  
  // Calculate E demand based on new biomass growh in the current day
  maxNdemand=(tmp->leaf.biomass)/(tmp->leaf.E.CN)+(tmp->stem.biomass)/(tmp->stem.E.CN)
           +(tmp->root.biomass)/(tmp->root.E.CN)+(tmp->rhizome.biomass)/(tmp->rhizome.E.CN);
  // I need to calculate N demand in gram /m2
 // to convert Mg/ha of biomass to gram C/m2 I need to multiple
 // by 0.01 (Mg/ha to gram/m2) and 0.4 (carbohydrate to C)
 Ndemand=Ndemand*0.01*0.4;      
           
 NfromRemob= (Leafremobilized/current->leaf.E.CN)+ (Stemremobilized/current->stem.E.CN)
             + (Rootremobilized/current->root.E.CN)+ (Rhizomeremobilized/current->rhizome.E.CN);
             
 // unit conversion Mg biomass/ha to gram C/m2
 NfromRemob=NfromRemob*0.01*0.04;
 // How much from Soil is Required?
 NdemandfromSoil=maxNdemand-NfromRemob;
 Navailability=getAvailableN(&CropCent_soilML);
    if(NdemandfromSoil<0.0)
        { 
          extraNforRhizome=(-1)*NdemandfromSoil;
          NdemandfromSoil=0.0
        }
     else
     {
          extraNforRhizome=0.0;
          NavailablefromSoil=get_max_available_N_from_CropCent(&cropcent);
          if(NavailablefromSoil>NdemandfromSoil)
          {
            remove_CropCent_E(&cropcent,NdemandfromSoil);
          }
          else
          {
            remove_CropCent_E(&cropcent,NavailablefromSoil);
            addjustPlantEuptake(tmp,NavailablefromSoil); // now E uptake by plant is limited by maximum soil E availability
          } 
     }
     return;
}

struct plant DailyPlantNutrientUptake (struct plant *current, struct plant *daily, struct crop_phenology *DBP, double TTc)
{
  // CE cpmcemtratopm of different plant components can be read directly from partitioning table
  daily->leaf.E = DBP->leaf.E;
  daily->stem.E = DBP->stem.E;
  daily->root.E = DBP ->root.E;
  daily->rhizome.E = DBP ->rhizome.E;
  
  // Calculate E demand based on new biomass growh in the current day
  Ndemand=(daily->leaf.biomass)/(daily->leaf.E.CN)+(daily->stem.biomass)/(daily->stem.E.CN)
           +(daily->root.biomass)/(daily->root.E.CN)+(daily->rhizome.biomass)/(daily->rhizome.E.CN);
 
 // I need to calculate N demand in gram /m2
 // to convert Mg/ha of biomass to gram C/m2 I need to multiple
 // by 0.01 (Mg/ha to gram/m2) and 0.4 (carbohydrate to C)
 Ndemand=Ndemand*0.01*0.4;
 
 NfromRemob=
 
 
  
  
  
}


double dailyEdemand(double plant *plant, double plant *dailygrowth,)
{
  
}
*/
double getThermaltime (double temp, double Tbase)
{
  double res;
  res =(temp<=Tbase)? 0.0:(temp-Tbase);
  return(res);
}


double getThermalSenescence(double criticalTT, double currentTT, double biomass, double dailyfractionalloss)
{
  /*************************************
   * Purpose
   * To calculate senescence as a constant fraction of exisiting biomass after a critical threshold temperature
   * Input:
   * criticalTT: This is critical Thermal temperature, beyond which senescence begins
   * currentTT: Current thermal TT or phenological stage
   * biomass: Exisiting biomass
   * dailyfractionalloss: Fraction of biomass which will be lost
   * Output:
   * 
   * Returns senesced biomass
   * 
   * **********************************/
   double tmp;
  if(currentTT <= criticalTT)
  {
   tmp=0.0;
  }
  else
  {
   tmp=biomass*dailyfractionalloss;
  }
  return(tmp);
}

double getFrostdamage(double T0, double T100,double T, double biomass)
{
   /*************************************
   * Purpose
   * To calculate senescence as a fraction of exisiting biomass due to frost
   * Input:
   * T0: This is maximum temperature when frost damage occurs, This also represent Temperature 
   * damage will be zero percentage of exisiting biomass
   * T100: This is minimum temperature where all the existing biomass senescee (i.e. 100% senescence)
   * biomass: Exisiting biomass
   *
   * Output:
   * 
   * Returns senesced biomass
   * 
   * **********************************/
  double temp,fraction;
  fraction = (T0-T)/(T0-T100);
  temp = fraction>1.0? 1.0:(fraction <0.0? 0.0:fraction);
  return(temp*biomass);
}

double canopyNsenescence(struct leaf *leaf, double SLA, double kN,  double leafNsen)
{
  /*****************************************************
   * Purpose:
   * 
   * Purpose of this function is to calculate additional N senescence due to N deficiency
   * It uses current value of average canopy N, use this and an exponential coefficient to
   * determine vertical distribution of leaf N canopy.
   * If this vertical distrbution results in leaf N of lower canopy which is less than a threshold value
   *  then an leaf biomass (equivalent to LAI which is having lower than threshold leaf N conc.) is senesced
   * Input
   * leaf: pointer to leaf structure
   * SLA: specific leaf area (in ha/Mg)
   * kN:  coefficient determining exponentially declining leaf N conc (dimensionless)
   * leafNsen: any leaf with leaf N conc lower than leafNsen will be senesced (mmol N m-2)
   * Output/return
   * Senesced leaf biomass (Mg/ha)
   * ***************************************************/
  double tmp;
  double laimax,laicurrent;
  double leafN;
  leafN=leaf->biomassE.CN;
  laimax=(1/kN)*log(1+(kN*leafN/leafNsen));
  laicurrent=leaf->biomass*SLA;
  if(laicurrent<=laimax)
  {
    tmp=0.0;
  }
  else
  {
    tmp=(laicurrent-laimax)/SLA;
  }
  return(tmp);
}


double getLeafSenescence(struct leaf *leaf, double criticalTT, double senefraction,  double Temp, struct frostParms *frostparms, double TT,struct canopyparms *canopyparm)
{
  /******************************************************
   * Purpose:
   * This function calculates Leaf Senescence
   * 
   * Input Arguments:
   * stem : is a pointer to stem structure
   * criticalTT: is critical value of accumulated thermal time where senesence begins
   * Temp: minimum daily temperature
   * frostparms: is a pointer to structure containing parameters to somulate damage due to frost
   * TT: is current [daily] value of accumutaed thermal time
   * 
   * Output/Returns
   * Biomass  to be semesced due to aging (thermal time) and frost, together
   * ****************************************************/
  double ttsen, frostsen, Nsen;
  double totalsen;
  Nsen=0.0;
  ttsen=getThermalSenescence(criticalTT,TT,leaf->biomass,senefraction);
//  Rprintf("TSEN=%f,critcalT=%f, TT=%f, biomass=%f, senfraction=%f \n",ttsen,criticalTT, TT,leaf->biomass,senefraction);
  frostsen=getFrostdamage(frostparms->leafT0,frostparms->leafT100, Temp,leaf->biomass);
  // Nsen=canopyNsenescence(leaf, canopyparm->SLA, canopyparm->kN,canopyparm->leafNsen);
  totalsen=ttsen+frostsen+Nsen;
  totalsen= (totalsen>(leaf->biomass))?(leaf->biomass):totalsen;
  return(totalsen);
}

double getStemSenescence(struct stem *stem, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT)
{
  /******************************************************
   * Purpose:
   * This function calculates Stem Senescence
   * 
   * Input Arguments:
   * stem : is a pointer to stem structure
   * criticalTT: is critical value of accumulated thermal time where senesence begins
   * Temp: minimum daily temperature
   * frostparms: is a pointer to structure containing parameters to somulate damage due to frost
   * TT: is current [daily] value of accumutaed thermal time
   * 
   * Output/Returns
   * Biomass to be semesced due to aging (thermal time) and frost, together
   * ****************************************************/
  double ttsen, frostsen;
  double totalsen;
  frostsen=0.0;
  ttsen=getThermalSenescence(criticalTT,TT,stem->biomass,senefracion);
  frostsen=getFrostdamage(frostparms->stemT0, frostparms->stemT100, Temp,stem->biomass);
  totalsen=ttsen+frostsen;
  totalsen= (totalsen>(stem->biomass))?(stem->biomass):totalsen;
  return(totalsen);
}

double getRootSenescence(struct root *root, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT)
{
  /******************************************************
   * Purpose:
   * This function calculates Root Senescence
   * 
   * Input Arguments:
   * stem : is a pointer to stem structure
   * criticalTT: is critical value of accumulated thermal time where senesence begins
   * Temp: minimum daily temperature
   * frostparms: is a pointer to structure containing parameters to somulate damage due to frost
   * TT: is current [daily] value of accumutaed thermal time
   * 
   * Output/Returns
   * Biomassto be semesced due to aging (thermal time) and frost, together
   * ****************************************************/
  double ttsen, frostsen;
  double totalsen;
  frostsen=0.0;
  ttsen=getThermalSenescence(criticalTT,TT,root->biomass,senefracion);
  frostsen=getFrostdamage(frostparms->rootT0, frostparms->rootT100, Temp,root->biomass);
  totalsen=ttsen+frostsen;
  totalsen= (totalsen>(root->biomass))?(root->biomass):totalsen;
  return(totalsen);
}

double getRhizomeSenescence(struct rhizome *rhizome, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT)
{
  /******************************************************
   * Purpose:
   * This function calculates Root Senescence
   * 
   * Input Arguments:
   * stem : is a pointer to stem structure
   * criticalTT: is critical value of accumulated thermal time where senesence begins
   * Temp: minimum daily temperature
   * frostparms: is a pointer to structure containing parameters to somulate damage due to frost
   * TT: is current [daily] value of accumutaed thermal time
   * 
   * Output/Returns
   * Biomass  to be senesced due to aging (thermal time) and frost, together
   * ****************************************************/
  double ttsen, frostsen;
  double totalsen;
  frostsen=0.0;
  ttsen=getThermalSenescence(criticalTT,TT,rhizome->biomass,senefracion);
  frostsen=getFrostdamage(frostparms->rhizomeT0, frostparms->rhizomeT100, Temp,rhizome->biomass);
  totalsen=ttsen+frostsen;
  totalsen= (totalsen>(rhizome->biomass))?(rhizome->biomass):totalsen;
  return(totalsen);
}

void updatedailyvec(struct dailyvec *dailyvec,double newbiomass,double newlitter, int N,double delTT)
{
  int i;
  for (i=0; i<=N;i++)
  {
     dailyvec[i].ageinTT+=delTT;
  }
  dailyvec[N].newbiomass=newbiomass;
  dailyvec[N].newlitter=newlitter;
  return;
}

void updatebiomass(double *res,double toadd,double toremove)
{
  *res=*res+toadd-toremove;
  return;
}

double newbiomass(double AcanopyNet,double k, double GrowthRespCoeff)
{
  double tmp;
  
  tmp=(k>=0.0)?AcanopyNet*k/(1+GrowthRespCoeff):0.0;
  return(tmp);
}

void getfrostparms(struct frostParms *frostparms)
{
   frostparms->leafT0=0.0;
   frostparms->leafT100=-5.0;
   frostparms->stemT0=0.0;
   frostparms->stemT100=-10.0;
   frostparms->rootT0=-200.0;
   frostparms->rootT100=-200.0;
   frostparms->rhizomeT0=-200.0;
   frostparms->rhizomeT100=-200.0;
   return;
}
void getsenescenceparms(struct senthermaltemp *senparms)
{
    senparms->leafcriticalT = 3600;
    senparms->leaffr=0.05;
    senparms->stemcriticalT = 3600;
    senparms->stemfr=0.02;
    senparms->rootcriticalT = 3600;
    senparms->rootfr=0.05;
    senparms->rhizomecriticalT = 3600;
    senparms->rhizomefr=0.001;
  return;
}


double CalculateGrowthResp(double newbiomass,double growthRespCoeff)
{
  double tmp;
  tmp=(newbiomass>0.0)?newbiomass*growthRespCoeff:0.0;
  return(tmp);
}
// struct miscanthus 
void dailymiscanthus(struct miscanthus *miscanthus,double coefs[25],double TherPrds[6], double TherTime, double Temp,double dailynetassim,
struct senthermaltemp *senparms, struct canopyparms *canopyparms, struct frostParms *frostparms, int N, double delTT,
struct respirationParms *RESP, int emergence)

{

  // double remobStem,remobRoot,remobRhiz; unused
  // remobStem=canopyparms->remobFac; set but not used
  // remobRoot=canopyparms->remobFac; set but not used
  // remobRhiz=canopyparms->remobFac; set but not used
  double deadleaf,deadstem,deadroot,deadrhiz;
  double dailysenesced;
  // double remobilized; unused
  double totalassimilate;
  struct crop_phenology cropdbp;
  // double newleaf,newstem,newrhiz,newroot; unused
  double kLeaf,kStem,kRoot,kRhizome;
  double newLeaf,newStem,newRhizome,newRoot;
  double newLeaflitter,newStemlitter,newRhizomelitter,newRootlitter;
  double totalmaintenance;
  double RemobilizedFromLitter,RemobilizedFromRhizome;
  double SumofKpart,Dailybalance;

// double getStemSenescence(struct stem *stem, struct littervec *littervec, double criticalTT, double Temp, double remobfa, struct frostParms *frostparms, int N)
 // calculate senescing biomass for today based on Thermal Time, N conce. and Frost conditions 
 
   getfrostparms(frostparms);
   getsenescenceparms(senparms);
   
   
   if(emergence ==1)
   {
          cropcent_dbp(coefs,TherPrds,TherTime, &cropdbp);
          kLeaf = cropdbp.DBP.kLeaf;
          kStem = cropdbp.DBP.kStem;
          kRoot = cropdbp.DBP.kRoot;
          kRhizome = cropdbp.DBP.kRhiz;

           // calculate senescence, Remobilization due to senescece will contribute to growth of new plant organs
          deadleaf=getLeafSenescence(&miscanthus->leaf,senparms->leafcriticalT,senparms->leaffr, Temp, frostparms,TherTime,canopyparms);
          deadstem=getStemSenescence(&miscanthus->stem,senparms->stemcriticalT,senparms->stemfr, Temp, frostparms,TherTime);
          deadroot=getRootSenescence(&miscanthus->root, senparms->rootcriticalT,senparms->rootfr, Temp, frostparms,TherTime);
          deadrhiz=getRhizomeSenescence(&miscanthus->rhizome,senparms->rhizomecriticalT,senparms->rhizomefr, Temp, frostparms,TherTime);    
          dailysenesced=deadleaf+deadstem+deadroot+deadrhiz;
          //Calculate how much of dead biomass is remobilized based on a remobilization factor
          RemobilizedFromLitter = dailysenesced*canopyparms->remobFac; // assuming all the dead organs remobilizes same fraction
         
         // Check and calculate whether carbohydrate in Rhizome is being remobilized baksed on K(negative value of partitioning coefficient)
         if(kRhizome>=0)
          {
            RemobilizedFromRhizome=0;          
          }
          else
          {
            // I need to multiply by (-1) because kRhizome is negative
            RemobilizedFromRhizome=(-1)*kRhizome*miscanthus->rhizome.biomass;
          }
          
          // Total maintenance respiration, which is a temperature dependet function of existing biomass
          totalmaintenance=miscanthus->autoresp.stemmaint+miscanthus->autoresp.rootmaint+miscanthus->autoresp.rhizomemaint;
          // Total assimilate includes net canopy assimilation and remobilization from litter and rhizome, also it must satisfy total maintenance respiration
          totalassimilate=dailynetassim+RemobilizedFromLitter+RemobilizedFromRhizome-totalmaintenance;
          
          // If total assimilate for growth (& growth respiration) is negative then this is provided by rhizome only (storage carb.) leaving other parts intacts
          if(totalassimilate<=0.0)
          {
          newLeaf=0.0;
          miscanthus->autoresp.stemgrowth=0.0;
          newStem=0.0;
          miscanthus->autoresp.rootgrowth=0.0;
          newRoot=0.0;
          miscanthus->autoresp.rhizomegrowth=0.0;
          // New rhizome must provide for total negative assimilate. Also we need to update remobilization
          newRhizome=totalassimilate+RemobilizedFromRhizome;
          miscanthus->autoresp.rhizomegrowth=0.0;
          }
          else
          {
          SumofKpart=((kLeaf>0)?kLeaf:0)+((kStem>0)?kStem:0)+((kRoot>0)?kRoot:0)+((kRhizome>0)?kRhizome:0);
            if(SumofKpart!=1)
            {
            error("Sum of Positive partitioning coefficient is not one");
            }
          newLeaf=newbiomass(totalassimilate,kLeaf,0.0);          
          newStem=newbiomass(totalassimilate,kStem,RESP->growth.stem);
          miscanthus->autoresp.stemgrowth=CalculateGrowthResp(newStem,RESP->growth.stem);
          newRoot=newbiomass(totalassimilate,kRoot,RESP->growth.root);
          miscanthus->autoresp.rootgrowth=CalculateGrowthResp(newRoot,RESP->growth.stem);
          // Make sure that remobilization termis taken care of in the calculation of newRhizome
          newRhizome=(kRhizome>0)?newbiomass(totalassimilate,kRhizome,RESP->growth.rhizome):(-1)*RemobilizedFromRhizome;
          
          miscanthus->autoresp.rhizomegrowth=CalculateGrowthResp(newRhizome,RESP->growth.stem);
          }
           
         // Now we know growth respiration of each component & we can update total autotrophic respiration
          miscanthus->autoresp.total=miscanthus->autoresp.leafdarkresp+totalmaintenance+miscanthus->autoresp.stemgrowth+miscanthus->autoresp.rootgrowth+miscanthus->autoresp.rhizomegrowth;
         // Here, we are updating net primary productivity
         miscanthus->NPP=miscanthus->GPP-miscanthus->autoresp.total;
          
        // Here, we are evaluating new litter, This is fraction of dead biomass which was not remobilized  
          newLeaflitter=(deadleaf>0)?deadleaf*(1-canopyparms->remobFac):0.0;
          newStemlitter=(deadleaf>0)?deadstem*(1-canopyparms->remobFac):0.0;
          newRootlitter=(deadroot>0)?deadroot*(1-canopyparms->remobFac):0.0;
          newRhizomelitter=(deadrhiz>0)?deadrhiz*(1-canopyparms->remobFac):0.0;
          

          Dailybalance=newLeaf- deadleaf+newRoot- deadroot+newStem- deadstem+newRhizome- deadrhiz+newLeaflitter+newStemlitter+newRootlitter + newRhizomelitter;
         Dailybalance=Dailybalance-miscanthus->NPP;
         if(Dailybalance>1e-10)
          {
          Rprintf("\nNPP and Daily Change inBiomass is not matching & difference is %f\n", Dailybalance);
          Rprintf("Thermal Time = %f, GPP = %f, Autotrophic Respiration = %f, NPP = %f, Remobilized from Litter = %f, Remobilized from Rhizome = %f \n", TherTime,miscanthus->GPP, miscanthus->autoresp.total,miscanthus->NPP,RemobilizedFromLitter,RemobilizedFromRhizome);
          Rprintf("kLeaf=%f, kstem=%f, kRoot=%f, kRhizome=%f \n", kLeaf, kStem, kRoot,kRhizome);
          Rprintf("NewLeaf = %f Dead Leaf=%f, newLeafLitter=%f\n",newLeaf, deadleaf,newLeaflitter); 
          Rprintf("NewStem = %f Dead Stem=%f, newStemLitter=%f\n",newStem, deadstem,newStemlitter);
          Rprintf("NewRoot = %f Dead Root=%f, newRootLitter=%f\n",newRoot, deadroot,newRootlitter);
          Rprintf("NewRhizome = %f Dead Rhizome=%f, newRhizomeLitter=%f\n",newRhizome, deadrhiz,newRhizomelitter);
          Rprintf("LeafDarkResp=%f, Total maintenance (ExceptLeaf) = %f, StemGrowthResp=%f, RootGrowthResp=%f, RhizGrowthResp=%f\n",miscanthus->autoresp.leafdarkresp,totalmaintenance,miscanthus->autoresp.stemgrowth,miscanthus->autoresp.rootgrowth,miscanthus->autoresp.rhizomegrowth);
              Rprintf("Daily Biomas Balance Gain = %f", Dailybalance);
              Rprintf("--------Emergence= %i-, N= %i---------------------\n",emergence, N);
          }

          // Adding new biomass of green components
          UpdateStandingbiomass(&miscanthus->leaf.biomass, newLeaf);
          UpdateStandingbiomass(&miscanthus->stem.biomass, newStem);
          UpdateStandingbiomass(&miscanthus->root.biomass, newRoot);
          UpdateStandingbiomass(&miscanthus->rhizome.biomass, newRhizome);
          
          // Subtracting dead biomass from the green components
          // before thant I must multiply all the dead leaf components by -1, so I can still use 
          // Updatestandingbiomass function for SUBTRACTING instead of ADDING
          
          deadleaf*=(-1);
          deadstem*=(-1);
          deadroot*=(-1);
          deadrhiz*=(-1);
          
          UpdateStandingbiomass(&miscanthus->leaf.biomass, deadleaf);
          UpdateStandingbiomass(&miscanthus->stem.biomass, deadstem);
          UpdateStandingbiomass(&miscanthus->root.biomass, deadroot);
          UpdateStandingbiomass(&miscanthus->rhizome.biomass, deadrhiz);
          
          
          // Updating standing biomass of litter  components
          UpdateStandingbiomass(&miscanthus->leaf.litter, newLeaflitter);
          UpdateStandingbiomass(&miscanthus->stem.litter, newStemlitter);
          UpdateStandingbiomass(&miscanthus->root.litter, newRootlitter);
          UpdateStandingbiomass(&miscanthus->rhizome.litter, newRhizomelitter);
          
        
          
   }
   else // Dormant stange simulation, where rhizome provides for respiration loss
   {
          miscanthus->stem.biomass=0.0;
          miscanthus->autoresp.stemgrowth=0;
          miscanthus->leaf.biomass=0.0;
          miscanthus->autoresp.leafdarkresp=0.0;
          
          //Perhaps I can implement senescence rate of belowground components during dormant stage?
//          miscanthus->root.biomass=0.0;
          miscanthus->autoresp.rootgrowth=0;
//          miscanthus->rhizome.biomass=0.0;
          miscanthus->autoresp.rhizomegrowth=0;
          
          totalmaintenance=miscanthus->autoresp.stemmaint+miscanthus->autoresp.rootmaint+miscanthus->autoresp.rhizomemaint;
          miscanthus->autoresp.total=miscanthus->autoresp.leafdarkresp+totalmaintenance+miscanthus->autoresp.stemgrowth+miscanthus->autoresp.rootgrowth+miscanthus->autoresp.rhizomegrowth;
          miscanthus->NPP=miscanthus->GPP-miscanthus->autoresp.total;
          newRhizome=(-1)* miscanthus->autoresp.total;
          UpdateStandingbiomass(&miscanthus->rhizome.biomass, newRhizome);
          if(miscanthus->rhizome.biomass <0)error("rhizome has become negative");
   } 
    return;
}

void UpdateStandingbiomass (double *standing, double newbiomass)
{
  *standing= *standing +newbiomass;
   return;
}


void UpdateStandingLeaf(struct leaf *leaf, double newbiomass, double deadleaf, double remobFactor)
{
  leaf->biomass+=newbiomass-deadleaf;
  leaf->litter+=deadleaf*(1-remobFactor);
  return;
}

void UpdateStandingStem(struct stem *stem, double newbiomass, double deadstem, double remobFactor)
{
  stem->biomass+=newbiomass-deadstem;
  stem->litter+=deadstem*(1-remobFactor);
  return;
}

void UpdateStandingRoot(struct root *root, double newbiomass, double deadroot, double remobFactor)
{
  root->biomass+=newbiomass-deadroot;
  root->litter+=deadroot*(1-remobFactor);
  return;
}

void UpdateStandingRhizome(struct rhizome *rhizome, double newbiomass, double deadrhizome, double remobFactor)
{
  rhizome->biomass+=newbiomass-deadrhizome;
  rhizome->litter+=deadrhizome*(1-remobFactor);
  return;
}

int CheckEmergence(struct dailyclimate *dailyclimate, double EmergTemperature)
{
  int flag;
  if(((dailyclimate->minimumTemp)>EmergTemperature)&&((dailyclimate->doy)>90)&&((dailyclimate->doy)<270)){
    flag =1;
  }
  else
  {
    flag = 0;
  }
  return(flag);
}


void getdailyclimate(struct dailyclimate *dailyclimate, int *doy,double *solar,double *temp, double *rh, double *windspeed, double*precip, int currenthour, int vecsize)
{
  //For the first hour after planting
  int i,indx;
  if ((currenthour==0)||(currenthour==vecsize))
      {
         dailyclimate->doy=*(doy+currenthour);
         dailyclimate->solar=*(solar+currenthour);
         dailyclimate->temp=*(temp+currenthour);
         dailyclimate->rh=*(rh+currenthour);
         dailyclimate->windspeed=*(windspeed+currenthour);
         dailyclimate->precip=*(precip+currenthour);
         dailyclimate->minimumTemp=*(temp+currenthour);
      }
   else
      {
        dailyclimate->solar=0.0;
        dailyclimate->temp=0.0;
        dailyclimate->minimumTemp=*(temp+currenthour);
        dailyclimate->rh=0.0;
        dailyclimate->windspeed=0.0;
        dailyclimate->precip=0.0;
          for(i=1;i<=24;i++)
            {
              indx = currenthour-i+1;      
              dailyclimate->solar+=*(solar+indx);
              dailyclimate->temp+=*(temp+indx);
              dailyclimate->rh+=*(rh+indx);
              dailyclimate->windspeed+=*(windspeed+indx);
              dailyclimate->precip+=*(precip+indx);
              dailyclimate->minimumTemp=(dailyclimate->minimumTemp>(*(temp+indx)))?(*(temp+indx)):dailyclimate->minimumTemp;
            }
            dailyclimate->solar=dailyclimate->solar/24.0;
            dailyclimate->temp=dailyclimate->temp/24.0;
            dailyclimate->rh=dailyclimate->rh/24.0;
            dailyclimate->windspeed= dailyclimate->windspeed/24.0;
            dailyclimate->doy=*(doy+currenthour);
      }
     return;     
}


void dailymiscanthusupdate(struct miscanthus *miscanthus,struct miscanthus *deltamiscanthus)
{
  double toadd,toremove;
  
  toadd=deltamiscanthus->leaf.biomass;
  toremove=deltamiscanthus->leaf.litter;
  updatebiomass(&miscanthus->leaf.biomass,toadd,toremove);
  updatebiomass(&miscanthus->leaf.litter,toremove,0.0);
  
  toadd=deltamiscanthus->stem.biomass;
  toremove=deltamiscanthus->stem.litter;
  updatebiomass(&miscanthus->stem.biomass,toadd,toremove);
  updatebiomass(&miscanthus->stem.litter,toremove,0.0);
  
  toadd=deltamiscanthus->root.biomass;
  toremove=deltamiscanthus->root.litter;
  updatebiomass(&miscanthus->root.biomass,toadd,toremove);
  updatebiomass(&miscanthus->root.litter,toremove,0.0);
  
  toadd=deltamiscanthus->rhizome.biomass;
  toremove=deltamiscanthus->rhizome.litter;
  updatebiomass(&miscanthus->rhizome.biomass,toadd,toremove);
  updatebiomass(&miscanthus->rhizome.litter,toremove,0.0);
   return;
}




void updatedormantstage(struct miscanthus *miscanthus)
{
  double LostinRespiration;
  // double availablecarb; unused
  // double newcarb; unused
  LostinRespiration=miscanthus->autoresp.stemmaint + miscanthus->autoresp.rootmaint +  miscanthus->autoresp.rhizomemaint;
  // This loss will occur at the expense of carbohydrate fraction of rhizome, resulting in change in the carbohydrate fraction of the rhizome
  // availablecarb= miscanthus->rhizome.biomass*miscanthus->rhizome.carbohydratefraction;
  // newcarb=availablecarb-LostinRespiration; // unused
  miscanthus->rhizome.biomass= miscanthus->rhizome.biomass- LostinRespiration;
 
  if((miscanthus->rhizome.carbohydratefraction)<0.05)miscanthus->rhizome.carbohydratefraction=0.05;
  if(miscanthus->rhizome.biomass <0)error("rhizome has become negative");


  miscanthus->autoresp.stemgrowth=0;
  miscanthus->autoresp.rootgrowth=0;
  miscanthus->autoresp.rhizomegrowth=0;
  miscanthus->autoresp.leafdarkresp=0.0;
  miscanthus->autoresp.total= LostinRespiration;
  miscanthus->NPP=miscanthus->GPP-miscanthus->autoresp.total;
  // Do i need to specify root death rate ? or probably Ican simply kill the roots at the time of harvest
  
}
