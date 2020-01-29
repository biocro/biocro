#include <Rinternals.h>
#include "module_library/BioCro.h"

extern "C" {

SEXP R_sunML(
        SEXP IDIR,
        SEXP IDIFF,
        SEXP LAI,
        SEXP NLAYERS,
        SEXP COSTHETA,
        SEXP KD,
        SEXP CHIL,
        SEXP HEIGHTF)
{
    double const i_dir = REAL(IDIR)[0];
    double const i_diff = REAL(IDIFF)[0];
    double const lai = REAL(LAI)[0];
    int const nlayers = INTEGER(NLAYERS)[0];
    double const cos_theta = REAL(COSTHETA)[0];
    double const kd = REAL(KD)[0];
    double const chi_l = REAL(CHIL)[0];
    double const height_f = REAL(HEIGHTF)[0];

    SEXP list;
    SEXP names;
    SEXP direct_irradiance;
    SEXP diffuse_irradiance;
    SEXP total_irradiance;
    SEXP sunlit_fraction;
    SEXP shaded_fraction;
    SEXP height;

    PROTECT(list = Rf_allocVector(VECSXP, 6));
    PROTECT(names = Rf_allocVector(STRSXP, 6));
    PROTECT(direct_irradiance = Rf_allocVector(REALSXP, nlayers));
    PROTECT(diffuse_irradiance = Rf_allocVector(REALSXP, nlayers));
    PROTECT(total_irradiance = Rf_allocVector(REALSXP, nlayers));
    PROTECT(sunlit_fraction = Rf_allocVector(REALSXP, nlayers));
    PROTECT(shaded_fraction = Rf_allocVector(REALSXP, nlayers));
    PROTECT(height = Rf_allocVector(REALSXP, nlayers));

    Light_profile result = sunML(i_dir, i_diff, lai, nlayers, cos_theta, kd, chi_l, height_f);

    for (int i = 0; i < nlayers; ++i) {
        REAL(direct_irradiance)[i] = result.direct_irradiance[i];
        REAL(diffuse_irradiance)[i] = result.diffuse_irradiance[i];
        REAL(total_irradiance)[i] = result.total_irradiance[i];
        REAL(sunlit_fraction)[i] = result.sunlit_fraction[i];
        REAL(shaded_fraction)[i] = result.shaded_fraction[i];
        REAL(height)[i] = result.height[i];
    }

    SET_VECTOR_ELT(list, 0, direct_irradiance);
    SET_VECTOR_ELT(list, 1, diffuse_irradiance);
    SET_VECTOR_ELT(list, 2, total_irradiance);
    SET_VECTOR_ELT(list, 3, sunlit_fraction);
    SET_VECTOR_ELT(list, 4, shaded_fraction);
    SET_VECTOR_ELT(list, 5, height);

    SET_STRING_ELT(names, 0, Rf_mkChar("direct_irradiance"));
    SET_STRING_ELT(names, 1, Rf_mkChar("diffuse_irradiance"));
    SET_STRING_ELT(names, 2, Rf_mkChar("total_irradiance"));
    SET_STRING_ELT(names, 3, Rf_mkChar("sunlit_fraction"));
    SET_STRING_ELT(names, 4, Rf_mkChar("shaded_fraction"));
    SET_STRING_ELT(names, 5, Rf_mkChar("height"));
    Rf_setAttrib(list, R_NamesSymbol, names);

    UNPROTECT(8);
    return list;
}
}  // extern "C"

