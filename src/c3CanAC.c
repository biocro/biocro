#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "BioCro.h"
#include "AuxBioCro.h"
#include "AuxwillowGro.h"
#include "c3photo.h"
#include "c3canopy.h"

struct Can_Str c3CanAC(double LAI,
		int DOY,
		int hr,
		double solarR,
		double Temp,
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
		double StomataWS,
		int ws)
{

    struct ET_Str tmp5_ET , tmp6_ET; 
    struct c3_str tmpc3 = {0, 0, 0, 0};
    struct c3_str tmpc32 = {0, 0, 0, 0}; 
    struct Can_Str ans = {0, 0, 0};

    const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
    /* Need a different conversion factor for transpiration */
    const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 

    int i;
    double Idir, Idiff, cosTh;
    double LAIc;
    double IDir, IDiff, Itot,rh, layerWindSpeed;
    double pLeafsun, pLeafshade;
    double Leafsun = 0.0, Leafshade = 0.0;
    double CanHeight;

    // double vmax1, leafN_lay; unused
    double TempIdir = 0.0,
		   AssIdir = 0.0, AssIdiff = 0.0,
    	   GAssIdir = 0.0, GAssIdiff = 0.0; 
    // double TempIdiff; unused

    double CanopyA = 0.0, CanopyT =0.0, GCanopyA = 0.0;
    double chil=1.0;

    /* For Assimilation */
    /* 3600 converts seconds to hours */
    /* 1e-6 converts micro mols to mols */
    /* 30 is the grams in one mol of CO2 */
    /* 1e-6 converts g to Mg */
    /* 10000 scales from meter squared to hectare */

    /* For Transpiration */
    /* 3600 converts seconds to hours */
    /* 1e-3 converts mili mols to mols */
    /* 18 is the grams in one mol of H20 */
    /* 1e-6 converts g to Mg */
    /* 10000 scales from meter squared to hectare */

    struct Light_model light_model;
    light_model = lightME(lat, DOY, hr);

    Idir = light_model.irradiance_direct * solarR;
    Idiff = light_model.irradiance_diffuse * solarR;
    cosTh = light_model.cosine_zenith_angle;

    struct Light_profile light_profile;
    light_profile = sunML(Idir, Idiff, LAI, nlayers, cosTh, kd, chil, heightf);

    /* results from multilayer model */
    LAIc = LAI / nlayers;

    /* Next I need the RH and wind profile */
    double relative_humidity_profile[nlayers];
    RHprof(RH, nlayers, relative_humidity_profile);

    double wind_speed_profile[nlayers];
    WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);

    double leafN_profile[nlayers];
    LNprof(leafN, LAI, nlayers, kpLN, leafN_profile);

    /* Next use the EvapoTrans function */
    CanopyA=0.0;
    GCanopyA=0.0;
    CanopyT=0.0;

    for(i=0; i<nlayers; i++)
    {
        int current_layer = nlayers - 1 - i;
        /* vmax depends on leaf nitrogen and this in turn depends on the layer */
        /* if(lnfun == 0) {
			vmax1 = Vmax;
		}else{
			vmax1 = leafN_lay * lnb1 + lnb0;
		} unused variables are assigned here */
        /* For now alpha is not affected by leaf nitrogen */

        rh = relative_humidity_profile[current_layer];
        layerWindSpeed = wind_speed_profile[current_layer];

        IDir = light_profile.direct_irradiance[current_layer];
        Itot = light_profile.total_irradiance[current_layer];
        pLeafsun = light_profile.sunlit_fraction[current_layer];
        CanHeight = light_profile.height[current_layer];

        Leafsun = LAIc * pLeafsun;

        tmp5_ET = c3EvapoTrans(IDir, Itot, Temp, rh, layerWindSpeed, LAIc, CanHeight, Vmax, Jmax, Rd, b0, b1, Catm, o2, theta);

        TempIdir = Temp + tmp5_ET.Deltat;
        tmpc3 = c3photoC(IDir, TempIdir, rh, Vmax, Jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, ws);
        AssIdir = tmpc3.Assim;
        GAssIdir =tmpc3.GrossAssim;

        IDiff = light_profile.diffuse_irradiance[current_layer];
        pLeafshade = light_profile.shaded_fraction[current_layer];
        Leafshade = LAIc * pLeafshade;

        tmp6_ET = c3EvapoTrans(IDiff, Itot, Temp, rh, layerWindSpeed, LAIc, CanHeight, Vmax, Jmax, Rd, b0, b1, Catm, o2, theta);
        // TempIdiff = Temp + tmp6_ET.Deltat; set but not used
        tmpc32 = c3photoC(IDiff, TempIdir, rh, Vmax, Jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, ws);
        AssIdiff = tmpc32.Assim;
        GAssIdiff = tmpc32.GrossAssim;

        CanopyA = CanopyA + Leafsun * AssIdir + Leafshade * AssIdiff;
        GCanopyA = GCanopyA + Leafsun * GAssIdir + Leafshade * GAssIdiff;
        CanopyT = CanopyT + Leafsun * tmp5_ET.TransR + Leafshade * tmp6_ET.TransR;
    }

    if(ISNAN(CanopyA)) {
        Rprintf("LAI %.2f \n",LAI); 
        Rprintf("Leafsun %.2f \n",Leafsun);
        Rprintf("AssIdir %.2f \n", AssIdir);
        Rprintf("Leafshade %.2f \n",Leafshade);
        Rprintf("AssIdiff %.2f \n", AssIdiff);    
        error("Something is NA \n");
    }

    /*## These are micro mols of CO2 per m2 per sec for Assimilation
## and mili mols of H2O per m2 per sec for Transpiration
## Need to convert to 
## 3600 converts seconds to hours
## 10^-6 converts micro mols to mols
## 30 converts mols of CO2 to grams
## (1/10^6) converts grams to Mg
## 10000 scales up to ha */
    /* A similar conversion is made for water but
       replacing 30 by 18 and mili mols are converted to
       mols (instead of micro) */
    ans.Assim = cf * CanopyA ;
    ans.Trans = cf2 * CanopyT; 
    ans.GrossAssim=cf*GCanopyA;
    return(ans);
}

