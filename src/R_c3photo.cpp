#include <Rinternals.h>
#include "module_library/c3photo.hpp"

extern "C" {

SEXP c3photo(SEXP Qp1, SEXP Tl1, SEXP RH1, SEXP VCMAX, SEXP JMAX,
	     SEXP RD, SEXP CA, SEXP B0, SEXP B1, SEXP OX2, SEXP THETA,SEXP STOMWS, SEXP WATER_STRESS_APPROACH, SEXP ELECTRONS_PER_CARBOXYLATION, SEXP ELECTRONS_PER_OXYGENATION)
{
	struct c3_str tmp = {0,0,0,0};

	double vcmax = REAL(VCMAX)[0];
	double jmax = REAL(JMAX)[0];
	double Bet0 = REAL(B0)[0];
	double Bet1 = REAL(B1)[0];
	double Rd = REAL(RD)[0];
	double O2 = REAL(OX2)[0];
	double theta = REAL(THETA)[0];
	double StomWS = REAL(STOMWS)[0];
    double electrons_per_carboxylation = REAL(ELECTRONS_PER_CARBOXYLATION)[0];
    double electrons_per_oxygenation = REAL(ELECTRONS_PER_OXYGENATION)[0];
	int water_stress_approach = INTEGER(WATER_STRESS_APPROACH)[0];
	SEXP lists, names;
	SEXP GsV;
	SEXP ASSV;
	SEXP CiV;

	size_t nq = Rf_length(Qp1);

	PROTECT(lists = Rf_allocVector(VECSXP,3));
	PROTECT(names = Rf_allocVector(STRSXP,3));

	PROTECT(GsV = Rf_allocVector(REALSXP,nq));
	PROTECT(ASSV = Rf_allocVector(REALSXP,nq));
	PROTECT(CiV = Rf_allocVector(REALSXP,nq));
 
	for(size_t i = 0; i < nq; ++i)
	{
		double Qp = REAL(Qp1)[i];
		double Tl = REAL(Tl1)[i];
		double RH = REAL(RH1)[i];
		double Catm = REAL(CA)[i];

		tmp = c3photoC(Qp, Tl, RH, vcmax, jmax, Rd, Bet0, Bet1, Catm, O2, theta,StomWS, water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation);

		REAL(GsV)[i] = tmp.Gs;
		REAL(ASSV)[i] = tmp.Assim;    
		REAL(CiV)[i] = tmp.Ci;
	}

	SET_VECTOR_ELT(lists,0,GsV);
	SET_VECTOR_ELT(lists,1,ASSV);
	SET_VECTOR_ELT(lists,2,CiV);
	SET_STRING_ELT(names,0,Rf_mkChar("Gs"));
	SET_STRING_ELT(names,1,Rf_mkChar("canopy_assimilation_rate"));
	SET_STRING_ELT(names,2,Rf_mkChar("Ci"));
	Rf_setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(5);   
	return(lists);
}
}  // extern "C"

