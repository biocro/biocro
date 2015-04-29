#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "crocent.h"

void assignManagement(struct management *management)
{
management->harvestparms.harvestdoy=300;
management->harvestparms.frleaf=1.0;
management->harvestparms.frleaflitter=0.0;
management->harvestparms.frstem=1.0;
management->harvestparms.frstemlitter=0.0;
management->harvestparms.frdeadroot=0.0;
management->harvestparms.frdeadrhizome=0.0;

management->emergenceparms.minimumdoy=90;
management->emergenceparms.emergenceTemp=6.0;
management->emergenceparms.StoragetoLeaffraction=0.02;
management->emergenceparms.StemtoLeaffraction=0.02;
management->emergenceparms.plantingrate=20;
return;
}

void updateafterharvest(struct miscanthus *miscanthus,struct management *management)
{
  miscanthus->leaf.biomass=(1-management->harvestparms.frleaf)*miscanthus->leaf.biomass;
  miscanthus->leaf.litter=(1-management->harvestparms.frleaflitter)*miscanthus->leaf.litter;
  
  miscanthus->stem.biomass=(1-management->harvestparms.frstem)*miscanthus->stem.biomass;
  miscanthus->stem.litter=(1-management->harvestparms.frstemlitter)*miscanthus->stem.litter;
  
  miscanthus->root.biomass=(1-management->harvestparms.frdeadroot)*miscanthus->root.biomass;
  miscanthus->root.litter=management->harvestparms.frdeadroot*miscanthus->root.biomass;
  
  miscanthus->rhizome.biomass=(1-management->harvestparms.frdeadrhizome)*miscanthus->rhizome.biomass;
  miscanthus->rhizome.litter=management->harvestparms.frdeadrhizome*miscanthus->rhizome.biomass;
  return;
}

void updateafteremergence(struct miscanthus *miscanthus,struct management *management)
{
  miscanthus->leaf.biomass=(management->emergenceparms.StoragetoLeaffraction)* miscanthus->rhizome.biomass;
  miscanthus->rhizome.biomass= miscanthus->rhizome.biomass - miscanthus->leaf.biomass;
  return;
}
