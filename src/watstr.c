#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

/* This is a new function that attempts to keep a water budget and then
   calcualte an empirical coefficient that reduces the specific leaf area.
   This results from the general idea that water stress reduces first the 
   rate of leaf expansion. */ 

/* This is meant to be a simple function that calculates a
   simple empirical coefficient that reduces specifi leaf area
   according to the water stress of the plant. This is done
   for now, with a very simple empirical approach. */

struct soilText_str soilTchoose(int soiltype){

	/* This function is based on Campbell and Norman.
	   Introduction to Environmental Biophysics. pg 130. */

	struct soilText_str tmp;

	tmp.silt = 0;
	tmp.clay = 0;
	tmp.sand = 0;
	tmp.air_entry = 0;
	tmp.b = 0;
	tmp.Ks = 0;
	tmp.fieldc = 0;
	tmp.wiltp = 0;

	if(soiltype == 0){
	/* sand soil */
	tmp.silt = 0.05;
	tmp.clay = 0.03;
	tmp.sand = 0.92;
	tmp.air_entry = -0.7;
	tmp.b = 1.7;
	tmp.Ks = 5.8e-3;
	tmp.fieldc = 0.09;
	tmp.wiltp = 0.03;

	} else

	if(soiltype == 1){
	/* loamy sand */
	tmp.silt = 0.12;
	tmp.clay = 0.07;
	tmp.sand = 0.81;
	tmp.air_entry = -0.9;
	tmp.b = 2.1;
	tmp.Ks = 1.7e-3;
	tmp.fieldc = 0.13;
	tmp.wiltp = 0.06;

	} else

	if(soiltype == 2){
	/* sandy loam */
	tmp.silt = 0.25;
	tmp.clay = 0.10;
	tmp.sand = 0.65;
	tmp.air_entry = -1.5;
	tmp.b = 3.1;
	tmp.Ks = 7.2e-4;
	tmp.fieldc = 0.21;
	tmp.wiltp = 0.10;

	} else

	if(soiltype == 3){
	/* loam */
	tmp.silt = 0.40;
	tmp.clay = 0.18;
	tmp.sand = 0.52;
	tmp.air_entry = -1.1;
	tmp.b = 4.5;
	tmp.Ks = 3.7e-4;
	tmp.fieldc = 0.27;
	tmp.wiltp = 0.12;

	} else

	if(soiltype == 4){
	/* silt loam */
	tmp.silt = 0.65;
	tmp.clay = 0.15;
	tmp.sand = 0.20;
	tmp.air_entry = -2.1;
	tmp.b = 4.7;
	tmp.Ks = 1.9e-4;
	tmp.fieldc = 0.33;
	tmp.wiltp = 0.13;

	} else

	if(soiltype == 5){
	/* sandy clay loam */
	tmp.silt = 0.13;
	tmp.clay = 0.27;
	tmp.sand = 0.60;
	tmp.air_entry = -2.8;
	tmp.b = 4;
	tmp.Ks = 1.2e-4;
	tmp.fieldc = 0.26;
	tmp.wiltp = 0.15;

	} else

	if(soiltype == 6){
	/* clay loam */
	tmp.silt = 0.34;
	tmp.clay = 0.34;
        tmp.sand = 0.32;
	tmp.air_entry = -2.6;
	tmp.b = 5.2;
	tmp.Ks = 6.4e-5;
	tmp.fieldc = 0.32;
	tmp.wiltp = 0.20;

	} else

	if(soiltype == 7){
	/* silty clay loam */
	tmp.silt = 0.58;
	tmp.clay = 0.33;
	tmp.sand = 0.09;
	tmp.air_entry = -3.3;
	tmp.b = 6.6;
	tmp.Ks = 4.2e-5;
	tmp.fieldc = 0.37;
	tmp.wiltp = 0.21; /* Correction from the book from here http://www.public.iastate.edu/~bkh/teaching/505/norman_book_corrections.pdf */

	} else

	if(soiltype == 8){
	/* sandy clay */
	tmp.silt = 0.07;
	tmp.clay = 0.40;
	tmp.sand = 0.53;
	tmp.air_entry = -2.9;
	tmp.b = 6;
	tmp.Ks = 3.3e-5;
	tmp.fieldc = 0.34;
	tmp.wiltp = 0.24;

	} else

	if(soiltype == 9){
	/* silty clay */
	tmp.silt = 0.45;
	tmp.clay = 0.45;
	tmp.sand = 0.10;
	tmp.air_entry = -3.4;
	tmp.b = 7.9;
	tmp.Ks = 2.5e-5;
	tmp.fieldc = 0.39;
	tmp.wiltp = 0.25;

	} else

