#ifndef AUXBIOCRO_H
#define AUXBIOCRO_H

/*
 *  /src/AuxBioCro.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 */


/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */


#define MAXLAY    200 /* Maximum number of layers */

/* These are global variables. */
int sp1,sp2,sp3,sp4,sp5,sp6;

double layIdir[MAXLAY];
double layIdiff[MAXLAY];
double layItotal[MAXLAY];
double layFsun[MAXLAY];
double layFshade[MAXLAY];
double layHeight[MAXLAY];

struct ET_Str {
  double TransR;
  double EPenman;
  double EPriestly;
  double Deltat;
  double LayerCond;
};

struct Can_Str {
  double Assim;
  double Trans;
  double GrossAssim;
};

struct ws_str {
  double rcoefPhoto;
  double rcoefSpleaf;
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
  double hourlyWflux[MAXLAY];
  double dailyWflux[MAXLAY];
  double width[MAXLAY];
  double swclimit[MAXLAY];
  double pH[MAXLAY];
  double bulkd[MAXLAY];
  double fieldc[MAXLAY];
  double dpthmx[MAXLAY];
  double dpthmn[MAXLAY];
  double soiltavg[MAXLAY];
  double sand[MAXLAY];
  double silt[MAXLAY];
  double clay[MAXLAY];
  int Num_BioCro_soil_layers;
};


struct dbp_str{
	double kLeaf;
	double kStem;
	double kRoot;
	double kRhiz;
	double kGrain;

};

struct soilText_str{
  double silt;
  double clay;
  double sand;
  double air_entry;
  double b;
  double Ks;
  double satur;
  double fieldc;
  double wiltp;
  double bulkd;

};

struct soilText_str soilTchoose(int soiltype);

struct seqRD_str{
  double rootDepths[MAXLAY+1];
};

struct seqRD_str seqRootDepth(double to, int lengthOut);

struct rd_str{
  double rootDist[MAXLAY];
};

struct rd_str rootDist(int layer, double rootDepth, double *depths, double rfl);

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
        struct dbp_str DBP;
        struct minerals1 {
        double CN;
        double CP;
        double CK;
        double CS;
      }leaf,stem,root,rhiz;
};

double leafboundarylayer(double windspeed, double leafwidth, double AirTemp,
                         double deltat, double stomcond, double vappress);

void cropcent_dbp(double coefs[25],double TherPrds[6], double TherTime, struct crop_phenology *cropdbp);

double SoilEvapo(double LAI, double k, double AirTemp, double IRad,
                 double awc, double fieldc, double wiltp, double winds, double RelH, double rsec );

void LNprof(double LeafN, double LAI, int nlayers, double kpLN, double* leafNla);

#endif

