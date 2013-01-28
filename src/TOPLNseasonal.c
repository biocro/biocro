# include <R.h>
# include <Rmath.h>
# include <math.h>


double TOPLNseasonal (double maxLN, double minLN, int day, int daymaxLN, int dayinyear, double lat)

{
	
	double a,b,temp,leafN;  // temporary variable to calculate cyclic variation
               
	a = (double)(day-daymaxLN)/(double)(dayinyear);
		b=0.25;
	 if(lat >0) 
                  {
		  temp=sin(2*3.1415*(a-b));
	          }
	if(lat <0)
	          {
		  temp=sin(2*3.1415*(a+b));
 	           }
        leafN =(maxLN+minLN)*0.5+(maxLN-minLN)*temp*0.5; 
        return (leafN);
                 
 } 
