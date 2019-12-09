//
//  nitrogen_uptake_partitioning.hpp
//  
//
//  Created by Megan Matthews on 11/4/19.
//

#ifndef NITROGEN_UPTAKE_PARTITIONING_H
#define NITROGEN_UPTAKE_PARTITIONING_H

// OpenSimRoot gives a nitrogen uptake rate in g/d, or cumulative nitrogen uptake in g. We then want to partition this between the different plant components.
//To start with, just going to look at partitioning to the leaf

//For soybean see Gaspar et al., 2017: doi: 10.2135/cropsci2016.05.0322
//and Bender et al., 2015: doi:10.2134/agronj14.0435

//According to Gaspar et al., Nitrogen is partitioned to the various plant parts until growth stage R5.5, after which it is only partitioned to the seed. Additionally, the nitrogen from the other plant parts are remobilized to the seed. At R5.5, ~43% of the total N taken up was partitioned to the leaves. According to Fig. 3, Almost all of the N is partioned to the leaves in the vegetative stages

// If pass in cumulative nitrogen uptake in grams (will need to convert to g/unit area), have leaf N be a percentage of the cumulative uptake based on DVI or growth stage, and convert to umol/m2 units.
//Makes more sense to partition the uptake rate though.



#endif /* nitrogen_uptake_partitioning_h */
