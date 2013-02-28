/*
 *  BioCro/src/maizeGro.c by Fernando Ezequiel Miguez  Copyright (C) 2011
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

/* Structure for the laiFun */

#define MAXLEAFNUMBER 30

struct lai_str {

	double leafarea[MAXLEAFNUMBER];
	double totalLeafArea;

};

struct maize_dbp_str {

	double kLeaf;
	double kStem;
	double kRoot;
	double kGrain;

};

double Aei(double leafnumber, double Ax, 
	   double LNx, double a1, double a2);

double Alogistic(double time, double A, double kei, double tei);

struct maize_dbp_str maize_sel_dbp_coef(double dbpCoefs[13], double phenoStage);

