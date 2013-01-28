/*
 *  /src/AuxBioCro.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 or 3 of the License
 *  (at your option).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  http://www.r-project.org/Licenses/
 *
 */


/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */


#define MAXLAY    50 /* Maximum number of layers */

/* These are global variables. */
int sp1,sp2,sp3,sp4,sp5,sp6;
int tp3,tp4,tp5;

double tmp1[3]; 
double layIdir[MAXLAY];
double layIdiff[MAXLAY];
double layItotal[MAXLAY];
double layFsun[MAXLAY];
double layFshade[MAXLAY];
double layHeight[MAXLAY];
double tmp3[MAXLAY];
double tmp4[MAXLAY];
double tmp5[MAXLAY];

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
  double fieldc;
  double wiltp;

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
