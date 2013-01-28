#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

/* RH  profile function */


void RHprof(double RH, int nlayers)
{
	int i;
	double kh, hsla, j;
        extern int tp4;
        extern double tmp4[];

	/*   kh = 1 - RH; */
	kh = 0.2;
	for(i=0;i<nlayers;i++)
	{
		j = i;
		hsla = RH * exp(-kh * (1-(j/nlayers)));
		/*     hsla = RH; */
		tmp4[tp4++] = hsla;
	}
	/* It should return values in the 0-1 range */
}
