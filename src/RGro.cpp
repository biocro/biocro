#include <memory>
#include <stdexcept>
#include <R.h>
#include <Rinternals.h>
#include <math.h>
#include <Rmath.h>
#include "BioCro.h"
#include "Century.h"
#include "modules.h"

state_map map_from_list(SEXP const &list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    state_map m;
    int n = length(list);
    for (int i = 0; i < n; ++i) {
        m.insert(std::pair<string, double>(CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]));
    }
    return m;
}

state_vector_map map_vector_from_list(SEXP const &list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    int n = length(list);
    state_vector_map m;
    for (int i = 0; i < n; ++i) {
        int p = length(VECTOR_ELT(list, i));
        vector<double> temporary;
        temporary.reserve(p);
        for (int j = 0; j < p; ++j) {
            temporary.push_back(REAL(VECTOR_ELT(list, i))[j]);
        }
        m.insert(std::pair<string, vector<double>>(CHAR(STRING_ELT(names, i)), temporary));
    }
    return m;
}


SEXP list_from_map(state_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(allocVector(VECSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        SET_VECTOR_ELT(list, i, ScalarReal(it->second));
        SET_STRING_ELT(names, i, mkChar(it->first.c_str()));
    }
    setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

SEXP list_from_map(state_vector_map const &m)
{
    auto n = m.size();
    SEXP list =  PROTECT(allocVector(VECSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        int j = 0;
        auto p = it->second.size();
        SEXP values = PROTECT(allocVector(REALSXP, p));
        for (auto vit = it->second.begin(); vit != it->second.end(); ++vit, ++j) {
            REAL(values)[j] = it->second[j];
        } 
        SET_VECTOR_ELT(list, i, values);
        SET_STRING_ELT(names, i, mkChar(it->first.c_str()));
        UNPROTECT(1);
    }
    setAttrib(list, R_NamesSymbol, names);
    UNPROTECT(2);
    return list;
}

void output_map(state_map const &m) {
    Rprintf("The map contains the following items: ");
    int i = 0;
    auto it = m.begin();
    for(; std::next(it) != m.end(); ++it) {
        Rprintf("%s, %0.04f; ", it->first.c_str(), it->second);
        if (++i % 5 == 0) Rprintf("\n");
    }
    Rprintf("%s, %0.04f.\n\n", it->first.c_str(), it->second);
}

void output_list(SEXP const &list) {
    int n;
    n = length(list);
    SEXP names = PROTECT(allocVector(STRSXP, n));
    names = getAttrib(list, R_NamesSymbol);
    Rprintf("The list contains the following items: ");
    for (int i = 0; i < n - 1; ++i) {
        Rprintf("%s, %0.04f; ", CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]);
    }
    Rprintf("%s, %0.04f.\n\n", CHAR(STRING_ELT(names, n - 1)), REAL(VECTOR_ELT(list, n - 1))[0]);
    UNPROTECT(1);
}

std::unique_ptr<IModule> make_module(string const &module_name)
{
    if (module_name.compare("c4_canopy") == 0) {
        return std::unique_ptr<IModule>(new c4_canopy);
    }
    else if (module_name.compare("one_layer_soil_profile") == 0) {
        return std::unique_ptr<IModule>(new one_layer_soil_profile);
    }
    else {
        return std::unique_ptr<IModule>(new c3_canopy);
    }
}

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

