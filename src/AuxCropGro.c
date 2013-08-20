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
  if(temp<=Tbase)
  {
    res=0.0;
  }
  else
  {
    res=(temp-Tbase);
  }
  return(res);
}


double getThermalSenescence(struct dailyvec *dailyvec,double criticalTT, int N, double additionalsenescence)
{
  double tmp, tmpupdate;
  int i;
  tmp=0.0;
  
  // Killing based on thermal age
  for( i = 0; i <=N; i++)
  {
    if(dailyvec[i].ageinTT >= criticalTT)
    {
      tmp+=dailyvec[i].newbiomass;
      dailyvec[i].newbiomass=0.0;
    }
  }
  return(tmp);
}



double getStemSenescence(struct stem *stem, struct dailyvec *dailyvec, double criticalTT, double Temp, double remobfa, struct frostParms *frostparms, int N)
{
  double ttsen, frostsen,frdead;
  double totalsen;
  double extrasene;
  frostsen=0.0;
  ttsen=getThermalSenescence(dailyvec, criticalTT,N,extrasene);
  totalsen=ttsen+frostsen;
  return(totalsen);
}


double getRootSenescence(struct root *root, struct dailyvec *dailyvec, double criticalTT, double Temp, double remobfa, struct frostParms *frostparms, int N)
{
  double ttsen, frostsen,frdead;
  double totalsen;
  double extrasene;
  frostsen=0.0;
  ttsen=getThermalSenescence(dailyvec, criticalTT,N,extrasene);
  totalsen=ttsen+frostsen;
  return(totalsen);
}


double getRhizomeSenescence(struct rhizome *rhiz, struct dailyvec *dailyvec, double criticalTT, double Temp, double remobfa, struct frostParms *frostparms, int N)
{
  double ttsen, frostsen,frdead;
  double totalsen;
  double extrasene;
  frostsen=0.0;
  ttsen=getThermalSenescence(dailyvec, criticalTT,N,extrasene);
  totalsen=ttsen+frostsen;
  return(totalsen);
}




