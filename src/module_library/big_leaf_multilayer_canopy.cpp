#include <math.h>
#include "convergence_iteration.hpp"
#include "big_leaf_multilayer_canopy.hpp"
#include "BioCro.h"

/*
state_map light_extinction(double Idir,          // micromole / m^2 / s
        double Idiff,                     // micromole / m^2 / s
        double LAI,                       // dimensionless from m^2 / m^2
        int nlayers,
        double cosTheta,                  // dimensionless
        double kd,                        //
        double chil,                      // dimensionless from m^2 / m^2.
        double heightf)                   // m^-1 from m^2 / m^2 / m.  Leaf area density; LAI per height of canopy.
{
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }
    if (cosTheta > 1 || cosTheta < -1) {
        throw std::out_of_range("cosTheta must be between -1 and 1.");
    }
    if (kd > 1 || kd < 0) {
        throw std::out_of_range("kd must be between 0 and 1.");
    }
    if (chil < 0) {
        throw std::out_of_range("chil must be non-negative.");
    }
    if (heightf <= 0) {
        throw std::out_of_range("heightf must greater than zero.");
    }

    constexpr double alphascatter = 0.8;

    double theta = acos(cosTheta);
    double k0 = sqrt( pow(chil, 2) + pow(tan(theta), 2) );
    double k1 = chil + 1.744 * pow((chil + 1.183), -0.733);
    double k = k0 / k1;  // Canopy extinction coefficient for an ellipsoidal leaf angle distribution. Page 251, Campbell and Norman. Environmental Biophysics. second edition.

    double LAIi = LAI / nlayers;

    Light_profile light_profile;
    const double Ibeam = Idir * cosTheta;
    double Isolar = Ibeam * k;
    for (int i = 0; i < nlayers; ++i) {
        const double CumLAI = LAIi * (i + 0.5);

        const double Iscat = Ibeam * (exp(-k * sqrt(alphascatter) * CumLAI) - exp(-k * CumLAI));

        double Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat;  // The exponential term is equation 15.6, pg 255 of Campbell and Normal. Environmental Biophysics. with alpha=1 and Kbe(phi) = Kd.
        const double Ls = (1 - exp(-k * LAIi)) * exp(-k * CumLAI) / k;

        double Fsun = Ls / LAIi;
        double Fshade = 1 - Fsun;

        double Iaverage = (Fsun * (Isolar + Idiffuse) + Fshade * Idiffuse) * (1 - exp(-k * LAIi)) / k;

        // For values of cosTheta close to or less than 0, in place of the
        // calculations above, we want to use the limits of the above
        // expressions as cosTheta approaches 0 from the right:
        if (cosTheta <= 1E-10) {
            Isolar = Idir / k1;
            Idiffuse = Idiff * exp(-kd * CumLAI);
            Fsun = 0;
            Fshade = 1;
            Iaverage = 0;
        }

        light_profile.direct_irradiance[i] = Isolar + Idiffuse;  // micromole / m^2 / s
        light_profile.diffuse_irradiance[i]= Idiffuse;  // micromole / m^2 / s
        light_profile.total_irradiance[i] = Iaverage;  // micromole / m^2 / s
        light_profile.sunlit_fraction[i] = Fsun;  // dimensionless from m^2 / m^2
        light_profile.shaded_fraction[i] = Fshade;  // dimensionless from m^2 / m^2
        light_profile.height[i] = (LAI - CumLAI) / heightf;  // meters
    }
    state_map result {
        { "direct_ppfd", ... },
        { "diffuse_ppdf", },
    }

}
*/


