/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */

#include <string>
#include <stdexcept>  // for std::out_of_range, std::range_error
#include <algorithm>  // for std::max, std::min
#include <cmath>      // for exp, log, pow, lgamma, std::abs
#include <vector>
#include "c4photo.h"
#include "BioCro.h"
#include "boundary_layer_conductance.h"  // for leaf_boundary_layer_conductance_nikolov
#include "sunML.h"                       // for thick_layer_absorption
#include "water_and_air_properties.h"    // for saturation_vapor_pressure,
                                         // TempToDdryA, TempToLHV, TempToSFS
#include "../framework/constants.h"      // for pi, e, molar_mass_of_water,
                                         // celsius_to_kelvin, stefan_boltzmann

using std::vector;

// The probability density for the Poisson distribution is
// e^(-lambda) * lambda^x / x!
//
// Both the numerator lambda^x and denominator x! can become very
// large, potentially causing numerical errors when calculating their
// ratio.
//
// In such cases, we take the log of the above expression to get
//
//   -lambda + (x * log(lambda)) - log(x!).
//
// We can obtain log(x!) using std::lgamma:
//
//   lgamma(x + 1) ~= log(x!)
//
// Then we return the result of applying exp to this.
//
double poisson_density(int x, double lambda)
{
    // Use lgamma to calculate the (log of) the factorial for large
    // values of x:
    if (x > 10) {
        double log_result = -lambda + x * log(lambda) - lgamma(x + 1);
        return exp(log_result);
    }

    // Do closer to exact calculation for smaller x:
    double factorial_x{1};
    for (int i = 1; i <= x; ++i) {
        factorial_x *= i;
    }

    return exp(-lambda) * pow(lambda, x) / factorial_x;
}

/* Additional Functions needed for EvapoTrans */

/**
 * @brief Wind profile function
 *
 * Preconditions:
 *     `WindSpeed` is non-negative.
 *     `LAI` is non-negative
 *     `wind_speed_profile` is a vector of size at most MAXLAY.
 */
void WINDprof(double WindSpeed, double LAI, vector<double>& wind_speed_profile)
{
    auto nlayers = wind_speed_profile.size();
    constexpr double k = 0.7;
    double LI = LAI / nlayers;

    for (vector<double>::size_type i = 0; i < nlayers; ++i) {
        double CumLAI = LI * (i + 1);
        wind_speed_profile[i] = WindSpeed * exp(-k * (CumLAI - LI));
    }
}

