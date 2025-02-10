#include <algorithm>  // for std::min
#include "respiration.h"
#include "temperature_response_functions.h"  // for Q10_temperature_response

/**
 *  @brief Subtracts respiratory losses from a carbon production rate
 *
 *  @param [in] base_rate The base rate of carbon production that does not
 *                        include respiratory losses. Any units are acceptable,
 *                        e.g. mol / m^2 / s or Mg / ha / hour.
 *
 *  @param [in] grc Growth respiration coefficient (dimensionless)
 *
 *  @param [in] temp Temperature (degrees C)
 *
 *  @return A struct containing the rate of respiratory losses and the modified
 *          net rate (having the same units as `base_rate`) where respiratory
 *          losses have been subtracted from the base rate.
 *
 *  ### Model overview
 *
 *  The idea here is that the net carbon assimilation rate (`A_n`) is given by
 *
 *  > `A_n = A_g - R` (1)
 *
 *  where `A_g` is the gross assimilation rate or the production rate and `R` is
 *  the respiration rate. A further assumption is that respiration is
 *  proportional to production, i.e.
 *
 *  > `R = R_c * A_g` (2)
 *
 *  where `R_c` is a dimensionless coefficient that lies on the interval [0,1].
 *  Combining equations (1) and (2), we can express the net assimilation rate
 *  as a function of the gross assimilation rate and the respiration
 *  coefficient:
 *
 *  > `A_n = A_g - R_c * A_g =  A_g * (1 - R_c)` (3)
 *
 *  In this function, the temperature dependence of the respiration coefficient
 *  is modeled using a simple "Q10" method where `Q10 = 2` and the base
 *  temperature is 0 degrees C, i.e.,
 *
 *  > `R_c = R_c_0 * 2^(T / 10)` (4)
 *
 *  where `T` is the temperature in Celsius. Combining equations (3) and (4), we
 *  finally arrive at a relationship between net assimilation, gross
 *  assimilation, and temperature:
 *
 *  > `A_n = A_g * (1 - R_c_0 * 2^(T / 10))` (5)
 *
 *  In the code below, `base_rate` represents the gross assimilation rate `A_g`,
 *  `grc` represents the growth respiration coefficient `R_c_0`, and `temp`
 *  represents the temperature `T`.
 *
 *  ### Sources
 *
 *  In Stephen Humphries's thesis, he describes the respiration model in the
 *  following way: "The respiration model of McCree (1970) modified according to
 *  Penning de Vries (1972) and Thornley (1970) is used here to predict plant
 *  respiration... The respiration associated with each plant structure is
 *  modified according to the temperature of the structure using a Q10
 *  approximation with a value of 2 as described by Spain and Keen (1992)."
 *
 *  I (EBL) believe these references to be to the following documents:
 *
 *  - [de Vries, F. P. "Respiration and growth" in "Crop processes in controlled
 *    environments" 327–347 (Academic Press, 1972)]
 *    (https://library.wur.nl/WebQuery/wurpubs/fulltext/218533)
 *
 *  - [Thornley, J. H. M. "Respiration, Growth and Maintenance in Plants" Nature
 *    227, 304–305 (1970)](https://doi.org/10.1038/227304b0)
 *
 *  - McCree, K. "An equation for the rate of respiration of whiteclover plants
 *    grown under controlled conditions" in: "Prediction and measurement
 *    of photosynthetic productivity" Wageningen: Centre for Agricultural
 *    Publishing and Documentation, 221-229 (1970)
 *
 *  - Keen, R. E. & Spain, J. D. "Computer simulation in biology. A BASIC
 *    introduction" (1992)
 *
 *  I believe the McCree, Thornley, and de Vries papers describe the
 *  relationship between gross assimilation and respiration, while the Keen &
 *  Spain book describes the temperature dependence. Unfortunately, I can't find
 *  an online version of that book so I can't be sure.
 *
 *  For some general discussions about respiration, see the following two
 *  sources:
 *
 *  - [Amthor, J. S. "The McCree–de Wit–Penning de Vries–Thornley Respiration
 *    Paradigms: 30 Years Later" Ann Bot 86, 1–20 (2000)]
 *    (https://doi.org/10.1006/anbo.2000.1175)
 *
 *  - [Amthor, J. S. "The role of maintenance respiration in plant growth."
 *    Plant, Cell & Environment 7, 561–569 (1984)]
 *    (https://doi.org/10.1111/1365-3040.ep11591833)
 *
 *  The Humphries thesis is also unfortunately not available online:
 *
 *  Humphries, S. "Will mechanistically rich models provide us with new insights
 *  into the response of plant production to climate change?: development and
 *  experiments with WIMOVAC: (Windows Intuitive Model of Vegetation response
 *  to Atmosphere & Climate Change)" (University of Essex, 2002)
 *
 *  [YH] This function scales the assimilation rate. It should be called the growth
 *  respiration instead of maintenance respiration, as defined in these papers:
 *  - Apsim: (https://apsimdev.apsim.info/ApsimX/Documents/AgPastureScience.pdf)
 *  - Thornley, J. H. M. "Growth, maintenance and respiration: a re-interpretation."
 *    Annals of Botany 41.6 (1977): 1191-1203.
 */
respiration_outputs resp(double base_rate, double grc, double temp)
{
    // Define the reference temperature for the Q10 function
    double constexpr Tref = 0.0;  // degrees C

    // Calculate the respiratory losses
    double losses = base_rate * grc * Q10_temperature_response(temp, Tref);

    // Calculate the net rate
    double net_rate = base_rate - losses;

    // Do not allow the net rate to become negative
    if (net_rate < 0) {
        // Reset the net rate to 0
        net_rate = 0;

        // If net_rate = 0, then base_rate - losses = 0, so losses = base_rate
        losses = base_rate;
    }

    return respiration_outputs{
        /* net_rate = */ net_rate,
        /* respiration_rate = */ losses
    };
}