state_map big_leaf_multilayer_canopy::do_operation (state_map const& s) const
{
//    check_state s = check_state(ss);
    constexpr double wind_speed_extinction = 0.7;
    const double layer_LAI = s.at("lai") / s.at("nlayers");
    const double RH = s.at("rh");
    const double kh = 1 - RH;
    const double wind_speed = s.at("windspeed");
    const double kd = s.at("kd");

    constexpr double leaf_radiation_absorptivity = 0.8;

    Light_model irradiance_fractions = lightME(s.at("lat"), s.at("doy"), s.at("hour"));

    // TODO: The light at the top of the canopy should not be part of this function. They should be calculated before this is called.
    // If cos(theta) is less than zero, the Sun is below the horizon, and lightME sets direct fraction to 0.
    // Thus, calculation depending on direct_par_at_canopy_top should not need to check cos(theta), although other calcuations may need to.
    const double direct_par_at_canopy_top = irradiance_fractions.direct_irradiance_fraction * s.at("solar");
    const double diffuse_par_at_canopy_top = irradiance_fractions.diffuse_irradiance_fraction * s.at("solar");

    const double cosine_theta = irradiance_fractions.cosine_zenith_angle;
    const double theta = acos(cosine_theta);
    const double chil = s.at("chil");
    const double k0 = sqrt( pow(chil, 2) + pow(tan(theta), 2) );
    const double k1 = chil + 1.744 * pow((chil + 1.183), -0.733);
    const double light_extinction_k = k0 / k1;  // Canopy extinction coefficient for an ellipsoidal leaf angle distribution. Page 251, Campbell and Norman. Environmental Biophysics. second edition.

    const double beam_radiation_at_canopy_top = direct_par_at_canopy_top * cosine_theta;
    double mean_incident_direct_par = beam_radiation_at_canopy_top * light_extinction_k;  // This is the same for all layers.
    const double n_layers = s.at("nlayers");
    //const double LeafN = s.at("LeafN");
    //const double kpLN = s.at("kpLN");
    const double canopy_lai = s.at("lai");

    double canopy_assimilation = 0;
    double canopy_conductance = 0;
    double canopy_transpiration = 0;

    state_map layer_state(s);
    layer_state["vmax"] = s.at("vmax1");
    layer_state["alpha"] = s.at("alpha1");

    for (unsigned int n = 0; n < n_layers; ++n) {
        const double layer_lai = canopy_lai / n_layers;
        const double CumLAI = layer_lai * (n + 1);
        const double layer_wind_speed = wind_speed * exp(-wind_speed_extinction * (CumLAI - layer_LAI));
        //const double layer_leafN      = LeafN     * exp(-kpLN * (CumLAI - layer_LAI));

        const double j = n + 1;  // Explicitly make j a double so that j / n_layers isn't truncated.
        const double layer_relative_humidity = RH * exp(kh * (j / n_layers));

        const double layer_scattered_par = beam_radiation_at_canopy_top * (exp(-light_extinction_k * sqrt(leaf_radiation_absorptivity) * CumLAI) - exp(-light_extinction_k * CumLAI));
        double layer_diffuse_par = diffuse_par_at_canopy_top * exp(-kd * CumLAI) + layer_scattered_par;  // The exponential term is equation 15.6, pg 255 of Campbell and Normal. Environmental Biophysics. with alpha=1 and Kbe(phi) = Kd.

        double sunlit_lai = (1 - exp(-light_extinction_k * layer_lai)) * exp(-light_extinction_k * CumLAI) / light_extinction_k;

        if (cosine_theta <= 1e-10) {
            // For values of cos(theta) close to or less than 0, the sun is below the horizon.
            // In that case, no leaves are sunlit.
            // I don't know how C++ handles that calculation since light_extinction_k would be infinite, so set sunlai_lai to 0 explicitly.
            sunlit_lai = 0;
            mean_incident_direct_par = direct_par_at_canopy_top / k1;
            layer_diffuse_par = diffuse_par_at_canopy_top * exp(-light_extinction_k * CumLAI);

            //Rprintf("below horizon\n");
        }

        const double shaded_lai = layer_lai - sunlit_lai;

        const double layer_shaded_par = layer_diffuse_par;
        const double layer_sunlit_par = mean_incident_direct_par + layer_shaded_par;

        constexpr double fraction_of_irradiance_in_PAR = 0.5;  // dimensionless.
        constexpr double joules_per_micromole_PAR = 0.235;   // J / micromole. For the wavelengths that make up PAR in sunlight, one mole of photons has, on average, approximately 2.35 x 10^5 joules:

        const double layer_sunlit_incident_irradiance = layer_sunlit_par * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR;  // W / m^2.
        const double layer_shaded_incident_irradiance = layer_shaded_par * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR; // W / m^2.

        layer_state["layer_wind_speed"] = layer_wind_speed;
        layer_state["rh"] = layer_relative_humidity;

        layer_state = replace_or_insert_state(layer_state, this->water_vapor_module->run(layer_state));

        layer_state["incident_par"] = layer_sunlit_par;
        layer_state["incident_irradiance"] = layer_sunlit_incident_irradiance;

        state_map sunlit_leaf = combine_state(layer_state, this->leaf_assim_module->run(layer_state));
        //sunlit_leaf = combine_state(sunlit_leaf, this->leaf_evapo_module->run(sunlit_leaf));
        state_map sunlit_trans = this->leaf_evapo_module->run(sunlit_leaf);

        layer_state["incident_par"] = layer_shaded_par;
        layer_state["incident_irradiance"] = layer_shaded_incident_irradiance;
        state_map shaded_leaf = combine_state(layer_state, this->leaf_assim_module->run(layer_state));
        //shaded_leaf = combine_state(shaded_leaf, this->leaf_evapo_module->run(shaded_leaf));
        state_map shaded_trans = this->leaf_evapo_module->run(shaded_leaf);

        canopy_assimilation += sunlit_lai * sunlit_leaf["leaf_assimilation_rate"] + shaded_lai * shaded_leaf["leaf_assimilation_rate"];  // micromoles / m^2 / s. Ground-area basis.
        canopy_conductance += sunlit_lai * sunlit_leaf["leaf_stomatal_conductance"] + shaded_lai * shaded_leaf["leaf_stomatal_conductance"];  // mmol / m^2 / s. Ground-area basis.
        canopy_transpiration += sunlit_lai * sunlit_trans["leaf_transpiration_rate"] + shaded_lai * shaded_trans["leaf_transpiration_rate"];  // kg / m^2 / s. Ground-area basis.

        //canopy_assimilation += shaded_lai * shaded_leaf["leaf_assimilation_rate"];  // micromoles / m^2 / s. Ground-area basis.
        //canopy_conductance += shaded_lai * shaded_leaf["leaf_stomatal_conductance"];  // mmol / m^2 / s. Ground-area basis.
        //canopy_transpiration += shaded_lai * shaded_leaf["leaf_transpiration_rate"];  // kg / m^2 / s. Ground-area basis.


        if (canopy_assimilation * 3600 * 1e-6 * 30 * 1e-6 * 10000 < -0.01) {
            //output_map(sunlit_leaf);
            //output_map(shaded_leaf);
            //Rprintf("cosine_theta: %f\n", cosine_theta);
        }
        //output_map(sunlit_leaf);
        //Rprintf("Loop %i; canopy_transpiration %f; shaded_trans %f; sunlit_trans %f; shaded_lai %f; sunlit_lai %f; canopy_lai %f; solar %f; shaded_par %f; shade_leaf[assim] %f; sunlit_par %f; sun_leaf[assim] %f\n", n, canopy_transpiration, shaded_leaf.at("leaf_transpiration_rate"), sunlit_leaf.at("leaf_transpiration_rate"), shaded_lai, sunlit_lai, canopy_lai, s.at("solar"), layer_shaded_par, shaded_leaf.at("leaf_assimilation_rate"), layer_sunlit_par, sunlit_leaf.at("leaf_assimilation_rate"));
        //output_map(shaded_leaf);
    }

    /* Convert assimilation units
     * 3600 - seconds per hour
     * 1e-6 - moles per micromole
     * 30 - grams per mole for CO2
     * 1e-6 - megagrams per gram
     * 10000 - meters squared per hectar
     */

    /* Convert transpiration units
     * 3600 - seconds per hour
     * 1e-3 - megagrams per kilogram
     * 10000 - meters squared per hectare
     */

    //TODO: Figure out why canopy_conductance is always 0.
    state_map new_state = {
        { "canopy_assimilation_rate", canopy_assimilation * 3600 * 1e-6 * 30 * 1e-6 * 10000 },
        { "canopy_transpiration_rate", canopy_transpiration * 3600 * 1e-3 * 10000 },
        { "canopy_conductance", canopy_conductance },
    };

    return new_state;
}

