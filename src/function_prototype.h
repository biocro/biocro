#ifndef FUNCTION_PROTOTYPE_H
#define FUNCTION_PROTOTYPE_H

#include "AuxBioCro.h"
#include "crocent.h"
#include "soilwater.h"

void Copy_SoilWater_BioCro_To_CropCent(struct soilML_str *soilMLS, double soildepths[], struct cropcentlayer *CROPCENT);
void CalculateBiogeochem(struct miscanthus *miscanthus, struct cropcentlayer *CROPCENT,struct dailyclimate *dailyclimate);
void Filling_BioCro_SoilStructure (struct soilML_str *soilMLS, struct soilText_str *soTexS, int soillayers,double *depths);
void Calculate_Soil_Layer_Temperature(double soiltavg[],int num_layers,struct dailyclimate *dailyclimate);  
void Copy_CropCent_To_DayCent_Structures(struct cropcentlayer *CROPCENT, SITEPAR_SPT sitepar,LAYERPAR_SPT layers,SOIL_SPT soil);
#endif