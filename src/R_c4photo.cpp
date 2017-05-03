#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c4photo.h"

extern "C" {
SEXP c4photo(SEXP Qp, SEXP Tl, SEXP RH, SEXP VMAX, SEXP ALPHA,
	     SEXP KPAR, SEXP THETA, SEXP BETA, SEXP RD, SEXP CA, SEXP B0, SEXP B1, SEXP STOMWS, SEXP WATER_STRESS_APPROACH,SEXP UPPERTEMP, SEXP LOWERTEMP)
{
	struct c4_str tmp;

	double Bet0,Bet1;

	double vmax, alpha, K, Rd, StomWS;/*\ref{parm:Vmax}\ref{parm:Rd}*/
	double theta, beta;
	double upperT, lowerT;

	vmax = REAL(VMAX)[0];/*\ref{eqn:Vmax}*/
	alpha = REAL(ALPHA)[0];
	theta = REAL(THETA)[0];
	beta = REAL(BETA)[0];
	K = REAL(KPAR)[0];
	Bet0 = REAL(B0)[0];
	Bet1 = REAL(B1)[0];
	Rd = REAL(RD)[0];/*\ref{eqn:Rd}*/
	StomWS = REAL(STOMWS)[0];
        upperT=REAL(UPPERTEMP)[0];
        lowerT=REAL(LOWERTEMP)[0];


	SEXP lists, names;
	SEXP GsV;
	SEXP ASSV;
    SEXP GASSV;
	SEXP CiV;

	size_t nq = length(Qp);
    // nt = length(Tl);nr = length(RH); unused

	PROTECT(lists = allocVector(VECSXP,4));
	PROTECT(names = allocVector(STRSXP,4));

	PROTECT(GsV = allocVector(REALSXP,nq));
	PROTECT(ASSV = allocVector(REALSXP,nq));
  PROTECT(GASSV = allocVector(REALSXP,nq));
	PROTECT(CiV = allocVector(REALSXP,nq));
  
	double *pt_Qp = REAL(Qp);
	double *pt_Tl = REAL(Tl);
	double *pt_RH = REAL(RH);
	double *pt_CA = REAL(CA);
  
	int water_stress_approach = INTEGER(WATER_STRESS_APPROACH)[0];

	double *pt_GSV = REAL(GsV);
	double *pt_ASSV = REAL(ASSV);
    double *pt_GASSV = REAL(GASSV);
	double *pt_CiV = REAL(CiV);
	/* Start of the loop */
	for(size_t i = 0; i < nq; ++i)
	{

		tmp = c4photoC(*(pt_Qp+i), *(pt_Tl+i), *(pt_RH+i),
			       vmax, alpha, K,theta, beta, Rd, /*\ref{parm:Vmax}\ref{parm:Rd}*/
			       Bet0, Bet1, StomWS, 
			       *(pt_CA+i), water_stress_approach,upperT,lowerT);

		*(pt_GSV + i) = tmp.Gs;
		*(pt_ASSV + i) = tmp.Assim;    
    *(pt_GASSV + i) = tmp.GrossAssim; 
		*(pt_CiV + i) = tmp.Ci;    
/* Here it is using the REAL function every time */
/* I should change this to a pointer too at some point */ 
	}

	SET_VECTOR_ELT(lists,0,GsV);
	SET_VECTOR_ELT(lists,1,ASSV);
	SET_VECTOR_ELT(lists,2,CiV);
  SET_VECTOR_ELT(lists,3,GASSV);
	SET_STRING_ELT(names,0,mkChar("Gs"));
	SET_STRING_ELT(names,1,mkChar("Assim"));
	SET_STRING_ELT(names,2,mkChar("Ci"));
  SET_STRING_ELT(names,3,mkChar("GrossAssim"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(6);   
	return(lists);
}

/* Here I'm including a version of the McMCc4photo function where the likelihood
   is approximated by using exp(-RSS) */
SEXP McMCc4photo(SEXP ASSIM, SEXP QP, SEXP TEMP,
		 SEXP RH, SEXP NITER, SEXP iVCMAX,
		 SEXP iALPHA, SEXP iKPARM, 
		 SEXP iTHETA, SEXP iBETA,
		 SEXP iRD,
                 SEXP CATM, SEXP B0, SEXP B1, SEXP STOMWS,
                 SEXP SCALE, SEXP SD1, SEXP SD2, SEXP WATER_STRESS_APPROACH, SEXP PRIOR,SEXP UPPERTEMP, SEXP LOWERTEMP)
{
	/* First manipulate R objects */
	int niter;
	int nObs = length(ASSIM);
	niter = INTEGER(NITER)[0];

	/* Second define the needed variables */
	double oldRSS = 1e6;
	int n1 = 0;

	int i;

	double scale = REAL(SCALE)[0];
	double sd1, sd2;

	double Rd = REAL(iRD)[0];/*\ref{eqn:Rd}*/
	double ikparm = REAL(iKPARM)[0];
	double ibeta = REAL(iBETA)[0];
	double itheta = REAL(iTHETA)[0];
	double Catm = REAL(CATM)[0];
	double b0 = REAL(B0)[0];
	double b1 = REAL(B1)[0];
	double StomWS = REAL(STOMWS)[0];
	int water_stress_approach = INTEGER(WATER_STRESS_APPROACH)[0];

	double index;
	double rnum , rden; 
	double lratio; 
	double lmr;
	double U;

	double pmuVmax = REAL(PRIOR)[0];
	double psdVmax = REAL(PRIOR)[1];
	double pmuAlpha = REAL(PRIOR)[2];
	double psdAlpha = REAL(PRIOR)[3];

        double upperT =REAL(UPPERTEMP)[0];
        double lowerT =REAL(LOWERTEMP)[0];

	double RSS;
	double rnewVcmax, rnewAlpha;
	double oldAlpha, oldVcmax;

	SEXP lists;
	SEXP names;
	SEXP mat1;
	SEXP accept;


	PROTECT(lists = allocVector(VECSXP,2));
	PROTECT(names = allocVector(STRSXP,2));
	PROTECT(mat1 = allocMatrix(REALSXP,3,niter));
	PROTECT(accept = allocVector(REALSXP,1));

	GetRNGstate();

	oldVcmax = REAL(iVCMAX)[0];
	oldAlpha = REAL(iALPHA)[0];

	sd1 = REAL(SD1)[0] * scale;
	sd2 = REAL(SD2)[0] * scale;

	for(i = 0; i < niter; i++){

		/* Replacing the rnormC4 function */
		index = runif(0,1); 
		if(index < 0.5){ 
			rnewVcmax = oldVcmax + rnorm(0,sd1);
			rnewAlpha = oldAlpha;
		}else{ 
			rnewAlpha = oldAlpha +  rnorm(0, sd2);
			rnewVcmax = oldVcmax;
		} 
		/* This is the end of the random generation */

		rnum = dnorm(rnewVcmax,pmuVmax,psdVmax,0)*dnorm(rnewAlpha,pmuAlpha,psdAlpha,0); 
		rden = dnorm(oldVcmax,pmuVmax,psdVmax,0)*dnorm(oldAlpha,pmuAlpha,psdAlpha,0); 
	  
		lratio = log(rnum) - log(rden); 

		RSS = RSS_C4photo(REAL(ASSIM),REAL(QP),REAL(TEMP),REAL(RH),rnewVcmax,rnewAlpha, ikparm, itheta, ibeta, Rd, Catm, b0, b1, StomWS, water_stress_approach ,upperT,lowerT, nObs);

/*       mr = (exp(-RSS) / exp(-oldRSS)) * ratio; */
/* In the previous expression we can take  the log and have instead the 
   log metropolis ratio and have a simpler calculation. We then need to compare this to
   the log of a uniform. */
		lmr = -RSS + oldRSS + lratio;

		U = runif(0,1);
		if(lmr > log(U)){
			oldVcmax = rnewVcmax;
			oldAlpha = rnewAlpha;
			oldRSS = RSS;
			n1++;
		}

		REAL(mat1)[i*3] = oldVcmax ;
		REAL(mat1)[i*3 + 1] = oldAlpha ;
		REAL(mat1)[i*3 + 2] = oldRSS;
	}
     
	PutRNGstate();    

	REAL(accept)[0] = n1;

	SET_VECTOR_ELT(lists,0,accept);
	SET_VECTOR_ELT(lists,1,mat1);

	SET_STRING_ELT(names,0,mkChar("accept"));
	SET_STRING_ELT(names,1,mkChar("resuMC"));

	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(4);

	return(lists);
}

}  // extern "C"

