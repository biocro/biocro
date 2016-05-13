#include "BioCro.h"
#include "c4photo.h"

struct Can_Str CanAC(
		double LAI,
        int DOY,
        int hr,
        double solarR,
        double Temp,
        double RH,
        double WindSpeed,
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
        struct nitroParms nitroP,
        double leafwidth,
        int eteq,
        double StomataWS,
        int ws)
{

    struct Can_Str ans = {0, 0, 0};
    struct ET_Str direct_ET, indirect_ET;
    struct c4_str temp_photo_results = {0, 0, 0, 0};

    const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
    /* For Assimilation */
    /* 3600 - seconds per hour */
    /* 1e-6 - micromoles per mole */
    /* 30 - grams per mole for CO2 */
    /* 1e-6 - megagrams per gram */
    /* 10000 - meters squared per hectare*/

    /* Need a different conversion factor for transpiration */
    const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
    /* For Transpiration */
    /* 3600 - seconds per hour */
    /* 1e-3 - millimoles per mole */
    /* 18 - grams per mole for H2O */
    /* 1e-6 - megagrams per  gram */
    /* 10000 - meters squared per hectare */

    int i;
    double Idir, Idiff, cosTh;
    double LAIc;
    double IDir, IDiff, Itot, rh, layerWindSpeed;
    double TempIdir = 0.0, TempIdiff = 0.0, AssIdir = 0.0, AssIdiff = 0.0, GAssIdir = 0.0 , GAssIdiff = 0.0;
    double pLeafsun, pLeafshade;
    double Leafsun = 0.0, Leafshade = 0.0;

    double CanopyA = 0.0, GCanopyA = 0.0;
    double CanopyT = 0.0;
    double CanopyPe = 0.0, CanopyPr = 0.0;
    double canopy_conductance = 0.0;
    double CanHeight;

    double vmax1, leafN_lay;

    double result_matrix[21 * MAXLAY];

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

    for(i=0; i<nlayers; i++)
    {
        int current_layer = nlayers - 1 - i;
        leafN_lay = leafN_profile[current_layer];
        if(lnfun == 0) {
            vmax1 = Vmax;
        } else {
            vmax1=nitroP.Vmaxb1*leafN_lay+nitroP.Vmaxb0;
            if(vmax1<0) vmax1=0.0;
		    if(vmax1>Vmax) vmax1=Vmax;
            Alpha=nitroP.alphab1*leafN_lay+nitroP.alphab0;
            Rd=nitroP.Rdb1*leafN_lay+nitroP.Rdb0;
        }

        rh = relative_humidity_profile[current_layer];
        layerWindSpeed = wind_speed_profile[current_layer];

        IDir = light_profile.direct_irradiance[current_layer];
        Itot = light_profile.total_irradiance[current_layer];
        pLeafsun = light_profile.sunlit_fraction[current_layer];
        CanHeight = light_profile.height[current_layer];
        Leafsun = LAIc * pLeafsun;

        temp_photo_results = c4photoC(IDir, Temp, rh, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, ws, upperT, lowerT);
        direct_ET = EvapoTrans2(IDir, Itot, Temp, rh, layerWindSpeed, LAIc, CanHeight, temp_photo_results.Gs, leafwidth, eteq);

        TempIdir = Temp + direct_ET.Deltat;
        temp_photo_results = c4photoC(IDir, TempIdir, rh, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, ws, upperT, lowerT);
        AssIdir = temp_photo_results.Assim;
        GAssIdir =temp_photo_results.GrossAssim;

        IDiff = light_profile.diffuse_irradiance[current_layer];
        pLeafshade = light_profile.shaded_fraction[current_layer];
        Leafshade = LAIc * pLeafshade;

        temp_photo_results = c4photoC(IDiff, Temp, rh, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, ws, upperT, lowerT);
        indirect_ET = EvapoTrans2(IDiff, Itot, Temp, rh, layerWindSpeed, LAIc, CanHeight, temp_photo_results.Gs, leafwidth, eteq);
        TempIdiff = Temp + indirect_ET.Deltat;
        temp_photo_results = c4photoC(IDiff, TempIdiff, rh, vmax1, Alpha, Kparm, theta, beta, Rd, b0, b1, StomataWS, Catm, ws, upperT, lowerT);
        AssIdiff = temp_photo_results.Assim;
        GAssIdiff = temp_photo_results.GrossAssim;

        CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
        CanopyT += Leafsun * direct_ET.TransR + Leafshade * indirect_ET.TransR;
        GCanopyA += Leafsun * GAssIdir + Leafshade * GAssIdiff;

        CanopyPe += Leafsun * direct_ET.EPenman + Leafshade * indirect_ET.EPenman;
        CanopyPr += Leafsun * direct_ET.EPriestly + Leafshade * indirect_ET.EPriestly;
        canopy_conductance += Leafsun * direct_ET.LayerCond + Leafshade * indirect_ET.LayerCond;
        
        result_matrix[    i*21] = IDir;
        result_matrix[1 + i*21] = IDiff;
        result_matrix[2 + i*21] = Leafsun;
        result_matrix[3 + i*21] = Leafshade;
        result_matrix[4 + i*21] = direct_ET.TransR;
        result_matrix[5 + i*21] = indirect_ET.TransR;
        result_matrix[6 + i*21] = AssIdir;
        result_matrix[7 + i*21] = AssIdiff;
        result_matrix[8 + i*21] = direct_ET.Deltat;
        result_matrix[9 + i*21] = indirect_ET.Deltat;
        result_matrix[10 + i*21] = direct_ET.LayerCond; 
        result_matrix[11 + i*21] = indirect_ET.LayerCond; 
        result_matrix[12 + i*21] = leafN_lay; 
        result_matrix[13 + i*21] = vmax1;
        result_matrix[14 + i*21] = rh; 
        result_matrix[15 + i*21] = GAssIdir;
        result_matrix[16 + i*21] = GAssIdiff;
        result_matrix[17 + i*21] = Alpha;
        result_matrix[18 + i*21] = leafN_lay;
        result_matrix[19 + i*21] = layerWindSpeed;
        result_matrix[20 + i*21] = CanHeight;
    }
    /*## These are micromoles of CO2 per m2 per sec for Assimilation
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
    ans.Assim = cf * CanopyA;
    ans.Trans = cf2 * CanopyT; 
    ans.GrossAssim = cf * GCanopyA;
    ans.canopy_transpiration_penman = CanopyPe;
    ans.canopy_transpiration_priestly = CanopyPr;
    ans.canopy_conductance = canopy_conductance;

    for (int i = 0; i < nlayers * 21; i++) {
        ans.result_matrix[i] = result_matrix[i];
    }
    return(ans);
}

