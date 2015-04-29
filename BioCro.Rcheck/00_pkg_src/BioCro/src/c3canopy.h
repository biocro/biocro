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

#endif
