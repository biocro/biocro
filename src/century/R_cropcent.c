#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "soilwater.h"
#include "n2o_model.h"
#include "swconst.h"
#include "crocent.h"


SEXP cropcent (SEXP FAKE)
{
  struct cropcentlayer CROPCENT;
//  SOIL_SPT soil;
  double hetResp;
  int woody, Eflag;
  double *fake;
  woody = 0; // not testing for woody for now
  Eflag=1; //N simulation for now
//  assignPools(&CROPCENT);
//  assignParms(&CROPCENT);
  CROPCENTTimescaling(&CROPCENT);
//  assignENV(&CROPCENT);
  assignFluxRatios(&CROPCENT);
  Rprintf("Before Decomposition\n");
  printcropcentout(CROPCENT,fake,fake,fake,fake,fake,fake,fake,fake,fake,fake,fake);
  decomposeCROPCENT(&CROPCENT, woody,Eflag);
  Rprintf("After Decomposition \n");
  printcropcentout(CROPCENT,fake,fake,fake,fake,fake,fake,fake,fake,fake,fake,fake);
  Rprintf("After updating\n");
  updatecropcentpools(&CROPCENT);
  printcropcentout(CROPCENT,fake,fake,fake,fake,fake,fake,fake,fake,fake,fake,fake);
  SEXP res;
  PROTECT(res=allocVector(REALSXP,1));
  REAL(res)[0]=1.0;
  UNPROTECT(1);
  return(res);
}