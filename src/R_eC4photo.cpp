

#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include "BioCro.h"
#include "CanA.h"
#include "eC4photo.h"

extern "C" {

SEXP eC4photo(SEXP QP, SEXP TEMP, SEXP RH, SEXP CA,
		SEXP OA, SEXP VCMAX, SEXP VPMAX, SEXP VPR,
		SEXP JMAX)
{
    int nqp = length(QP);
    double P = 101.3;
    double G1 = 3;
    double G0 = 0.08;

    SEXP names, lists;
    SEXP Assim;
    SEXP Gs;
    SEXP Ci;
    SEXP Oxy;

    PROTECT(lists = allocVector(VECSXP,4));
    PROTECT(names = allocVector(STRSXP,4));

    PROTECT(Assim = allocVector(REALSXP,nqp));
    PROTECT(Gs = allocVector(REALSXP,nqp));
    PROTECT(Ci = allocVector(REALSXP,nqp));
    PROTECT(Oxy = allocVector(REALSXP,nqp));

    const double  gs = 3 * 1e-3 ;
    /* mol m-2 s-1 physical conductance to CO2 leakage*/
    /* const double gammaStar = 0.000193  */
    const double gammaStar =  0.000193 ;
    /* half the reciprocal of Rubisco specificity */
    /*    const double go = 0.047 * gs     at 25 C */
    const double alpha = 0.01 ; /* alpha in the notes*/
    const double Kp = 80 ; /*  mu bar */
    const double theta = 0.7;
    const double R = 0.008314472; // kJ K^-1 mol^-1

    /* ADDING THE TEMPERATURE RESPONSE FUNCTION */
    const double Ep  = 47.1 ; /* Activation energy of PEPc kj/mol */
    const double Erb = 72 ; /* Activation energy of Rubisco kj/mol */
    /*    const double Q10bf = 1.7 */
    const double EKc = 79.43;
    const double EKo = 36.38;
    const double Q10cb = 1.7;

    const double Ko2 = 450 ; /* mbar at 25 C */
    const double Kc2 = 650 ; /*  mu bar at 25 C */
    
    double Vcmax, Vpmax;
    double Kc, Ko;
    double Aj, Ac, A;

    double Ca = REAL(CA)[0];
    double Oa = REAL(OA)[0];
    double Vcmax1 = REAL(VCMAX)[0];
    double Vpmax1 = REAL(VPMAX)[0];
    double Vpr = REAL(VPR)[0];
    double Jmax1 = REAL(JMAX)[0];

    double Cs;
    double Cs0, Cs1;
    double StomCond;
    

    for (int i = 0; i < nqp; i++) {

        double Idir = REAL(QP)[i];
        double AirTemp = REAL(TEMP)[i];
        double rh = REAL(RH)[i];

        double Q10p = exp(Ep *(1/(R * 298.15)-1/(R * (AirTemp + 273.15))));
        double Q10rb = exp(Erb *(1/(R * 298.15)-1/(R * (AirTemp + 273.15))));
        double Q10Kc = exp(EKc *(1/(R * 298.15)-1/(R * (AirTemp + 273.15))));
        double Q10Ko = exp(EKo *(1/(R * 298.15)-1/(R * (AirTemp + 273.15))));

        Vcmax = Vcmax1 * Q10rb;
        Kc = Kc2 * Q10Kc;
        Ko = Ko2 * Q10Ko;
        Vpmax = Vpmax1 * Q10p;        
        double Jmax = Jmax1 * pow(Q10cb,(AirTemp-25)/10);

        double Cm = 0.4 * Ca ; 
        double Om = Oa ;

        double Rd = 0.08;
        double Rm = 0.5 * Rd ;

        /* Light limited */
        double I2 = (Idir * 0.85)/2;
        double J = (Jmax + I2  - sqrt(pow(Jmax+I2,2) - 4 * theta * I2 * Jmax ))/2*theta;        
        /* Long formula for light limited */

        double aa = 1 - (7 * gammaStar * alpha )/3 * 0.047;
        double bb = -(((0.4 * J)/2 - Rm + gs * Cm) + ((1-0.4)*J/3-Rd) + 
                gs*(7*gammaStar*Om/3) + 
                (alpha*gammaStar/0.047)*((1-0.04)*J/3 + 7*Rd/3));
        double cc = ((0.4 * J)/2 - Rm + gs * Cm)*((1-0.4)*J/3-Rd) -
            gs * gammaStar * Om *((1-0.4)*J/3 + 7*Rd/3);

        Aj = (-bb - sqrt(pow(bb,2)-4*aa*cc))/2*aa;

        /* Other part */
        double Vp = (Cm * Vpmax) / (Cm + Kp) ;
        if (Vpr < Vp){
            Vp = Vpr;
        }

        /* Alternative formula */
        double Ko1 = Ko * 1e3;
        double Om1 = Om * 1e3;

        double a1 = 1 - alpha / 0.047 * Kc/Ko1;
        double b1 = -((Vp - Rm + gs * Cm) + (Vcmax - Rd) +
                gs * (Kc * (1 + Om1 / Ko1)) + ((alpha / 0.047) * (gammaStar * Vcmax + Rd * Kc / Ko1)));
        double c1 = (Vcmax - Rd) * (Vp - Rm + gs * Cm) - (Vcmax * gs * gammaStar * Om1 + Rd * gs * Kc * (1 + Om1 / Ko1));

        double c3 = pow(b1,2) - 4 * a1 * c1;
        if (c3 < 0){
            c3 = 0;
        }
        double Ac0 = (-b1 - sqrt(c3)) / 2 * a1 ;

        double AcLCO2 = (Cm * Vpmax/(Cm+Kp)) - Rm + gs * Cm;

        if (Ac0 < AcLCO2){
            Ac = Ac0;
        } else {
            Ac = AcLCO2;
        }

        if (Aj < Ac){
            A = Aj;
        } else {
            A = Ac;
        }

        double Os = alpha * A / 0.047 * gs + Om;

        if (Aj <= Ac){ 
            Cs = Cm + (Vp - Aj - Rm)/gs;
        } else { 
            Cs0 = gammaStar * Os + Kc*(1+Os/Ko)*((Ac+Rd)/Vcmax); 
            Cs1 = 1 - (Ac+Rd)/Vcmax; 
            Cs = Cs0/Cs1;
        }

        /* Calculating gs */
        if (A < 0){
            StomCond = G0;
        } else {
            StomCond = G1 * (A * rh * P) / (Cm*1e-1) + G0;
        }
        /* Organizing the results */
        REAL(Assim)[i] = A;
        REAL(Gs)[i] = StomCond;
        REAL(Ci)[i] = Cs;
        REAL(Oxy)[i] = Os;
    }

    SET_VECTOR_ELT(lists,0,Assim);
    SET_VECTOR_ELT(lists,1,Gs);
    SET_VECTOR_ELT(lists,2,Ci);
    SET_VECTOR_ELT(lists,3,Oxy);

    SET_STRING_ELT(names,0,mkChar("Assim"));
    SET_STRING_ELT(names,1,mkChar("Gs"));
    SET_STRING_ELT(names,2,mkChar("Ci"));
    SET_STRING_ELT(names,3,mkChar("Oxy"));
    setAttrib(lists,R_NamesSymbol,names);
    UNPROTECT(6);
    return(lists);
}

SEXP eCanA(SEXP lai, SEXP Doy, SEXP Hr, SEXP SolarR, SEXP ATemp,
	  SEXP RelH, SEXP WindS, SEXP CA, SEXP OA,
	   SEXP VCMAX, SEXP VPMAX,
	  SEXP VPR, SEXP JMAX, SEXP STOMATAWS)
{
	/* const int NLAYERS = 3;  this should eventually be replaced with
	   an argument coming in from R */
	struct ET_Str direct_et, diffuse_et;

	double IDir, IDiff, Itot, rh, wind_speed;
	double pLeafsun, pLeafshade;
	double Leafsun, Leafshade;

	double TempIdir,TempIdiff,AssIdir,AssIdiff;

	double CanopyA;
	double CanHeight;

	const double cf = 3600 * 1e-6 ;

	int DOY , hr;
	double LAI, solarR, Temp, RH, WindSpeed;
	double Ca, Oa, Vcmax, Vpmax, Vpr, Jmax;
	double upperT=27.5, lowerT=3.0;

	double lat = 40;
	int nlayers = 3;
	double kd = 0.1;
	double chil = 1;
	double stomataws;

	SEXP growth;

	PROTECT(growth = allocVector(REALSXP,1));


	LAI = REAL(lai)[0];
	DOY = INTEGER(Doy)[0];
	hr = INTEGER(Hr)[0];
	solarR = REAL(SolarR)[0];
	Temp = REAL(ATemp)[0];
	RH = REAL(RelH)[0];
	WindSpeed = REAL(WindS)[0];
	Ca = REAL(CA)[0];
	Oa = REAL(OA)[0];
	Vcmax = REAL(VCMAX)[0];
	Vpmax = REAL(VPMAX)[0];
	Vpr = REAL(VPR)[0];
	Jmax = REAL(JMAX)[0];
	stomataws = REAL(STOMATAWS)[0];

	struct Light_model light_model;
	light_model = lightME(lat, DOY, hr);

	double Idir = light_model.direct_irradiance_fraction * solarR;
	double Idiff = light_model.diffuse_irradiance_fraction * solarR;
	double cosTh = light_model.cosine_zenith_angle;

	struct Light_profile light_profile;
	light_profile = sunML(Idir, Idiff, LAI, nlayers, cosTh, kd, chil, 3);

	/* results from multilayer model */
	double LAIc = LAI / nlayers;
	double relative_humidity_profile[nlayers];
	RHprof(RH, nlayers, relative_humidity_profile);

	double wind_speed_profile[nlayers];
	WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);

	/* Next use the EvapoTrans function */
	CanopyA=0.0;
	for(int i = 0; i < nlayers; ++i)
	{
		int current_layer = nlayers - 1 - i;
		rh = relative_humidity_profile[current_layer];
		wind_speed = wind_speed_profile[current_layer];

		IDir = light_profile.direct_irradiance[current_layer];
		Itot = light_profile.total_irradiance[current_layer];
		pLeafsun = light_profile.sunlit_fraction[current_layer];
		CanHeight = light_profile.height[current_layer];
		Leafsun = LAIc * pLeafsun;
		direct_et = EvapoTrans(IDir,Itot,Temp,rh,wind_speed,Leafsun,CanHeight,stomataws,1,39,0.04,0.7,0.83,0.93,0.8,0.01,3,upperT,lowerT, Ca);
		/* not the right thing to do here to add these values at the end of the ET function
		   but just a simple fix for now. The problem is that the eC4photoC function should have its own
		   EvapoTrans function. */
		TempIdir = Temp + direct_et.Deltat;
		AssIdir = eC4photoC(IDir,TempIdir,rh,Ca,Oa,Vcmax,
				Vpmax,Vpr,Jmax);

		IDiff = light_profile.diffuse_irradiance[current_layer];
		pLeafshade = light_profile.shaded_fraction[current_layer];
		Leafshade = LAIc * pLeafshade;
		diffuse_et = EvapoTrans(IDiff,Itot,Temp,rh,wind_speed,Leafshade,CanHeight,stomataws,1,39,0.04,0.7,0.83,0.93,0.8,0.01,3,upperT,lowerT, Ca);
		/* not the right thing to do here to add these values at the end of the ET function
		   but just a simple fix for now*/
		TempIdiff = Temp + diffuse_et.Deltat;
		AssIdiff = eC4photoC(IDiff,TempIdiff,rh,Ca,Oa,Vcmax,
				Vpmax,Vpr,Jmax);

		CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
	}
	/* These are micro mols of CO2 per m2 per sec
	   Need to convert to 
	   3600 converts seconds to hours
	   10^-6 converts micro mols to mols
	   */
	REAL(growth)[0] = cf * CanopyA ;
	UNPROTECT(1);  
	return(growth);

}

}  // extern "C"

