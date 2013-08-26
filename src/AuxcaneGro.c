/*
 *  /src/AuxBioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007 - 2012
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
#include "AuxBioCro.h"
#include "AuxcaneGro.h"
#include "c4photo.h"

struct soilML_str soilML_rootfront(double precipit, double transp, double *cws, double soildepth, double *depths, double fieldc, double wiltp, double phi1, double phi2, struct soilText_str soTexS, int wsFun, int layers, double rootDB, double LAI, double k, double AirTemp, double IRad, double winds, double RelH, int hydrDist, double rfl, double rsec, double rsdf,int optiontocalculaterootdepth, double rootfrontvelocity ,double dap){

	struct rd_str tmp4;
	struct seqRD_str tmp3;
	struct soilML_str tmp;
        /* Constant */
	/* const double G = 6.67428e-11;  m3 / (kg * s-2)  ##  http://en.wikipedia.org/wiki/Gravitational_constant */
	const double g = 9.8; /* m / s-2  ##  http://en.wikipedia.org/wiki/Standard_gravity */
	/* Variables */
	double waterIn, oldWaterIn = 0.0;
/* Here is a convention aw is available water in volume and awc
   is available water content as a fraction of the soil section being investigated.
   paw is plant available water aw - wiltp */
	double aw, paw, awc, awc2, Newpawha;
	double drainage = 0.0;
	double wsPhoto = 0.0, wsSpleaf = 0.0, phi10;
	double wsPhotoCol = 0.0, wsSpleafCol = 0.0;
	double slp = 0.0, intcpt = 0.0, theta = 0.0; 
	double Nleach = 0.0;
	double layerDepth;
	double diffw;
	double rootATdepth, rootDepth;
	double EvapoTra = 0.0, oldEvapoTra = 0.0, Sevap = 0.0, Ctransp = 0.0;
	double psim1 = 0.0, psim2 = 0.0, K_psim = 0.0, J_w = 0.0, dPsim = 0.0;
	double theta_s; /* This is the saturated soil water content. Larger than FieldC.*/
	int i;
	int j = layers - 1; 

	/* Specify the soil type */

	if(fieldc < 0){
		fieldc = soTexS.fieldc;
	}
	if(wiltp < 0){
		wiltp = soTexS.wiltp;
	}

	theta_s = soTexS.satur;
	/* rooting depth */
	/* Crude empirical relationship between root biomass and rooting depth*/
	           if (optiontocalculaterootdepth==1) {
		                            rootDepth = rootDB * rsdf;
	                                   }

		                      else {
					      rootDepth = rootfrontvelocity*dap*0.01;// to convert root depth from cm to meter
				           }

	if(rootDepth > soildepth) rootDepth = soildepth;

	tmp3 = seqRootDepth(rootDepth,layers);
	tmp4 = rootDist(layers,rootDepth,&depths[0],rfl);

	/* unit conversion for precip */
	waterIn = precipit * 1e-3; /* convert precip in mm to m*/

	for(j=0,i=layers-1;j<layers;j++,i--){
	/* for(i=0;i<layers;i++){ */
		/* It decreases because I increase the water content due to precipitation in the last layer first*/

		/* This supports unequal depths. */
		if(i == 0){
			layerDepth = depths[1];
		}else{
			layerDepth = depths[i] - depths[i-1];
		}


		if(hydrDist > 0){
			/* For this section see Campbell and Norman "Environmental BioPhysics" Chapter 9*/
			/* First compute the matric potential */
			psim1 = soTexS.air_entry * pow((cws[i]/theta_s),-soTexS.b) ; /* This is matric potential of current layer */
			if(i > 0){
				psim2 = soTexS.air_entry * pow((cws[i-1]/theta_s),-soTexS.b) ; /* This is matric potential of next layer */
				dPsim = psim1 - psim2;
				/* The substraction is from the layer i - (i-1). If this last term is positive then it will move upwards. If it is negative it will move downwards. Presumably this term is almost always positive. */
			}else{
				dPsim = 0;
			}
			K_psim = soTexS.Ks * pow((soTexS.air_entry/psim1),2+3/soTexS.b); /* This is hydraulic conductivity */
			J_w = K_psim * (dPsim/layerDepth) - g * K_psim ; /*  Campbell, pg 129 do not ignore the graviational effect*/
                        /* Notice that K_psim is positive because my
                            reference system is reversed */
			/* This last result should be in kg/(m2 * s)*/
			 J_w *= 3600 * 0.9882 * 1e-3 ; /* This is flow in m3 / (m^2 * hr). */
			/* Rprintf("J_w %.10f \n",J_w);  */
			if(i == (layers-1) && J_w < 0){
					/* cws[i] = cws[i] + J_w /
					 * layerDepth; Although this
					 * should be done it drains
					 * the last layer too much.*/
					drainage += J_w;
			}else{
				if(i > 0){
					cws[i] = cws[i] -  J_w / layerDepth;
					cws[i - 1] =  cws[i-1] +  J_w / layerDepth;
				}else{
					cws[i] = cws[i] -  J_w / layerDepth;
				}
			}
		}

		 if(cws[i] > fieldc) cws[i] = fieldc; 
		/* if(cws[i+1] > fieldc) cws[i+1] = fieldc; */
		 if(cws[i] < wiltp) cws[i] = wiltp; 
		/* if(cws[i+1] < wiltp) cws[i+1] = wiltp;  */

		aw = cws[i] * layerDepth;
/* Available water (for this layer) is the current water status times the layer depth */

		if(waterIn > 0){
			/* There is some rain. Need to add it.*/
			aw += waterIn / layers + oldWaterIn; /* They are both in meters so it works */
                        /* Adding the same amount to water to each layer */
                        /* In case there is overflow */
			diffw = fieldc * layerDepth - aw;

			if(diffw < 0){
				/* This means that precipitation exceeded the capacity of the first layer */
				/* Save this amount of water for the next layer */
				oldWaterIn = -diffw;
				aw = fieldc * layerDepth;
			}else{
				oldWaterIn = 0.0;
			}
		}

		/* Root Biomass */
		rootATdepth = rootDB * tmp4.rootDist[i];
		tmp.rootDist[i] = rootATdepth;
/* Plant available water is only between current water status and permanent wilting point */
		/* Plant available water */
		paw = aw - wiltp * layerDepth;
		if(paw < 0) paw = 0; 

		if(i == 0){
			/* Only the first layer is affected by soil evaporation */
			awc2 = aw / layerDepth;
			/* SoilEvapo function needs soil water content  */
			Sevap = SoilEvapo(LAI,k,AirTemp,IRad,awc2,fieldc,wiltp,winds,RelH,rsec);
			/* I assume that crop transpiration is distributed simlarly to
			   root density.  In other words the crop takes up water proportionally
			   to the amount of root in each respective layer.*/
			Ctransp = transp*tmp4.rootDist[0];
			EvapoTra = Ctransp + Sevap;
			Newpawha = (paw * 1e4) - EvapoTra / 0.9982; /* See the watstr function for this last number 0.9882 */
			/* The first term in the rhs (paw * 1e4) is the m3 of water available in this layer.
			   EvapoTra is the Mg H2O ha-1 of transpired and evaporated water. 1/0.9882 converts from Mg to m3 */
		}else{
			Ctransp = transp*tmp4.rootDist[i];
			EvapoTra = Ctransp;
			Newpawha = (paw * 1e4) - (EvapoTra + oldEvapoTra);
		}

		if(Newpawha < 0){
/* If the Demand is not satisfied by this layer. This will be stored and added to subsequent layers*/
			oldEvapoTra = -Newpawha;
			 aw = wiltp * layerDepth; 
		}

		paw = Newpawha / 1e4 ;
		awc = paw / layerDepth + wiltp;   

/* This might look like a weird place to populate the structure, but is more convenient*/
		tmp.cws[i] = awc;

		if(wsFun == 0){
			slp = 1/(fieldc - wiltp);
			intcpt = 1 - fieldc * slp;
			wsPhoto = slp * awc + intcpt ;
		}else
		if(wsFun == 1){
			phi10 = (fieldc + wiltp)/2;
			wsPhoto = 1/(1 + exp((phi10 - awc)/ phi1));
		}else
		if(wsFun == 2){
			slp = (1 - wiltp)/(fieldc - wiltp);
			intcpt = 1 - fieldc * slp;
			theta = slp * awc + intcpt ;
			wsPhoto = (1 - exp(-2.5 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-2.5));
		}else
		if(wsFun == 3){
			wsPhoto = 1;
		}

		if(wsPhoto <= 0 )
			wsPhoto = 1e-20; /* This can be mathematically lower than zero in some cases but I should prevent that. */

		wsPhotoCol += wsPhoto;

		wsSpleaf = pow(awc,phi2)/pow(fieldc,phi2); 
		if(wsFun == 3){ 
			wsSpleaf = 1;
		}
		wsSpleafCol += wsSpleaf;
	}

	if(waterIn > 0){ 
		drainage = waterIn;
		/* Need to convert to units used in the Parton et al 1988 paper. */
		/* The data comes in mm/hr and it needs to be in cm/month */
		Nleach = drainage * 0.1 * (1/24*30) / (18 * (0.2 + 0.7 * soTexS.sand));
	}


