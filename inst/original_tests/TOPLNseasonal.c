// This subroutine calculates leaf N concentration [based on seasonal cyclic variation as shown in Fig 2 of Liu and Bull in Ecologicall Modelling 144 (2001) 181-211.

// input Arguments are
//maxLN: maximum leaf N concentration (gram/m2)
//minLN: minimum leaf N concentration (gram/m2)
// day: day for which we want to evaluate leaf N concentration
//daymaxLN: day when maximum concentration of leaf N is observed. For simplicity, we can use solstice (21 june for northern hemisphere, and 21 december for southerm hemisphere)
// dayinyear: Total numbers of day in year, either 365 or 366
// lat: latitude of location (positive for northern hemisphere and negative for southern hemisphere


// This function returns
// leafN: concentration of leaf N on day of year= day

#include <R.h>
void TOPLNseasonal (double *maxLN, double *minLN, int *day, int *daymaxLN, int *dayinyear, double *lat,int *n, double *leafN)

        {
	
		double temp;         // temporary variable to calculate cyclic variation
                int i;
                for (i=1;i<=*n;i++)
		{
			if( *lat >0) temp = sin (2*3.1415*((day[i]-(*daymaxLN))/(*dayinyear)-0.25));// evaluate cyclic variation for northern hemisphere for which latitude is positive
			if( *lat <=0) temp = sin (2*3.1415*((day[i]-(*daymaxLN))/(*dayinyear)+0.25));// evaluate cyclic variation in southerm hemisphere for which latitude is negative
		leafN[i]=(*maxLN+*minLN)*0.5+(*maxLN-*minLN)*temp; // calculate normalized leaf N concentration of third leaf
                }
        } 

