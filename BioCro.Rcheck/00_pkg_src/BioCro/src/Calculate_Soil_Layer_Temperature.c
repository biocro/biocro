#include "crocent.h"
void Calculate_Soil_Layer_Temperature(double soiltavg[],int num_layers,struct dailyclimate *dailyclimate)
{
  /************************************************************************
   * Purpose:
   * To calculate average  temperature of multiple soil layer
   * Input
   * daily climate structure
   * number of layers in soil profile num_layers
   * Output
   * soiltavg[num_layers]
   * 
   * Currently I am assuming temperature of all the soil layers is same as air temperature
   * This is similar to what i got in the version Melaniie gave me.
   * I need to change it to make it more sophisticated, 
   * Also, perhaps I will need to include maxm and minm temperature of layers too 
   * *********************************************************************/
  int i;
  for (i=0; i<num_layers; i++)
  {
    soiltavg[i]=dailyclimate->temp;
  }
  return;
}
