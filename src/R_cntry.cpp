#include <R.h>
#include <Rinternals.h>
#include "Century.h"
#include "AuxBioCro.h"

extern "C" {

SEXP cntry(SEXP LEAFL,           /* 1 */ 
	   SEXP STEML,           /* 2 */ 
	   SEXP ROOTL,           /* 3 */ 
	   SEXP RHIZL,           /* 4 */ 
           SEXP SMOIST,          /* 5 */ 
	   SEXP STEMP,           /* 6 */ 
	   SEXP TIMESTEP,        /* 7 */ 
	   SEXP SCS,             /* 8 */ 
	   SEXP LEACHWATER,      /* 9 */ 
	   SEXP NFERT,           /* 10 */ 
	   SEXP IMINN,           /* 11 */ 
	   SEXP PRECIP,          /* 12 */ 
	   SEXP LEAFLLN,         /* 13 */ 
	   SEXP STEMLLN,         /* 14 */ 
	   SEXP ROOTLLN,         /* 15 */ 
	   SEXP RHIZLLN,         /* 16 */ 
	   SEXP LEAFLN,          /* 17 */ 
	   SEXP STEMLN,          /* 18 */ 
	   SEXP ROOTLN,          /* 19 */ 
	   SEXP RHIZLN,          /* 20 */ 
	   SEXP SOILTYPE,        /* 21 */ 
	   SEXP KS){          /* 22 */ 

  struct cenT_str tmp;

  int j;

  double LeafL = REAL(LEAFL)[0];
  double StemL = REAL(STEML)[0];
  double RootL = REAL(ROOTL)[0];
  double RhizL = REAL(RHIZL)[0];

  SEXP lists, names;

  SEXP SCs;
  SEXP SNs;
  SEXP MinN, Resp;

  PROTECT(lists = allocVector(VECSXP,4));
  PROTECT(names = allocVector(STRSXP,4));
  PROTECT(SCs = allocVector(REALSXP,9));
  PROTECT(SNs = allocVector(REALSXP,9));
  PROTECT(MinN = allocVector(REALSXP,1));
  PROTECT(Resp = allocVector(REALSXP,1));

  tmp = Century(&LeafL, 
		&StemL, 
		&RootL, 
		&RhizL,
                REAL(SMOIST)[0], 
		REAL(STEMP)[0], 
		INTEGER(TIMESTEP)[0], 
		REAL(SCS),
                REAL(LEACHWATER)[0], 
		REAL(NFERT)[0], 
		REAL(IMINN)[0], 
		REAL(PRECIP)[0],
                REAL(LEAFLLN)[0], 
		REAL(STEMLLN)[0], 
		REAL(ROOTLLN)[0], 
		REAL(RHIZLLN)[0],
                REAL(LEAFLN)[0], 
		REAL(STEMLN)[0], 
		REAL(ROOTLN)[0], 
		REAL(RHIZLN)[0], 
		INTEGER(SOILTYPE)[0], 
		REAL(KS));

  REAL(MinN)[0] = tmp.MinN;
  REAL(Resp)[0] = tmp.Resp;

  for(j=0;j<9;j++){
  
    REAL(SCs)[j] = tmp.SCs[j];
    REAL(SNs)[j] = tmp.SNs[j];
  }

  SET_VECTOR_ELT(lists,0,SCs);
  SET_VECTOR_ELT(lists,1,SNs);
  SET_VECTOR_ELT(lists,2,MinN);
  SET_VECTOR_ELT(lists,3,Resp);

  SET_STRING_ELT(names,0,mkChar("SCs"));
  SET_STRING_ELT(names,1,mkChar("SNs"));
  SET_STRING_ELT(names,2,mkChar("MinN"));
  SET_STRING_ELT(names,3,mkChar("Resp"));
  setAttrib(lists,R_NamesSymbol,names);
  UNPROTECT(6);
  return(lists);

}
}  // extern "C"

