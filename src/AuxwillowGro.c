/*
 *  /src/AuxBioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007 - 2012
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 or 3 of the License
 *  (at your option).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  http://www.r-project.org/Licenses/
 *
 *  Part of the code here (sunML, EvapoTrans, SoilEvapo, TempTo and
 *  the *prof functions) are based on code in WIMOVAC. WIMOVAC is
 *  copyright of Stephen Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 */


/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c3photo.h"
#include "AuxwillowGro.h"
#include "AuxBioCro.h"

struct Can_Str c3CanAC(double LAI,int DOY, int hr,double solarR,double Temp,
	             double RH,double WindSpeed,double lat,int nlayers, double Vmax,double Jmax,
		     double Rd, double Catm, double o2, double b0, double b1,
                     double theta, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun)

         
{

struct ET_Str tmp5_ET , tmp6_ET; 
   struct c3_str tmpc3={0,0,0}, tmpc32={0,0,0}; 
   struct Can_Str ans;

  const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
/* Need a different conversion factor for transpiration */
  const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
	
  int i;
  double Idir, Idiff, cosTh;
  double LAIc;
  double IDir, IDiff, Itot,rh, WindS;
  double TempIdir,TempIdiff,AssIdir,AssIdiff;
  double  pLeafsun, pLeafshade;
  double Leafsun, Leafshade;

  double CanopyA ;
  double CanopyT , CanopyPe = 0.0, CanopyPr = 0.0;
  double CanopyC = 0.0;
  double CanHeight;

  double vmax1;
  double leafN_lay;


	/* For Assimilation */
	/* 3600 converts seconds to hours */
	/* 1e-6 converts micro mols to mols */
	/* 30 is the grams in one mol of CO2 */
	/* 1e-6 converts g to Mg */
	/* 10000 scales from meter squared to hectare */

	/* For Transpiration */
	/* 3600 converts seconds to hours */
	/* 1e-3 converts mili mols to mols */
	/* 18 is the grams in one mol of H20 */
	/* 1e-6 converts g to Mg */
	/* 10000 scales from meter squared to hectare */

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

    RHprof(RH,nlayers);
    /* It populates tmp4. */

     WINDprof(WindSpeed,LAI,nlayers);
    /* It populates tmp3. */

     LNprof(leafN, LAI, nlayers, kpLN);
    /* It populates tmp5 */

    /* Next use the EvapoTrans function */
    CanopyA=0.0;
    CanopyT=0.0;

    for(i=0;i<nlayers;i++)
    {
/* vmax depends on leaf nitrogen and this in turn depends on the layer */
      leafN_lay = tmp5[--tp5];
	    if(lnfun == 0){
		    vmax1 = Vmax;
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
/* Need a new evapo transpiration function specifically for c3*/
	    tmp5_ET = c3EvapoTrans(IDir,Itot,Temp,rh,WindS,LAIc,CanHeight,
				 Vmax,Jmax,Rd,b0,b1,Catm,o2,theta);
	    TempIdir = Temp + tmp5_ET.Deltat;
	    tmpc3 = c3photoC(IDir,TempIdir,rh,Vmax,Jmax,Rd,b0,b1,Catm,o2,theta);
	    AssIdir = tmpc3.Assim;

	    IDiff = layIdiff[--sp2];
	    pLeafshade = layFshade[--sp5];
	    Leafshade = LAIc * pLeafshade;
	    tmp6_ET = c3EvapoTrans(IDiff,Itot,Temp,rh,WindS,LAIc,CanHeight,
  			 Vmax,Jmax,Rd,b0,b1,Catm,o2,theta);
	    TempIdiff = Temp + tmp6_ET.Deltat;
	    tmpc32 = c3photoC(IDiff,TempIdir,rh,Vmax,Jmax,Rd,b0,b1,Catm,o2,theta);
	    AssIdiff = tmpc32.Assim;

		CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
		CanopyT += Leafsun * tmp5_ET.TransR + Leafshade * tmp6_ET.TransR;
	}
	/*## These are micro mols of CO2 per m2 per sec for Assimilation
	  ## and mili mols of H2O per m2 per sec for Transpiration
	  ## Need to convert to 
	  ## 3600 converts seconds to hours
	  ## 10^-6 converts micro mols to mols
	  ## 30 converts mols of CO2 to grams
	  ## (1/10^6) converts grams to Mg
	  ## 10000 scales up to ha */
/* A similar conversion is made for water but
   replacing 30 by 18 and mili mols are converted to
   mols (instead of micro) */
	ans.Assim = cf * CanopyA ;
	ans.Trans = cf2 * CanopyT; 
	return(ans);
}
