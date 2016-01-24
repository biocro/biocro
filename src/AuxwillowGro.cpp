/*
 *  /src/AuxwillowGro.c by Dandan Wang and Deepak Jaiswal  Copyright (C) 2012 - 2015
 *
 */

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c3photo.h"
#include "BioCro.h"
#include "AuxwillowGro.h"
#include "c3canopy.h"
#include "c3EvapoTrans.h"

void createCanopy (struct canopy *canopy, int Nlayers, double LAItotal)
{
  /**********************************************
   * Purpose:
   * creare a NULL canopy structure
   * allocate memory based on number of layers
   * initialize all the components of canopy structure
   * ***********************************************/
  int i;
  canopy->Nlayers=Nlayers;
  canopy->Idirtop=0.0;
  canopy->Idifftop=0.0;
  canopy->CosZenithAngle =0.0;
  canopy->LAItotal= LAItotal;
  canopy->ENV =  (struct canopyEnv*)malloc((Nlayers+1)*sizeof(*canopy->ENV));
  canopy->Leaf = (struct canopyLeaf*)malloc((Nlayers+1)*sizeof(*canopy->Leaf));
  canopy->OUT = (struct canopyoutput*)malloc((Nlayers+1)*sizeof(*canopy->OUT));
  for (i =0; i<Nlayers; i++)
  {
      canopy->ENV[i].Idir=0.0;
      canopy->ENV[i].Idiff=0.0;
      canopy->ENV[i].Itotal=0.0;
      canopy->ENV[i].RH=0.0;
      canopy->ENV[i].windspeed=0.0;
      canopy->ENV[i].Catm=0.0;
      canopy->ENV[i].Temp=0.0;   
    
      canopy->Leaf[i].LAI=0.0;
      canopy->Leaf[i].LeafN=0.0;
      canopy->Leaf[i].pLeafsun=0.0;
      canopy->Leaf[i].pLeafshade=0.0;   
      canopy->Leaf[i].heightf=0.0;  

      canopy->OUT[i].sunlittemp=0.0;
      canopy->OUT[i].shadedtemp=0.0;
      canopy->OUT[i].TotalAnet=0.0;
      canopy->OUT[i].TotalAgross=0.0;
      canopy->OUT[i].TotalTrans=0.0;
      canopy->OUT[i].shadedAnet=0.0;
      canopy->OUT[i].shadedAgross=0.0;
      canopy->OUT[i].sunlitAnet=0.0;
      canopy->OUT[i].sunlitAgross=0.0;
      canopy->OUT[i].shadedTemp=0.0;
      canopy->OUT[i].sunlitTemp=0.0;
      canopy->OUT[i].shadedTranspiration=0.0;
      canopy->OUT[i].sunlitTranspiration=0.0;
  }
  return;
}

void discretizeCanopy(struct canopy *canopy)
{
  /******************************************************************************
   * Purpose:
   * This function discretize the canopy  by assigning total LAI into each canopy layer equally
   * In Future, this function can be changed to accomodate Leaf Area Density
   ******************************************************************************/
  int i;
  
  for (i=0;i<(canopy->Nlayers);i++)
  {
    canopy->Leaf[i].LAI=canopy->LAItotal/canopy->Nlayers;
  }
  return;
}

