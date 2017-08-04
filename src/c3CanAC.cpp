#include "BioCro.h"
#include "c3photo.h"

struct Can_Str c3CanAC(double LAI,
		int DOY,
		int hr,
		double solarR,
		double air_temperature,
		double RH,
		double WindSpeed,
		double lat,
		int nlayers,
		double Vmax,
		double Jmax,
		double Rd,
		double Catm,
		double o2,
		double b0,
		double b1,
		double theta,
		double kd,
		double heightf,
		double leafN,
		double kpLN,
		double lnb0,
		double lnb1,
		int lnfun,
        double chil,
		double StomataWS,
		int water_stress_approach,
        double electrons_per_carboxylation,
        double electrons_per_oxygenation)
{
    const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;

    /* Need a different conversion factor for transpiration */
    const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
    /* For Transpiration */
    /* 3600 - seconds per hour */
    /* 1e-3 - millimoles per mole */
    /* 18 - grams per mole for H2O */
    /* 1e-6 - grams per  megagram */
    /* 10000 - meters squared per hectare */

    double CanopyA = 0.0, CanopyT = 0.0, GCanopyA = 0.0;

    double IDir, IDiff;

    double vmax1;
	double leafN_lay;

    struct Light_model light_model = lightME(lat, DOY, hr);

    double Idir = light_model.direct_irradiance_fraction * solarR;
    double Idiff = light_model.diffuse_irradiance_fraction * solarR;
    double cosTh = light_model.cosine_zenith_angle;

    struct Light_profile light_profile = sunML(Idir, Idiff, LAI, nlayers, cosTh, kd, chil, heightf);

    double LAIc = LAI / nlayers;

    double relative_humidity_profile[nlayers];
    RHprof(RH, nlayers, relative_humidity_profile);

    double wind_speed_profile[nlayers];
    WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);

    double leafN_profile[nlayers];
    LNprof(leafN, LAI, nlayers, kpLN, leafN_profile);

    for (int i = 0; i < nlayers; ++i)
    {
        int current_layer = nlayers - 1 - i;
		leafN_lay = leafN_profile[current_layer];
        if (lnfun == 0) {
			vmax1 = Vmax;
		} else {
			vmax1 = leafN_lay * lnb1 + lnb0;
		}

        double relative_humidity = relative_humidity_profile[current_layer];
        double layer_wind_speed = wind_speed_profile[current_layer];

        IDir = light_profile.direct_irradiance[current_layer];
        double Itot = light_profile.total_irradiance[current_layer];
        double pLeafsun = light_profile.sunlit_fraction[current_layer];
        double CanHeight = light_profile.height[current_layer];

        double Leafsun = LAIc * pLeafsun;

        double stomatal_conductance_direct = c3photoC(IDir, air_temperature, relative_humidity, vmax1, Jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation).Gs;  // Estimate stomatal_conductance by assuming the leaf has the same temperature as the air.
        struct ET_Str et_direct = c3EvapoTrans(Itot, air_temperature, relative_humidity, layer_wind_speed, CanHeight, stomatal_conductance_direct);

        double leaf_temperature_Idir = air_temperature + et_direct.Deltat;
        struct c3_str temp_photo_results = c3photoC(IDir, leaf_temperature_Idir, relative_humidity, vmax1, Jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation);
        double AssIdir = temp_photo_results.Assim;
        double GAssIdir = temp_photo_results.GrossAssim;

        IDiff = light_profile.diffuse_irradiance[current_layer];
        double pLeafshade = light_profile.shaded_fraction[current_layer];
        double Leafshade = LAIc * pLeafshade;

        double stomatal_conductance_diffuse = c3photoC(IDiff, air_temperature, relative_humidity, vmax1, Jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation).Gs;  // Estimate stomatal_conductance by assuming the leaf has the same temperature as the air.
        struct ET_Str et_diffuse = c3EvapoTrans(Itot, air_temperature, relative_humidity, layer_wind_speed, CanHeight, stomatal_conductance_diffuse);
        double leaf_temperature_Idiffuse = air_temperature + et_diffuse.Deltat;

        temp_photo_results = c3photoC(IDiff, leaf_temperature_Idiffuse, relative_humidity, vmax1, Jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation);
        double AssIdiff = temp_photo_results.Assim;
        double GAssIdiff = temp_photo_results.GrossAssim;

        CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
        GCanopyA += Leafsun * GAssIdir + Leafshade * GAssIdiff;
        CanopyT += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;
    }

    /* These are micro mols of CO2 per m2 per sec for Assimilation
       and mili mols of H2O per m2 per sec for Transpiration
       Need to convert to 
       3600 converts seconds to hours
       10^-6 converts micro mols to mols
       30 converts mols of CO2 to grams
       (1/10^6) converts grams to Mg
       10000 scales up to ha */
    /* A similar conversion is made for water but
       replacing 30 by 18 and mili mols are converted to
       mols (instead of micro) */

    struct Can_Str ans;
    ans.Assim = cf * CanopyA;
    ans.Trans = cf2 * CanopyT; 
    ans.GrossAssim = cf * GCanopyA;
    return(ans);
}

