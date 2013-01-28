#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

struct dbp_str SUGARCANE_DBP(double TT)
{
	struct dbp_str dbp;
        double mu[4],D[4],W0[4],Wf[4];
	double B[4],W[4],DWDT[4];
	double ttemp=0.0;
	int i;

/*	mu[1]=0.00834;
	mu[2]=0.01122;
	mu[3]=0.01185;

	D[1]=0.000549;
	D[2]=0.001292;
	D[3]=0.001079;

	W0[1]=0.001;
	W0[2]=0.001;
	W0[3]=0.001;

	Wf[1]=39.0;
	Wf[2]=5.9;
	Wf[3]=2.4;
*/

/* These values assuming Wf=1.2 W 
	mu[1]=0.01063;
	mu[2]=0.00576;
	mu[3]=0.00664;

	D[1]= 0.000920;
	D[2]=0.000663;
	D[3]=0.000853;

	W0[1]=0.001;
	W0[2]=0.001;
	W0[3]=0.001;

	Wf[1]=46.8;
	Wf[2]=7.08;
	Wf[3]=2.88;
*/

/* This part is taken from Cuadra */
/*
	mu[1]=0.00848;
	mu[2]=0.00902;
	mu[3]=0.00961;

	D[1]= 0.000563;
	D[2]=0.000935;
	D[3]=0.001138;

	W0[1]=0.001;
	W0[2]=0.001;
	W0[3]=0.001;

	Wf[1]=39.0;
	Wf[2]=7.08;
	Wf[3]=3.6;

if(TT <= 200)
	{
		dbp.kLeaf=0.0;
	        dbp.kStem=7.15e-08;
	        dbp.kRoot=1.0;
	        dbp.kRhiz=-1.36e-04;
		dbp.kGrain=0.0;
	}

	if((TT >200)&& (TT<=400))
       {
		dbp.kLeaf=0.003*TT-0.6;
		dbp.kStem=9.23e-25;
		dbp.kRoot=1.0- dbp.kLeaf;
		dbp.kRhiz=-1.69e-05;
		dbp.kGrain=0.0;
	}

	if((TT >400)&& (TT<=800))
       {
		dbp.kLeaf=0.5;
		dbp.kStem=0.25;
		dbp.kRoot=0.25;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}

	if((TT >800)&& (TT<=2400))
       {
		dbp.kLeaf=0.675-0.000219*TT;
		dbp.kRoot=0.1;
	        dbp.kStem=1.0-dbp.kRoot-dbp.kLeaf;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}

	if((TT >2000)&& (TT<10000))
       {
		dbp.kLeaf=0.15;
		dbp.kStem=0.75;
		dbp.kRoot=0.1;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}

*/
	/*This part was ok before chaning SLA = 0.6 */

	if(TT <= 200)
	{
		dbp.kLeaf=1.61e-08;
		dbp.kStem=7.15e-08;
		dbp.kRoot=1.0;
		dbp.kRhiz=-1.36e-04;
		dbp.kGrain=0.0;
	}

	if((TT >200)&& (TT<=800))
       {
		dbp.kLeaf=2.61e-01;
		dbp.kStem=9.23e-25;
		dbp.kRoot=0.7391;
		dbp.kRhiz=-1.69e-05;
		dbp.kGrain=0.0;
	}

	if((TT >800)&& (TT<=1200))
       {
		dbp.kLeaf=2.61e-01;
		dbp.kStem=0.539;
		dbp.kRoot=0.20;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}

	if((TT >1200)&& (TT<=2000))
       {
		dbp.kLeaf=0.15;
		dbp.kStem=0.75;
		dbp.kRoot=0.1;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}

	if((TT >2000)&& (TT<10000))
       {
		dbp.kLeaf=0.17;
		dbp.kStem=0.75;
		dbp.kRoot=0.08;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}



/* This part is a continuous function based on SUguimera thesis
/*   
       
	if(TT >800)

	{
		for ( i=1; i<=3; i++)
		{
			B[i]=Wf[i] *(exp(mu[i]/D[i])-1)/(exp(mu[i]/D[i])-(Wf[i]/W0[i]));
		
			W[i]=(W0[i]*B[i])/(W0[i] +(B[i]-W0[i])*(exp((-1)*mu[i]*(1-exp((-1)*D[i]*TT))/D[i])));                   
                        
			DWDT[i]=(mu[i]*W[i])*(1.0-(W[i]/B[i]))*exp((-1)*D[i]*TT);
			
			ttemp=ttemp+DWDT[i];
		}
		
		dbp.kStem=DWDT[1]/ttemp;
		dbp.kLeaf=DWDT[2]/ttemp;
		dbp.kRoot=DWDT[3]/ttemp;
		dbp.kRhiz=0.0;
		dbp.kGrain=0.0;
	}
*/
	return(dbp);
}
	