/* returning the structure */
	tmp.rcoefPhoto = (wsPhotoCol/layers);
	tmp.drainage = drainage;
	tmp.Nleach = Nleach;
	tmp.rcoefSpleaf = (wsSpleafCol/layers);
	tmp.SoilEvapo = Sevap;

	return(tmp);
}


struct dbp_sugarcane_str SUGARCANE_DBP_CUADRA(double TT, double TT0,double TTseed,double Tmaturity, double Rd, double Alm, double Arm, double Clstem, double Ilstem, double Cestem, double Iestem, double Clsuc, double Ilsuc, double Cesuc, double Iesuc,double Temperature)
{
	struct dbp_sugarcane_str dbp;
	double F1, F2, F3, F4;
	double Aa,Astem,Al,Ar,Asuc,Astuc,Afib;
	double RM;
	double cutemp1,cutemp2,RM0;
        
/* Germination Phase */
        
	if(TT < TT0)
	{
		
		dbp.kLeaf=0.001;
		dbp.kStem=0.7;
		dbp.kRoot=0.299;
		dbp.kSeedcane=(-1)*0.693*(0.0053*Temperature-0.0893);
		dbp.kSugar=0.0;
		dbp.kFiber=0.0;
		return(dbp);		
	}
        else
	

	dbp.kSeedcane=0;
        /* Calculate Relative Maturity and correct it for germination phase* */
        /* Now reference thermal time has changed from 0 to TT0 */
	RM=(TT-TT0)*100.0/Tmaturity;
/*	RM0=TT0*100.0/Tmaturity;
	RM=RM-RM0;   */

