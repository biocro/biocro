
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
        double Ibeam;

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
		CumLAI = LAIi * (i+0.5);
		
		Ibeam=Idir*cosTheta;
		Iscat = Ibeam * exp(-k *sqrt(alphascatter)* CumLAI)-Ibeam * exp(-k * CumLAI);
		
		
		Isolar = Ibeam*k;
		Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat;
		
		
		Ls = (1-exp(-k*LAIi))*exp(-k*CumLAI)/k;
		Ld=LAIi-Ls;

		Fsun=Ls/(Ls+Ld);
		Fshade=Ld/(Ls+Ld);
		/*fraction intercepted*/
		Itotal =(Fsun*Isolar + Idiffuse) * (1-exp(-k*LAIi))/k;

		/* collecting the results */
		layIdir[sp1++] = Isolar + Idiffuse;
		layIdiff[sp2++] = Idiffuse;
		layItotal[sp3++] = Itotal;
		layFsun[sp4++] = Fsun;
		layFshade[sp5++] = Fshade;
		layHeight[sp6++] = CumLAI/heightf;
	}



}
