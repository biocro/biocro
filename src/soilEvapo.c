#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c4photo.h"
#include "AuxBioCro.h"



/* Soil Evaporation Function */
/* Variables I need */
/* LAI = Leaf Area Index */
/* k = extinction coefficient */
/* Temp = Air Temperature */
/* DirectRad = Direct Total Radiation */
/* awc, wiltp, fieldc = available water content, wilting point and field capacity */
/* winds = wind speed */


double SoilEvapo(double LAI, double k, double AirTemp, double IRad,
		 double awc, double fieldc, double wiltp, double winds, double RelH, double rsec ){

	double SoilArea;
	double SoilTemp;
	double Up; /*Maximum Dimensionless Uptake Rate */
	double TotalRadiation;
	double BoundaryLayerThickness, DiffCoef;
	double SoilBoundaryLayer, Ja, rlc;
	double PhiN, PsycParam, DeltaPVa;
	double Evaporation = 0.0;  
	double DdryA, LHV, SlopeFS, SWVC;

	double rawc; /* Relative available water content */

	int method = 1;

	/* some constants */
	const double SoilClodSize = 0.04;
	const double SoilReflectance = 0.2;
	const double SoilTransmission = 0.01;
	const double SpecificHeat = 1010;
	const double StefanBoltzman = 5.67e-8; /* Stefan Boltzman Constant */

	const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 

	/* For Transpiration */
	/* 3600 converts seconds to hours */
	/* 1e-3 converts mili mols to mols */
	/* 18 is the grams in one mol of H20 */
	/* 1e-6 converts g to Mg */
	/* 10000 scales from meter squared to hectare */

	/* Let us assume a simple way of calculating the proportion of the
	   soil with direct radiation */
	SoilArea = exp(-k * LAI);

	/* For now the temperature of the soil will be the same as the air.
	   At a later time this can be made more accurate. I looked at the
	   equations for this and the issue is that it is strongly dependent on
	   depth. Since the soil model now has a single layer, this cannot be
	   implemented correctly at the moment.  */

	SoilTemp = AirTemp;

	/* Let us use an idea of Campbell and Norman. Environmental
	   Biophysics. */
	/* If relative available water content is */
	rawc = (awc - wiltp)/(fieldc - wiltp);

	/* Page 142 */
	/* Maximum Dimensionless Uptake Rate */
	Up = 1 - pow((1 + 1.3 * rawc),-5);  
	/* This is a useful idea because dry soils evaporate little water when dry*/

	/* Total Radiation */
	/*' Convert light assuming 1 µmol PAR photons = 0.235 J/s Watts*/
	/* At the moment soil evaporation is grossly overestimated. In WIMOVAC
	   the light reaching the last layer of leaves is used. Here instead
	   of calculating this again, I will for now assume a 10% as a rough
	   estimate. Note that I could maybe get this since layIdir and
	   layIDiff in sunML are external variables.  Rprintf("IRad
	   %.5f",layIdir[0],"\n"); Update: 03-13-2009. I tried printing this
	   value but it is still too high and will likely overestimate soil
	   evaporation. However, this is still a work in progress.
	*/
	IRad *= rsec; /* Radiation soil evaporation coefficient */ 

	TotalRadiation = IRad * 0.235;
 
	DdryA = TempToDdryA(AirTemp) ;
	LHV = TempToLHV(AirTemp) * 1e6 ; 
/* Here LHV is given in MJ kg-1 and this needs to be converted
   to Joules kg-1  */
	SlopeFS = TempToSFS(AirTemp) * 1e-3;
	SWVC = TempToSWVC(AirTemp) * 1e-3;

	PsycParam = (DdryA * SpecificHeat) / LHV;
	DeltaPVa = SWVC * (1 - RelH / 100);

	BoundaryLayerThickness = 4e-3 * sqrt(SoilClodSize / winds); 
	DiffCoef = 2.126e-5 * 1.48e-7 * SoilTemp;
	SoilBoundaryLayer = DiffCoef / BoundaryLayerThickness;

	Ja = 2 * TotalRadiation * ((1 - SoilReflectance - SoilTransmission) / (1 - SoilTransmission));

	rlc = 4 * StefanBoltzman * pow((273 + SoilTemp),3) * 0.005;
/* the last term should be the difference between air temperature and
   soil. This is not actually calculated at the moment. Since this is
   mostly relevant to the first soil layer where the temperatures are
   similar. I will leave it like this for now. */

	PhiN = Ja - rlc; /* Calculate the net radiation balance*/
	if(PhiN < 0) PhiN = 1e-7;

	/* Priestly-Taylor */
	if(method == 0){
		Evaporation = 1.26 * (SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam));
	}else{
		/* Penman-Monteith */
		Evaporation = (SlopeFS * PhiN + LHV * PsycParam * SoilBoundaryLayer * DeltaPVa) / (LHV * (SlopeFS + PsycParam));
	}
/*  Report back the soil evaporation rate in Units mmoles/m2/s */
/*     Evaporation = Evaporation * 1000:   ' Convert Kg H20/m2/s to g H20/m2/s */
/*     Evaporation = Evaporation / 18:     ' Convert g H20/m2/s to moles H20/m2/s */
/*     Evaporation = Evaporation * 1000:   ' Convert moles H20/m2/s to mmoles H20/m2/s */
    
/*     If Evaporation <= 0 Then Evaporation = 0.00001: 
       ' Prevent any odd looking values which might get through at very low light levels */

	Evaporation *= 1e6/18;
	/* Adding the area dependence and the effect of drying */
	/* Converting from m2 to ha (times 1e4) */
	/* Converting to hour */
	Evaporation *= SoilArea * Up * cf2; 
	if(Evaporation < 0) Evaporation = 1e-6;

	return(Evaporation);
}
