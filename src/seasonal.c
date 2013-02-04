# include <R.h>
# include <Rmath.h>
# include <math.h>


double seasonal (double maxLN, double minLN, double day, double daymaxLN, double dayinyear, double lat)

{
	
	double temp,leafN;  // temporary variable to calculate cyclic variation
              
         temp = sin (2*3.1415*((day-(daymaxLN))/(dayinyear)+0.25));
         leafN=(maxLN+minLN)*0.5+(maxLN-minLN)*temp*0.5;  

        return (leafN);
                 
 } 
