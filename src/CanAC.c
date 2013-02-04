#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c4photo.h"
#include "AuxBioCro.h"
#include "canopyPROF.h"

/* CanA function */
/* This file will contain the function which will consolidate
   the previous functions into one. The idea is to go from
   existing LAI and weather conditions to canopy assimilation
   and transpiration */

struct Can_Str CanAC(double LAI,int DOY, int hr,double solarR,double Temp,
	             double RH,double WindSpeed,double lat,int nlayers, double Vmax,
		     double Alpha, double Kparm, double theta, double beta,
		     double Rd, double Catm, double b0, double b1,
                     double StomataWS, int ws, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun,double upperT, double lowerT)
{

	struct ET_Str tmp5_ET, tmp6_ET;
	struct Can_Str ans;
	struct c4_str tmpc4;
	struct c4_str tmpc42;

	int i;
	double Idir, Idiff, cosTh;
	double LAIc;
	double IDir, IDiff, Itot, rh, WS;
	double pLeafsun, pLeafshade;
	double Leafsun, Leafshade;
	double CanHeight;

	double vmax1, leafN_lay;
	double TempIdir,TempIdiff,AssIdir,AssIdiff;

	double CanopyA, CanopyT;

	const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
	const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 

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
    
	sunML(Idir,Idiff,LAI,nlayers,cosTh, kd, chil, heightf);

	/* results from multilayer model */
	LAIc = LAI / nlayers;
	/* Next I need the RH and wind profile */
	RHprof(RH,nlayers);
	WINDprof(WindSpeed,LAI,nlayers);

	LNprof(leafN, LAI, nlayers, kpLN);
	/* It populates tmp5 */

	/* Next use the EvapoTrans function */
	CanopyA=0.0;
	CanopyT=0.0;
	for(i=0;i<nlayers;i++)
	{
		leafN_lay = tmp5[--tp5];
		
		//	if(lnfun == 0){
		//	vmax1 = Vmax;
		// }else{
		// 	vmax1 = leafN_lay * lnb1 + lnb0;
		// /* For now alpha is not affected by leaf nitrogen */
		// }
  
// This part is sugarcane specific
		vmax1=1.38*leafN_lay-38.5;
              	if(vmax1<0) vmax1=0.0;
                Alpha=0.000349*leafN_lay+0.0166;
		Rd=0.0216*leafN_lay+3.46;
// Sugarcane specific part ends here

		IDir = layIdir[--sp1];
		Itot = layItotal[--sp3];

		rh = tmp4[--tp4];
		WS = tmp3[--tp3];
		pLeafsun = layFsun[--sp4];
		CanHeight = layHeight[--sp6];
		Leafsun = LAIc * pLeafsun;
		tmp5_ET = EvapoTrans(IDir,Itot,Temp,rh,WS,LAIc,CanHeight,StomataWS,ws,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,upperT,lowerT);
		TempIdir = Temp + tmp5_ET.Deltat;
		tmpc4 = c4photoC(IDir,TempIdir,rh,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,StomataWS, Catm, ws,upperT,lowerT);
		AssIdir = tmpc4.Assim;


		IDiff = layIdiff[--sp2];
		pLeafshade = layFshade[--sp5];
		Leafshade = LAIc * pLeafshade;
		tmp6_ET = EvapoTrans(IDiff,Itot,Temp,rh,WS,LAIc,CanHeight,StomataWS,ws,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,upperT,lowerT);
		TempIdiff = Temp + tmp6_ET.Deltat;
		tmpc42 = c4photoC(IDiff,TempIdiff,rh,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,StomataWS, Catm, ws,upperT,lowerT);
		AssIdiff = tmpc42.Assim;

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