/**
 * @brief Calculates a relative humidity profile throughout a multilayer
 * canopy.
 *
 * @param[in] RH relative humidity just above the canopy `(0 <= RH <= 1)`
 *
 * @param[in] nlayers number of layers in the canopy `(1 <= nlayers <= MAXLAY)`
 *
 * @param[out] relative_humidity_profile array of relative humidity values
 * expressed as fractions between 0 and 1, where the value at index `i`
 * represents relative humidity at the bottom of canopy layer `i` and `i = 0`
 * corresponds to the top canopy layer.
 *
 * One can derive an expression for the relative humidity (`h`) throughout a
 * plant canopy by making the following assumptions:
 * - `h` at the top of the canopy is the same as the ambient value (`h = h0`)
 * - `h` at the bottom of the canopy is one hundred percent (`h = 1`)
 * - `h` follows an exponential profile throughout the canopy
 *
 * To enforce the exponential profile, we can write
 *
 * `h(x) = A * exp(B * x)`              [Equation (1)]
 *
 * where `x` is a normalized expression of depth within the canopy (`x = 0` at
 * the canopy top and `x = 1` at the bottom) and the values of `A` and `B` are
 * yet to be determined.
 *
 * To enforce the `h` value at the canopy top, we require `h0 = h(0) = A`; in
 * other words, `A = h0`. To enforce the `h` value at the canopy bottom, we
 * require `1 = h(1) = h0 * exp(B)`; in other words, `B = -ln(h0)`. Putting it
 * all together, we see that under these assumptions, `h` throughout the canopy
 * is given by
 *
 * `h(x) = h0 * exp(-ln(h0) * x)`       [Equation (2)]
 *
 * If we additionally assume that `h0` is close to 1, we can simplify
 * `B = -ln(h0)` by taking just the linear part of the Taylor series for
 * `-ln(h0)` centered at `h0 = 1`, which is `B = -(h0 - 1)`. With this
 * modification, we have
 *
 * `h(x) = h0 * exp((1 - h0) * x)`      [Equation (3)]
 *
 * When `h0 = 1`, Equations (2) and (3) are in perfect agreement, reducing to
 * `h(x) = 1`. However, as `h0` deviates further from `h0 = 1`, the
 * linearization for `B` becomes less accurate and the two versions diverge,
 * especially deeper in the canopy. For example, when `h0 = 0.7`, Equation (2)
 * becomes `h(x) = 0.7 * exp(0.357 * x)` while Equation (3) becomes
 * `h(x) = 0.7 * exp(0.300 * x)`. Both versions agree at the top of the canopy
 * where `x = 0` and `h(x) = 1`, but are different at the bottom: 1.00 vs. 0.94.
 * For lower `h0` values, the difference at the canopy bottom becomes even more
 * pronounced, significantly violating one of the original assumptions. However,
 * there isn't a strong scientific justification for assuming humidity is 1 at
 * the bottom of *every* canopy, so the error due to the approximation for `B`
 * is deemed to be acceptable.
 *
 * ---
 *
 * In BioCro, we divide the canopy into equally sized layers; i.e., the interval
 * `0 <= x <= 1` is divided into `n` segments of length `1 / n` by `n + 1`
 * boundaries occurring at `x = 0`, `x = 1 / n`, `x = 2 / n`, ..., `x = 1`. Here
 * we wish to find the `h` value at the bottom of each layer, so we use Equation
 * (3) with `x = 1 / n`, `x = 2 / n`, ..., `x = 1`.
 *
 * In the code below, the `RH` input argument corresponds to `h0`, the
 * exponential growth constant `kh` corresponds to `B`, and `nlayers`
 * corresponds to `n`.
 *
 * ---
 *
 * Note 1: the explanation for this code was "reverse engineered" by EBL from
 * some crytpic documentation found in earlier versions of this function and
 * from Stephen Humphries's thesis, which is not available online:
 *
 * Humphries, S. "Will mechanistically rich models provide us with new insights
 * into the response of plant production to climate change?: development and
 * experiments with WIMOVAC: (Windows Intuitive Model of Vegetation response
 * to Atmosphere & Climate Change)" (University of Essex, 2002).
 *
 * ---
 *
 * Note 2: Equation (2) can be rewritten by noting that `h0 = exp(ln(h0))`. With
 * this replacement, the equation becomes
 *
 * `h(x) = exp(ln(h0) - ln(h0) * x) = exp(-B * (1 - x))`
 *
 * The equation takes this form in Humphries's thesis.
 *
 */
void RHprof(double RH, int nlayers, double* relative_humidity_profile)
{
    if (RH > 1 || RH < 0) {
        throw std::out_of_range("RH must be between 0 and 1.");
    }
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }

    const double kh = 1 - RH;

    for (int i = 0; i < nlayers; ++i) {
        double j = i + 1;  // Explicitly make j a double so that j / nlayers isn't truncated.
        double temp_rh = RH * exp(kh * (j / nlayers));
        relative_humidity_profile[i] = temp_rh;
    }
}

void LNprof(double LeafN, double LAI, double kpLN, vector<double>& leafN_profile)
{
    auto nlayers = leafN_profile.size();
    double LI = LAI / nlayers;
    for (vector<double>::size_type i = 0; i < nlayers; ++i) {
        double CumLAI = LI * (i + 1);
        leafN_profile[i] = LeafN * exp(-kpLN * (CumLAI - LI));
    }
}

