#ifndef BIOCRO_H
#define BIOCRO_H

#include "AuxBioCro.h"

struct BioGro_results_str {
	double day_of_year[8760];
	double hour[8760];
	double CanopyAssim[8760];
	double canopy_transpiration[8760];
	double Leafy[8760];
	double Stemy[8760];
	double Rooty[8760];
	double Rhizomey[8760];
	double Grainy[8760];
	double LAIc[8760];
	double thermal_time[8760];
	double soil_water_content[8760];
	double stomata_cond_coefs[8760];
	double leaf_reduction_coefs[8760];
	double leaf_nitrogen[8760];
	double above_ground_litter[8760];
	double below_ground_litter[8760];
	double vmax[8760];
	double alpha[8760];
	double specific_leaf_area[8760];
	double min_nitro[8760];
	double respiration[8760];
	double soil_evaporation[8760];
	double leaf_psim[8760];
	double *psim;
	double *water_status;
	double *root_distribution;
};

void initialize_biogro_results(struct BioGro_results_str *results, int soil_layers, size_t vector_size);
void free_biogro_results(struct BioGro_results_str *results);

struct Model_state {
	double leaf;
	double stem;
	double root;
	double rhizome;
	double lai;
	double grain;
	double k_leaf;
	double k_stem;
	double k_root;
	double k_rhizome;
	double k_grain;
	double new_leaf;
	double new_stem;
	double new_root;
	double new_rhizome;
	double new_grain;
	double thermal_time;
	int doy;
	int hour;
	double solar;
	double temp;
	double rh;
	double windspeed;
	double vmax;
	double alpha;
	double LeafN;
	double StomataWS;
};

struct Light_model {
	double direct_irradiance_fraction;
	double diffuse_irradiance_fraction;
	double cosine_zenith_angle;
};

void BioGro(double lat, int doy[], int hr[], double solar[], double temp[], double rh[],
        double windspeed[], double precip[], double kd, double chil, double leafwidth, int et_equation,
        double heightf, int nlayers, double initial_biomass[4],
        double sencoefs[], int timestep, size_t vecsize,
        double Sp, double SpD, double dbpcoefs[25], double thermalp[], double tbase, double vmax1, 
        double alpha1, double kparm, double theta, double beta, double Rd, double Catm, double b0, double b1, 
        double soilcoefs[], double ileafn, double kLN, double vmaxb1,
        double alphab1, double mresp[], int soilType, int wsFun, int water_stress_approach, double centcoefs[],
        int centTimestep, double centks[], int soilLayers, double soilDepths[],
        double cws[], int hydrDist, double secs[], double kpLN, double lnb0, double lnb1, int lnfun , double upperT, double lowerT, const struct nitroParms &nitroP, double StomataWS,
		double (*leaf_n_limitation)(double kLn, double leaf_n_0, const struct Model_state &current_state), struct BioGro_results_str *results);

struct Can_Str CanAC(double LAI, int DOY, int hr, double solarR, double Temp,
		     double RH, double WindSpeed, double lat, int nlayers, double Vmax, double Alpha, 
		     double Kparm, double beta, double Rd, double Catm, double b0, 
		     double b1, double theta, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun, double upperT,
		     double lowerT, const struct nitroParms &nitroP, double leafwidth, int eteq, double StomataWS, int water_stress_approach);
         
struct Can_Str c3CanAC(double LAI, int DOY, int hr, double solarR, double Temp,
                       double RH, double WindSpeed, double lat, int nlayers, double Vmax, double Jmax,
  	                   double Rd, double Catm, double o2, double b0, double b1,
                       double theta, double kd, double heightf,
		                    double leafN, double kpLN, double lnb0, double lnb1, int lnfun, double StomataWS, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation);
                        

struct dbp_str sel_dbp_coef(double coefs[25], double TherPrds[6], double TherTime);

double resp(double comp, double mrc, double temp);

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, 
                     double fieldc, double wiltp, double phi1, double phi2, int soiltype, int wsFun);


double SoilEvapo(double LAI, double k, double AirTemp, double DirectRad,
		 double awc, double fieldc, double wiltp, double winds, double RelH, double rsec);

struct soilML_str soilML(double precipit, double transp, double *cws, double soildepth,
			 double *depths, double fieldc, double wiltp, double phi1, double phi2,
                         const struct soilText_str &soTexS, int wsFun, int layers, double rootDB,
			 double LAI, double k, double AirTemp, double IRad, double winds, double RelH,
			 int hydrDist, double rfl, double rsec, double rsdf);

double TempToSWVC(double Temp);
double TempToSFS(double Temp);
double TempToLHV(double Temp);
double TempToDdryA(double Temp);
void RHprof(double RH, int nlayers, double* relative_humidity_profile);
void WINDprof(double WindSpeed, double LAI, int nlayers, double* wind_speed_profile);
struct Light_profile sunML(double Idir, double Idiff, double LAI, int nlayers, double cosTheta, double kd, double chil, double heightf);
struct Light_model lightME(double lat, int DOY, int td);

struct FL_str FmLcFun(double Lig, double Nit);

struct flow_str flow(double *SC, double CNratio, double A, double Lc, double Tm, double resp, int kno, double Ks[8]);
double AbiotEff(double smoist, double stemp);

struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH,
                         double WindSpeed, double LeafAreaIndex, double CanopyHeight, double StomataWS, int water_stress_approach,
                         double vmax2, double alpha2, double kparm, double theta, double beta, double Rd2, double b02, double b12, double upperT, double lowerT, double Catm);
/* Definition of the new EvapoTrans function */
struct ET_Str EvapoTrans2(double Rad, double Iave, double Airtemperature, double RH,
			 double WindSpeed, double LeafAreaIndex, double CanopyHeight, 
			  double stomatacond, double leafw, int eteq);

struct ET_Str c3EvapoTrans(double Rad, double Itot, double Airtemperature, double RH, double WindSpeed,
			   double LeafAreaIndex, double CanopyHeight, double vcmax2, double jmax2, double Rd2, 
			   double b02, double b12, double Catm2, double O2, double theta2,
			   double StomWS, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation);

// Function to calculate leaf N limitation. Definitions are in leaf_n_limitation_functions.c
double thermal_leaf_nitrogen_limitation(double kLn, double leaf_n_0, const struct Model_state &current_state);

double biomass_leaf_nitrogen_limitation(double kLn, double leaf_n_0, const struct Model_state &current_state);

#endif

