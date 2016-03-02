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
        SEXP soil_evaporation_module)
{
    state_map s = map_from_list(initial_state);
    state_map ip = map_from_list(invariate_parameters);
    state_vector_map vp = map_vector_from_list(varying_parameters);

    //output_map(s);
    //output_map(ip);

    std::unique_ptr<IModule> canopy = make_module(CHAR(STRING_ELT(canopy_photosynthesis_module, 0)));
    std::unique_ptr<IModule> soil_evaporation = make_module(CHAR(STRING_ELT(soil_evaporation_module, 0)));

    state_vector_map ans;
    try {
        ans = Gro(s, ip, vp, canopy, soil_evaporation, biomass_leaf_nitrogen_limitation);
    }
    catch (std::out_of_range const &oor) {
            Rprintf("Exception thrown: %s\n", oor.what());
    }

    //SEXP result =  allocVector(VECSXP, 1);
    SEXP result = list_from_map(ans);
    return result;
}

} // extern "C"

