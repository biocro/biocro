#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "Tempfunct.h"

double TempToDdryA(double Temp)
{
	double DdryA;
	DdryA = 1.295163636 + -0.004258182 * Temp;
	return(DdryA);
}


/* Define Purpose */
double TempToLHV(double Temp)
{
	double LHV;
	LHV = 2.501 + -0.002372727 * Temp;
	return(LHV);
}

/* Define Purpose */
double TempToSFS(double Temp)
{
	double SlopeFS;
	SlopeFS = 0.338376068 +  0.011435897 * Temp +  0.001111111 * pow(Temp,2);
	return(SlopeFS);
}

/* Define Purpose */

double TempToSWVC(double Temp)
{
	double SWVC;
	SWVC =  4.90820192 +   0.06387253 * Temp +    0.02745742 * pow(Temp,2);
	return(SWVC);
}
