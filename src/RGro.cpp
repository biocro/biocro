#include <stdexcept>
#include <Rinternals.h>
#include "BioCro.h"
#include "modules.h"
#include "R_helper_functions.h"

extern "C" {

SEXP RGro(SEXP initial_state,
        SEXP invariate_parameters,
        SEXP varying_parameters,
        SEXP canopy_photosynthesis_module,
        SEXP soil_evaporation_module,
        SEXP growth_module,
        SEXP senescence_module)
{
    state_map s = map_from_list(initial_state);
    state_map ip = map_from_list(invariate_parameters);
    state_vector_map vp = map_vector_from_list(varying_parameters);

    //output_map(s);
    //output_map(ip);
    std::unique_ptr<IModule> canopy;
    std::unique_ptr<IModule> soil_evaporation;
    std::unique_ptr<IModule> growth;
    std::unique_ptr<IModule> senescence;

    try {
        canopy = make_module(CHAR(STRING_ELT(canopy_photosynthesis_module, 0)));
        soil_evaporation = make_module(CHAR(STRING_ELT(soil_evaporation_module, 0)));
        growth = make_module(CHAR(STRING_ELT(growth_module, 0)));
        senescence = make_module(CHAR(STRING_ELT(senescence_module, 0)));
    }
    catch (std::out_of_range const &oor) {
        error("%s was passed as a module, but that module could not be found.\n", oor.what());
    }

    vector<string> required_state = {"iSp", "doy", "SpD", "Leaf",
                            "LeafN_0", "vmaxb1", "vmax1", "alphab1",
                            "alpha1", "TTc", "temp", "tbase", "timestep",
                            "mrc1", "seneLeaf", "Stem", "seneStem",
                            "mrc2", "Root", "seneRoot", "Rhizome", "seneRhizome", "kln", "growth_respiration_fraction"};

    state_map all_state = combine_state(s, combine_state(ip, at(vp, 0)));

    vector<string> missing_state;
    for (auto it = required_state.begin(); it != required_state.end(); ++it) {
        if (all_state.find(*it) == all_state.end()) {
            missing_state.push_back(*it);
        }
    }

    if (!missing_state.empty()) {
        Rprintf("The following state variables are required but are missing: ");
        for(vector<string>::iterator it = missing_state.begin(); it != missing_state.end() - 1; ++it) {
            Rprintf("%s, ", it->c_str());
        }
        Rprintf("%s.\n", missing_state.back().c_str());
        error("This function cannot continue unless all state variables are set.");
    }

    state_vector_map ans;
    try {
        ans = Gro(s, ip, vp, canopy, soil_evaporation, growth, senescence, biomass_leaf_nitrogen_limitation);
    }
    catch (std::out_of_range const &oor) {
            Rprintf("Out of range exception thrown in RGro.cpp: %s\n", oor.what());
    }

    SEXP result = list_from_map(ans);
    return result;
}

} // extern "C"

