
#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

/* Respiration. It is assumed that some of the energy produced by the
   plant has to be used in basic tissue maintenance. This is handled
   quite empirically by some relationships developed by McCree (1970)
   and Penning de Vries (1972) */

double resp(double comp, double mrc, double temp){

	double ans;

	ans = comp *  (1 - (mrc * pow(2,(temp/10.0))));

	if(ans <0) ans = 0;

	return(ans);

}
