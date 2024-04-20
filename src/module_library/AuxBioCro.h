#ifndef AUXBIOCRO_H
#define AUXBIOCRO_H

#include <map>
#include <vector>
#include "../framework/constants.h" // for ideal_gas_constant

/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */


#define MAXLAY    200 /* Maximum number of layers */

struct ET_Str {
  double TransR;
  double EPenman;
  double EPriestly;
  double Deltat;
  double boundary_layer_conductance;
};

struct ws_str {
  double awc;
  double psim;
  double runoff;
  double Nleach;
};

struct soilML_str {
  double rcoefPhoto;
  double rcoefSpleaf;
  double cws[MAXLAY];
  double drainage;
  double Nleach;
  double SoilEvapo;
  double rootDist[MAXLAY];
};


struct dbp_str{
	double kLeaf;
	double kStem;
	double kRoot;
	double kRhiz;
	double kGrain;

};

struct seqRD_str{
  double rootDepths[MAXLAY+1];
};

seqRD_str seqRootDepth(double to, int lengthOut);

struct rd_str{
  double rootDist[MAXLAY];
};

rd_str rootDist(int layer, double rootDepth, double *depths, double rfl);

struct frostParms {
  double leafT0;
  double leafT100;
  double stemT0;
  double stemT100;
  double rootT0;
  double rootT100;
  double rhizomeT0;
  double rhizomeT100;
};

struct nitroParms {
  double ileafN;
	double kln;
	double Vmaxb1;
	double Vmaxb0;
	double alphab1;
	double alphab0;
	double Rdb1;
	double Rdb0;
	double kpLN;
	double lnb0;
	double lnb1;
	int lnFun;
	double maxln;
    double minln;
	double daymaxln;
};

struct crop_phenology {
        dbp_str DBP;
        struct minerals1 {
        double CN;
        double CP;
        double CK;
        double CS;
      }leaf,stem,root,rhiz;
};

double leaf_boundary_layer_conductance_nikolov(
    double windspeed,
    double leafwidth,
    double air_temperature,
    double delta_t,
    double stomcond,
    double water_vapor_pressure,
    double minimum_gbw
);

double leaf_boundary_layer_conductance_thornley(
    double CanopyHeight,
    double WindSpeed,
    double minimum_gbw,
    double WindSpeedHeight
);

void LNprof(double LeafN, double LAI, double kpLN, std::vector<double>& leafNla);

/**
 *  @brief Calculates the exponential term of the Arrhenius equation.
 *
 *  The Arrhenius equation gives the dependence of the rate constant of a
 *  chemical reaction on the absolute temperature and is often written as
 *
 *  > `k = A * e^(-E_a / R / T)` (1)
 *
 *  where `k` is the rate constant, `E_a` is the activation energy, `R` is the
 *  ideal gas constant, and `T` is the absolute temperature. As `T` approaches
 *  infinity, `E_a / R / T` approaches zero and so `k` approaches `A`. Thus,
 *  `A` represents the rate constant in the limit of infinite temperature. From
 *  a practical standpoint, `A` is not a particularly useful parameter and for
 *  this reason it is often written as
 *
 *  > `A = k_0 * e^c` (2)
 *
 *  where `k_0` is the rate constant measured at a reference temperature (e.g.
 *  25 degrees C) and `c` is a dimensionless parameter chosen so that `k = k_0`
 *  at the reference temperature.
 *
 *  In order to make this function reusable, it only calculates the exponential
 *  factor, which is always dimensionless. I.e., it calculates
 *  `e^(c - E_a / R / T)`.
 *
 *  @param [in] c Dimensionless parameter related to the reference temperature
 *                at which the rate constant was measured
 *
 *  @param [in] activation_energy Activation energy of the reaction in J / mol
 *
 *  @param [in] temperature Absolute temperature in Kelvin
 *
 *  @return Return The Arrhenius exponential `e^(c - E_a / R / T)`
 */
inline double arrhenius_exponential(
    double c,                  // dimensionless
    double activation_energy,  // J / mol
    double temperature         // Kelvin
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol
    return exp(c - activation_energy / (ideal_gas_constant * temperature));
}

#endif