void getcanopylightme(struct canopy * canopy, double lat, double DOY, int td, double solarR)
{
  
  /***********************************************************************
   * Purpose: To calculate direct and diffused sunlight at the top of the canopy
   * Input
   * lat:- Latitude
   * DOY:- Day of Year
   * td= Time of day
   * solarR :- Incoming Solar Radiation
   * Output:
   * canopy structure is updated for the following variables
   * Idirtop :- direct light at the top of the canopy
   * Idifftop:- Diffused light at the top of the canopy
   * CosZenithAngle:- Zenith Angle, to be used in the subsequent calculations
   * Reference:
   * Humphries Thesis
   * page 51
   * ***********************************************************************/
        double omega, delta0, delta, deltaR;
        double tf, SSin, CCos, PPo;
        double CosZenithAngle, CosHour;
        double CosHourDeg;
        double Idir, Idiff, propIdir, propIdiff;
        const double DTR = M_PI/180;
        const double tsn = 12.0;
        const double alpha = 0.85;
        const double SolarConstant = 2650;
        const double atmP = 1e5;

        omega = lat * DTR;
        delta0 = 360.0 * ((DOY + 10)/365.0);
        delta = -23.5 * cos(delta0*DTR);
        deltaR = delta * DTR;

        tf = (15.0*(td - tsn))*DTR;
        SSin = sin(deltaR) * sin(omega);
        CCos = cos(deltaR) * cos(omega);

        CosZenithAngle = SSin + CCos * cos(tf);
        if(CosZenithAngle < pow(10,-10))
                CosZenithAngle = pow(10,-10);

        CosHour = -tan(omega) * tan(deltaR);
        CosHourDeg = (1/DTR)*CosHour;
        if(CosHourDeg < -57)
                CosHour = -0.994;

        PPo = 1e5 / atmP;
        Idir = SolarConstant * (pow(alpha,(PPo/CosZenithAngle)));
        Idiff = 0.3 * SolarConstant *(1 - pow(alpha,(PPo/CosZenithAngle))) * CosZenithAngle ;

        propIdir = Idir / (Idir + Idiff);
        propIdiff = Idiff / (Idir + Idiff);
        
        canopy->Idirtop=propIdir*solarR;
        canopy->Idifftop= propIdiff*solarR ;
        canopy->CosZenithAngle =CosZenithAngle;
        return;
}


void getCanopysunML(struct canopy *canopy,double kd, double chil, double heightf)
{
  /****************************************************************************************************
   * Purpose:
   * Calculate fraction of shaded and sunlit leaves  in each of the canopy layers
   * Also, Calculate Direct, diffused and Scattered Light for each of the canopy layer
   * 
   * Input
   * Direct Sun Light at the Top of the Canopy
   * Diffused Light at the TOp of the Canopy
   * LAI Total Leaf Area Index of the Canopy
   * Nlayers Number of Canopy Layers
   * cosTheta: Cosine of Zenith Angel
   * Chil: Factor determining leaf angle distribtion of the canopy
   * heightF: Height Factor of the Canopy
   * 
   * Output
   * Fraction of Sunlit Leaves in Each Layer
   * Fraction of Shaded leaves in Each layer
   * Amount of Light (Direct + Diffuses+Scattered) on the sunlit leaves of each layer
   * Amount of Light (Diffused+Scattered) on the shaded leaves of each layer
   * *****************************************************************************************/
        int i;
        double k0, k1, k;
        double LAIi, CumLAI;
        double Isolar, Idiffuse, Ibeam, Iscat, Itotal,alphascatter;
        double Ls, Ld;
        double Fsun, Fshade;
        alphascatter=0.8;
        k0 = sqrt(pow(chil ,2) + pow(tan(acos(canopy->CosZenithAngle)),2));
        k1 = chil + 1.744*pow((chil+1.183),-0.733);
        k = k0/k1;
        if(k<0)
                k = -k;

        LAIi = canopy->LAItotal / canopy->Nlayers;

        for(i=0;i<canopy->Nlayers;i++)
        {
                CumLAI = LAIi * (i+0.5);          
                Ibeam=canopy->Idirtop*canopy->CosZenithAngle;
                Iscat = Ibeam * exp(-k *sqrt(alphascatter)* CumLAI)-Ibeam * exp(-k * CumLAI);     
                Isolar = Ibeam*k;
                Idiffuse = canopy->Idifftop * exp(-kd * CumLAI) + Iscat;                
                Ls = (1-exp(-k*LAIi))*exp(-k*CumLAI)/k;
                Ld=LAIi-Ls;
                Fsun=Ls/(Ls+Ld);
                Fshade=Ld/(Ls+Ld);
                canopy->Leaf[i].pLeafsun=Fsun;
                canopy->Leaf[i].pLeafshade=Fshade;
                canopy->Leaf[i].heightf = CumLAI/heightf;
                /*fraction intercepted*/
                Itotal =(Fsun*Isolar + Idiffuse) * (1-exp(-k*LAIi))/k;
                /* collecting the results */
                canopy->ENV[i].Idir= Isolar+Idiffuse ;
                canopy->ENV[i].Idiff = Idiffuse;
                canopy->ENV[i].Itotal = Itotal;                
        }
        return;
}


