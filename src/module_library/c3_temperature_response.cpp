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
 *  - `Gstar_norm`
 *  - `Jmax_norm`
 *  - `Kc_norm`
 *  - `Ko_norm`
 *  - `RL_norm`
 *  - `Vcmax_norm`
 *  A reference temperature of 25 degrees C is used.
 *
 *  The following parameters are calculated using a second-order polynomial
 *  temperature response (`polynomial_response()`) as in Bernacchi et al.
 *  (2001):
 *  - `phi_PSII`
 *  - `theta`
 *
 *  The following parameters are calculated using a Johnson, Eyring, & Williams
 *  temperature response (`johnson_eyring_williams_response()`) as in Yang et
 *  al. (2016):
 *  - `TPU_norm`
 *  A reference temperature of 25 degrees C is used.
 *
 *  The following parameters are calculating using a peaked Arrhenius response
 *  (`peaked_arrhenius_response()`) as in Bernacchi et al. (2002):
 *  - `gm_norm`
 *  A reference temperature of 25 degrees C is used.
 *
 *  References:
 *  - [Yang, J. T., Preiser, A. L., Li, Z., Weise, S. E. & Sharkey, T. D. Planta
 *    243, 687–698 (2016)](https://doi/org/10.1007/s00425-015-2436-8)
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
    // Get reference temperature in Kelvin
    double constexpr Tref_K = 25.0 + celsius_to_kelvin;  // K

    // Get leaf temperature in Kelvin
    double const Tleaf_K = Tleaf + celsius_to_kelvin;  // K

    return c3_param_at_tleaf{
        /* gm_norm =    */ peaked_arrhenius_response(param.gm_Ha, param.gm_Hd, Tref_K, param.gm_S, Tleaf_K),
        /* Gstar_norm = */ arrhenius_exponential(param.Gstar_Ea, Tref_K, Tleaf_K),
        /* Jmax_norm =  */ arrhenius_exponential(param.Jmax_Ea, Tref_K, Tleaf_K),
        /* Kc_norm =    */ arrhenius_exponential(param.Kc_Ea, Tref_K, Tleaf_K),
        /* Ko_norm =    */ arrhenius_exponential(param.Ko_Ea, Tref_K, Tleaf_K),
        /* phi_PSII =   */ polynomial_response(param.phi_PSII_0, param.phi_PSII_1, param.phi_PSII_2, Tleaf),
        /* RL_norm =    */ arrhenius_exponential(param.RL_Ea, Tref_K, Tleaf_K),
        /* theta =      */ polynomial_response(param.theta_0, param.theta_1, param.theta_2, Tleaf),
        /* Tp_norm =    */ johnson_eyring_williams_response(param.Tp_Ha, param.Tp_Hd, Tref_K, param.Tp_S, Tleaf_K),
        /* Vcmax_norm = */ arrhenius_exponential(param.Vcmax_Ea, Tref_K, Tleaf_K)};
}
