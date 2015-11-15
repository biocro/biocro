#ifndef C3CANOPY_H
#define C3CANOPY_H

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <math.h>


struct canopyEnv
{
  double Idir;
	double Idiff;
  double Itotal;
	double RH;
	double windspeed;
	double Catm;
	double Temp;
};

struct canopyLeaf
{
	double LAI;
	double pLeafsun;
	double pLeafshade;
  double heightf;
  double LeafN;
};

struct canopyoutput
{
  
  double sunlittemp;
  double shadedtemp;
  double TotalAnet;
  double TotalAgross;
  double TotalTrans;
  double shadedAnet;
  double shadedAgross;
  double sunlitAnet;
  double sunlitAgross;
  double shadedTemp;
  double sunlitTemp;
  double shadedTranspiration;
  double sunlitTranspiration;
};

struct canopy
{
  int Nlayers;
  double Idirtop, Idifftop, CosZenithAngle;
  double LAItotal;
  struct canopyEnv *ENV;
  struct canopyLeaf *Leaf;
  struct canopyoutput *OUT;
};

void createCanopy (struct canopy *canopy, int Nlayers, double LAItotal);

void discretizeCanopy(struct canopy *canopy);

void getcanopylightme(struct canopy * canopy, double lat, double DOY, int td, double solarR);

void getCanopysunML(struct canopy *canopy,double kd, double chil, double heightf);

void getcanopyWINDprofile(struct canopy *canopy, double WindSpeed);

void getcanopyRHprof(struct canopy *canopy, double RH);

void getcanopyLNprof(struct canopy *canopy, double LeafN,double kpLN);

void freecanopy(struct canopy *canopy);

#endif

