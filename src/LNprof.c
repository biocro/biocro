#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

/* Leaf N distribution along the canopy height */

void LNprof(double LeafN, double LAI, int nlayers, double kpLN)
{

	int i;
	double leafNla, LI, CumLAI;
        extern int tp5;
        extern double tmp5[];

	LI  = LAI / nlayers;
	for(i=0;i<nlayers;i++)
	{
		CumLAI = LI * (i + 1);
		leafNla = LeafN * exp(-kpLN * (CumLAI-LI));
		tmp5[tp5++] = leafNla;
	}

}
