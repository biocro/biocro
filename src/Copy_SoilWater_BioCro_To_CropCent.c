#include "AuxBioCro.h"
#include "crocent.h"
void Copy_SoilWater_BioCro_To_CropCent(struct soilML_str *soilMLS,  struct cropcentlayer *CROPCENT)
{
  /***************************************************************************************
   * File Name:Copy_SoilWater_BioCro_To_CropCent.c
   * 
   * Function Name: Copy_SoilWater_BioCro_To_CropCent
   * 
   * Purpose: Soil Water Module of BioCro is used to update multilayer soil structure of cropcent layer,
   *          which will be eventually used for the calculation of soil biogeochemistry cycling
   * 
   * Input:
   * struct soilML_str *soilMLS:  pointer to structure soilMLS defined in AuxBioCro.h This structure 
   *                   contains calculations of soul hydrology 
   * int soillayers:  Number of soil layers being used in the calculations
   * struct cropcentlayer *CROPCENT: Pointer to structure cropcent defined in crocent.h
   * 
   * Output:
   * soilprofile of CROPCENT is updated
   * ************************************************************************************/
    int i;
     CROPCENT->soilprofile.number_layers = soilMLS->Num_BioCro_soil_layers;
      for (i = 0; i < CROPCENT->soilprofile.number_layers; i++)
        {
        CROPCENT->soilprofile.pools.rootbiomass[i]=soilMLS->rootDist[i];
        CROPCENT->soilprofile.pools.swc[i]=soilMLS->cws[i];
        CROPCENT->soilprofile.flux.waterflux[i]=soilMLS->dailyWflux[i]; 
        CROPCENT->soilprofile.properties.width[i]=soilMLS->width[i];
        CROPCENT->soilprofile.properties.dpthmx[i]=soilMLS->dpthmx[i];
        CROPCENT->soilprofile.properties.dpthmn[i]=soilMLS->dpthmn[i];
        CROPCENT->soilprofile.properties.pH[i]=soilMLS->pH[i];
        CROPCENT->soilprofile.properties.bulkd[i]=soilMLS->bulkd[i];
        CROPCENT->soilprofile.properties.swclimit[i]=soilMLS->swclimit[i];
        CROPCENT->soilprofile.properties.fieldc[i]=soilMLS->fieldc[i];
        }      
      return;
}
