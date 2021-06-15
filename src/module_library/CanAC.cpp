#include "BioCro.h"
#include "c4photo.h"

struct Can_Str CanAC(
    double LAI,          // dimensionless from m^2 / m^2
    int DOY,             // day
    double hr,           // hr
    double solarR,       // micromol / m^2 / s
    double temperature,  // degrees C
    double RH,           // dimensionless from Pa / Pa
    double WindSpeed,    // m / s
    double lat,          // degrees
    int nlayers,         // dimensionless
    double Vmax,
    double Alpha,
    double Kparm,
    double beta,  // dimensionless
    double Rd,
    double Catm,  // ppm
    double b0,
    double b1,
    double Gs_min,  // mmol / m^2 / s
    double theta,   // dimensionless
    double kd,
    double chil,
    double heightf,
    double leafN,
    double kpLN,
    double lnb0,
    double lnb1,
    int lnfun,      // dimensionless switch
    double upperT,  // degrees C
    double lowerT,  // degrees C
    const struct nitroParms& nitroP,
    double leafwidth,             // m
    int eteq,                     // dimensionless switch
    double StomataWS,             // dimensionless
    double specific_heat_of_air,  // J / kg / K
    double atmospheric_pressure,  // Pa
    int water_stress_approach,    // dimensionless switch
    double absorptivity_par       // dimensionless
)
{
    struct Light_model light_model = lightME(lat, DOY, hr, atmospheric_pressure);

    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double q_dir = light_model.direct_irradiance_fraction * solarR;    // micromole / m^2 / s
    double q_diff = light_model.diffuse_irradiance_fraction * solarR;  // micromole / m^2 / s
    double cosTh = light_model.cosine_zenith_angle;                    // dimensionless

    struct Light_profile light_profile =
        sunML(q_dir, q_diff, LAI, nlayers, cosTh, kd, chil, absorptivity_par,
              heightf);

    double LAIc = LAI / nlayers;  // dimensionless

    double relative_humidity_profile[nlayers];
    RHprof(RH, nlayers, relative_humidity_profile);  // Modifies relative_humidity_profile

    double wind_speed_profile[nlayers];
    WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);  // Modifies wind_speed_profile

    double leafN_profile[nlayers];
    LNprof(leafN, LAI, nlayers, kpLN, leafN_profile);  // Modifies leafN_profile

    double CanopyA = 0.0;             // micromol / m^2 / s
    double GCanopyA = 0.0;            // micromol / m^2 / s
    double CanopyT = 0.0;             // micromol / m^2 / s
    double CanopyPe = 0.0;            // micromol / m^2 / s
    double CanopyPr = 0.0;            // micromol / m^2 / s
    double canopy_conductance = 0.0;  // mmol / m^2 / s

    for (int i = 0; i < nlayers; ++i) {
        // Calculations that are the same for sunlit and shaded leaves
        int current_layer = nlayers - 1 - i;
        double leafN_lay = leafN_profile[current_layer];

        double vmax1;
        if (lnfun == 0) {
            vmax1 = Vmax;
        } else {
            vmax1 = nitroP.Vmaxb1 * leafN_lay + nitroP.Vmaxb0;
            if (vmax1 < 0) {
                vmax1 = 0.0;
            }
            if (vmax1 > Vmax) {
                vmax1 = Vmax;
            }
            Alpha = nitroP.alphab1 * leafN_lay + nitroP.alphab0;
            Rd = nitroP.Rdb1 * leafN_lay + nitroP.Rdb0;
        }

        double relative_humidity = relative_humidity_profile[current_layer];     // dimensionless
        double layer_wind_speed = wind_speed_profile[current_layer];             // m / s
        double CanHeight = light_profile.height[current_layer];                  // m
        double j_avg = light_profile.absorbed_shortwave_average[current_layer];  // J / m^2 / s

        // Calculations for sunlit leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double i_dir = light_profile.incident_ppfd_direct[current_layer];       // micromole / m^2 / s
        double j_dir = light_profile.absorbed_shortwave_direct[current_layer];  // J / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction[current_layer];         // dimensionless. Fraction of LAI that is sunlit.
        double Leafsun = LAIc * pLeafsun;                                       // dimensionless

        double direct_stomatal_conductance =
            c4photoC(
                i_dir, temperature, relative_humidity, vmax1, Alpha, Kparm,
                theta, beta, Rd, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, water_stress_approach, upperT, lowerT)
                .Gs;  // mmol / m^2 / s

        struct ET_Str et_direct =
            EvapoTrans2(
                j_dir, j_avg, temperature, relative_humidity, layer_wind_speed,
                LAIc, CanHeight, direct_stomatal_conductance, leafwidth,
                specific_heat_of_air, eteq);

        double leaf_temperature_dir = temperature + et_direct.Deltat;  // degrees C

        struct c4_str direct_photo =
            c4photoC(
                i_dir, leaf_temperature_dir, relative_humidity, vmax1, Alpha,
                Kparm, theta, beta, Rd, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, water_stress_approach, upperT, lowerT);

        // Calculations for shaded leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double i_diff = light_profile.incident_ppfd_diffuse[current_layer];       // micromole / m^2 / s
        double j_diff = light_profile.absorbed_shortwave_diffuse[current_layer];  // J / m^2 / s
        double pLeafshade = light_profile.shaded_fraction[current_layer];         // dimensionless. Fraction of LAI that is shaded.
        double Leafshade = LAIc * pLeafshade;                                     // dimensionless

        double diffuse_stomatal_conductance =
            c4photoC(
                i_diff, temperature, relative_humidity, vmax1, Alpha, Kparm,
                theta, beta, Rd, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, water_stress_approach, upperT, lowerT)
                .Gs;  // mmol / m^2 / s

        struct ET_Str et_diffuse =
            EvapoTrans2(
                j_diff, j_avg, temperature, relative_humidity, layer_wind_speed,
                LAIc, CanHeight, diffuse_stomatal_conductance, leafwidth,
                specific_heat_of_air, eteq);

        double leaf_temperature_diff = temperature + et_diffuse.Deltat;  // degrees C

        struct c4_str diffuse_photo =
            c4photoC(
                i_diff, leaf_temperature_diff, relative_humidity, vmax1, Alpha,
                Kparm, theta, beta, Rd, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, water_stress_approach, upperT, lowerT);

        // Combine sunlit and shaded leaves
        CanopyA += Leafsun * direct_photo.Assim + Leafshade * diffuse_photo.Assim;             // micromol / m^2 / s
        CanopyT += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;                 // micromol / m^2 / s
        GCanopyA += Leafsun * direct_photo.GrossAssim + Leafshade * diffuse_photo.GrossAssim;  // micromol / m^2 / s

        CanopyPe += Leafsun * et_direct.EPenman + Leafshade * et_diffuse.EPenman;        // micromol / m^2 / s
        CanopyPr += Leafsun * et_direct.EPriestly + Leafshade * et_diffuse.EPriestly;    // micromol / m^2 / s
        canopy_conductance += Leafsun * direct_photo.Gs + Leafshade * diffuse_photo.Gs;  // mmol / m^2 / s
    }

    /* For Assimilation */
    /* 3600 - seconds per hour */
    /* 1e-6 - moles per micromole */
    /* 30 - grams of C6H12O6 (glucose) incorporated into dry biomass per mole of CO2 */
    /* 1e-6 - megagrams per gram */
    /* 10000 - meters squared per hectare*/
    const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;

    /* For Transpiration */
    /* 3600 - seconds per hour */
    /* 1e-3 - millimoles per mole */
    /* 18 - grams per mole for H2O */
    /* 1e-6 - megagrams per  gram */
    /* 10000 - meters squared per hectare */
    const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000;

    struct Can_Str ans;
    ans.Assim = CanopyA * cf;                      // Mg / ha / hr
    ans.GrossAssim = GCanopyA * cf;                // Mg / ha / hr
    ans.Trans = CanopyT * cf2;                     // Mg / ha / hr
    ans.canopy_transpiration_penman = CanopyPe;    // micromol / m^2 / s
    ans.canopy_transpiration_priestly = CanopyPr;  // micromol / m^2 / s
    ans.canopy_conductance = canopy_conductance;   // micromol / m^2 / s

    return ans;
}
