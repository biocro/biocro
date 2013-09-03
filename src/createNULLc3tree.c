#include "crocent.h"
void createNULLc3tree(struct miscanthus *miscanthus,int vecsize)
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
