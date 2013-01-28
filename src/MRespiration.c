# include <R.h>
# include <Rmath.h>
/**************************************************8  
The function MRespiration calculates Maintenance Respiration 
 Arguments to the functions are
 W= current biomass in Mg/ha of a plant component
 Q= Q factor to simulate effect of temperature
 m = respiration coefficients in day-1
 Temp = Temperature in degree C
 deltime<-Time interval in hrs ( 1 hr, 2 hr, 24 hr)

Return:
 maintenance respiration in deltaT time
***********************************************/
double MRespiration (double W,double Q, double m,double T, double deltime)
{
	double result,ttemp;
	/* change m from per day basis to per hr basis for deltime=1 hr */
	m=m*deltime/24;  
        ttemp=(T-25.0)/10.0;
        result=W*m*pow(Q,ttemp);
	return(result);
		}
