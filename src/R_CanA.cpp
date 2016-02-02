/*
 *  BioCro/src/CanA.c by Fernando Ezequiel Miguez Copyright (C)
 *  2007-2012 contributions by Deepak Jaiswal, mainly for the upper
 *  and lower temperature and the nitrogen parameters.
 */

#include <R.h>
#include <Rinternals.h>
#include "BioCro.h"
#include "c4photo.h"
#include "CanA.h"

extern "C" {

SEXP CanA(
		SEXP Lai,
		SEXP Doy,
		SEXP HR,
		SEXP SOLAR,
		SEXP TEMP,
		SEXP ReH,
		SEXP windspeed,
		SEXP LAT,
		SEXP NLAYERS,
		SEXP STOMATAWS,
		SEXP VMAX,
		SEXP ALPH,
		SEXP KPARM,
		SEXP THETA,
		SEXP BETA,
		SEXP RD,
		SEXP B0,
		SEXP B1,
		SEXP CATM,
		SEXP KD,
		SEXP HEIGHTF,
		SEXP WS,
		SEXP LEAFN,
		SEXP KPLN,
		SEXP LNB0,
		SEXP LNB1,
		SEXP LNFUN,
		SEXP CHIL,
		SEXP UPPERTEMP,
		SEXP LOWERTEMP,
		SEXP NNITROP,
		SEXP LEAFWIDTH)
{
  double LAI = REAL(Lai)[0];
  int DOY = INTEGER(Doy)[0];
  int hr = INTEGER(HR)[0];
  double solarR = REAL(SOLAR)[0];
  double temp = REAL(TEMP)[0];
  double RH = REAL(ReH)[0];
  double WindSpeed = REAL(windspeed)[0];
  double lat = REAL(LAT)[0];
  int nlayers = INTEGER(NLAYERS)[0];
  double Vmax = REAL(VMAX)[0];
  double alpha = REAL(ALPH)[0];
  double kparm = REAL(KPARM)[0];
  double theta = REAL(THETA)[0];
  double beta = REAL(BETA)[0];
  double Rd = REAL(RD)[0];
  double b0 = REAL(B0)[0];
  double b1 = REAL(B1)[0];
  double Catm = REAL(CATM)[0];
  double kd = REAL(KD)[0];
  double heightf = REAL(HEIGHTF)[0];
  int ws = INTEGER(WS)[0];
  double leafN = REAL(LEAFN)[0];
  double kpLN = REAL(KPLN)[0];
  double lnb0 = REAL(LNB0)[0];
  double lnb1 = REAL(LNB1)[0];
  int lnfun = INTEGER(LNFUN)[0];
  double chil = REAL(CHIL)[0];
    double upperT=REAL(UPPERTEMP)[0];
    double lowerT=REAL(LOWERTEMP)[0];
  double leafwidth = REAL(LEAFWIDTH)[0];
  double eteq = 0.0;
  double stomataws = REAL(STOMATAWS)[0];

  SEXP lists;
  SEXP names;
  SEXP growth;
  SEXP trans;
  SEXP Ggrowth;

  PROTECT(lists = allocVector(VECSXP,7));
  PROTECT(names = allocVector(STRSXP,7));
  PROTECT(growth = allocVector(REALSXP,1));
  PROTECT(trans = allocVector(REALSXP,1));
  PROTECT(Ggrowth = allocVector(REALSXP,1));

   //NITROPARMS STRUCTURE IS PASSED and READ
  struct nitroParms nitroP;
	double TEMPdoubletoint;
	nitroP.ileafN=REAL(NNITROP)[0];
        nitroP.kln=REAL(NNITROP)[1];
	nitroP.Vmaxb1=REAL(NNITROP)[2];
	nitroP.Vmaxb0=REAL(NNITROP)[3];
	nitroP.alphab1=REAL(NNITROP)[4];
	nitroP.alphab0=REAL(NNITROP)[5];
        nitroP.Rdb1=REAL(NNITROP)[6];
	nitroP.Rdb0=REAL(NNITROP)[7];
	nitroP.kpLN=REAL(NNITROP)[8];
	nitroP.lnb0=REAL(NNITROP)[9];
	nitroP.lnb1=REAL(NNITROP)[10];
	TEMPdoubletoint=REAL(NNITROP)[11];
	nitroP.lnFun=(int)TEMPdoubletoint;
	nitroP.maxln=REAL(NNITROP)[12];
	nitroP.minln=REAL(NNITROP)[13];
	nitroP.daymaxln=REAL(NNITROP)[14];
  /////////////////////////////////////////

  struct Can_Str ans = {0, 0, 0};
  ans = CanAC(LAI, DOY, hr, solarR, temp,
		  RH, WindSpeed, lat, nlayers, Vmax,
		  alpha, kparm, beta, Rd, Catm,
		  b0, b1, theta, kd, chil,
		  heightf, leafN, kpLN, lnb0, lnb1,
		  lnfun, upperT, lowerT, nitroP, leafwidth,
		  eteq, stomataws, ws);

    if(ISNAN(ans.Assim)) {
        error("Something is NA \n");
    }

    REAL(growth)[0] = ans.Assim;
    REAL(trans)[0] = ans.Trans;
    REAL(Ggrowth)[0] = ans.GrossAssim;

    SET_VECTOR_ELT(lists, 0, growth);
    SET_VECTOR_ELT(lists, 1, trans);
    SET_VECTOR_ELT(lists, 2, Ggrowth);

    SET_STRING_ELT(names, 0, mkChar("CanopyAssim"));
    SET_STRING_ELT(names, 1, mkChar("CanopyTrans"));
    SET_STRING_ELT(names, 2, mkChar("GrossCanopyAssim"));
    setAttrib(lists, R_NamesSymbol, names);

    UNPROTECT(5);
    return(lists);
   }
}  // extern "C"
