#include "AuxBioCro.h"
void Filling_BioCro_SoilStructure (struct soilML_str *soilMLS, struct soilText_str *soTexS, int soillayers,double *depths)
 {
    /*************************************************************************************
    *Purpose
    * 
    * Surrently Soil water module of BioCro is very simple, using a multi-layer concept but
    * average soil properties and all layers must be of same size.
    * 
    * We have added additional elements in soil multilayer structure used in BioCro where
    * each layer is explicitly defined and also additional properties which are required for
    * biogeochemical calculations are also defined for each layer
    * 
    * We are simply filling these additional elements in a way that it is consistent with the 
    * current BioCro-soil water module [average soil properties for all the layers, each layer is
    * about 5 cm depth] so that older function of soilML can be used.I have to ensure from R that each layer is 5 cm 
    * by defining  Nlayers such that Nlayers= round(Rooting depth/5 , 0).
    * 
    * Once Explicit definition of soil layers are used, Error associated with Rooting depth will disappear
    * 
    * IN future, I can use additional information to write more sophisticated soil water function
    * which cansiders effect of varying soil properties within a single soil profile and also
    * effect of using non-uniform discretization of the soil profile
    * 
    * Variables Initialized
    * soilMLS->
    * width
    * dpthmn
    * dpthmx
    * bulkd
    * fieldc
    * pH
    * swclimit
    * ***********************************************************************************/
    int i;
    soilMLS->Num_BioCro_soil_layers= soillayers;
    
    for (i=0; i <soillayers;i++)
      {
                if(i == 0)
                {
                 soilMLS->width[i] = depths[1];
                 soilMLS->dpthmn[i]=0.0;
                 soilMLS->dpthmx[i]=depths[1];
                 
                } 
                else
                {
                 soilMLS->width[i] = depths[i] - depths[i-1];
                 soilMLS->dpthmn[i]=depths[i-1];
                 soilMLS->dpthmx[i]=depths[i];
                }
          soilMLS->pH[i]=7.0;
          soilMLS->swclimit[i]=soTexS->wiltp-0.0008; // soil/in file says swc limit = wiltpoint - deltamin, which is 0.0008 in example file
          soilMLS->bulkd[i]=soTexS->bulkd;
          soilMLS->fieldc[i]=soTexS->fieldc;
          soilMLS->sand[i]=soTexS->sand;
          soilMLS->silt[i]=soTexS->silt;
          soilMLS->clay[i]=soTexS->clay;
      }
   return;
 }