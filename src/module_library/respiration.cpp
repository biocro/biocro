#include "respiration.h"
#include "temperature_response_functions.h"  // for Q10_temperature_response

/**
 *  @brief Calculates respiratory losses associated with a particular base rate
 *         of biomass available for growth.
 *
 *  @param [in] base_rate The base rate of carbon production that does not
 *         include respiratory losses. Any flux units are acceptable, such as
 *         mol / m^2 / s or Mg / ha / hour.
 *
 *  @param [in] grc Growth respiration coefficient (dimensionless)
 *
 *  @return A rate of respiratory losses having the same units as `base_rate`
 *
 *  ### Model overview
 *
 *  The idea here is that when `A_base` is the rate of carbon being allocated to
 *  a tissue for growth, a fraction `f_g` of this carbon is lost to respiration
 *  rather than being converted into new biomass. In other words, the rate of
 *  respiratory losses `R_g` is given by
 *
 *  > `R_g = f_g * A_base (1)
 *
 *  The remaining carbon available for growth (`A_growth`) is given by
 *
 *  > `A_growth = A_base * (1 - f_g) (2)
 *
 *  If `A_base` is negative, then no growth is occurring; in this case, `R_g`
 *  should be zero. With this in mind, we actually use
 *
 *  > `R_g = 0` (3)
 *
 *  when `A_base < 0` in place of Equation 3.
 *
 *  In the code below, `base_rate` represents `A_base` and `grc` represents
 *  `f_g`.
 *
 *  ### Sources
 *
 *  For growth respiration, see these papers:
 *  - Apsim: (https://apsimdev.apsim.info/ApsimX/Documents/AgPastureScience.pdf)
 *  - Thornley, J. H. M. "Growth, maintenance and respiration: a re-interpretation."
 *    Annals of Botany 41.6 (1977): 1191-1203.
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
 */
double growth_resp(
    double const base_rate,  // any flux units
    double const grc         // dimensionless
)
{
    // Check for error conditions
    if (grc < 0.0 || grc > 1.0) {
        throw std::range_error("Thrown in growth_resp: grc is outside [0, 1].");
    }

    return base_rate < 0 ? 0.0 : base_rate * grc;
}

/**
 *  @brief Calculates respiratory losses associated with a particular base rate
 *         of biomass available for growth.
 *
 *  @param [in] base_rate The base rate of carbon production that does not
 *         include respiratory losses. Any flux units are acceptable, such as
 *         mol / m^2 / s or Mg / ha / hour.
 *
 *  @param [in] grc0 Growth respiration coefficient at the reference temperature
 *         (dimensionless)
 *
 *  @param [in] Tleaf Leaf temperature (degrees C)
 *
 *  @param [in] Tref Reference temperature for the Q10 response (degrees C)
 *
 *  @return A rate of respiratory losses having the same units as `base_rate`
 *
 *  ### Model overview
 *
 *  This function follows the same general approach as `growth_resp()`, but
 *  calculates the growth respiration coefficient using a simple "Q10" method:
 *
 *  > `f_g = f_g_0 * 2^((T - Tref) / 10)`
 *
 *  where `Tref` is the reference temperature for the Q10 response and `f_g_0`
 *  is the growth respiration coefficient at the reference temperature.
 *  This is accomplished using the `Q10_temperature_response()` function.
 *
 *  In the code below, `base_rate` represents `A_base`, `grc0` and `grc`
 *  represent `f_g_0` and `f_g`, `Tleaf` represents the temperature `T`, and
 *  `Tref` represents the reference temperature.
 *
 *  ### Historical Notes and Sources
 *
 *  This function was originally called `resp` and was first explained in
 *  Stephen Humphries's thesis.
 *
 *  There, he describes the respiration model in the following way: "The
 *  respiration model of McCree (1970) modified according to Penning de Vries
 *  (1972) and Thornley (1970) is used here to predict plant respiration... The
 *  respiration associated with each plant structure is modified according to
 *  the temperature of the structure using a Q10 approximation with a value of 2
 *  as described by Spain and Keen (1992)."
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
 *  The Humphries thesis is also unfortunately not available online:
 *
 *  Humphries, S. "Will mechanistically rich models provide us with new insights
 *  into the response of plant production to climate change?: development and
 *  experiments with WIMOVAC: (Windows Intuitive Model of Vegetation response
 *  to Atmosphere & Climate Change)" (University of Essex, 2002)
 *
 *  Originally, this function was described as applying costs due to maintenance
 *  respiration. Yufeng [YH] has pointed out that since the cost is proportional
 *  to the growth rate, this is actually growth respiration. See these papers:
 *  - Apsim: (https://apsimdev.apsim.info/ApsimX/Documents/AgPastureScience.pdf)
 *  - Thornley, J. H. M. "Growth, maintenance and respiration: a re-interpretation."
 *    Annals of Botany 41.6 (1977): 1191-1203.
 *
 *  Ever since the days of WIMOVAC, there had been an additional constraint that
 *  `A_growth = A_base - R_g` is clamped to be greater than or equal to zero. We
 *  can see that `A_growth` would be negative whenever `A_base` is negative,
 *  since the fraction `f_g` must lie on [0, 1] by definition. So, this clamping
 *  operation is equivalent to setting `f_g` = 1 when `A_base` is negative. This
 *  caused some strange behavior with unrealistic physical meaning, because
 *  respiration costs were allowed to be negative. This behavior has since been
 *  changed so that no growth respiration occurs when `A_base` is negative.
 */