struct Can_Str newCanAC(
        double LAI,
        int DOY,
        int hr,
        double solarR,
        double temperature,  // degrees C
        double RH,           // dimensionless from Pa / Pa
        double WindSpeed,    // m / s
        double lat,
        int nlayers,
        double Vmax,
        double Alpha,
        double Kparm,
        double beta,
        double Rd,
        double Catm,
        double b0,
        double b1,
        double theta,
        double kd,
        double chil,
        double heightf,
        double leafN,
        double kpLN,
        double lnb0,
        double lnb1,
        int lnfun,
        double upperT,
        double lowerT,
        const struct nitroParms &nitroP,
        double leafwidth,    // meter
        int eteq,
        double StomataWS,
        int water_stress_approach)
{
    IModule * leaf_assim_module = new collatz_leaf();
    IModule * leaf_evapo_module = new penman_monteith_transpiration();
    IModule * water_vapor_module = new water_vapor_properties_from_air_temperature();
    struct Light_model light_model = lightME(lat, DOY, hr);

    double Idir = light_model.direct_irradiance_fraction * solarR;  // micromole / m^2 / s. Flux through a plane perpendicular to the rays of the sun.
    double Idiff = light_model.diffuse_irradiance_fraction * solarR;
    double cosTh = light_model.cosine_zenith_angle;

    struct Light_profile light_profile = sunML(Idir, Idiff, LAI, nlayers, cosTh, kd, chil, heightf);

    double LAIc = LAI / nlayers;

    double relative_humidity_profile[nlayers];
    RHprof(RH, nlayers, relative_humidity_profile);  // Modifies relative_humidity_profile.

    double wind_speed_profile[nlayers];
    WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);  // Modifies wind_speed_profile.

    double leafN_profile[nlayers];
    LNprof(leafN, LAI, nlayers, kpLN, leafN_profile);  // Modifies leafN_profile.

    double CanopyA = 0.0,  CanopyT = 0.0;
    double canopy_conductance = 0.0;

    for (int i = 0; i < nlayers; ++i)
    {
        int current_layer = nlayers - 1 - i;
        double leafN_lay = leafN_profile[current_layer];

        double vmax1;
        if (lnfun == 0) {
            vmax1 = Vmax;
        } else {
            vmax1 = nitroP.Vmaxb1 * leafN_lay + nitroP.Vmaxb0;
            if (vmax1 < 0) vmax1 = 0.0;
            if (vmax1 > Vmax) vmax1 = Vmax;
            Alpha = nitroP.alphab1 * leafN_lay + nitroP.alphab0;
            Rd = nitroP.Rdb1 * leafN_lay + nitroP.Rdb0;
        }

        double relative_humidity = relative_humidity_profile[current_layer];
        double layer_wind_speed = wind_speed_profile[current_layer];
        double Itot = light_profile.total_irradiance[current_layer];  // micromole / m^2 / s

        double IDir = light_profile.direct_irradiance[current_layer];  // micromole / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction[current_layer];  // dimensionless. Fraction of LAI that is sunlit.
        double Leafsun = LAIc * pLeafsun;

        state_map layer_state {
            {"temp", temperature},
            {"water_stress_approach", water_stress_approach},
            {"StomataWS",  StomataWS},
            {"Catm", Catm},
            {"kparm", Kparm},
            {"theta", theta},
            {"beta", beta},
            {"Rd", Rd},
            {"upperT", upperT},
            {"lowerT", lowerT},
            {"k_Q10", 2},
            {"rh", RH},
            {"b0", b0},
            {"b1", b1},
            {"leafwidth", leafwidth},
            {"hour", 1 },
            {"doy", 1 },
        };

        layer_state["wind_speed"] = layer_wind_speed;
        layer_state["vmax"] = vmax1;
        layer_state["alpha"] = Alpha;
        layer_state["rh"] = relative_humidity;

        layer_state = combine_state(layer_state, water_vapor_module->run(layer_state));

        layer_state["incident_par"] = IDir;
        layer_state["incident_irradiance"] = Itot;
        state_map sunlit_leaf = combine_state(layer_state, leaf_assim_module->run(layer_state));
        sunlit_leaf = combine_state(sunlit_leaf, leaf_evapo_module->run(sunlit_leaf));

        double IDiff = light_profile.diffuse_irradiance[current_layer];  // micromole / m^2 / s
        double pLeafshade = light_profile.shaded_fraction[current_layer];  // dimensionless. Fraction of LAI that is shaded.
        double Leafshade = LAIc * pLeafshade;

        layer_state["incident_par"] = IDiff;
        layer_state["incident_irradiance"] = Itot;
        state_map shaded_leaf = combine_state(layer_state, leaf_assim_module->run(layer_state));
        shaded_leaf = combine_state(shaded_leaf, leaf_evapo_module->run(shaded_leaf));

        CanopyA += Leafsun * sunlit_leaf["leaf_assimilation_rate"] + Leafshade * shaded_leaf["leaf_assimilation_rate"];
        CanopyT += Leafsun * sunlit_leaf["leaf_transpiration_rate"] + Leafshade * shaded_leaf["leaf_transpiration_rate"];

        canopy_conductance += Leafsun * sunlit_leaf["leaf_conductance"] + Leafshade * shaded_leaf["leaf_conductance"];
    }

    delete leaf_assim_module;
    delete leaf_evapo_module;
    delete water_vapor_module;

    struct Can_Str ans;
    /* For Assimilation */
    /* 3600 - seconds per hour */
    /* 1e-6 - moles per micromole */
    /* 30 - grams per mole for CO2 */
    /* 1e-6 - megagrams per gram */
    /* 10000 - meters squared per hectare*/
    ans.Assim = CanopyA * 3600 * 1e-6 * 30 * 1e-6 * 10000;  // Mg / ha / hr.

    /* For Transpiration */
    /* 3600 - seconds per hour */
    /* 1e-3 - millimoles per mole */
    /* 18 - grams per mole for H2O */
    /* 1e-6 - megagrams per  gram */
    /* 10000 - meters squared per hectare */
    ans.Trans = CanopyT * 3600 * 1e-3 * 18 * 1e-6 * 10000;  // Mg / ha / hr.
    ans.canopy_conductance = canopy_conductance;

    return ans;
}

