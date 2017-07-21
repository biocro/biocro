#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "modules.h"

//static int counter = 0;

state_map utilization_growth_module::do_operation(state_vector_map const &s, state_vector_map const &deriv_history, state_map const &p) const
{
// NOTE: This approach record new tissue derived from assimilation in the new*col arrays, but it doesn't
// record any new tissue derived from reallocation from other tissues, e.g., from rhizomes to the rest of the plant.
// Since it's not recorded, that part will never senesce.
// Also, the partitioning coefficiencts (kLeaf, kRoot, etc.) must be set to 0 for a long enough time
// at the end of the season for all of the tissue to senesce.
// This doesn't seem like a good approach.

    state_map derivs;
    auto t = s.begin()->second.size() - 1;

    double carbon_input = p.at("CanopyA"); //Pg in paper

    double Leaf = s.at("Leaf")[t];
    double Stem = s.at("Stem")[t];
    double Root = s.at("Root")[t];
    double Grain = s.at("Grain")[t];
    double Rhizome = s.at("Rhizome")[t];

    double kLeaf = p.at("rate_constant_leaf");
    double kStem = p.at("rate_constant_stem");
    double kRoot = p.at("rate_constant_root");
    double kRhizome = p.at("rate_constant_rhizome");
    double kGrain = p.at("rate_constant_grain");

    double beta = Leaf+Grain+Stem+Root+Rhizome;

    double KmLeaf = p.at("KmLeaf");
    double KmStem = p.at("KmStem");
    double KmRoot = p.at("KmRoot");
    double KmRhizome = p.at("KmRhizome");
    double KmGrain = p.at("KmGrain");

    double resistance_leaf_to_stem = p.at("resistance_leaf_to_stem");
    double resistance_stem_to_grain = p.at("resistance_stem_to_grain");
    double resistance_stem_to_root = p.at("resistance_stem_to_root");
    double resistance_stem_to_rhizome = p.at("resistance_stem_to_rhizome");

    double substrate_pool_leaf = s.at("substrate_pool_leaf")[t];
    double substrate_pool_grain = s.at("substrate_pool_grain")[t];
    double substrate_pool_stem = s.at("substrate_pool_stem")[t];
    double substrate_pool_root = s.at("substrate_pool_root")[t];
    double substrate_pool_rhizome = s.at("substrate_pool_rhizome")[t];


    double ratio_leaf = 1;
    double ratio_stem = 1;
    //Rprintf("Loop %d\n", counter++);
    //Rprintf("Before mass fraction\n");

    double mass_fraction_leaf = 0, mass_fraction_stem = 0, mass_fraction_root = 0, mass_fraction_rhizome = 0, mass_fraction_grain = 0;
    if (Leaf != 0) mass_fraction_leaf = substrate_pool_leaf / Leaf;
    if (Stem != 0) mass_fraction_stem = substrate_pool_stem / Stem;
    if (Root != 0) mass_fraction_root = substrate_pool_root / Root;
    if (Rhizome != 0) mass_fraction_rhizome = substrate_pool_rhizome / Rhizome;
    if (Grain != 0) mass_fraction_grain = substrate_pool_grain / Grain;

    double transport_leaf_to_stem = std::max(beta*(mass_fraction_leaf - mass_fraction_stem) / resistance_leaf_to_stem, 0.0);
    double transport_stem_to_grain = std::max(beta*(mass_fraction_stem - mass_fraction_grain) / resistance_stem_to_grain, 0.0);
    double transport_stem_to_root = std::max(beta*(mass_fraction_stem - mass_fraction_root) / resistance_stem_to_root, 0.0);
    double transport_stem_to_rhizome = std::max(beta*(mass_fraction_stem - mass_fraction_rhizome) / resistance_stem_to_rhizome, 0.0);

    double utilization_leaf = mass_fraction_leaf * kLeaf / (KmLeaf + mass_fraction_leaf);
    double utilization_grain = mass_fraction_grain * kGrain / (KmGrain + mass_fraction_grain);
    double utilization_stem = mass_fraction_stem * kStem / (KmStem + mass_fraction_stem);
    double utilization_root = mass_fraction_root * kRoot / (KmRoot + mass_fraction_root);
    double utilization_rhizome = mass_fraction_rhizome * kRhizome / (KmRhizome + mass_fraction_rhizome);

    //Rprintf("Before leaf\n");
    // When the change in the substrate pool would make the substrate pool negative, there are two special cases to handle: 1) The large time step produces nonsensible derivatives and 2) respiration uses more carbon than is available in the soluble substrate pool.
    if (carbon_input - transport_leaf_to_stem - utilization_leaf <= -substrate_pool_leaf) {
    //Rprintf("large derivative\n");
        if (carbon_input < -substrate_pool_leaf) {  // Respiration uses more carbon than there is in the substrate pool. The carbon must come from somewhere, so even though utilization for growth is thought of as irreversible, remove previously fixed carbon and don't grow or transport carbon.
    //Rprintf("very negative carbon_input\n");
            transport_leaf_to_stem = 0;
            utilization_leaf = 0;
            derivs["newLeafcol"] = 0;
            derivs["substrate_pool_leaf"] = -substrate_pool_leaf;  // Use up anything remainaing in the substrate pool.
            derivs["Leaf"] = carbon_input - derivs["substrate_pool_leaf"];  // Take respiration away from carbon previously fixes in leaves, accounting for what was already removed from the substrate pool.
        } else {
    //Rprintf("probably broken intergration\n");
            transport_leaf_to_stem = 0;
            ratio_leaf = (substrate_pool_leaf + carbon_input) / (transport_leaf_to_stem + utilization_leaf);
            derivs["newLeafcol"] = derivs["Leaf"] = utilization_leaf * ratio_leaf ;
            derivs["substrate_pool_leaf"] = carbon_input + (-transport_leaf_to_stem - utilization_leaf) * ratio_leaf;
        }
    } else {
    //Rprintf("everything is ok\n");
        derivs["newLeafcol"] = derivs["Leaf"] = utilization_leaf;
        if (derivs["newLeafcol"] < 0) derivs["newLeafcol"] = 0;  // TODO: This check wouldn't be necessary if we could limit leaf mass to greater or equal to 0.
        derivs["substrate_pool_leaf"] = carbon_input - transport_leaf_to_stem - utilization_leaf;
    }

    if (transport_stem_to_grain + transport_stem_to_root + transport_stem_to_rhizome + utilization_stem > substrate_pool_stem + transport_leaf_to_stem) {
        if (transport_stem_to_grain + transport_stem_to_root + transport_stem_to_rhizome + utilization_stem != 0)
            ratio_stem = (substrate_pool_stem + transport_leaf_to_stem) / (transport_stem_to_grain + transport_stem_to_root + transport_stem_to_rhizome + utilization_stem);
    }


    derivs["newStemcol"] = derivs["Stem"] = utilization_stem * ratio_stem;
    derivs["substrate_pool_stem"] = transport_leaf_to_stem + (-transport_stem_to_grain - transport_stem_to_root - transport_stem_to_rhizome - utilization_stem) * ratio_stem;



    derivs["ratio_stem"] = ratio_stem;
    derivs["transport_leaf_to_stem"] = transport_leaf_to_stem;
    derivs["transport_stem_to_grain"] = transport_stem_to_grain;
    derivs["transport_stem_to_root"] = transport_stem_to_root;
    derivs["transport_stem_to_rhizome"] = transport_stem_to_rhizome;
    derivs["utilization_stem"] = utilization_stem;






    if (utilization_grain > substrate_pool_grain + transport_stem_to_grain) {
        utilization_grain = substrate_pool_grain + transport_stem_to_grain;
    }
    derivs["Grain"] = utilization_grain;
    derivs["substrate_pool_grain"] = transport_stem_to_grain - utilization_grain;

    if (utilization_root > substrate_pool_root + transport_stem_to_root) {
        utilization_root = substrate_pool_root + transport_stem_to_root;
    }
    derivs["newRootcol"] = derivs["Root"] = utilization_root;
    derivs["substrate_pool_root"] = transport_stem_to_root - utilization_root;

    if (utilization_rhizome > substrate_pool_rhizome + transport_stem_to_rhizome) {
        utilization_rhizome = substrate_pool_rhizome + transport_stem_to_rhizome;
    }
    derivs["newRhizomecol"] = derivs["Rhizome"] = utilization_rhizome;
    derivs["substrate_pool_rhizome"] = transport_stem_to_rhizome - utilization_rhizome;

    return (derivs);
}