double canopyNsenescence(struct leaf *leaf, double SLA, double kN,  double leafNsen)
{
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


double getLeafSenescence(struct leaf *leaf, struct dailyvec *dailyvec, double criticalTT, double Temp, 
                     struct canopyparms *canopyparm, struct frostParms *frostparms, int N, double TT)
{
  double ttsen, Nsen,frostsen;
  double totalsen;
  double frdead; 
  double extrasene;
//  Nsen=canopyNsenescence(leaf, canopyparm->SLA, canopyparm->kN,canopyparm->leafNsen);
//  frdead=s->ostdamage(frostparms->leafT0,frostparms->leafT100,Temp);
//  extrasene = Nsen+frdead;
     extrasene=0.0;
  ttsen=getThermalSenescence(dailyvec, criticalTT,N,extrasene);
  frostsen=(leaf->biomass)*frdead;
  totalsen=ttsen+Nsen+frostsen;
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
  tmp=AcanopyNet*k/(1+GrowthRespCoeff);
  return(tmp);
}

double CalculateGrowthResp(double newbiomass,double growthRespFactor)
{
  return(newbiomass*growthRespFactor);
}
// struct miscanthus 
void dailymiscanthus(struct miscanthus *miscanthus,double coefs[25],double TherPrds[6], double TherTime, double Temp,double dailygrossassim,
struct senthermaltemp *senparms, struct canopyparms *canopyparms, struct frostParms *frostparms, int N, double delTT,int emergence,
struct respirationParms *RESP)

{

  double remobStem,remobRoot,remobRhiz;
  remobStem=canopyparms->remobFac;
  remobRoot=canopyparms->remobFac;
  remobRhiz=canopyparms->remobFac;
  double deadleaf,deadstem,deadroot,deadrhiz;
  double dailysenesced;
  double remobilized;
  double totalassimilate;
  struct crop_phenology cropdbp;
  double newleaf,newstem,newrhiz,newroot;
  double kLeaf,kStem,kRoot,kRhizome;
  double newLeaf,newStem,newRhizome,newRoot;
  double newLeaflitter,newStemlitter,newRhizomelitter,newRootlitter;
  double totalmaintenance;
  double newsdeadbiomass, newalivebiomass;
  int check;

// double getStemSenescence(struct stem *stem, struct littervec *littervec, double criticalTT, double Temp, double remobfa, struct frostParms *frostparms, int N)
 // calculate senescing biomass for today based on Thermal Time, N conce. and Frost conditions 
   
switch(emergence)
    {
      case 1:
                   deadleaf=getLeafSenescence(&miscanthus->leaf, miscanthus->leafvec, senparms->leaf, Temp, canopyparms, frostparms,N,TherTime);
                   deadstem=getStemSenescence(&miscanthus->stem,miscanthus->stemvec,senparms->stem,Temp,remobStem,frostparms,N);
                   deadroot=getRootSenescence(&miscanthus->root, miscanthus->rootvec,senparms->root,Temp,remobStem,frostparms,N);
                   deadrhiz=getRhizomeSenescence(&miscanthus->rhizome,miscanthus->rhizomevec ,senparms->rhiz,Temp,remobRhiz,frostparms,N); 
                   break;
              
      case 0:
                    deadleaf=0.0;
                    deadstem=0.0;
                    deadroot=0.0;
                    deadrhiz=0.0;
                    break;
    }
          dailysenesced=deadleaf+deadstem+deadroot+deadrhiz;
          remobilized = dailysenesced*canopyparms->remobFac; // assuming all the dead organs remobilizes same fraction
          totalmaintenance=miscanthus->autoresp.stemmaint+miscanthus->autoresp.rootmaint+miscanthus->autoresp.rhizomemaint;
          totalassimilate=dailygrossassim+remobilized-totalmaintenance;
          if(totalassimilate<0.0)
          {
          miscanthus->rhizome.biomass-=totalmaintenance;
          totalassimilate=0.0;
          }
          cropcent_dbp(coefs,TherPrds,TherTime, &cropdbp);
          kLeaf = cropdbp.DBP.kLeaf;
          kStem = cropdbp.DBP.kStem;
          kRoot = cropdbp.DBP.kRoot;
          kRhizome = cropdbp.DBP.kRhiz;
          if(totalassimilate>0.0)
           {
              newLeaf=newbiomass(totalassimilate,kLeaf,0.0);
              newStem=newbiomass(totalassimilate,kStem,RESP->growth.stem);
              newRoot=newbiomass(totalassimilate,kRoot,RESP->growth.root);
              newRhizome=newbiomass(totalassimilate,kRhizome,RESP->growth.rhizome);     
           }
           else
           {
              newLeaf=0.0;
              newStem=0.0;
              newRoot=0.0;
              newRhizome=totalassimilate;
           }
           // updating miscanthus growth respiration
          miscanthus->autoresp.stemgrowth=CalculateGrowthResp(newStem,RESP->growth.stem);
          miscanthus->autoresp.rootgrowth=CalculateGrowthResp(newRoot,RESP->growth.stem);
          miscanthus->autoresp.rhizomegrowth=CalculateGrowthResp(newRhizome,RESP->growth.stem);
           
           
//   Rprintf("New Biomass of leaf,stem,root,rhiz= %f, %f, %f, %f\n",newLeaf, newStem, newRoot,newRhizome);
    
    deadleaf =deadleaf*(1-canopyparms->remobFac);
    deadstem =deadstem*(1-canopyparms->remobFac);
    deadroot =deadroot*(1-canopyparms->remobFac);
    deadrhiz =deadrhiz*(1-canopyparms->remobFac);


    updatedailyvec(miscanthus->leafvec,newLeaf,deadleaf,N,delTT);
    updatedailyvec(miscanthus->stemvec,newStem,deadstem,N,delTT);
    updatedailyvec(miscanthus->rootvec,newRoot,deadroot,N,delTT);
    updatedailyvec(miscanthus->rhizomevec,newRhizome,deadrhiz,N,delTT);
    
    UpdateStandingLeaf(&miscanthus->leaf,miscanthus->leafvec,N);
    UpdateStandingStem(&miscanthus->stem,miscanthus->stemvec,N);
    UpdateStandingRoot(&miscanthus->root,miscanthus->rootvec,N);
    UpdateStandingRhizome(&miscanthus->rhizome,miscanthus->rhizomevec,N); 
    
    miscanthus->autoresp.total= miscanthus->autoresp.leafdarkresp+miscanthus->autoresp.stemmaint+miscanthus->autoresp.rootmaint
                               +miscanthus->autoresp.rhizomemaint+miscanthus->autoresp.stemgrowth+miscanthus->autoresp.rootgrowth
                               +miscanthus->autoresp.rhizomegrowth;

    newsdeadbiomass=deadleaf+deadstem+deadroot+deadrhiz;
    newalivebiomass=newLeaf+newStem+newRoot+newRhizome;
    check = miscanthus->GPP-miscanthus->autoresp.total-newsdeadbiomass-newalivebiomass;
    if(check !=0){
    printf("mass balance is satisfied (check = %i) satisfied at index i = %i\n,",check, N);
    }
    
    return;
}



void UpdateStandingLeaf(struct leaf *leaf, struct dailyvec *dailyleafvec,int N)
{
  leaf->biomass+=dailyleafvec[N].newbiomass;
  leaf->litter+=dailyleafvec[N].newlitter;
  return;
}

void UpdateStandingStem(struct stem *stem, struct dailyvec *dailystemvec,int N)
{
  stem->biomass+=dailystemvec[N].newbiomass;
  stem->litter+=dailystemvec[N].newlitter;
  return;
}

void UpdateStandingRoot(struct root *root, struct dailyvec *dailyrootvec,int N)
{
  root->biomass+=dailyrootvec[N].newbiomass;
  root->litter+=dailyrootvec[N].newlitter;
  return;
}

void UpdateStandingRhizome(struct rhizome *rhizome, struct dailyvec *dailyrhizomevec,int N)
{
  rhizome->biomass+=dailyrhizomevec[N].newbiomass;
  rhizome->litter+=dailyrhizomevec[N].newlitter;
  return;
}

int CheckEmergence(struct dailyclimate *dailyclimate, double EmergTemperature)
{
  int flag;
  if(dailyclimate->minimumTemp>EmergTemperature)
  {
    flag=1;
  }
  else
  {
    flag=0;
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

