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

void LNprof(double LeafN, double LAI, double kpLN, std::vector<double>& leafNla);

#endif