void getcanopyWINDprofile(struct canopy *canopy, double WindSpeed)
{
        int i;
        double k=0.7;
        double CumLAI;
        
          CumLAI=0.0;
        for(i=0;i<(canopy->Nlayers);i++)
        {
               canopy->ENV[i].windspeed= WindSpeed * exp(-k * CumLAI);
               CumLAI+=canopy->Leaf[i].LAI;
        }
        return;
}

void getcanopyRHprof(struct canopy *canopy, double RH)
{
       int i;
        double kh, hsla, j;

        kh = 1 - RH;
       
        for(i=0;i<(canopy->Nlayers);i++)
        {
                j = i + 1;
                hsla = RH * exp(kh * (j/canopy->Nlayers));
//              /*hsla = RH * exp(-kh * (j/nlayers));  /*new simpler version from Joe Iverson*/
                if(hsla > 1) hsla = 0.99; 
                canopy->ENV[i].RH = hsla;
        }
        return;
}


void getcanopyLNprof(struct canopy *canopy, double LeafN,double kpLN)
{
  /*******************************************************************
   * This function calculates Leaf N distrivution of different canopy layers
   * Leaf N will subsequently be used in calculation of photosynthesis parameters
   * Input parameters
   * Leaf N: Leaf N at the top of the canopy
   * kpLN: Extinction coefficient for exponential decline in leaf N with LAI
   * ************************************************************************/
   
   
        int i;
        // double leafNla; unused
		double CumLAI;

        CumLAI=0.0;
        for(i=0;i<(canopy->Nlayers);i++)
        {
                canopy->Leaf[i].LeafN= LeafN * exp(-kpLN * CumLAI);
                CumLAI = CumLAI+canopy->Leaf[i].LAI;
        }
        return;
}


void freecanopy(struct canopy *canopy)
{
  free(canopy->ENV);
  free(canopy->Leaf);
  free(canopy->OUT);
  return;
}

struct Can_Str newc3CanAC(double LAI,int DOY, int hr,double solarR,double Temp,
	             double RH,double WindSpeed,double lat,int nlayers, double Vmax,double Jmax,
		     double Rd, double Catm, double o2, double b0, double b1,
                     double theta, double kd, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun,double StomWS,int ws)
         