/* Soil Evaporation Function */
/* Variables I need */
/* LAI = Leaf Area Index */
/* k = extinction coefficient */
/* ppfd = Photosynthetic photon flux density */
/* awc, wiltp, fieldc = available water content, wilting point and field capacity */
/* winds = wind speed */

double SoilEvapo(
    double LAI,
    double k,
    double air_temperature,
    double ppfd,
    double soil_water_content,
    double fieldc,
    double wiltp,
    double winds,
    double RelH,
    double rsec,
    double soil_clod_size,
    double soil_reflectance,
    double soil_transmission,
    double specific_heat_of_air,
    double par_energy_content)
{
    int method = 1;

    // A simple way of calculating the proportion of the soil that is hit by
    // direct radiation.
    double soil_area_sunlit_fraction = exp(-k * LAI);  // dimensionless.

    // For now the temperature of the soil will be the same as the air. At a
    // later time this can be made more accurate. I looked at the equations for
    // this and the issue is that it is strongly dependent on depth. Since the
    // soil model now has a single layer, this cannot be implemented correctly
    // at the moment.
    double SoilTemp = air_temperature;

    /* From Campbell and Norman. Environmental Biophysics. */
    /* If relative available water content is */
    double rawc = (soil_water_content - wiltp) / (fieldc - wiltp);  // dimensionless. relative available water content.

    /* Campbell and Norman. Environmental Physics, page 142 */
    double maximum_uptake_rate = 1 - pow((1 + 1.3 * rawc), -5);  // dimensionless
    /* This is a useful idea because dry soils evaporate little water when dry*/

    // Total Radiation: At the moment soil evaporation is grossly overestimated.
    // In WIMOVAC the light reaching the last layer of leaves is used. Here
    // instead of calculating this again, I will for now assume a 10% as a rough
    // estimate. Note that I could maybe get this since layIdir and layIDiff in
    // sunML are external variables.  Rprintf("ppfd %.5f",layIdir[0],"\n");
    // Update: 03-13-2009. I tried printing this value but it is still too high
    // and will likely overestimate soil evaporation. However, this is still a
    // work in progress.
    ppfd *= rsec; /* Radiation soil evaporation coefficient */

    double TotalRadiation = ppfd * par_energy_content;

    double DdryA = TempToDdryA(air_temperature);
    double LHV = TempToLHV(air_temperature);  // J / kg
    double SlopeFS = TempToSFS(air_temperature);
    double SWVC = saturation_vapor_pressure(air_temperature) * 1e-5;

    double PsycParam = (DdryA * specific_heat_of_air) / LHV;
    double vapor_density_deficit = SWVC * (1 - RelH / 100);

    double BoundaryLayerThickness = 4e-3 * sqrt(soil_clod_size / winds);
    double DiffCoef = 2.126e-5 + 1.48e-7 * SoilTemp;
    double SoilBoundaryLayer = DiffCoef / BoundaryLayerThickness;

    // Here we calculate the total amount of PAR energy absorbed by the soil
    // using `thick_layer_absorption`. We assume half of the solar energy lies
    // in the PAR band, so we multiply by 2 to get the total absorbed solar
    // energy. This is almost certainly untrue for light that has passed through
    // a plant canopy.
    double Ja = 2 * thick_layer_absorption(
                        soil_reflectance,
                        soil_transmission,
                        TotalRadiation);

    double rlc = 4 * physical_constants::stefan_boltzmann * pow((conversion_constants::celsius_to_kelvin + SoilTemp), 3) * 0.005;
    // The last term should be the difference between air temperature and soil.
    // This is not actually calculated at the moment. Since this is mostly
    // relevant to the first soil layer where the temperatures are similar. I
    // will leave it like this for now.

    double PhiN = Ja - rlc; /* Calculate the net radiation balance*/
    if (PhiN < 0) PhiN = 1e-7;

    double Evaporation = 0.0;
    if (method == 0) {
        /* Priestly-Taylor */
        Evaporation = 1.26 * (SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s.
    } else {
        /* Penman-Monteith */
        Evaporation = (SlopeFS * PhiN + LHV * PsycParam * SoilBoundaryLayer * vapor_density_deficit) / (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s.
    }

    Evaporation *= soil_area_sunlit_fraction * maximum_uptake_rate;  // kg / m^2 / s.
    if (Evaporation < 0) Evaporation = 1e-6;                         // Prevent odd values at very low light levels.

    return Evaporation;  // kg / m^2 / s.
}

double compute_wsPhoto(int wsFun, double fieldc, double wiltp, double phi1, double awc)
{
    // Three different type of equations for modeling the effect of
    // water stress on vmax and leaf area expansion.  The equation for
    // leaf area expansion is more severe than the one for vmax. */
    double wsPhoto;
    switch (wsFun) {
        case 0: { /* linear */
            double slp = 1 / (fieldc - wiltp);
            double intcpt = 1 - fieldc * slp;
            wsPhoto = slp * awc + intcpt;
            break;
        }
        case 1: {
            double phi10 = (fieldc + wiltp) / 2;
            wsPhoto = 1 / (1 + exp((phi10 - awc) / phi1));
            break;
        }
        case 2: {
            double slp = (1 - wiltp) / (fieldc - wiltp);
            double intcpt = 1 - fieldc * slp;
            double theta = slp * awc + intcpt;
            wsPhoto = (1 - exp(-2.5 * (theta - wiltp) / (1 - wiltp))) / (1 - exp(-2.5));
            break;
        }
        case 3:
            wsPhoto = 1;
            break;
        default:
            wsPhoto = 0;
    }

    // wsPhoto can be mathematically lower than zero in some cases but
    // I should prevent that:
    if (wsPhoto <= 0) {
        wsPhoto = 1e-10;
    }

    // Apparently wsPhoto can be greater than 1.
    if (wsPhoto > 1) wsPhoto = 1;

    return wsPhoto;
}

/* This is a new function that attempts to keep a water budget and then
   calcualte an empirical coefficient that reduces the specific leaf area.
   This results from the general idea that water stress reduces first the
   rate of leaf expansion. */

/* This is meant to be a simple function that calculates a
   simple empirical coefficient that reduces specifi leaf area
   according to the water stress of the plant. This is done
   for now, with a very simple empirical approach. */

ws_str watstr(
    double precipit,
    double evapo,
    double cws,
    double soildepth,
    double fieldc,
    double wiltp,
    double soil_saturation_capacity,
    double sand,
    double Ks,
    double air_entry,
    double b)
{
    constexpr double g = 9.8;  // m / s^2  ##  http://en.wikipedia.org/wiki/Standard_gravity

    double precipM = precipit * 1e-3;                        /* convert precip in mm to m*/
    double soil_water_fraction = precipM / soildepth + cws;  // m^3 m^-3

    double runoff = 0.0;
    double Nleach = 0.0; /* Nleach is the NO3 leached. */

    if (soil_water_fraction > soil_saturation_capacity) {
        runoff = (soil_water_fraction - soil_saturation_capacity) * soildepth; /* This is in meters */
        /* Here runoff is interpreted as water content exceeding saturation level */
        /* Need to convert to units used in the Parton et al 1988 paper. */
        /* The data come in mm/hr and need to be in cm/month */
        Nleach = runoff / (1e3 * physical_constants::molar_mass_of_water) * (0.2 + 0.7 * sand);
        soil_water_fraction = soil_saturation_capacity;
    }

    /* The density of water is 0.9982 Mg / m^3 at 20 degrees Celsius. */
    /* evapo is demanded water (Mg / ha) */
    double npaw = soil_water_fraction - wiltp - evapo / 0.9982 / 1e4 / soildepth;  // fraction of saturation.

    /* If demand exceeds supply, the crop is getting close to wilting point and transpiration
       will be over estimated. In this one layer model though, the crop is practically dead. */
    if (npaw < 0) npaw = 0.0;
    double awc = npaw + wiltp;

    /* Calculating the soil water potential based on equations from Norman and Campbell */
    /* tmp.psim = soTexS.air_entry * pow((awc/soTexS.fieldc*1.1),-soTexS.b); */
    /* New version of the soil water potential is based on
     * "Dynamic Simulation of Water Deficit Effects upon Maize Yield" R. F. Grant Agricultural Systems. 33(1990) 13-39. */
    ws_str tmp;
    tmp.psim = -exp(log(0.033) + (log(fieldc) - log(awc)) / (log(fieldc) - log(wiltp)) * (log(1.5) - log(0.033))) * 1e3; /* This last term converts from MPa to kPa */

    /* This is drainage */
    if (awc > fieldc) {
        double K_psim = Ks * pow((air_entry / tmp.psim), 2 + 3 / b); /* This is hydraulic conductivity */

        // Campbell, pg 129 do not ignore the graviational effect. I multiply
        // soil depth by 0.5 to calculate the average depth.
        double J_w = -K_psim * (-tmp.psim / (soildepth * 0.5)) - g * K_psim;

        double drainage = J_w * 3600 * 0.9982 * 1e-3; /* This is flow in m^3 / (m^2 * hr). */
        awc = awc + drainage / soildepth;
    }

    tmp.awc = awc;
    tmp.runoff = runoff;
    tmp.Nleach = Nleach;
    return tmp;
}

/* Function to simulate the multilayer behavior of soil water. In the
   future this could be coupled with Campbell (BASIC) ideas to
   esitmate water potential. */
soilML_str soilML(
    double precipit,
    double transp,
    double* cws,
    double soildepth,
    double* depths,
    double soil_field_capacity,
    double soil_wilting_point,
    double soil_saturation_capacity,
    double soil_air_entry,
    double soil_saturated_conductivity,
    double soil_b_coefficient,
    double soil_sand_content,
    double phi1,
    double phi2,
    int wsFun,
    int layers,
    double rootDB,
    double LAI,
    double k,
    double AirTemp,
    double IRad,
    double winds,
    double RelH,
    int hydrDist,
    double rfl,
    double rsec,
    double rsdf,
    double soil_clod_size,
    double soil_reflectance,
    double soil_transmission,
    double specific_heat_of_air,
    double par_energy_content)
{
    constexpr double g = 9.8; /* m / s-2  ##  http://en.wikipedia.org/wiki/Standard_gravity */

    soilML_str return_value;

    /* Crude empirical relationship between root biomass and rooting depth*/
    double rootDepth = fmin(rootDB * rsdf, soildepth);

    rd_str root_distribution = rootDist(layers, rootDepth, &depths[0], rfl);

    /* unit conversion for precip */
    double oldWaterIn = 0.0;
    double waterIn = precipit * 1e-3; /* convert precip in mm to m */

    double drainage = 0.0;
    double wsPhotoCol = 0.0;
    double LeafWSCol = 0.0;
    double Sevap = 0.0;
    double oldEvapoTra = 0.0;

    // The counter, i, decreases because I increase the water content due to
    // precipitation in the last layer first
    for (int i = layers - 1; i >= 0; --i) {
        double layerDepth = depths[i + 1] - depths[i]; /* This supports unequal depths. */

        if (hydrDist > 0) {
            /* For this section see Campbell and Norman "Environmental BioPhysics" Chapter 9*/
            /* First compute the matric potential */
            double psim1 = soil_air_entry * pow((cws[i] / soil_saturation_capacity), -soil_b_coefficient); /* This is matric potential of current layer */
            double dPsim;
            if (i > 0) {
                double psim2 = soil_air_entry * pow((cws[i - 1] / soil_saturation_capacity), -soil_b_coefficient); /* This is matric potential of next layer */
                dPsim = psim1 - psim2;
                // The substraction is from the layer i - (i-1). If this last
                // term is positive then it will move upwards. If it is negative
                // it will move downwards. Presumably this term is almost always
                // positive.
            } else {
                dPsim = 0.0;
            }

            double K_psim = soil_saturated_conductivity * pow((soil_air_entry / psim1), 2 + 3 / soil_b_coefficient); /* This is hydraulic conductivity */
            double J_w = K_psim * (dPsim / layerDepth) - g * K_psim;                                                 /*  Campbell, pg 129 do not ignore the graviational effect*/
            /* Notice that K_psim is positive because my reference system is reversed */
            /* This last result should be in kg/(m2 * s) */
            J_w *= 3600 * 0.9882 * 1e-3; /* This is flow in m3 / (m^2 * hr). */

            if (i == (layers - 1) && J_w < 0) {
                /* cws[i] = cws[i] + J_w / layerDepth; Although this should be done it drains the last layer too much.*/
                drainage += J_w;
            } else {
                cws[i] = cws[i] - J_w / layerDepth;
                if (i > 0) {
                    cws[i - 1] = cws[i - 1] + J_w / layerDepth;
                }
            }
        }

        if (cws[i] > soil_saturation_capacity) cws[i] = soil_saturation_capacity;
        if (cws[i] < soil_wilting_point) cws[i] = soil_wilting_point;
        // Here is a convention aw is available water in volume and awc
        // is available water content as a fraction of the soil section being investigated.
        double aw = cws[i] * layerDepth;
        /* Available water (for this layer) is the current water status times the layer depth */

        if (waterIn > 0) {
            /* There is some rain. Need to add it.*/
            aw += waterIn / layers + oldWaterIn; /* They are both in meters so it works */
            /* Adding the same amount to water to each layer */
            /* In case there is overflow */
            double diffw = soil_field_capacity * layerDepth - aw;

            if (diffw < 0) {
                /* This means that precipitation exceeded the capacity of the first layer */
                /* Save this amount of water for the next layer */
                oldWaterIn = -diffw;
                aw = soil_field_capacity * layerDepth;
            } else {
                oldWaterIn = 0.0;
            }
        }

        /* Root Biomass */
        double rootATdepth = rootDB * root_distribution.rootDist[i];
        return_value.rootDist[i] = rootATdepth;
        /* Plant available water is only between current water status and permanent wilting point */
        /* Plant available water */
        double pawha = (aw - soil_wilting_point * layerDepth) * 1e4;

        if (pawha < 0) pawha = 0;

        double Ctransp = 0.0;
        double EvapoTra = 0.0;
        double Newpawha;

        if (i == 0) {
            /* Only the first layer is affected by soil evaporation */
            double awc2 = aw / layerDepth;
            /* SoilEvapo function needs soil water content  */
            Sevap = SoilEvapo(
                        LAI, k, AirTemp, IRad, awc2, soil_field_capacity,
                        soil_wilting_point, winds, RelH, rsec, soil_clod_size,
                        soil_reflectance, soil_transmission, specific_heat_of_air,
                        par_energy_content) *
                    3600 * 1e-3 * 10000;  // Mg / ha / hr. 3600 s / hr * 1e-3 Mg / kg * 10000 m^2 / ha.
            /* I assume that crop transpiration is distributed simlarly to
               root density.  In other words the crop takes up water proportionally
               to the amount of root in each respective layer.*/
            Ctransp = transp * root_distribution.rootDist[0];
            EvapoTra = Ctransp + Sevap;
            constexpr double density_of_water_at_20_celcius = 0.9982;  // Mg m^-3.
            Newpawha = pawha - EvapoTra / density_of_water_at_20_celcius;
            /* The first term in the rhs pawha is the m3 of water available in this layer.
               EvapoTra is the Mg H2O ha-1 of transpired and evaporated water. 1/0.9882 converts from Mg to m3 */
        } else {
            Ctransp = transp * root_distribution.rootDist[i];
            EvapoTra = Ctransp;
            Newpawha = pawha - (EvapoTra + oldEvapoTra);
        }

        if (Newpawha < 0) {
            /* If the Demand is not satisfied by this layer. This will be stored and added to subsequent layers*/
            oldEvapoTra = -Newpawha;
            aw = soil_wilting_point * layerDepth;
        }

        double awc = Newpawha / 1e4 / layerDepth + soil_wilting_point;

        /* This might look like a weird place to populate the structure, but is more convenient*/
        return_value.cws[i] = awc;

        // To-do: Replace this block with a call to compute_wsPhoto.
        /* three different type of equations for modeling the effect of water stress on vmax and leaf area expansion.
       The equation for leaf area expansion is more severe than the one for vmax. */
        double wsPhoto = 0.0;
        double slp = 0.0;
        double intcpt = 0.0;
        double theta = 0.0;

        if (wsFun == 0) { /* linear */
            slp = 1 / (soil_field_capacity - soil_wilting_point);
            intcpt = 1 - soil_field_capacity * slp;
            wsPhoto = slp * awc + intcpt;
        } else if (wsFun == 1) {
            double phi10 = (soil_field_capacity + soil_wilting_point) / 2;
            wsPhoto = 1 / (1 + exp((phi10 - awc) / phi1));
        } else if (wsFun == 2) {
            slp = (1 - soil_wilting_point) / (soil_field_capacity - soil_wilting_point);
            intcpt = 1 - soil_field_capacity * slp;
            theta = slp * awc + intcpt;
            wsPhoto = (1 - exp(-2.5 * (theta - soil_wilting_point) / (1 - soil_wilting_point))) / (1 - exp(-2.5));
        } else if (wsFun == 3) {
            wsPhoto = 1;
        }

        if (wsPhoto <= 0)
            wsPhoto = 1e-10; /* This can be mathematically lower than zero in some cases but I should prevent that. */

        wsPhotoCol += wsPhoto;

        double LeafWS = pow(awc, phi2) * 1 / pow(soil_field_capacity, phi2);
        if (wsFun == 3) {
            LeafWS = 1;
        }
        LeafWSCol += LeafWS;
    }

    if (waterIn > 0) {
        drainage = waterIn;
        /* Need to convert to units used in the Parton et al 1988 paper. */
        /* The data comes in mm/hr and it needs to be in cm/month */
        return_value.Nleach = drainage * 0.1 * (1 / 24 * 30) / (1e3 * physical_constants::molar_mass_of_water * (0.2 + 0.7 * soil_sand_content));
    } else {
        return_value.Nleach = 0.0;
    }

    return_value.rcoefPhoto = (wsPhotoCol / layers);
    return_value.drainage = drainage;
    return_value.rcoefSpleaf = (LeafWSCol / layers);
    return_value.SoilEvapo = Sevap;

    return return_value;
}

seqRD_str seqRootDepth(double to, int lengthOut)
{
    double by = to / lengthOut;

    seqRD_str result;
    for (int i = 0; i <= lengthOut; ++i) {
        result.rootDepths[i] = i * by;
    }
    return result;
}

rd_str rootDist(int n_layers, double rootDepth, double* depths, double rfl)
{
    /*
     * Calculate the fraction of total root mass for each layer in `depths` assuming the mass
     * is follows a Poisson distribution along the depth.
     *
     * Returns a struct with one array of size `n_layers`.
     * Each element in the array is the fraction of total root mass in that layer.
     * The sum of all elements of the result equals 1.
     */

    double layerDepth = 0.0;
    double CumLayerDepth = 0.0;
    int CumRootDist = 1;
    vector<double> rootDist(n_layers);
    double cumulative_a = 0.0;

    for (int i = 0; i < n_layers; ++i) {
        if (i == 0) {
            layerDepth = depths[1];
        } else {
            layerDepth = depths[i] - depths[i - 1];
        }

        CumLayerDepth += layerDepth;

        if (rootDepth > CumLayerDepth) {
            CumRootDist++;
        }
    }

    for (int j = 0; j < n_layers; ++j) {
        if (j < CumRootDist) {
            double a = poisson_density(j + 1, CumRootDist * rfl);
            rootDist[j] = a;
            cumulative_a += a;
        } else {
            rootDist[j] = 0;
        }
    }

    rd_str result;
    for (int k = 0; k < n_layers; ++k) {
        result.rootDist[k] = rootDist[k] / cumulative_a;
    }
    return result;
}
