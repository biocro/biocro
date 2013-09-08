#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "AuxcaneGro.h"
#include "crocent.h"


void dailywillow(struct c3tree *willow,double coefs[25],double TherPrds[6], double TherTime, double Temp,double dailynetassim,
struct senthermaltemp *senparms, struct canopyparms *canopyparms, struct frostParms *frostparms, int N, double delTT,
struct respirationParms *RESP, int emergence)

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
          deadleaf=getLeafSenescence(&willow->leaf,senparms->leafcriticalT,senparms->leaffr, Temp, frostparms,TherTime,canopyparms);
          deadstem=getStemSenescence(&willow->stem,senparms->stemcriticalT,senparms->stemfr, Temp, frostparms,TherTime);
          deadroot=getRootSenescence(&willow->root, senparms->rootcriticalT,senparms->rootfr, Temp, frostparms,TherTime);
          deadrhiz=getRhizomeSenescence(&willow->rhizome,senparms->rhizomecriticalT,senparms->rhizomefr, Temp, frostparms,TherTime);    
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
            RemobilizedFromRhizome=(-1)*kRhizome*willow->rhizome.biomass;
          }
          
          // Total maintenance respiration, which is a temperature dependet function of existing biomass
          totalmaintenance=willow->autoresp.stemmaint+willow->autoresp.rootmaint+willow->autoresp.rhizomemaint;
          // Total assimilate includes net canopy assimilation and remobilization from litter and rhizome, also it must satisfy total maintenance respiration
          totalassimilate=dailynetassim+RemobilizedFromLitter+RemobilizedFromRhizome-totalmaintenance;
          
          // If total assimilate for growth (& growth respiration) is negative then this is provided by rhizome only (storage carb.) leaving other parts intacts
          if(totalassimilate<=0.0)
          {
          newLeaf=0.0;
          willow->autoresp.stemgrowth=0.0;
          newStem=0.0;
          willow->autoresp.rootgrowth=0.0;
          newRoot=0.0;
          willow->autoresp.rhizomegrowth=0.0;
          // New rhizome must provide for total negative assimilate. Also we need to update remobilization
          newRhizome=totalassimilate+RemobilizedFromRhizome;
          willow->autoresp.rhizomegrowth=0.0;
          }
          else
          {
          SumofKpart=((kLeaf>0)?kLeaf:0)+((kStem>0)?kStem:0)+((kRoot>0)?kRoot:0)+((kRhizome>0)?kRhizome:0);
            if((SumofKpart-1.0)>1e-10)
            {
            Rprintf("Kleaf=%f,kStem=%f,kRoot=%f,kRhiz=%f, Sum=%f\n",kLeaf,kStem,kRoot,kRhizome,SumofKpart);
 //           error("Sum of Positive partitioning coefficient is not one");
            
            }
          newLeaf=newbiomass(totalassimilate,kLeaf,0.0);          
          newStem=newbiomass(totalassimilate,kStem,RESP->growth.stem);
          willow->autoresp.stemgrowth=CalculateGrowthResp(newStem,RESP->growth.stem);
          newRoot=newbiomass(totalassimilate,kRoot,RESP->growth.root);
          willow->autoresp.rootgrowth=CalculateGrowthResp(newRoot,RESP->growth.stem);
          // Make sure that remobilization termis taken care of in the calculation of newRhizome
          newRhizome=(kRhizome>0)?newbiomass(totalassimilate,kRhizome,RESP->growth.rhizome):(-1)*RemobilizedFromRhizome;
          
          willow->autoresp.rhizomegrowth=CalculateGrowthResp(newRhizome,RESP->growth.stem);
          }
           
         // Now we know growth respiration of each component & we can update total autotrophic respiration
          willow->autoresp.total=willow->autoresp.leafdarkresp+totalmaintenance+willow->autoresp.stemgrowth+willow->autoresp.rootgrowth+willow->autoresp.rhizomegrowth;
         // Here, we are updating net primary productivity
         willow->NPP=willow->GPP-willow->autoresp.total;
          
        // Here, we are evaluating new litter, This is fraction of dead biomass which was not remobilized  
          newLeaflitter=(deadleaf>0)?deadleaf*(1-canopyparms->remobFac):0.0;
          newStemlitter=(deadleaf>0)?deadstem*(1-canopyparms->remobFac):0.0;
          newRootlitter=(deadroot>0)?deadroot*(1-canopyparms->remobFac):0.0;
          newRhizomelitter=(deadrhiz>0)?deadrhiz*(1-canopyparms->remobFac):0.0;
          

          Dailybalance=newLeaf- deadleaf+newRoot- deadroot+newStem- deadstem+newRhizome- deadrhiz+newLeaflitter+newStemlitter+newRootlitter + newRhizomelitter;
         Dailybalance=Dailybalance-willow->NPP;
         if(Dailybalance>1e-10)
          {
          Rprintf("\nNPP and Daily Change inBiomass is not matching & difference is %f\n", Dailybalance);
          Rprintf("Thermal Time = %f, GPP = %f, Autotrophic Respiration = %f, NPP = %f, Remobilized from Litter = %f, Remobilized from Rhizome = %f \n", TherTime,willow->GPP, willow->autoresp.total,willow->NPP,RemobilizedFromLitter,RemobilizedFromRhizome);
          Rprintf("kLeaf=%f, kstem=%f, kRoot=%f, kRhizome=%f \n", kLeaf, kStem, kRoot,kRhizome);
          Rprintf("NewLeaf = %f Dead Leaf=%f, newLeafLitter=%f\n",newLeaf, deadleaf,newLeaflitter); 
          Rprintf("NewStem = %f Dead Stem=%f, newStemLitter=%f\n",newStem, deadstem,newStemlitter);
          Rprintf("NewRoot = %f Dead Root=%f, newRootLitter=%f\n",newRoot, deadroot,newRootlitter);
          Rprintf("NewRhizome = %f Dead Rhizome=%f, newRhizomeLitter=%f\n",newRhizome, deadrhiz,newRhizomelitter);
          Rprintf("LeafDarkResp=%f, Total maintenance (ExceptLeaf) = %f, StemGrowthResp=%f, RootGrowthResp=%f, RhizGrowthResp=%f\n",willow->autoresp.leafdarkresp,totalmaintenance,willow->autoresp.stemgrowth,willow->autoresp.rootgrowth,willow->autoresp.rhizomegrowth);
              Rprintf("Daily Biomas Balance Gain = %f", Dailybalance);
              Rprintf("--------Emergence= %i-, N= %i---------------------\n",emergence, N);
          }

          // Adding new biomass of green components
          UpdateStandingbiomass(&willow->leaf.biomass, newLeaf);
          UpdateStandingbiomass(&willow->stem.biomass, newStem);
          UpdateStandingbiomass(&willow->root.biomass, newRoot);
          UpdateStandingbiomass(&willow->rhizome.biomass, newRhizome);
          
          // Subtracting dead biomass from the green components
          // before thant I must multiply all the dead leaf components by -1, so I can still use 
          // Updatestandingbiomass function for SUBTRACTING instead of ADDING
          
          deadleaf*=(-1);
          deadstem*=(-1);
          deadroot*=(-1);
          deadrhiz*=(-1);
          
          UpdateStandingbiomass(&willow->leaf.biomass, deadleaf);
          UpdateStandingbiomass(&willow->stem.biomass, deadstem);
          UpdateStandingbiomass(&willow->root.biomass, deadroot);
          UpdateStandingbiomass(&willow->rhizome.biomass, deadrhiz);
          
          
          // Updating standing biomass of litter  components
          UpdateStandingbiomass(&willow->leaf.litter, newLeaflitter);
          UpdateStandingbiomass(&willow->stem.litter, newStemlitter);
          UpdateStandingbiomass(&willow->root.litter, newRootlitter);
          UpdateStandingbiomass(&willow->rhizome.litter, newRhizomelitter);
          
        
          
   }
   else // Dormant stange simulation, where rhizome provides for respiration loss
   {
          willow->stem.biomass=0.0;
          willow->autoresp.stemgrowth=0;
          willow->leaf.biomass=0.0;
          willow->autoresp.leafdarkresp=0.0;
          
          //Perhaps I can implement senescence rate of belowground components during dormant stage?
//          willow->root.biomass=0.0;
          willow->autoresp.rootgrowth=0;
//          willow->rhizome.biomass=0.0;
          willow->autoresp.rhizomegrowth=0;
          
          totalmaintenance=willow->autoresp.stemmaint+willow->autoresp.rootmaint+willow->autoresp.rhizomemaint;
          willow->autoresp.total=willow->autoresp.leafdarkresp+totalmaintenance+willow->autoresp.stemgrowth+willow->autoresp.rootgrowth+willow->autoresp.rhizomegrowth;
          willow->NPP=willow->GPP-willow->autoresp.total;
          newRhizome=(-1)* willow->autoresp.total;
          UpdateStandingbiomass(&willow->rhizome.biomass, newRhizome);
          if(willow->rhizome.biomass <0)error("rhizome has become negative");
   } 
    return;
}
