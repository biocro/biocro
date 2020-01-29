#include "BioCro.h"
#include "c4photo.h"

struct Can_Str CanAC(
        double LAI,
        int DOY,
        double hr,
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

    double CanopyA = 0.0, GCanopyA = 0.0, CanopyT = 0.0, CanopyPe = 0.0, CanopyPr = 0.0;
    double canopy_conductance = 0.0;
    double result_matrix[21 * MAXLAY];

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
        double CanHeight = light_profile.height[current_layer];  // meters
        double Itot = light_profile.total_irradiance[current_layer];  // micromole / m^2 / s

        double IDir = light_profile.direct_irradiance[current_layer];  // micromole / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction[current_layer];  // dimensionless. Fraction of LAI that is sunlit.
        double Leafsun = LAIc * pLeafsun;

        double direct_stomatal_conductance = c4photoC(IDir, temperature, relative_humidity, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, water_stress_approach, upperT, lowerT).Gs; // mmol / m^2 / s
        struct ET_Str et_direct = EvapoTrans2(IDir, Itot, temperature, relative_humidity, layer_wind_speed, LAIc, CanHeight, direct_stomatal_conductance, leafwidth, eteq);
        double leaf_temperature_Idir = temperature + et_direct.Deltat;
        struct c4_str direct_photo = c4photoC(IDir, leaf_temperature_Idir, relative_humidity, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, water_stress_approach, upperT, lowerT);

        double IDiff = light_profile.diffuse_irradiance[current_layer];  // micromole / m^2 / s
        double pLeafshade = light_profile.shaded_fraction[current_layer];  // dimensionless. Fraction of LAI that is shaded.
        double Leafshade = LAIc * pLeafshade;

        //Rprintf("current_layer,CumLAI,leafN_lay,relative_humidity,layer_wind_speed,CanHeight,Itot,IDir,pLeafSun,IDiff,pLeafShade\n");
        //Rprintf("%i,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", current_layer, LAIc * (current_layer + 0.5), leafN_lay, relative_humidity, layer_wind_speed, CanHeight, Itot, IDir, pLeafsun, IDiff, pLeafshade);

        double diffuse_stomatal_conductance = c4photoC(IDiff, temperature, relative_humidity, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, water_stress_approach, upperT, lowerT).Gs;
        struct ET_Str et_diffuse = EvapoTrans2(IDiff, Itot, temperature, relative_humidity, layer_wind_speed, LAIc, CanHeight, diffuse_stomatal_conductance, leafwidth, eteq);
        double leaf_temp_Idiffuse = temperature + et_diffuse.Deltat;
        struct c4_str diffuse_photo = c4photoC(IDiff, leaf_temp_Idiffuse, relative_humidity, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, water_stress_approach, upperT, lowerT);

        CanopyA += Leafsun * direct_photo.Assim + Leafshade * diffuse_photo.Assim;
        CanopyT += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;
        GCanopyA += Leafsun * direct_photo.GrossAssim + Leafshade * diffuse_photo.GrossAssim;

        //Rprintf("shade leaf assim %f; sun leaf assim %f; shade trans %f; sun trans %f\n", diffuse_photo.Assim, direct_photo.Assim, et_diffuse.TransR, et_direct.TransR);

        CanopyPe += Leafsun * et_direct.EPenman + Leafshade * et_diffuse.EPenman;
        CanopyPr += Leafsun * et_direct.EPriestly + Leafshade * et_diffuse.EPriestly;
        canopy_conductance += Leafsun * direct_photo.Gs + Leafshade * diffuse_photo.Gs;

        result_matrix[    i*21] = IDir;
        result_matrix[1 + i*21] = IDiff;
        result_matrix[2 + i*21] = Leafsun;
        result_matrix[3 + i*21] = Leafshade;
        result_matrix[4 + i*21] = et_direct.TransR;
        result_matrix[5 + i*21] = et_diffuse.TransR;
        result_matrix[6 + i*21] = direct_photo.Assim;
        result_matrix[7 + i*21] = diffuse_photo.Assim;
        result_matrix[8 + i*21] = et_direct.Deltat;
        result_matrix[9 + i*21] = et_diffuse.Deltat;
        result_matrix[10 + i*21] = direct_photo.Gs;
        result_matrix[11 + i*21] = diffuse_photo.Gs;
        result_matrix[12 + i*21] = leafN_lay;
        result_matrix[13 + i*21] = vmax1;
        result_matrix[14 + i*21] = relative_humidity;
        result_matrix[15 + i*21] = direct_photo.GrossAssim;
        result_matrix[16 + i*21] = diffuse_photo.GrossAssim;
        result_matrix[17 + i*21] = Alpha;
        result_matrix[18 + i*21] = leafN_lay;
        result_matrix[19 + i*21] = layer_wind_speed;
        result_matrix[20 + i*21] = CanHeight;
    }

    struct Can_Str ans;
    /* For Assimilation */
    /* 3600 - seconds per hour */
    /* 1e-6 - moles per micromole */
    /* 30 - grams of C6H12O6 (glucose) incorporated into dry biomass per mole of CO2 */
    /* 1e-6 - megagrams per gram */
    /* 10000 - meters squared per hectare*/
    ans.Assim = CanopyA * 3600 * 1e-6 * 30 * 1e-6 * 10000;  // Mg / ha / hr.
    ans.GrossAssim = GCanopyA * 3600 * 1e-6 * 30 * 1e-6 * 10000;  // Mg / ha / hr.

    /* For Transpiration */
    /* 3600 - seconds per hour */
    /* 1e-3 - millimoles per mole */
    /* 18 - grams per mole for H2O */
    /* 1e-6 - megagrams per  gram */
    /* 10000 - meters squared per hectare */
    ans.Trans = CanopyT * 3600 * 1e-3 * 18 * 1e-6 * 10000;  // Mg / ha / hr.
    ans.canopy_transpiration_penman = CanopyPe;
    ans.canopy_transpiration_priestly = CanopyPr;
    ans.canopy_conductance = canopy_conductance;

    for (int i = 0; i < nlayers * 21; ++i) {
        ans.result_matrix[i] = result_matrix[i];
    }
    return ans;
}