	if(soiltype == 10){
	/* clay */
	tmp.silt = 0.20;
	tmp.clay = 0.60;
	tmp.sand = 0.20;
	tmp.air_entry = -3.7;
	tmp.b = 7.6;
	tmp.Ks = 1.7e-5;
	tmp.fieldc = 0.4;
	tmp.wiltp = 0.27;

	}

	return(tmp);

}

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, 
                     double fieldc, double wiltp, double phi1, double phi2, 
		     int soiltype, /* soil type indicator */ 
		     int wsFun) /* flag for which water stress function to use */
{

	struct ws_str tmp;
	struct soilText_str soTexS;
	/* Variables */
	double precipM;
	/* available water and per hectare */
	double aw, naw; 
	double pawha, Newpawha, npaw; /* new 04-27-2009 */
	double runoff = 0.0, runoff2 = 0.0;
	/* variable needed for calculation of water stress*/
	double wsPhoto = 0.0, wsSpleaf, phi10;
	double slp = 0.0, intcpt = 0.0, theta = 0.0; 
	double Nleach = 0.0;
	/* Nleach is the NO3 leached and Ts is the sand content of the soil*/

	/* Specify the soil type */
	soTexS = soilTchoose(soiltype);
/*   Ts = soTexS.sand; */

	if(fieldc < 0){
		fieldc = soTexS.fieldc;
	}
	if(wiltp < 0){
		wiltp = soTexS.wiltp;
	}

	/* unit conversion for precip */
	precipM = precipit * 1e-3; /* convert precip in mm to m*/

	/*    cws is current water status */
	/*    available water */

	aw = precipM + cws;

	if(aw > fieldc){ 
		runoff = aw - fieldc;
		/* Need to convert to units used in the Parton et al 1988 paper. */
		/* The data comes in mm/hr and it needs to be in cm/month */
		runoff2 = runoff * 0.10 * (1/24*30);
		Nleach = runoff /18 * (0.2 + 0.7 * soTexS.sand);
		aw = fieldc;
	}
	/* Tipping bucket need to collect it if want to estimate runoff */ 
	/* plant available water per hectare (pawha) */
	pawha = (aw - wiltp) * 1e4 * soildepth;
	/* The density of water is 998.2 kg/m3 at 20 degrees Celsius */
	/* or 0.9882 Mg/m3 */
	/* pawha is plant available water (m3) per hectare */
	/* evapo is demanded water (Mg) per hectare */

	Newpawha = pawha - evapo / 0.9882; /* New version 04-27-2009 */

	/*  Here both are in m3 of water per ha-1 so this */
	/*  subtraction should be correct */
	/* go back to original units of water in the profile */

	npaw = Newpawha * 1e-4 * (1/soildepth); /* New 04-27-2009 */

	if(npaw < 0) npaw = 0.0;

	naw = npaw + wiltp;

        /* Calculating the soil water potential based on equations from Norman and Campbell */
	tmp.psim = soTexS.air_entry * pow((naw/soTexS.fieldc*1.1),-soTexS.b) ;

	/* three different type of equations for modeling the effect of water stress on vmax and leaf area expansion. 
	   The equation for leaf area expansion is more severe than the one for vmax. */
	if(wsFun == 0){
		slp = 1/(fieldc - wiltp);
		intcpt = 1 - fieldc * slp;
		wsPhoto = slp * naw + intcpt ;
	}else
	if(wsFun == 1){
		phi10 = (fieldc + wiltp)/2;
		wsPhoto = 1/(1 + exp((phi10 - naw)/ phi1));
	}else
	if(wsFun == 2){
		slp = (1 - wiltp)/(fieldc - wiltp);
		intcpt = 1 - fieldc * slp;
		theta = slp * naw + intcpt ;
		wsPhoto = (1 - exp(-2.5 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-2.5));
	}else
		if(wsFun == 3){
			wsPhoto = 1;
		}

	if(wsPhoto <= 0 )
		wsPhoto = 1e-20; /* This can be mathematically lower than zero in some cases but I should prevent that. */

	wsSpleaf = pow(naw,phi2) * 1/pow(fieldc,phi2); 
	if(wsFun == 3){ 
		wsSpleaf = 1;
	}



	/* returning the structure*/
	tmp.rcoefPhoto = wsPhoto;
	tmp.rcoefSpleaf = wsSpleaf;
	tmp.awc = naw;
	tmp.runoff = runoff;
	tmp.Nleach = Nleach;
	return(tmp);
}
