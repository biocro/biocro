#include "AuxBioCro.h"
#include "crocent.h"
#include "soilwater.h"

void Copy_CropCent_To_DayCent_Structures(struct cropcentlayer *CROPCENT, SITEPAR_SPT sitepar,LAYERPAR_SPT layers,SOIL_SPT soil)
{
  int i;
      
      layers->numlyrs=CROPCENT->soilprofile.number_layers;
      for (i=0; i<CROPCENT->soilprofile.number_layers; i++)
      {
        layers->width[i]= CROPCENT->soilprofile.properties.width[i];
        layers->swc[i]= CROPCENT->soilprofile.pools.swc[i];
        layers->swclimit[i]= CROPCENT->soilprofile.properties.swclimit[i];
        layers->pH[i]= CROPCENT->soilprofile.properties.pH[i];
        layers->bulkd[i]= CROPCENT->soilprofile.properties.bulkd[i];
        layers->fieldc[i]= CROPCENT->soilprofile.properties.fieldc[i];
        layers->dpthmx[i]= CROPCENT->soilprofile.properties.dpthmx[i];
        layers->dpthmn[i]= CROPCENT->soilprofile.properties.dpthmn[i];
        layers->tcoeff[i]=CROPCENT->soilprofile.pools.rootbiomass[i];
        soil->soiltavg[i]=CROPCENT->soilprofile.properties.soiltavg[i];
      }
      sitepar->Ncoeff=CROPCENT->sitepar.Ncoeff;
      sitepar->jdayStart=CROPCENT->sitepar.jdayStart;
      sitepar->jdayEnd=CROPCENT->sitepar.jdayEnd;
      sitepar->N2Oadjust=CROPCENT->sitepar.N2Oadjust;
}

