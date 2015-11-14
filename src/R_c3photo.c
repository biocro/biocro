#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c3photo.h"

SEXP c3photo(SEXP Qp1, SEXP Tl1, SEXP RH1, SEXP VCMAX, SEXP JMAX,
	     SEXP RD, SEXP CA, SEXP B0, SEXP B1, SEXP OX2, SEXP THETA,SEXP STOMWS, SEXP WS)
{
	struct c3_str tmp = {0,0,0,0};

	double Qp, Tl, RH, Catm;
	double Bet0,Bet1;

	double vcmax, jmax, Rd, Ca, O2, theta,StomWS;

	vcmax = REAL(VCMAX)[0];
	jmax = REAL(JMAX)[0];
	Bet0 = REAL(B0)[0];
	Bet1 = REAL(B1)[0];
	Rd = REAL(RD)[0];
	O2 = REAL(OX2)[0];
	theta = REAL(THETA)[0];
	StomWS = REAL(STOMWS)[0];
	int nq , nt, nr, i;
	int ws = INTEGER(WS)[0];
	SEXP lists, names;
	SEXP GsV;
	SEXP ASSV;
	SEXP CiV;

	nq = length(Qp1);nt = length(Tl1);nr = length(RH1);

	PROTECT(lists = allocVector(VECSXP,3));
	PROTECT(names = allocVector(STRSXP,3));

	PROTECT(GsV = allocVector(REALSXP,nq));
	PROTECT(ASSV = allocVector(REALSXP,nq));
	PROTECT(CiV = allocVector(REALSXP,nq));

	Ca = REAL(CA)[0]; /* partial pressure of CO2 at the leaf surface */
 
	/* Start of the loop */
	for(i = 0; i < nq ; i++)
	{
		/*pick the right element*/
		Qp = REAL(Qp1)[i];
		Tl = REAL(Tl1)[i];
		RH = REAL(RH1)[i];
		Catm = REAL(CA)[i];

		tmp = c3photoC(Qp, Tl, RH, vcmax, jmax, Rd, Bet0, Bet1, Catm, O2, theta,StomWS,ws);

		REAL(GsV)[i] = tmp.Gs;
		REAL(ASSV)[i] = tmp.Assim;    
		REAL(CiV)[i] = tmp.Ci;
	}

	SET_VECTOR_ELT(lists,0,GsV);
	SET_VECTOR_ELT(lists,1,ASSV);
	SET_VECTOR_ELT(lists,2,CiV);
	SET_STRING_ELT(names,0,mkChar("Gs"));
	SET_STRING_ELT(names,1,mkChar("Assim"));
	SET_STRING_ELT(names,2,mkChar("Ci"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(5);   
	return(lists);
}



/* Solubility of CO2 and O2 */

/* This function returns the solubility of CO2 at temperature t relative to CO2 */
/* solubility at 25C it does this by using a polynomial which has been fitted to */
/* experimental data. */

/* Function result units: micro mol mol^-1 */
/* Function found: */
/* Plant cell and Environment, (1991) 14, 729-739. */
/* Modification of the response of photosynthetic activity to */
/* rising temperature by atmospheric CO2 concentrations: Has its */
/* importance been underestimated? */

double solc(double LeafT){

	double tmp;

	if(LeafT > 24 && LeafT < 26){
		tmp = 1;
	}else{
		tmp = (1.673998 - 0.0612936 * LeafT + 0.00116875 * pow(LeafT,2) - 8.874081e-06 * pow(LeafT,3)) / 0.735465;
	}

	return(tmp);
}

double solo(double LeafT){

	double tmp;

	if(LeafT > 24 && LeafT < 26){
		tmp = 1;
	}else{
		tmp = (0.047 - 0.0013087 * LeafT + 2.5603e-05 * pow(LeafT,2) - 2.1441e-07 * pow(LeafT,3)) / 0.026934;
	}

	return(tmp);
}

