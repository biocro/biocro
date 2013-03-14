/*
 *  BioCro/src/maizeGro.c by Fernando Ezequiel Miguez  Copyright (C) 2012
 *
 */

#include <math.h>
#include "AuxMaizeGro.h"


struct lai_str laiLizasoFun(double thermalt, double phenostage, 
			    double phyllochron1, double phyllochron2, 
			    double Ax, double LT, double k0, double a1, 
			    double a2, double L0, double LLx, double Lx, double LNl){

	struct lai_str tmp;
	double A, kei, tei;
	double LNx = 0.67 * LT;
	double te1 = 25;
	double te2 = 50;
	double tti;
	double tsi;
	double leafnumber ;
	double totalLAI = 0.0;

        if (phenostage <= 0) {
	  leafnumber = 0;
	}else{
	  if (phenostage < 1){
	    leafnumber = phenostage* 100;
	  }else{
	    leafnumber = LT;
	  }
	}

/* Longevity */

        /* Eq 11 in Lisazo 2003 */
	if (LNl == -1){ 
	    LNl = 3.59 + 0.498 * LT;
	}
        /* Eq 12 in Lisazo 2003 */
	/* double Wl = 0.333333 * LT; not used ? */
	double LLi;

	double Wk = LT / 8.18;

	int i, i2, ln = 0;

	for(i2=0;i2<MAXLEAFNUMBER;i2++){
		tmp.leafarea[i2] = 0.0;
	}

	for(i=0;i<leafnumber;i++){

		/* Eq 7 in Lizaso 2003 */
		ln = i + 1;
		tti = (ln - 2) * phyllochron1 + te2;

		/* Eq 8 in Lizaso 2003 */
		kei = k0 + 0.174 * exp( - ((ln - 1)*(ln - 1))/(2 * Wk * Wk));

		/* Eq 10 in Lizaso 2003 */
		LLi = L0 + Lx * exp( - (ln - LNl)*(ln - LNl)/ (2*Wk*Wk));
		

		/* Eq 6 in Lizaso 2003 */
		if(ln == 1) tei = te1;
		if(ln == 2){
			tei = te2;
		}else{
			tei = tti + (2.197 /kei);
		}
		
		/* calculating time of 50% senescence from time of 50%
		 * expansion and leaf longevity */
		tsi = LLi + tei;

		/* Applying Eq 4 in Lizaso 2003 */
		A = Aei(ln, Ax, LNx, a1, a2);

		/* Applying Eq 2 in Lizaso 2003 */
		tmp.leafarea[i] = Alogistic(thermalt, A, kei, tei);

		/* calculating senesced area for this leaf */
		tmp.leafarea[i] -= Alogistic(thermalt, A, kei, tsi);

		/* Rprintf("leaf area %.1f \n", tmp.leafarea[i]);  */
		totalLAI += tmp.leafarea[i];
	}

	tmp.totalLeafArea = totalLAI;
	return(tmp);


}

struct lai_str laiBirchdiscontinuousFun(double phenostage, double LT, 
					double Amax, double c1, double c2, 
					double c3, double c4){
        struct lai_str tmp;
	double leafnumber, totalLAI = 0.0;
        if (phenostage <= 0) {
	  leafnumber = 0;
	}else{
	  if (phenostage < 1){
	    leafnumber = phenostage* 100;
	  }else{
	    leafnumber = LT;
	  }
	}	  
	if (Amax == -1){
	  /* Eq 13 in Birch*/
	  Amax = 1000 * exp(-1.17 + 0.047 * LT);
	}
	int i, i2, ln;
	for(i2=0;i2<MAXLEAFNUMBER;i2++){
	  tmp.leafarea[i2] = 0.0;
	}
	for (i = 0; i<leafnumber; i++){
	  ln = i + 1;
	  if (ln <= 3){
	    /* Eq 14 in Birch */
	    tmp.leafarea[i] = c1 * ln;
	  }else{
	    if (ln <= 11){
	      /* Eq 15 in Birch */
	      tmp.leafarea[i] = c2* ln * ln;
	    }else{
	      /* Eq 16 in Birch */
	      tmp.leafarea[i] = c3 * Amax + c4 * ln * ln;
	    }
	  }
	  totalLAI += tmp.leafarea[i];
	}
	tmp.totalLeafArea = totalLAI;
	return (tmp);
}


