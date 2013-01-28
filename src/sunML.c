
#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

/* sunML function. Sun-Shade Multi-Layer model */
void sunML(double Idir, double Idiff, double LAI, int nlayers, 
	   double cosTheta, double kd, double chil, double heightf)
{
	extern int sp1, sp2, sp3, sp4, sp5, sp6;
	extern double layIdir[], layIdiff[], layItotal[], layFsun[], layFshade[], layHeight[];
	int i;
	double k0, k1, k;
	double LAIi, CumLAI;
	double Isolar, Idiffuse, Itotal,Iscat,alphascatter;
	double Ls,Ls2, Ld, Ld2;
	double Fsun, Fshade,Fcanopy;
	double Lssaved,Ldsaved;

	/*extinction coefficient evaluated here is defnied as ratio of 
	  shadow on the horizontal area projected from a zenith angle Theta.
	  Earlier version of the code was based on ratio of area projected in
	  the direction of incoming beam radiation.
	  Therefore, cosTheta term in numerator is removed
	*/
	/** wishlist**/
	/* currently a default value of kd is used, this value can be calculated, see page 254 of Campbell and Norman */
	/* Long Wave radiation balanc has not been included in the canopy environment */

	alphascatter=0.8; /* This value is for PAR, for long wave radiation a shorter value = 0.2 is recommended see page  255 of campbell and Norman */
	k0 =  sqrt(pow(chil ,2) + pow(tan(acos(cosTheta)),2));
	k1 = chil + 1.744*pow((chil+1.183),-0.733);
	k = k0/k1;
	if(k<0)
		k = -k;

	LAIi = LAI / nlayers;

	for(i=0;i<nlayers;i++)
	{
		CumLAI = LAIi * (i+1);
		if(i == 0)
		      {
		
			Iscat=Idir * exp(-k *sqrt(alphascatter)* CumLAI)-Idir * exp(-k * CumLAI); /* scattered radiation Eq 15.19 from Campbell and Norman pp 259 */
			Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat; /* Eq 15.19 from Campbell and Norman pp 259 */
			Isolar = Idir * k; /* Avergae direct radiation on sunlit leaves */
			Itotal = Isolar + Idiffuse; /* Total radiation */
	                
			Fcanopy=CumLAI; /* leaf area of canopy currenly under consideration */
			Ls = (1-exp(-k*CumLAI))/k; /* To evaluate sunlit leaf area in Fcanopy*/
			Lssaved=Ls;              /* To use for evaluating sunlit leaves in the lower layer */
			Ld=Fcanopy-Ls;          /* shaded leaf area */
			Ldsaved=Ld;              /* To use for evaluating shaded leaves in the lower layer*/
			Fsun=Ls/(Ls+Ld);         /*fracction of sunlit leaves in the current layer */
			Fshade=Ld/(Ls+Ld);       /* fraction of shaded leaves in the current layer */
			}
		
		else
		{
			Iscat=Idir * exp(-k *sqrt(alphascatter)* CumLAI)-Idir * exp(-k * CumLAI); /* scattered radiation Eq 15.19 from Campbell and Norman pp 259 */
			Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat; /* Eq 15.19 from Campbell and Norman pp 259 */
			Isolar = Idir * k; /* Avergae direct radiation on sunlit leaves */
			Itotal = Isolar + Idiffuse;
	                
			Fcanopy=CumLAI;  /* leaf area of canopy currenly under consideration */
			Ls = (1-exp(-k*CumLAI))/k; /* To evaluate  sunlit leaf area  in Fcanopy*/
			Ld=Fcanopy-Ls; /* evaluate shaded leaf area in Fcanopy */
			Ls= Ls-Lssaved; /* subtract the sunlit areas of all the upper layers saved in Lssaved; now we have sunlit area of current layer*/
			Lssaved = Ls+ Lssaved; /* update the Lssaved for calculation of the next lower layer */
			Ld=Ld-Ldsaved; /* subtract the shaded areas of all the upper layers saved in Ldsaved; now we have shaded area of current layer*/
			Ldsaved=Ld+Ldsaved; /* update the Ldsaved for calculation of the next lower layer */
			Fsun=Ls/(Ls+Ld);   /*fracction of sunlit leaves in the current layer */
			Fshade=Ld/(Ls+Ld);  /* fraction of shaded leaves in the current layer */
		}
		
                      /* collecting the results */

		layIdir[sp1++] = Isolar+Idiffuse;  /* This is used to calculate sunlit leaf photosynthesis */
		layIdiff[sp2++] = Idiffuse;       /* ok, this is used to calculate shaded leaf photosynthesis */
		layItotal[sp3++] = Itotal;        /* this is used to evaluate evapotranspiration..Think about other factors that might influence energy balance to include */
		layFsun[sp4++] = Fsun;
		layFshade[sp5++] = Fshade;
		layHeight[sp6++] = CumLAI / heightf;
	}



}
