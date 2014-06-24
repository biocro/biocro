#ifndef __CONSTANTS__
#define __CONSTANTS__

#include <stdlib.h>
using namespace std;

const double 	PI 			= 3.1415926535897932384;
const double    PI_div_2    = 1.5707963267948922192;
const double    PI_mul_4_div_9    = 1.39626340159546;
const double    mul_8_div_9 = 0.8888888888888888889;
const double 	TWO_PI 		= 6.2831853071795864769;
const double 	PI_ON_180 	= 0.0174532925199432957;
const double 	invPI 		= 0.3183098861837906715;
const double 	invTWO_PI 	= 0.1591549430918953358;

const double 	kEpsilon 	= 0.0001; 
const double	kHugeValue	= 1.0E10;


const float 	invRAND_MAX = 1.0 / (float)RAND_MAX;

//solar and climate
const float     kSOLAR_constant = 2600;


const double    ignor_PPFD_threashold = 1; //umol.m-2.s-1


//for reflect light
const double    BRDF_s  = 8.67e-01;
const double    BRDF_Rd = 5.61e-02;
const double    BRDF_m  = 1.46e-01;
const double    BRDF_F0 = 6.15e-03;
const double    BRDF_ad = 0.001;

//leaf reflectance and transmittance
// reflectance and transmittance are in triangle


#endif