struct lai_str laiBirchcontinuousFun(double phenostage, double a, 
				     double b, double Amax, double x0, 
				     double f, double g, double TT, 
				     double LT){
        struct lai_str tmp;
	double leafnumber = 0.0, totalLAI = 0.0, SLA = 0.0, TPLAmax = 0.0;

	int i, i2, i3, ln, ln2;

	for(i2 = 0; i2 < MAXLEAFNUMBER; i2++){
		tmp.leafarea[i2] = 0.0;
	}

        if (phenostage <= 0) {
	  leafnumber = 0;
	}else{
	  if (phenostage < 1){
	    leafnumber = phenostage* 100;
	  }else{
	    leafnumber = LT;
	  }
	}	  
	if (Amax == -1){
		/* Eq 13 in Birch*/
		Amax = 1000 * exp(-1.17 + 0.047 * LT);
	}
	if (a == -1){
		/* Eq 18 in Birch */
		a = -0.009 - exp(-0.2 * LT);
	}
	if (b == -1){
		/* Eq 19 in Birch */
		b = 0.0006 - exp(-0.43 * LT);
	}
	if (x0 == -1){
		/* Eq 14 in Birch */
		x0 = 0.67 * LT;
	}
	if (g == -1){
		/* Eq 20 in Birch */
		g = 107.9 * LT - 327;
	}

	for(i3 = 0; i3 < LT; i3++){
		ln2 = i3 + 1;
		TPLAmax += Amax * exp(a * (ln2 - x0) * (ln2 - x0) + b *
			     (ln2 - x0) * (ln2 - x0) * (ln2 - x0));
	}

	/* Birch Eq 11*/
	SLA = TPLAmax / (1 + exp(-f * (TT - g)));

	for (i = 0; i < leafnumber; i++){
	  ln = i + 1;
	  tmp.leafarea[i] = Amax * exp(a * (ln - x0) * (ln - x0) + b *
			     (ln - x0) * (ln - x0) * (ln - x0));

	   totalLAI += tmp.leafarea[i];
	 }
	 tmp.totalLeafArea = totalLAI - SLA;
	 return (tmp);
}



double Aei(double leafnumber, double Ax, 
	   double LNx, double a1, double a2){
/* Eq 4 in Lizaso 2003 */

	double res1, res2, res3, res;

	res1 = (leafnumber - LNx)/(LNx - 1);
	res2 = res1 * res1;
	res3 = res2 * res1;

	res = Ax * exp(a1 * res2 + a2 * res3);

	return(res);

}

double Alogistic(double time, double A, double kei, double tei){
/* Eq 2 in Lizaso 2003 */
	double ans;

	ans = A / (1 + exp(-kei * (time - tei)));

	return ans;

}

struct maize_dbp_str maize_sel_dbp_coef(double dbpCoefs[13], double phenoStage){

	struct maize_dbp_str tmp;

	tmp.kLeaf = 0.0;
	tmp.kStem = 0.0;
	tmp.kRoot = 0.0;
	tmp.kGrain = 0.0;

	if(phenoStage < 0.06){
/* For now there is one set of coefficients for V6 */
		tmp.kStem = dbpCoefs[0];
		tmp.kLeaf = dbpCoefs[1];
		tmp.kRoot = dbpCoefs[2];
	}else
	if(phenoStage < 0.12){
/* For now there is one set of coefficients until V12 */
		tmp.kStem = dbpCoefs[3];
		tmp.kLeaf = dbpCoefs[4];
		tmp.kRoot = dbpCoefs[5];
	}else
	if(phenoStage < 1){
/* For now there is one set of coefficients after V12 but before R1*/
		tmp.kStem = dbpCoefs[6];
		tmp.kLeaf = dbpCoefs[7];
		tmp.kRoot = dbpCoefs[8];
	}else
		if(phenoStage >= 1){
/* For now there is one set of coefficients after R1*/
			tmp.kStem = dbpCoefs[9];
			tmp.kLeaf = dbpCoefs[10];
			tmp.kRoot = dbpCoefs[11];
			tmp.kGrain = dbpCoefs[12];
		}

	return(tmp);

}
