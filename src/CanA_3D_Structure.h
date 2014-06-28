#ifndef CANOPY3D
#define CANOPY3D


#include "AuxBioCro.h"
#include "BioCro.h"

void update_3Dcanopy_structure(double **canopy3Dstructure,double canparms, int nrow, int ncol);
void getIdirIdiff(double *Idir, double *Idiff,double *cosTh,double solarR,double lat,int DOY, int hr);
struct Can_Str CanAC_3D (double canparms, double **canopy3Dstructure, int nrows, int ncols, double LAI,int DOY, int hr,double solarR,double Temp,
                        double RH,double WindSpeed,double lat,double Vmax,
                        double Alpha, double Kparm, double theta, double beta,
                        double Rd, double Catm, double b0, double b1,
                        double StomataWS, int ws,double kpLN, double upperT, 
                        double lowerT,struct nitroParms nitroP);
                        
                        
#endif