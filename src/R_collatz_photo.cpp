#include <Rinternals.h>
#include "module_library/collatz_photo.hpp"

extern "C" {
SEXP collatz_photoC(
        SEXP QP,
        SEXP LEAF_TEMPERATURE,
        SEXP VMAX,
        SEXP ALPHA,
        SEXP KPARM,
        SEXP THETA,
        SEXP BETA,
        SEXP RD,
        SEXP UPPERTEMP,
        SEXP LOWERTEMP,
        SEXP K_Q10,
        SEXP INTERCELLULAR_CO2_MOLAR_FRACTION)
{
    double const qp = REAL(QP)[0];
    double const leaf_temperature = REAL(LEAF_TEMPERATURE)[0];
    double const vmax = REAL(VMAX)[0];
    double const alpha = REAL(ALPHA)[0];
    double const kparm = REAL(KPARM)[0];
    double const theta = REAL(THETA)[0];
    double const beta = REAL(BETA)[0];
    double const rd = REAL(RD)[0];
    double const uppertemp = REAL(UPPERTEMP)[0];
    double const lowertemp = REAL(LOWERTEMP)[0];
    double const k_q10 = REAL(K_Q10)[0];
    double const intercellular_co2_molar_fraction = REAL(INTERCELLULAR_CO2_MOLAR_FRACTION)[0];

    SEXP assimilation;
    SEXP gross_assimilation;

    PROTECT(assimilation = Rf_allocVector(REALSXP, 1));
    PROTECT(gross_assimilation = Rf_allocVector(REALSXP, 1));

    struct collatz_result const result = collatz_photo(
                    qp,
                    leaf_temperature,
                    vmax,
                    alpha,
                    kparm,
                    theta,
                    beta,
                    rd,
                    uppertemp,
                    lowertemp,
                    k_q10,
                    intercellular_co2_molar_fraction);

    REAL(assimilation)[0] = result.assimilation;
    REAL(gross_assimilation)[0] = result.gross_assimilation;

    SEXP lists, names;
    PROTECT(lists = Rf_allocVector(VECSXP,2));
    PROTECT(names = Rf_allocVector(STRSXP,2));

    SET_VECTOR_ELT(lists, 0, assimilation);
    SET_VECTOR_ELT(lists, 1, gross_assimilation);

    SET_STRING_ELT(names, 0, Rf_mkChar("assimilation_rate"));
    SET_STRING_ELT(names, 1, Rf_mkChar("gross_assimilation_rate"));
    Rf_setAttrib(lists, R_NamesSymbol, names);

    UNPROTECT(4);   

    return lists;
}

}  // extern "C"