	/* Criteria for linear increase in stem biomass*/
	F1=RM*Clstem-Ilstem*Clstem;

	/*Criteria for log increase in stem biomass */
        F2=1-exp((RM*Cestem)-(Iestem*Cestem));

	/* Fraction of aerial biomass */
	cutemp1=(1-exp((-1)*Rd*RM));
	Aa=(1.0-Arm)*((cutemp1<1.0) ? cutemp1 :1.0);

	/*Fraction of stem biomass */
	cutemp1=(1.0-Alm-Arm);
	if ((F1<=0)&&(F2<=0))
	{
		cutemp2=0.0;
	}
	else
	{
		cutemp2=Aa*((F1>F2)?F1:F2);
	}
	Astem = (cutemp2<cutemp1)?cutemp2:cutemp1;

	/* Fraction of leaf biomass */
	Al=Aa-Astem;
	/* Fraction of root biomass */
        Ar=1-Aa;

        /* Partiitoning of stem between structural and sugar component */

	/* Criteria for linear increase */
	F3=RM*Clsuc-Ilsuc*Clsuc;

	/*Criteria for log increase */
        F4=1-exp((RM*Cesuc)-(Iesuc*Cesuc));
  
       /*fraction of sucrose with respective to total stem allocation */ 
       	cutemp2=0;
	if ((F3<=0)&&(F4<=0))
	{
		cutemp2=0;
	}
	else
	{
		cutemp2=Astem*((F3>F4)?F3:F4);
	}
	Asuc=cutemp2;
	Afib=Astem-Asuc;

	/*fraction of structural component of stem */
	Astuc=Astem-Asuc;

		dbp.kLeaf=Al;
		dbp.kStem=Astem;
		dbp.kRoot=Ar;
		dbp.kSugar=Asuc;
		dbp.kFiber=Astuc;
//	        dbp.kSeedcane=-0.00005;	
	return(dbp);
}




double GrowthRespiration(double CanopyA, double fraction) 
{
	CanopyA=CanopyA*(1.0-fraction);
	return(CanopyA);
}



double MRespiration (double W,double Q, double m,double T, double deltime)
{
	double result,ttemp;
	/* change m from per day basis to per hr basis for deltime=1 hr */
	m=m*deltime/24;  
        ttemp=(T-25.0)/10.0;
        result=W*m*pow(Q,ttemp);
	return(result);
		}


double seasonal (double maxLN, double minLN, double day, double daymaxLN, double dayinyear, double lat)

{
	
	double temp,leafN;  // temporary variable to calculate cyclic variation
              
         temp = sin (2*3.1415*((day-(daymaxLN))/(dayinyear)+0.25));
         leafN=(maxLN+minLN)*0.5+(maxLN-minLN)*temp*0.5;  

        return (leafN);
                 
 } 

