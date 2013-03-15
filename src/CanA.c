/*
 *  BioCro/src/CanA.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2012
 *
 */

#include <R.h>
#include <Rinternals.h>
#include <Rmath.h>
#include <math.h>
#include "c4photo.h"
#include "AuxBioCro.h"
#include "CanA.h"

/* CanA function */
/* This file will contain the function which will consolidate
 the previous functions into one. The idea is to go from
 existing LAI and weather conditions to canopy assimilation
 and transpiration */


SEXP CanA(SEXP Lai,SEXP Doy,SEXP HR,SEXP SOLAR,SEXP TEMP,
	  SEXP ReH,SEXP windspeed,SEXP LAT,SEXP NLAYERS, SEXP STOMATAWS,
	  SEXP VMAX, SEXP ALPH, SEXP KPARM, SEXP THETA, SEXP BETA,
	  SEXP RD, SEXP B0, SEXP B1, SEXP CATM, SEXP KD, SEXP HEIGHTF, 
	  SEXP WS, SEXP LEAFN, SEXP KPLN, SEXP LNB0, SEXP LNB1, SEXP LNFUN, SEXP CHIL,SEXP UPPERTEMP, SEXP LOWERTEMP)
{

    double upperT=REAL(UPPERTEMP)[0];
    double lowerT=REAL(LOWERTEMP)[0];
/* Declaring the struct for the Evaop Transpiration */
   struct ET_Str tmp5_ET , tmp6_ET; 
   struct c4_str tmpc4, tmpc42; 


  const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
/* Need a different conversion factor for transpiration */
  const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
  /* 3600 - number of seconds in an hour */
  /* 1e-3 - mili mols to mols */
  /* 18 - grams in one mol of H20 */
  /* 1e-6 - grams to mega grams */
  /* 10000 - meters in one hectare */

  int i;
  double Idir, Idiff, cosTh;
  double LAIc;
  double IDir, IDiff, Itot,rh, WindS;
  double TempIdir,TempIdiff,AssIdir,AssIdiff,GAssIdir,GAssIdiff;
  double pLeafsun, pLeafshade;
  double Leafsun, Leafshade;

  double CanopyA,GCanopyA ;
  double CanopyT , CanopyPe = 0.0, CanopyPr = 0.0;
  double CanopyC = 0.0;
  double CanHeight;
  /* Need to label them something different from the arguments of the c4photoC
     function because these need to be modified by the water stress effect on
     the stomata and assimilation. The Ball-Berry parameters are not implemented
     yet (I need to think about this a bit harder). */
  double vmax1;
  double leafN_lay;


  /* INTEGERS */
  int DOY = INTEGER(Doy)[0];
  int hr = INTEGER(HR)[0];
  int nlayers = INTEGER(NLAYERS)[0];

  /* REALS */
  double LAI = REAL(Lai)[0];
  double solarR = REAL(SOLAR)[0];
  double Temp = REAL(TEMP)[0];
  double RlH = REAL(ReH)[0];
  double WindSpeed = REAL(windspeed)[0];
  double lat = REAL(LAT)[0];
  double kd = REAL(KD)[0];
  double heightf = REAL(HEIGHTF)[0];
  double chil = REAL(CHIL)[0];
  double Catm = REAL(CATM)[0];

  /* Photosynthesis parameters */

  double alpha1 = REAL(ALPH)[0];
  double kparm1 = REAL(KPARM)[0];
  double theta = REAL(THETA)[0];
  double beta = REAL(BETA)[0];
  double Rd1 = REAL(RD)[0];
  double b01 = REAL(B0)[0];
  double b11 = REAL(B1)[0];
  double stomataws = REAL(STOMATAWS)[0];
  double  LeafN = REAL(LEAFN)[0];
  double  kpLN = REAL(KPLN)[0];
  double lnb0 = REAL(LNB0)[0];
  double lnb1 = REAL(LNB1)[0];
  int lnfun = INTEGER(LNFUN)[0];

  SEXP lists;
  SEXP names;
  SEXP growth;
  SEXP Ggrowth;
  SEXP trans;
  SEXP epen;
  SEXP epries;
  SEXP cond;

  SEXP mat1;

  PROTECT(lists = allocVector(VECSXP,7));
  PROTECT(names = allocVector(STRSXP,7));
  PROTECT(growth = allocVector(REALSXP,1));
  PROTECT(trans = allocVector(REALSXP,1));
  PROTECT(epen = allocVector(REALSXP,1));
  PROTECT(epries = allocVector(REALSXP,1));
  PROTECT(cond = allocVector(REALSXP,1));
  PROTECT(Ggrowth = allocVector(REALSXP,1));
  PROTECT(mat1 = allocMatrix(REALSXP,17,nlayers));


  /* Light Macro Environment. As a side effect it populates tmp1. This
   * should eventually be replaced by a structure. */

     lightME(lat,DOY,hr);

    Idir = tmp1[0] * solarR;
    Idiff = tmp1[1] * solarR;
    cosTh = tmp1[2];

/* sun multilayer model. As a side effect it populates the layIdir, layItotal, layFsun, layHeight,
layIdiff, layShade vectors. */
    
    sunML(Idir,Idiff,LAI,nlayers,cosTh,kd,chil,heightf);

    /* results from multilayer model */
    LAIc = LAI / nlayers;
    /* Next I need the RH and wind profile */

    RHprof(RlH,nlayers);
    /* It populates tmp4. */

     WINDprof(WindSpeed,LAI,nlayers);
    /* It populates tmp3. */

     LNprof(LeafN, LAI, nlayers, kpLN);
    /* It populates tmp5 */

    /* Next use the EvapoTrans function */
    CanopyA=0.0;
    CanopyT=0.0;

    for(i=0;i<nlayers;i++)
    {
/* vmax depends on leaf nitrogen and this in turn depends on the layer */
	    leafN_lay = tmp5[--tp5];
	    if(lnfun == 0){
		    vmax1 = REAL(VMAX)[0];
	    }else{
		    vmax1 = leafN_lay * lnb1 + lnb0;
/* For now alpha is not affected by leaf nitrogen */
	    }

	    IDir = layIdir[--sp1];
	    Itot = layItotal[--sp3];
	    
	    rh = tmp4[--tp4];
	    WindS = tmp3[--tp3];

	    pLeafsun = layFsun[--sp4];
	    CanHeight = layHeight[--sp6];
	    Leafsun = LAIc * pLeafsun;
	    tmp5_ET = EvapoTrans(IDir,Itot,Temp,rh,WindS,LAIc,CanHeight,stomataws,
				 INTEGER(WS)[0],vmax1,alpha1,kparm1,theta,beta,Rd1,b01,b11,upperT,lowerT);
	    TempIdir = Temp + tmp5_ET.Deltat;
	    tmpc4 = c4photoC(IDir,TempIdir,rh,vmax1,alpha1,kparm1,theta,beta,Rd1,b01,b11,stomataws, Catm,INTEGER(WS)[0],upperT,lowerT);
	    AssIdir = tmpc4.Assim;
      GAssIdir=tmpc4.GrossAssim;

	    IDiff = layIdiff[--sp2];
	    pLeafshade = layFshade[--sp5];
	    Leafshade = LAIc * pLeafshade;
	    tmp6_ET = EvapoTrans(IDiff,Itot,Temp,rh,WindS,LAIc,CanHeight,
				 stomataws,INTEGER(WS)[0],vmax1,alpha1,kparm1,theta,beta,Rd1,b01,b11,upperT,lowerT);
	    TempIdiff = Temp + tmp6_ET.Deltat;
	    tmpc42 = c4photoC(IDiff,TempIdiff,rh,vmax1,alpha1,kparm1,theta,beta,Rd1,b01,b11,stomataws, Catm,INTEGER(WS)[0],upperT,lowerT);
	    AssIdiff = tmpc42.Assim;
      GAssIdiff = tmpc42.GrossAssim;

    /* Collect direct radiation assim and trans in a matrix */
	    REAL(mat1)[i*17] = IDir;
	    REAL(mat1)[1 + i*17] = IDiff;
	    REAL(mat1)[2 + i*17] = Leafsun;
	    REAL(mat1)[3 + i*17] = Leafshade;
	    REAL(mat1)[4 + i*17] = tmp5_ET.TransR;
	    REAL(mat1)[5 + i*17] = tmp6_ET.TransR;
	    REAL(mat1)[6 + i*17] = AssIdir;
	    /*REAL(mat1)[7 + i*17] = AssIdiff;*/
      REAL(mat1)[7 + i*17] = AssIdiff;
	    REAL(mat1)[8 + i*17] = tmp5_ET.Deltat;
	    REAL(mat1)[9 + i*17] = tmp6_ET.Deltat;
	    REAL(mat1)[10 + i*17] = tmp5_ET.LayerCond; 
	    REAL(mat1)[11 + i*17] = tmp6_ET.LayerCond; 
	    REAL(mat1)[12 + i*17] = leafN_lay; 
	    REAL(mat1)[13 + i*17] = vmax1;
	    REAL(mat1)[14 + i*17] = rh; 
      REAL(mat1)[15 + i*17] = GAssIdir;
      REAL(mat1)[16 + i*17] = GAssIdiff;
/*      REAL(mat1)[11 + i*12] = rh;  */
    /*Layer conductance needs to be transformed back to the correct units here*/

	    CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
	    CanopyT += Leafsun * tmp5_ET.TransR + Leafshade * tmp6_ET.TransR; 
	    CanopyPe += Leafsun * tmp5_ET.EPenman + Leafshade * tmp6_ET.EPenman;
	    CanopyPr += Leafsun * tmp5_ET.EPriestly + Leafshade * tmp6_ET.EPriestly;
	    CanopyC += Leafsun * tmp5_ET.LayerCond + Leafshade * tmp6_ET.LayerCond;
      GCanopyA += Leafsun * GAssIdir + Leafshade * GAssIdiff;
    }
 /*## These are micro mols of CO2 per m2 per sec
   ## Need to convert to 
   ## 3600 converts seconds to hours
   ## 10^-6 converts micro mols to mols
   ## 30 converts mols of CO2 to grams
   ## (1/10^6) converts grams to Mg
   ## 10000 scales up to ha */
    REAL(growth)[0] = cf * CanopyA ;
    REAL(trans)[0] = cf2 * CanopyT ;
    REAL(epen)[0] = cf2 * CanopyPe ;
    REAL(epries)[0] = cf2 * CanopyPr ;
    REAL(cond)[0] = CanopyC;
    REAL(Ggrowth)[0] = cf * GCanopyA ;

    SET_VECTOR_ELT(lists,0,growth);
    SET_VECTOR_ELT(lists,1,trans);
    SET_VECTOR_ELT(lists,2,cond);
    SET_VECTOR_ELT(lists,3,epen);
    SET_VECTOR_ELT(lists,4,epries);
    SET_VECTOR_ELT(lists,5,Ggrowth);
    SET_VECTOR_ELT(lists,6,mat1);

    SET_STRING_ELT(names,0,mkChar("CanopyAssim"));
    SET_STRING_ELT(names,1,mkChar("CanopyTrans"));
    SET_STRING_ELT(names,2,mkChar("CanopyCond"));
    SET_STRING_ELT(names,3,mkChar("TranEpen"));
    SET_STRING_ELT(names,4,mkChar("TranEpries"));
    SET_STRING_ELT(names,5,mkChar("GrossCanopyAssim"));
    SET_STRING_ELT(names,6,mkChar("LayMat"));
    setAttrib(lists,R_NamesSymbol,names);

    UNPROTECT(9);
    return(lists);
   }