{

   struct ET_Str deepaktmp5_ET,deepaktmp6_ET; 
   struct c3_str deepaktmpc3;
   struct c3_str deepaktmpc32;
   struct Can_Str ans;


  const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
/* Need a different conversion factor for transpiration */
  const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 
	
  int i;
  double TempIdir,TempIdiff;
  double Leafsun, Leafshade;

  double CanopyA = 0.0;
  double GCanopyA = 0.0;
  double CanopyT =0.0;;
 
  // double CanHeight=0.0;; unused
  double chil =0.0;

   // double vmax1; unused
//  double leafN_lay;
  
 // double StomWS;
 // int ws;

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

  struct canopy ccanopy;
  createCanopy (&ccanopy,nlayers, LAI);  
  discretizeCanopy(&ccanopy);
  
  getcanopylightme(&ccanopy,lat, DOY,hr,solarR);
  getCanopysunML(&ccanopy,kd,chil,heightf);
  getcanopyRHprof(&ccanopy, RH);
  getcanopyWINDprofile(&ccanopy,WindSpeed);
  getcanopyLNprof(&ccanopy,leafN,kpLN);

      for(i=0;i<(ccanopy.Nlayers);i++)
    {
      	    //if(lnfun == 0){
      		    // vmax1 = Vmax; set but not used
      	    //}
            //else{
      		    // vmax1 = ccanopy.Leaf[i].LeafN * lnb1 + lnb0; set but not used
      	    //} 
     
	    Leafsun = ccanopy.Leaf[i].LAI *ccanopy.Leaf[i].pLeafsun;
   
      deepaktmp5_ET= c3EvapoTrans(ccanopy.ENV[i].Idir,ccanopy.ENV[i].Itotal,Temp,ccanopy.ENV[i].RH,ccanopy.ENV[i].windspeed,ccanopy.Leaf[i].LAI,ccanopy.Leaf[i].heightf,
  			                           Vmax,Jmax,Rd,b0,b1,Catm,o2,theta, StomWS, ws);
	    TempIdir = Temp + deepaktmp5_ET.Deltat;
      ccanopy.OUT[i].sunlittemp=TempIdir;
      ccanopy.OUT[i].sunlitTranspiration=deepaktmp5_ET.TransR;
     
      deepaktmpc3 = c3photoC(ccanopy.ENV[i].Idir,ccanopy.OUT[i].sunlittemp,ccanopy.ENV[i].RH,Vmax,Jmax,Rd,b0,b1,Catm,o2,theta,StomWS,ws);
	    ccanopy.OUT[i].sunlitAnet=deepaktmpc3.Assim;
      ccanopy.OUT[i].sunlitAgross=deepaktmpc3.GrossAssim;
//      Rprintf("%f, %f,%f\n",deepaktmpc3.Assim,deepaktmpc3.GrossAssim,ccanopy.ENV[i].Idir);

	    Leafshade = ccanopy.Leaf[i].LAI *ccanopy.Leaf[i].pLeafshade;
      deepaktmp6_ET=c3EvapoTrans(ccanopy.ENV[i].Idiff,ccanopy.ENV[i].Itotal,Temp,ccanopy.ENV[i].RH,ccanopy.ENV[i].windspeed,ccanopy.Leaf[i].LAI,ccanopy.Leaf[i].heightf,
    		 Vmax,Jmax,Rd,b0,b1,Catm,o2,theta, StomWS, ws);
      TempIdiff=Temp + deepaktmp6_ET.Deltat;
      ccanopy.OUT[i].shadedtemp=TempIdiff;
      ccanopy.OUT[i].shadedTranspiration=deepaktmp6_ET.TransR;
      deepaktmpc32=c3photoC(ccanopy.ENV[i].Idiff,ccanopy.OUT[i].shadedtemp,ccanopy.ENV[i].RH,Vmax,Jmax,Rd,b0,b1,Catm,o2,theta,StomWS,ws);
	    ccanopy.OUT[i].shadedAnet=deepaktmpc32.Assim;
      ccanopy.OUT[i].shadedAgross=deepaktmpc32.GrossAssim;
      
     ccanopy.OUT[i].TotalAnet =Leafsun* ccanopy.OUT[i].sunlitAnet + Leafshade*ccanopy.OUT[i].shadedAnet;
     ccanopy.OUT[i].TotalAgross=Leafsun* ccanopy.OUT[i].sunlitAgross + Leafshade*ccanopy.OUT[i].shadedAgross;
     ccanopy.OUT[i].TotalTrans =Leafsun* ccanopy.OUT[i].sunlitTranspiration + Leafshade*ccanopy.OUT[i].shadedTranspiration;
    
   
		 CanopyA = CanopyA + ccanopy.OUT[i].TotalAnet;
		 GCanopyA =GCanopyA+ ccanopy.OUT[i].TotalAgross;
     CanopyT = GCanopyA+ ccanopy.OUT[i].TotalTrans;
//     Rprintf("Inside Layers: net= %f, Gross=%f \n",ccanopy.OUT[i].TotalAnet,ccanopy.OUT[i].TotalAgross);
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
  ans.Trans= cf2 * CanopyT; 
  ans.GrossAssim=cf*GCanopyA;
//   Rprintf("output;-Net Assimilation = %f, Gross Assimilation = %f \n", CanopyA,GCanopyA );
//  Rprintf("returning structure;-Net Assimilation = %f, Gross Assimilation = %f \n", ans.Assim, ans.GrossAssim);
	return(ans);
}

