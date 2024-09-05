#include "../framework/constants.h"  // for celsius_to_kelvin, ideal_gas_constant
#include "AuxBioCro.h"               // for arrhenius_exponential
#include "c3_temperature_response.h"

using conversion_constants::celsius_to_kelvin;
using physical_constants::ideal_gas_constant;

double johnson_eyring_williams_response(
    double c,           // dimensionless
    double Ha,          // J / mol
    double Hd,          // J / mol
    double S,           // J / K / mol
    double temperature  // K
)
{
    double const top = temperature * arrhenius_exponential(c, Ha, temperature);

    double const bot =
        1.0 + arrhenius_exponential(S / ideal_gas_constant, Hd, temperature);

    return top / bot;
}

double polynomial_response(
    double c0,          // output units
    double c1,          // output units * (degrees C)^(-1)
    double c2,          // output units * (degrees C)^(-2)
    double temperature  // degrees C
)
{
    return c0 + c1 * temperature + c2 * pow(temperature, 2);
}

c3_param_at_tleaf c3_temperature_response(
    c3_temperature_response_parameters param,
    double Tleaf  // degrees C
)
{
    // Get leaf temperature in Kelvin
    double const Tleaf_K = Tleaf + celsius_to_kelvin;  // K

    return c3_param_at_tleaf{
        /* Gstar = */ arrhenius_exponential(param.Gstar_c, param.Gstar_Ea, Tleaf_K),
        /* Jmax_norm = */ arrhenius_exponential(param.Jmax_c, param.Jmax_Ea, Tleaf_K),
        /* Kc = */ arrhenius_exponential(param.Kc_c, param.Kc_Ea, Tleaf_K),
        /* Ko = */ arrhenius_exponential(param.Ko_c, param.Ko_Ea, Tleaf_K),
        /* phi_PSII = */ polynomial_response(param.phi_PSII_0, param.phi_PSII_1, param.phi_PSII_2, Tleaf),
        /* Rd_norm = */ arrhenius_exponential(param.Rd_c, param.Rd_Ea, Tleaf_K),
        /* theta = */ polynomial_response(param.theta_0, param.theta_1, param.theta_2, Tleaf),
        /* Tp_norm = */ johnson_eyring_williams_response(param.Tp_c, param.Tp_Ha, param.Tp_Hd, param.Tp_S, Tleaf_K),
        /* Vcmax_norm = */ arrhenius_exponential(param.Vcmax_c, param.Vcmax_Ea, Tleaf_K)};
}
