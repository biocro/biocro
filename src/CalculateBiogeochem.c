#include "AuxBioCro.h"
#include "crocent.h""
#include "soilwater.h"
#include "function_prototype.h"

void CalculateBiogeochem(struct miscanthus *miscanthus, struct cropcentlayer *CROPCENT,struct dailyclimate *dailyclimate)
{
 
  /**********************************************************************************************************************
   * Purpose:
   * To Evaluate biogeochemical cycle for the current day
   * 
   * Inputs/Outputs:
   * 
   * (1)
   * A pointer to a structure miscanthus. This is defineed in crocent.h 
   * This structure contain standing biomass of different plant organs and also litter components.
   * Idea is to use these litter component, create structures that can be send as input to soil decomposition
   * and update the standing biomass pool
   * 
   * (2)
   * A pointer to structure cropcentlayer. This is defined in crocent.h 
   * This contain all the parameters and input/output required to perform soil biogeochemistry calculations which is a function of
   * incoming litter, environmental conditions, soil properties. 
   * This structures contain all the input/output pool and after function call, all pools are updated for today.
   * 
   * (3)
   * A pointer to a structure dailyclimate. This is defined in crocent.h
   *This is a structure for storing daily climate. This is required to perform decomposition of SOC/ and calculations of trace gas 
   *which are done daily and require daily climate data 
   * 
   * Summary/Main output of interest:-
   *     (a)  Updated plant structure (after taking care of fall rate of leaf litter).
   *     (b)  Updated SOC pools
   *     (c)  Updated NH4 pool
   *     (d)  Updated NO3 in Layers
   *     (e)  Updated Flux of N from different Layers
   *     (f)  gaseous flux from soil, CH4, N20, N2, CO2
   *     (g)  Leaching of Organic matter and Nitrogen 
   * ***************************************************************************************************************************/
  // Structure Definition from DayCent Code Provided by Melanie that will be used in function call to trace gas calculations
  SITEPAR_SPT sitepar;
  LAYERPAR_SPT layers;
  SOIL_SPT soil;
  
  // these variable are in argument to trace_gas_model but are not used, I am defining them for consistency. I can remove them later on.
  double time,sand,silt,clay,nreduce,NOabsorp_grass,NOabsorp_tree,tree_lai;
  int texture;
  time= 0.0;
  texture=2;
  nreduce=1.0;
  CROPCENT->ENV.nit_amt=0.0;
  tree_lai=0.0;
  NOabsorp_grass=0.0;// These are outputs
  NOabsorp_tree=0.0;// These are outputs
  
/*   
   
//***********WE CAN DEFINE ALL THE FALL RATES & OTHER PARAMETERS IN MISCANTHUS STRUCTURE THEN  SEND ALL OF THIS IN A SEPARATE FUNCTION *******  
   //Now here We are simpyl taking litter vector and creating new structures to send to soil biogeochemistry simulations
   //BiocroToCrocent(&LeafLitter,leaf.fallrate,leaf.lignin, &leaf.E, isotoperatio, 1, 0,leaflitter); 
    BiocroToCrocent(&miscanthus.leaf.litter,1.0,0.2, &leaflitterE, 1.0, 1, 0,&leaflitter);
    BiocroToCrocent(&miscanthus.stem.litter,1.0,0.2, &stemlitterE, 1.0, 1, 0,&stemlitter);
    BiocroToCrocent(&miscanthus.root.litter,1.0,0.2, &rootlitterE, 1.0, 0, 0,&rootlitter);
    BiocroToCrocent(&miscanthus.rhizome.litter,1.0,0.2, &rhizomelitterE, 1.0, 0, 0,&rhizomelitter);
//**************************************************************************************************************************
   
   
   
//********* THIS CAN GO INTO A SEPARATE FUNCTION*********************************************************
   // Update SOC pools based on incoming litter
    if(leaflitter.C.totalC >0.0) 
     {
      UpdateCropcentPoolsFromBioCro(&CROPCENT, &leaflitter);
     }
      if(stemlitter.C.totalC >0.0) 
     {
      UpdateCropcentPoolsFromBioCro(&CROPCENT, &stemlitter);
     }
      if(rootlitter.C.totalC >0.0) 
     {
      UpdateCropcentPoolsFromBioCro(&CROPCENT, &rootlitter);
     }
      if(rhizomelitter.C.totalC >0.0) 
     {
      UpdateCropcentPoolsFromBioCro(&CROPCENT, &rhizomelitter);
     }
//******************************************************************************************* //  
 

     // Calculate Top Layer 0-15 cm Properties based on Multi-layer soil Structure
     //**************FILL IN THE FUNCTION****************************************/
     Calculate_Soil_Layer_Temperature(CROPCENT->soilprofile.properties.soiltavg,CROPCENT->soilprofile.number_layers, dailyclimate);
     
     //Call Function to Assign Environmental Variables in the top 0-15 cm depth for Decomposition
//     assignENV(&CROPCENT,fake,fake,fake,fake,fake,fake,fake);
       
     // When Management is Implemented, Use this space two modify decomposition rates (tillage) and Soil N (fertilization)
     //************INSERT IMPLEMENT MANAGEMENT FUNCTION HERE***********************
     
     // Call Function  to Assign Flux Ratios for movement between different Pools  
//     assignFluxRatios(&CROPCENT);
     
     // Call Function to Decompose SOC in the top 0-15 cm layer
//     decomposeCROPCENT(&CROPCENT, woody,Eflag);
       
    // Call Function to Update All the Pools
//     updatecropcentpools(&CROPCENT);
    
  // Copying to DayCent Structures from CropCent multilayer Soil Structure Before Calling Trace_gas_Model
   Copy_CropCent_To_DayCent_Structures(&CROPCENT->soilprofile, sitepar,layers, soil);
    
     
  // call the tracegas model
 
  trace_gas_model(&dailyclimate->doy,&time, &CROPCENT->ENV.newminN,&CROPCENT->ENV.ammonium,CROPCENT->soilprofile.pools.nitrate,
                  &texture,&sand,&silt,&clay,
                  &CROPCENT->ENV.SOILTEX.fieldc,&CROPCENT->ENV.SOILTEX.bulkd,&CROPCENT->sitepar.maxt,&dailyclimate->precip,
                  &dailyclimate->snow,&CROPCENT->ENV.SOILTEX.avgwfps,&CROPCENT->Emission.stormf,
                  &CROPCENT->Emission.basef, CROPCENT->soilprofile.flux.frlechd,CROPCENT->soilprofile.flux.stream,
                  &CROPCENT->Emission.inorglch, &CROPCENT->soilprofile.critflow,CROPCENT->soilprofile.flux.waterflux,
                  &CROPCENT->Emission.newCO2, &CROPCENT->Emission.NOflux, &CROPCENT->Emission.Nn20flux,&CROPCENT->Emission.Dn20flux,
                  &CROPCENT->Emission.Dn2flux,&CROPCENT->Emission.CH4,&CROPCENT->sitepar.isdecid, 
                  &CROPCENT->sitepar.isagri, &miscanthus->LAI,&tree_lai,
                  &NOabsorp_grass,&NOabsorp_tree,
                  &CROPCENT->ENV.nit_amt,&nreduce,
                  CROPCENT->soilprofile.flux.dN2lyr,CROPCENT->soilprofile.flux.dN2Olyr,sitepar,layers,soil);
                  
                  
   /*
   trace_gas_model(int *jday, double *time, double *newminrl, double *ammonium, double nitrate[],
                         int *texture, double *sand, double *silt, double *clay,
                         double *afiel, double *bulkd, double *maxt, double *ppt,
                         double *snow, double *avgwfps, double *stormf,
                         double *basef, double frlechd[], double stream[],
                         double *inorglch, double *critflow, double wfluxout[],
                         double *newCO2, double *NOflux, double *Nn2oflux, double *Dn2oflux,
                         double *Dn2flux, double *CH4, int *isdecid,
                         int *isagri, double *grass_lai, double *tree_lai,
                         double *NOabsorp_grass, double *NOabsorp_tree,
                         double *nit_amt, double *nreduce, 
                         double dN2lyr[], double dN2Olyr[],SITEPAR_SPT sitepar,LAYERPAR_SPT layers,SOIL_SPT soil);
                         
 */ 
  // Write All the Variables of Interest in the Cropcentlayer structure
  
  //***********Update CROPCENT********************************************
 
}

