#include <memory>
#include <stdexcept>
#include <R.h>
#include <Rinternals.h>
#include <math.h>
#include <Rmath.h>
#include "BioCro.h"
#include "Century.h"
#include "modules.h"

map<string, double> map_from_list(SEXP list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    map<string, double> m;
    int n = length(list);
    for (int i = 0; i < n; i++) {
        m.insert(std::pair<string, double>(CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]));
    }
    return m;
}

map<string, vector<double>> map_vector_from_list(SEXP list)
{
    SEXP names = getAttrib(list, R_NamesSymbol);
    int n = length(list);
    map<string, vector<double>> m;
    for (int i = 0; i < n; i++) {
        int p = length(VECTOR_ELT(list, i));
        vector<double> temporary;
        temporary.reserve(p);
        for (int j = 0; j < p; j++) {
            temporary.push_back(REAL(VECTOR_ELT(list, i))[j]);
        }
        m.insert(std::pair<string, vector<double>>(CHAR(STRING_ELT(names, i)), temporary));
    }
    return m;
}


SEXP list_from_map(map<string, double> m)
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

SEXP list_from_map(map<string, vector<double>> m)
{
    auto n = m.size();
    SEXP list =  PROTECT(allocVector(VECSXP, n));
    SEXP names = PROTECT(allocVector(STRSXP, n));
    int i = 0;
    for (auto it = m.begin(); it != m.end(); it++, i++) {
        int j = 0;
        auto p = it->second.size();
        SEXP values = PROTECT(allocVector(REALSXP, p));
        for (auto vit = it->second.begin(); vit != it->second.end(); vit++, j++) {
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

void output_map(map<string, double> m) {
    map<string, double>::iterator last_iterator = m.end();
    --last_iterator;
    Rprintf("The map contains the following items: ");
    for(map<string, double>::iterator it = m.begin(); it != last_iterator; it++) {
        Rprintf("%s, %0.03f; ", it->first.c_str(), it->second);
    }
    Rprintf("%s, %0.03f.\n", last_iterator->first.c_str(), last_iterator->second);
}

void output_list(SEXP list) {
    int n;
    n = length(list);
    SEXP names = PROTECT(allocVector(STRSXP, n));
    names = getAttrib(list, R_NamesSymbol);
    Rprintf("The list contains the following items: ");
    for (int i = 0; i < n - 1; i++) {
        Rprintf("%s, %0.03f; ", CHAR(STRING_ELT(names, i)), REAL(VECTOR_ELT(list, i))[0]);
    }
    Rprintf("%s, %0.03f.\n", CHAR(STRING_ELT(names, n - 1)), REAL(VECTOR_ELT(list, n - 1))[0]);
    UNPROTECT(1);
}

std::unique_ptr<IModule> make_module(string module_name)
{
    if (module_name.compare("c4_canopy") == 0) {
        return std::unique_ptr<IModule>(new c4_canopy);
    }
    else {
        return std::unique_ptr<IModule>(new c3_canopy);
    }
}

extern "C" {

SEXP RGro(SEXP initial_state,
        SEXP invariate_parameters,
        SEXP varying_parameters,
        SEXP canopy_photosynthesis_module)
{
    map<string, double> s = map_from_list(initial_state);
    map<string, double> ip = map_from_list(invariate_parameters);
    map<string, vector<double>> vp = map_vector_from_list(varying_parameters);

    output_map(s);
    output_map(ip);

    std::unique_ptr<IModule> canopy = make_module(CHAR(STRING_ELT(canopy_photosynthesis_module, 0)));

    map<string, vector<double>> ans;
    try {
    ans = Gro(s, ip, vp, canopy, biomass_leaf_nitrogen_limitation);
    }
    catch (const std::out_of_range& oor) {
            Rprintf("Exception thrown: %s\n", oor.what());
    }

    //SEXP result =  allocVector(VECSXP, 1);
    SEXP result = list_from_map(ans);
    return result;
}

} // extern "C"

