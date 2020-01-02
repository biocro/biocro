#include <Rinternals.h>
#include "module_library/c4photo.h"

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

	size_t nq = Rf_length(Qp);
    // nt = Rf_length(Tl);nr = Rf_length(RH); unused

	PROTECT(lists = Rf_allocVector(VECSXP,4));
	PROTECT(names = Rf_allocVector(STRSXP,4));

	PROTECT(GsV = Rf_allocVector(REALSXP,nq));
	PROTECT(ASSV = Rf_allocVector(REALSXP,nq));
  PROTECT(GASSV = Rf_allocVector(REALSXP,nq));
	PROTECT(CiV = Rf_allocVector(REALSXP,nq));
  
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
	SET_STRING_ELT(names,0,Rf_mkChar("Gs"));
	SET_STRING_ELT(names,1,Rf_mkChar("assimilation_rate"));
	SET_STRING_ELT(names,2,Rf_mkChar("Ci"));
  SET_STRING_ELT(names,3,Rf_mkChar("GrossAssim"));
	Rf_setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(6);   
	return(lists);
}

}  // extern "C"

