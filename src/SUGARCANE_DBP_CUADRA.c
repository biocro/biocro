#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

/* After modifying for the germination phase, This function implements
biomass partitioning procedure of Cuadra.
TT (C day)= Thermal time for which partitioning coefficients are desired
TT0 (C day)= Thermal time until the end of germination phase.(200)
TTseed + time when Seed material stops providing to plant growth
Tmaturity (C day) = Thermal time when crops reaches maturity (6000)
Rd = decline coefficients for root fraction (0.1)
Alm = minimum fraction to leaf (0.1)
Arm= minimum fraction to root (0.2)
Clstem = parameter to determine slope of linear phase of stem fraction (0.012)
Ilstem = parameters to determine when linear phase of stem fraction starts(10)
Cestem = parameter to determine log phase of stem fraction (-0.05)
Iestem = parameter to determine log phase of stem fraction (20)
Clsuc = parameter to determine linear phase of sugar fraction (0.01)
Ilsuc= parameter to determine linear phase of sugar fraction (25)
Cesuc = parameter to determine log phase of sugar fraction (-0.02)
Iesuc = parameter to determine log phase of sugar fraction (45)

*/

struct dbp_str SUGARCANE_DBP_CUADRA(double TT, double TT0,double TTseed,double Tmaturity, double Rd, double Alm, double Arm, double Clstem, double Ilstem, double Cestem, double Iestem, double Clsuc, double Ilsuc, double Cesuc, double Iesuc)
{
	struct dbp_str dbp;
	double F1, F2, F3, F4;
	double Aa,Astem,Al,Ar,Asuc,Astuc;
	double RM;
	double cutemp1,cutemp2,RM0;
        
/* Germination Phase */
	if(TT < TT0)
	{
		dbp.kLeaf=0;
		dbp.kStem=0;
		dbp.kRoot=1.0;
		dbp.kRhiz=-1.46e-05;
		dbp.kGrain=0.0;
		return(dbp);
	}
	
	else
	{

	/* Calculate Relative Maturity and correct it for germination phase*/
	RM=TT*100.0/Tmaturity;
	RM0=TT0*100.0/Tmaturity;
	RM=RM-RM0;

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
	Astem = (cutemp1<cutemp2)?cutemp1:cutemp2;

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
       	if ((F3<=0)&&(F4<=0))
	{
		cutemp2=0;
	}
	else
	{
		cutemp2=Aa*((F1>F2)?F1:F2);
	}
	Asuc=Astem*cutemp2;

	/*fraction of structural component of stem */
	Astuc=Astem-Asuc;

		dbp.kLeaf=Al;
		dbp.kStem=Astem;
		dbp.kRoot=Ar;
		if (TT< TTseed){
			dbp.kRhiz=-1.7e-05;
		}
		else{
			dbp.kRhiz=0.0;
		}
			dbp.kGrain=0.0;
	

	return(dbp);
	}
}


