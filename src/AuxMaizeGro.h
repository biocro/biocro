#ifndef AUXMAIZEGRO_H
#define AUXMAIZEGRO_H
/*
 *  BioCro/src/maizeGro.c by Fernando Ezequiel Miguez  Copyright (C) 2011
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

#endif