state_map canac_with_collatz::do_operation (state_map const& s) const
{

    struct nitroParms nitroP;
    nitroP.ileafN = s.at("nileafn");
    nitroP.kln = s.at("nkln");
    nitroP.Vmaxb1 = s.at("nvmaxb1");
    nitroP.Vmaxb0 = s.at("nvmaxb0");
    nitroP.alphab1 = s.at("nalphab1");
    nitroP.alphab0 = s.at("nalphab0");
    nitroP.Rdb1 = s.at("nRdb1");
    nitroP.Rdb0 = s.at("nRdb0");
    nitroP.kpLN = s.at("nkpLN");
    nitroP.lnb0 = s.at("nlnb0");
    nitroP.lnb1 = s.at("nlnb1");

    struct Can_Str can_result = newCanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax1"),
            s.at("alpha1"), s.at("kparm"), s.at("beta"), s.at("Rd"), s.at("Catm"),
            s.at("b0"), s.at("b1"), s.at("theta"), s.at("kd"), s.at("chil"),
            s.at("heightf"), s.at("LeafN"), s.at("kpLN"), s.at("lnb0"), s.at("lnb1"),
            (int)s.at("lnfun"), s.at("upperT"), s.at("lowerT"), nitroP, s.at("leafwidth"),
            (int)s.at("et_equation"), s.at("StomataWS"), (int)s.at("water_stress_approach"));

    state_map new_state {
        { "canopy_assimilation_rate", can_result.Assim },  // Mg / ha / hr.
        { "canopy_transpiration_rate", can_result.Trans },  // Mg / ha / hr.
        { "GrossAssim", can_result.GrossAssim }
    };

    return new_state;
}