double growth_resp_Q10(
    double const base_rate,  // any flux units
    double const grc0,       // dimensionless
    double const Tleaf,      // degrees C
    double const Tref        // degrees C
)
{
    // Get the growth respiration coefficient at the current temperature
    double const grc{grc0 * Q10_temperature_response(Tleaf, Tref)};  // dimensionless

    return growth_resp(base_rate, grc);
}

/**
 *  @brief Calculates respiratory losses associated with the total biomass of a
 *         particular tissue.
 *
 *  @param [in] tissue_mass The total biomass of a tissue component (Mg / ha)
 *
 *  @param [in] mrc0 Maintenance respiration coefficient at the reference
 *         temperature (dimensionless)
 *
 *  @param [in] Tleaf Leaf temperature (degrees C)
 *
 *  @param [in] Tref Reference temperature for the Q10 response (degrees C)
 *
 *  @return A rate of respiratory losses having the same units as `tissue_mass`
 *
 *  ### Model overview
 *
 *  The idea here is that some respiration is required to maintain living
 *  tissue, and that the rate of CO2 use for maintenance respiration (`R_m`) is
 *  given by
 *
 *  > `R_m = f_m * M_tissue (1)
 *
 *  where `M_tissue` is the mass of the tissue and `f_m` is a proportionality
 *  factor. In BioCro, `M_tissue` is expressed in Mg / ha and `R_m` in
 *  Mg / ha / hr`, so `f_m` must have dimensions of "mass per mass per time," or
 *  kg / kg / hr using customary BioCro time units.
 *
 *  In this function, the temperature dependence of the proportionality factor
 *  is modeled using a simple "Q10" method:
 *
 *  > `f_m = f_m_0 * 2^((T - Tref) / 10)` (3)
 *
 *  where `Tref` is the reference temperature for the Q10 response and `f_m_0`
 *  is the maintenance respiration coefficient at the reference temperature.
 *  This is accomplished using the `Q10_temperature_response()` function.
 *
 *  In the code below, `tissue_mass` represents `M_tissue`, `mrc0` and `mrc`
 *  represent `f_m_0` and `f_m`, `Tleaf` represents the temperature `T`, and
 *  `Tref` represents the reference temperature.
 *
 *  ### Sources
 *
 *  This ideas is from this paper: https://doi.org/10.1016/j.fcr.2010.07.007.
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
 */
double maintenance_resp_Q10(
    double const tissue_mass,  // Mg / ha
    double const mrc0,         // kg / kg / hr
    double const Tleaf,        // degrees C
    double const Tref          // degrees C
)
{
    // Get the maintenance respiration coefficient at the current temperature
    double const mrc{mrc0 * Q10_temperature_response(Tleaf, Tref)};  // kg / kg / hr

    return tissue_mass * mrc;
}
