/*****************************************************************************
**
**  FILE:    soilwater.h
**
**  AUTHOR:  Melannie Hartman  9/7/93 - 8/21/96
**
*****************************************************************************/

#include <stdio.h>
#include "swconst.h"

#define FILEPATH_MAX 60   /* Max # of chars in a file path-name */

#define INPTSTRLEN 120    /* Max length of input file line */
#define OTPTSTRLEN 80     /* Max length of output file string */

typedef struct {
  int verbose;
  int debug;
} FLAG_S, *FLAG_SPT;

typedef struct
{
  char wbname[FILEPATH_MAX];  /* Water Balance File */
  FILE *fp_outf;
  FILE *fp_soiln;
  FILE *fp_co2;
  FILE *fp_yearsum;
  FILE *fp_soilc;
  FILE *fp_tgmonth;
  FILE *fp_dN2lyr;
  FILE *fp_dN2Olyr;
  FILE *fp_cflows;
  FILE *fp_yrcflows;
/*!!  FILE *fp_snow; */
  int  write_wb;
  int  write_co2;
  int  write_soiln;
  int  write_swc;
  int  write_wfps;
  int  write_soiltavg;
  int  write_soiltmin;
  int  write_soiltmax;
  int  write_stempdx;
  int  write_bio;
  int  write_wflux;
  int  write_mresp;
  int  write_yearsum;
  int  write_livec;
  int  write_deadc;
  int  write_soilc;
  int  write_sysc;
  int  write_tgmonth;
  int  write_dN2lyr;
  int  write_dN2Olyr;
  int  write_gresp;
  int  write_dels;
  int  write_dcsip;
  int  write_harvest;
  int  write_cflows;
  int  write_yrcflows;
  int  write_psyn;
} FILES_S, *FILES_SPT;

typedef struct
{
  double  width[MAXLYR];
  double  depth[MAXLYR];
  double  dpthmn[MAXLYR];
  double  dpthmx[MAXLYR];
  double  bulkd[MAXLYR];
  double  fieldc[MAXLYR];
  double  wiltpt[MAXLYR];
  double  ecoeff[MAXLYR];
  double  tcoeff[MAXLYR];
  double  sandfrac[MAXLYR];
  double  clayfrac[MAXLYR];
  double  orgfrac[MAXLYR];
  double  swclimit[MAXLYR];
  double  satcond[MAXLYR];
  double  pH[MAXLYR];
  int    numlyrs;
  double  swcfc[MAXLYR];
  double  swcwp[MAXLYR];
  double swc[MAXLYR];
  double swcmin[MAXLYR];
  double  minpot[MAXLYR];
  double  wfps[MAXLYR];
  double  sumecoeff;
  int    nelyrs;
  double  thetas[MAXLYR];
  double  thetas_bd[MAXLYR];
  double  psis[MAXLYR];
  double  b[MAXLYR];
  double  binverse[MAXLYR];
  int    ubnd[CENTMAXLYR];
  int    lbnd[CENTMAXLYR];
} LAYERPAR_S, *LAYERPAR_SPT;

typedef struct
{
  int    usexdrvrs;
  double  sublimscale;
  double  reflec;
  double  albedo;
  double  fswcinit;
  double  dmpflux;
  double  hours_rain;
  int    watertable[NMONTH+1];
  double  hpotdeep;
  double  ksatdeep;
  double  rlatitude;
  double  cldcov[NMONTH+1];
  int    texture;
  double  tbotmn;
  double  tbotmx;
  double  dmp;
  double  timlag;
  double  Ncoeff;
  int    drainlag;
  int    jdayStart;
  int    jdayEnd;
  double  N2Oadjust;
  double  elevation;
  double  slope;
  double  aspect;
  double  ehoriz;
  double  whoriz;
} SITEPAR_S, *SITEPAR_SPT;

typedef struct
{
  double stmtemp[MAXSTLYR];
  double soiltavg[MAXLYR];
  double soiltmin[MAXLYR];
  double soiltmax[MAXLYR];
} SOIL_S, *SOIL_SPT;

double c_shwave(int month, double rlatitude, int jday);

void getdiff(double *valsubt, double valtochk, double valmin);

void initdaily(int month, double biolive, double biodead, double blitter,
               double *biomass, double *blivelai, double *vegcov, double *totagb,
               double *stcrlai, LAYERPAR_SPT layers);

void initlyrs(char *soilname, LAYERPAR_SPT layers, FLAG_SPT flags,
              SITEPAR_SPT sitepar);

void initsite(char *sitename, SITEPAR_SPT sitepar, LAYERPAR_SPT layers,
              FLAG_SPT flags, double sradadj[NMONTH], double *tminslope,
              double *tminintercept, double *maxphoto, double *bioabsorp);

void initsrad(double elevation, double latitude, double aspect, double slope,
              double ehoriz, double whoriz, double daylength[NDAY]);

void litstcr_evap(double *cwlit, double *cwstcr, double *petleft, double *aet,
                  double totlit, double totstcr);

double petrad(int jday, int month, double cldcov, double windsp, double rhumid,
             double rlatitude, double avgtemp, double reflec, double solrad,
             double albedo, double snowpack);

double pevapdly(double tmin, double tmax, double sitlat, double tmn2m[],
               double tmx2m[], int jday, int month);

void setamov(double amovdly[CENTMAXLYR], int nlayer, double wfluxout[MAXLYR],
             int numlyrs, int lbnd[CENTMAXLYR]);

void setasmos(double asmos[CENTMAXLYR], int *nlayer, double swcinit[MAXLYR],
              int *numlyrs, double avh2o[3], double rwcf[CENTMAXLYR]);

void showlyrs(double swc[MAXLYR], int numlyrs);

double svapor(double atemp);

double tanfunc(double z, double a, double b, double c, double d);

void wfps(LAYERPAR_SPT layers);

double wfunc_pulse(double *ppt, double *snow);

