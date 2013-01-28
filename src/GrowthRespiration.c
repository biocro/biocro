#include <R.h>
#include <Rmath.h>

double GrowthRespiration(double CanopyA, double fraction) 
{
	double RespLost;
	RespLost=CanopyA*fraction;
	CanopyA=CanopyA-RespLost;
	return(CanopyA);
}
