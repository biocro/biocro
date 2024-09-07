#include "../framework/constants.h"          // for celsius_to_kelvin
#include "temperature_response_functions.h"  // for arrhenius_exponential,
                                             // polynomial_response,
                                             // johnson_eyring_williams_response
#include "c3_temperature_response.h"

using conversion_constants::celsius_to_kelvin;

/**
 *  @brief Calculates the values of key C3 photosynthesis parameters at leaf
 *  temperature.
 *
 *  The following parameters are calculated using an Arrhenius temperature
 *  response (`arrhenius_exponential()`) as in Bernacchi et al. (2001) and
 *  Bernacchi et al. (2003):
 *  - `Gstar`
 *  - `Jmax_norm`
 *  - `Kc`
 *  - `Ko`
 *  - `Rd_norm`
 *  - `Vcmax_norm`
 *
 *  The following parameters are calculated using a second-order polynomial
 *  temperature response (`polynomial_response()`) as in Bernacchi et al.
 *  (2001):
 *  - `phi_PSII`
 *  - `theta`
 *
 *  The following parameters are calculated using a Johnson, Eyring, & Williams
 *  temperature response (`johnson_eyring_williams_response()`) as in Harley et
 *  al. (1992):
 *  - `TPU_norm`
 *
 * References:
 *  - [Harley, P. C., Thomas, R. B., Reynolds, J. F. & Strain, B. R. Plant, Cell
 *    & Environment 15, 271–282 (1992)](https://doi.org/10.1111/j.1365-3040.1992.tb00974.x)
 *
 *  - [Bernacchi, C. J., Singsaas, E. L., Pimentel, C., Jr, A. R. P. & Long, S. P.
 *    Plant, Cell & Environment 24, 253–259 (2001)](https://doi.org/10.1111/j.1365-3040.2001.00668.x)
 *
 *  - [Bernacchi, C. J., Pimentel, C. & Long, S. P. Plant, Cell & Environment
 *    26, 1419–1430 (2003)](https://doi.org/10.1046/j.0016-8025.2003.01050.x)
 */
c3_param_at_tleaf c3_temperature_response(
    c3_temperature_response_parameters param,
    double Tleaf  // degrees C
)
{
    // Get leaf temperature in Kelvin
    double const Tleaf_K = Tleaf + celsius_to_kelvin;  // K

    return c3_param_at_tleaf{
        /* Gstar =      */ arrhenius_exponential(param.Gstar_c, param.Gstar_Ea, Tleaf_K),
        /* Jmax_norm =  */ arrhenius_exponential(param.Jmax_c, param.Jmax_Ea, Tleaf_K),
        /* Kc =         */ arrhenius_exponential(param.Kc_c, param.Kc_Ea, Tleaf_K),
        /* Ko =         */ arrhenius_exponential(param.Ko_c, param.Ko_Ea, Tleaf_K),
        /* phi_PSII =   */ polynomial_response(param.phi_PSII_0, param.phi_PSII_1, param.phi_PSII_2, Tleaf),
        /* Rd_norm =    */ arrhenius_exponential(param.Rd_c, param.Rd_Ea, Tleaf_K),
        /* theta =      */ polynomial_response(param.theta_0, param.theta_1, param.theta_2, Tleaf),
        /* Tp_norm =    */ johnson_eyring_williams_response(param.Tp_c, param.Tp_Ha, param.Tp_Hd, param.Tp_S, Tleaf_K),
        /* Vcmax_norm = */ arrhenius_exponential(param.Vcmax_c, param.Vcmax_Ea, Tleaf_K)};
}
