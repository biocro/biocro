/*
 *  BioCro/src/c3CanA.c by Fernando Ezequiel Miguez  Copyright (C) 2010
 *
 */

#include <R.h>
#include <Rinternals.h>
#include "BioCro.h"
#include "c3photo.h"
#include "c3EvapoTrans.h"
#include "CanA.h"

SEXP c3CanA(SEXP Lai,
	    SEXP Doy,
	    SEXP HR,
	    SEXP SOLAR,
	    SEXP TEMP,
	    SEXP ReH,
	    SEXP windspeed,
	    SEXP LAT,
	    SEXP NLAYERS, 
	    SEXP PHOTOPS, 
	    SEXP KD, 
	    SEXP HEIGHTF, 
	    SEXP LNPS,
      SEXP STOMATAWS,
      SEXP WS
      )
{
	int DOY = INTEGER(Doy)[0];
	int hr = INTEGER(HR)[0];
	int nlayers = INTEGER(NLAYERS)[0];
	double LAI = REAL(Lai)[0];
	double solarR = REAL(SOLAR)[0];
	double Temp = REAL(TEMP)[0];
	double RH = REAL(ReH)[0];
	double WindSpeed = REAL(windspeed)[0];
	double lat = REAL(LAT)[0];
	double kd = REAL(KD)[0];
	double heightf = REAL(HEIGHTF)[0];
	double vmax = REAL(PHOTOPS)[0];
	double jmax = REAL(PHOTOPS)[1];
	double Rd = REAL(PHOTOPS)[2];
	double Catm = REAL(PHOTOPS)[3];
	double o2 = REAL(PHOTOPS)[4];
	double b0 = REAL(PHOTOPS)[5];
	double b1 = REAL(PHOTOPS)[6];
	double theta = REAL(PHOTOPS)[7];
	double  LeafN = REAL(LNPS)[0];
	double  kpLN = REAL(LNPS)[1];
	double lnb0 = REAL(LNPS)[2];
	double lnb1 = REAL(LNPS)[3];
	int lnfun = REAL(LNPS)[4]; /* Coercing a double to integer */
	double StomataWS = REAL(STOMATAWS)[0];
	int ws = INTEGER(WS)[0];

	SEXP lists;
	SEXP names;
	SEXP growth;
	SEXP trans;
	SEXP Ggrowth;

	PROTECT(lists = allocVector(VECSXP,6));
	PROTECT(names = allocVector(STRSXP,6));
	PROTECT(growth = allocVector(REALSXP,1));
	PROTECT(trans = allocVector(REALSXP,1));
  PROTECT(Ggrowth = allocVector(REALSXP,1));

	struct ET_Str tmp5_ET , tmp6_ET; 
    struct c3_str temp_photo_results = {0, 0, 0, 0};
	struct Can_Str ans = {0, 0, 0};

	const double cf = 3600 * 1e-6 * 12 * 1e-6 * 10000/0.48;
	/*12 is the molecular weight for C, 48% is assumbed to be the carbon content for leaf biomass */

	/* Need a different conversion factor for transpiration */
	const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
    /* For Transpiration */
    /* 3600 - seconds per hour */
    /* 1e-3 - millimoles per mole */
    /* 18 - grams per mole for H2O */
    /* 1e-6 - grams per  megagram */
    /* 10000 - meters squared per hectare */

	int i;
	double Idir, Idiff, cosTh;
	double LAIc;
	double IDir, IDiff, Itot, rh, layerWindSpeed;
	double pLeafsun, pLeafshade;
	double Leafsun = 0.0, Leafshade = 0.0;
	double CanHeight;

	double TempIdir = 0.0,
		   AssIdir = 0.0, AssIdiff = 0.0;
	double GAssIdir = 0.0, GAssIdiff = 0.0;
	double TempIdiff;

	double CanopyA = 0.0, CanopyT = 0.0, GCanopyA = 0.0;
	double chil = 1.0;
	double vmax1;
	double leafN_lay;

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
	LNprof(LeafN, LAI, nlayers, kpLN, leafN_profile);

	/* Next use the EvapoTrans function */
	for(i=0;i<nlayers;i++)
	{
		int current_layer = nlayers - 1 - i;
		/* vmax depends on leaf nitrogen and this in turn depends on the layer */
		leafN_lay = leafN_profile[current_layer];
		if(lnfun == 0) {
			vmax1 = vmax;
		} else {
			vmax1 = leafN_lay * lnb1 + lnb0;
			/* For now alpha is not affected by leaf nitrogen */
		}

		rh = relative_humidity_profile[current_layer];
		layerWindSpeed = wind_speed_profile[current_layer];

		IDir = light_profile.direct_irradiance[current_layer];
		Itot = light_profile.total_irradiance[current_layer];
		pLeafsun = light_profile.sunlit_fraction[current_layer];
		CanHeight = light_profile.height[current_layer];

		Leafsun = LAIc * pLeafsun;

		tmp5_ET = c3EvapoTrans(IDir, Itot, Temp, rh, layerWindSpeed, LAIc, CanHeight,
				vmax1, jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, ws);

		TempIdir = Temp + tmp5_ET.Deltat;
		temp_photo_results = c3photoC(IDir, TempIdir, rh, vmax1, jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, ws);
		AssIdir = temp_photo_results.Assim;
        GAssIdir = temp_photo_results.GrossAssim;

		IDiff = light_profile.diffuse_irradiance[current_layer];
		pLeafshade = light_profile.shaded_fraction[current_layer];
		Leafshade = LAIc * pLeafshade;

		tmp6_ET = c3EvapoTrans(IDiff, Itot, Temp, rh, layerWindSpeed, LAIc, CanHeight,
				vmax1, jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, ws);
		TempIdiff = Temp + tmp6_ET.Deltat;

		temp_photo_results = c3photoC(IDiff, TempIdiff, rh, vmax1, jmax, Rd, b0, b1, Catm, o2, theta, StomataWS, ws);
		AssIdiff = temp_photo_results.Assim;
        GAssIdiff = temp_photo_results.GrossAssim;

		CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
        GCanopyA += Leafsun * GAssIdir + Leafshade * GAssIdiff;
		CanopyT += Leafsun * tmp5_ET.TransR + Leafshade * tmp6_ET.TransR; 

    if(ISNAN(CanopyA)) {
        Rprintf("LAI %.2f \n",LAI); 
        Rprintf("Leafsun %.2f \n",Leafsun);
        Rprintf("AssIdir %.2f \n", AssIdir);
        Rprintf("Leafshade %.2f \n",Leafshade);
        Rprintf("AssIdiff %.2f \n", AssIdiff);    
        error("Something is NA \n");
    }
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

	REAL(growth)[0] = cf * CanopyA;
	REAL(trans)[0] = cf2 * CanopyT;
	REAL(Ggrowth)[0] = cf * GCanopyA;

	SET_VECTOR_ELT(lists, 0, growth);
	SET_VECTOR_ELT(lists, 1, trans);
	SET_VECTOR_ELT(lists, 2, Ggrowth);

	SET_STRING_ELT(names, 0, mkChar("CanopyAssim"));
	SET_STRING_ELT(names, 1, mkChar("CanopyTrans"));
	SET_STRING_ELT(names, 2, mkChar("GrossCanopyAssim"));
	setAttrib(lists,R_NamesSymbol,names);

	UNPROTECT(5);
	return(lists);
}

