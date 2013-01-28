#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"


/* Wind Profile along Canopy Height Function */

void WINDprof(double WindSpeed, double LAI, int nlayers)
{
	int i;
	double k=0.7;
	double LI, CumLAI;
	double Wind;
        extern int tp3;  
        extern double tmp3 [];

	LI  = LAI / nlayers;
	for(i=0;i<nlayers;i++)
	{
		CumLAI = LI * (i + 1);
		Wind = WindSpeed * exp(-k * (CumLAI-LI));
		tmp3[tp3++] = Wind;
	}
}
