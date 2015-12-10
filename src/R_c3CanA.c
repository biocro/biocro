/*
 *  BioCro/src/c3CanA.c by Fernando Ezequiel Miguez  Copyright (C) 2010
 *
 */

#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <math.h>
#include "c4photo.h"
#include "c3photo.h"
#include "AuxBioCro.h"
#include "CanA.h"
#include "R_c3CanA.h"

/* c3CanA function */
/* This file will contain the function which will consolidate
 the previous functions into one. The idea is to go from
 existing LAI and weather conditions to canopy assimilation
 and transpiration */


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
	/* Declaring the struct for the Evaop Transpiration */
	struct ET_Str tmp5_ET , tmp6_ET; 
	struct c3_str tmpc3, tmpc32; 

	const double cf = 3600 * 1e-6 * 12 * 1e-6 * 10000/0.48;
	/*12 is the molecular weight for C, 48% is assumbed to be the carbon content for leaf biomass
	  Need a different conversion factor for transpiration */
	const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
	/* 3600 - number of seconds in an hour */
	/* 1e-3 - mili mols to mols */
	/* 18 - grams in one mol of H20 */
	/* 1e-6 - grams to mega grams */
	/* 10000 - meters in one hectare */

	int i;
	double Idir, Idiff, cosTh;
	double LAIc;
	double IDir, IDiff, Itot,rh, WindS;
	double TempIdir,TempIdiff,AssIdir,AssIdiff;
	double  pLeafsun, pLeafshade;
	double Leafsun, Leafshade;

	double CanopyA ;
	double CanopyT , CanopyPe = 0.0, CanopyPr = 0.0;
	double CanopyC = 0.0;
	double CanHeight;
	/* Need to label them something different from the arguments of the c4photoC
	   function because these need to be modified by the water stress effect on
	   the stomata and assimilation. The Ball-Berry parameters are not implemented
	   yet (I need to think about this a bit harder). */
	double vmax1;
	double leafN_lay;
	double StomWS = REAL(STOMATAWS)[0];

	/* INTEGERS */
	int DOY = INTEGER(Doy)[0];
	int hr = INTEGER(HR)[0];
	int nlayers = INTEGER(NLAYERS)[0];

	/* REALS */
	double LAI = REAL(Lai)[0];
	double solarR = REAL(SOLAR)[0];
	double Temp = REAL(TEMP)[0];
	double RH = REAL(ReH)[0];
	double WindSpeed = REAL(windspeed)[0];
	double lat = REAL(LAT)[0];
	double kd = REAL(KD)[0];
	double heightf = REAL(HEIGHTF)[0];
	double chil = 1;

	/* Photosynthesis parameters */

	double jmax1 = REAL(PHOTOPS)[1];
	double Rd1 = REAL(PHOTOPS)[2];
	double Catm = REAL(PHOTOPS)[3];
	double O2 = REAL(PHOTOPS)[4];
	double b01 = REAL(PHOTOPS)[5];
	double b11 = REAL(PHOTOPS)[6];
	double theta = REAL(PHOTOPS)[7];

	double  LeafN = REAL(LNPS)[0];
	double  kpLN = REAL(LNPS)[1];
	double lnb0 = REAL(LNPS)[2];
	double lnb1 = REAL(LNPS)[3];
	int lnfun = REAL(LNPS)[4]; /* Coercing a double to integer */
	int ws = INTEGER(WS)[0];

	SEXP lists;
	SEXP names;
	SEXP growth;
	SEXP trans;
	SEXP epen;
	SEXP epries;
	SEXP cond;

	SEXP mat1;

	PROTECT(lists = allocVector(VECSXP,6));
	PROTECT(names = allocVector(STRSXP,6));
	PROTECT(growth = allocVector(REALSXP,1));
	PROTECT(trans = allocVector(REALSXP,1));
	PROTECT(epen = allocVector(REALSXP,1));
	PROTECT(epries = allocVector(REALSXP,1));
	PROTECT(cond = allocVector(REALSXP,1));

	PROTECT(mat1 = allocMatrix(REALSXP,14,nlayers));

	struct Light_model light_model;
	light_model = lightME(lat, DOY, hr);

	Idir = light_model.irradiance_direct * solarR;
	Idiff = light_model.irradiance_diffuse * solarR;
	cosTh = light_model.cosine_zenith_angle;

	/* sun multilayer model. As a side effect it populates the layIdir, layItotal, layFsun, layHeight,
	   layIdiff, layShade vectors. */

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
	CanopyA=0.0;
	CanopyT=0.0;

	for(i=0;i<nlayers;i++)
	{
		int current_layer = nlayers - 1 - i;
		/* vmax depends on leaf nitrogen and this in turn depends on the layer */
		leafN_lay = leafN_profile[current_layer];
		if(lnfun == 0){
			vmax1 = REAL(PHOTOPS)[0];
		}else{
			vmax1 = leafN_lay * lnb1 + lnb0;
			/* For now alpha is not affected by leaf nitrogen */
		}

		rh = relative_humidity_profile[current_layer];
		WindS = wind_speed_profile[current_layer];


		IDir = light_profile.direct_irradiance[current_layer];
		Itot = light_profile.total_irradiance[current_layer];
		pLeafsun = light_profile.sunlit_fraction[current_layer];
		CanHeight = light_profile.height[current_layer];
		Leafsun = LAIc * pLeafsun;
		/* Need a new evapo transpiration function specifically for c3*/
		tmp5_ET = c3EvapoTrans(IDir,Itot,Temp,rh,WindS,LAIc,CanHeight,
				vmax1,jmax1,Rd1,b01,b11,Catm,210,theta);
		TempIdir = Temp + tmp5_ET.Deltat;
		tmpc3 = c3photoC(IDir,TempIdir,rh,vmax1,jmax1,Rd1,b01,b11,Catm,O2,theta,StomWS,ws);
		AssIdir = tmpc3.Assim;

		IDiff = light_profile.diffuse_irradiance[current_layer];
		pLeafshade = light_profile.shaded_fraction[current_layer];
		Leafshade = LAIc * pLeafshade;
		tmp6_ET = c3EvapoTrans(IDiff,Itot,Temp,rh,WindS,LAIc,CanHeight,
				vmax1,jmax1,Rd1,b01,b11,Catm,210,theta);
		TempIdiff = Temp + tmp6_ET.Deltat;
		tmpc32 = c3photoC(IDiff,TempIdiff,rh,vmax1,jmax1,Rd1,b01,b11,Catm,O2,theta,StomWS,ws);
		AssIdiff = tmpc32.Assim;

		/* Collect direct radiation assim and trans in a matrix */
		REAL(mat1)[i*14] = IDir;
		REAL(mat1)[1 + i*14] = IDiff;
		REAL(mat1)[2 + i*14] = Leafsun;
		REAL(mat1)[3 + i*14] = Leafshade;
		REAL(mat1)[4 + i*14] = tmp5_ET.TransR;
		REAL(mat1)[5 + i*14] = tmp6_ET.TransR;
		REAL(mat1)[6 + i*14] = AssIdir;
		REAL(mat1)[7 + i*14] = AssIdiff;
		REAL(mat1)[8 + i*14] = tmp5_ET.Deltat;
		REAL(mat1)[9 + i*14] = tmp6_ET.Deltat;
		REAL(mat1)[10 + i*14] = tmp5_ET.LayerCond; 
		REAL(mat1)[11 + i*14] = tmp6_ET.LayerCond; 
		REAL(mat1)[12 + i*14] = leafN_lay; 
		REAL(mat1)[13 + i*14] = vmax1; 

		/*      REAL(mat1)[11 + i*12] = rh;  */
		/*Layer conductance needs to be transformed back to the correct units here*/

		CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
		CanopyT += Leafsun * tmp5_ET.TransR + Leafshade * tmp6_ET.TransR; 
		CanopyPe += Leafsun * tmp5_ET.EPenman + Leafshade * tmp6_ET.EPenman;
		CanopyPr += Leafsun * tmp5_ET.EPriestly + Leafshade * tmp6_ET.EPriestly;
		CanopyC += Leafsun * tmp5_ET.LayerCond + Leafshade * tmp6_ET.LayerCond;

	}
	/*## These are micro mols of CO2 per m2 per sec
## Need to convert to 
## 3600 converts seconds to hours
## 10^-6 converts micro mols to mols
## 30 converts mols of CO2 to grams
## (1/10^6) converts grams to Mg
## 10000 scales up to ha */
	REAL(growth)[0] = cf * CanopyA ;
	REAL(trans)[0] = cf2 * CanopyT ;
	REAL(epen)[0] = cf2 * CanopyPe ;
	REAL(epries)[0] = cf2 * CanopyPr ;
	REAL(cond)[0] = CanopyC;

	SET_VECTOR_ELT(lists,0,growth);
	SET_VECTOR_ELT(lists,1,trans);
	SET_VECTOR_ELT(lists,2,cond);
	SET_VECTOR_ELT(lists,3,epen);
	SET_VECTOR_ELT(lists,4,epries);
	SET_VECTOR_ELT(lists,5,mat1);

	SET_STRING_ELT(names,0,mkChar("CanopyAssim"));
	SET_STRING_ELT(names,1,mkChar("CanopyTrans"));
	SET_STRING_ELT(names,2,mkChar("CanopyCond"));
	SET_STRING_ELT(names,3,mkChar("TranEpen"));
	SET_STRING_ELT(names,4,mkChar("TranEpries"));
	SET_STRING_ELT(names,5,mkChar("LayMat"));
	setAttrib(lists,R_NamesSymbol,names);

	UNPROTECT(8);
	return(lists);
}

