/*
 *  BioCro/src/Century.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 */

#include <R.h>
#include <Rinternals.h>
#include <math.h>
#include "Century.h"
#include "AuxBioCro.h"

/* 

Begining of the Centurty function In this function the input should be
in g m^-2 Although the input from the MisGro function will be in Mg
ha^-1 I should (only for the version going inside MisGro) convert the
input to g m^-2 and then to Mg ha^-1 in the output

*/

struct cenT_str Century(double *LeafL, 
			double *StemL, 
			double *RootL, 
			double *RhizL, 
			double smoist, 
			double stemp, 
			int timestep, 
			double SCs[9] , 
			double leachWater, 
			double Nfert, 
			double iMinN, 
			double precip,
			double LeafL_Ln, 
			double StemL_Ln, 
			double RootL_Ln, 
			double RhizL_Ln,
			double LeafL_N, 
			double StemL_N, 
			double RootL_N, 
			double RhizL_N, 
			int soilType, 
	                double Ks_cf[8]){

  /* Converting Mg ha^-1 to g m^-2 */
  /* 1 Mg = 1e6 grams*/
  /* 1 ha = 1e4 m^2 */  
  const double cf = 100;
  *LeafL *= cf; 
  *StemL *= cf;
  *RootL *= cf;
  *RhizL *= cf;

  struct cenT_str tmp;
  struct soilText_str soilTemp;

  struct FL_str FmLc_Leaf, FmLc_Stem, FmLc_Root, FmLc_Rhiz;
  struct flow_str C1_5_Leaf, C2_5_Leaf, C1_5_Stem, C2_5_Stem;
  struct flow_str C1_7_Leaf_Ln, C1_7_Stem_Ln;
  struct flow_str C3_6_Root, C4_6_Root, C3_6_Rhiz, C4_6_Rhiz;
  struct flow_str C3_7_Root_Ln, C3_7_Rhiz_Ln;
  struct flow_str C5_7, C6, C7, C8_6;

  double C6_7, C6_8, C6_9, C7_6, C7_8;
  double C_ap, C_al, C_sp; 

  double SC1_Leaf, SC2_Leaf, SC1_Stem, SC2_Stem;
  double SC3_Root, SC4_Root, SC3_Rhiz, SC4_Rhiz;

/*  
    I need the separate fractions of Leaf Litter and Stem Litter 
  Because they have different lignin to N ratios 
  Presumably, the rhizome and root are similar 

  There is an issue here that I should be careful about I'm importing 
  biomass, but need only carbon in some of these calculations 

  I also need to determine the initial values of carbon for each 
  component 
*/

  soilTemp = soilTchoose(soilType);  
  double T = soilTemp.clay + soilTemp.silt; /*  silt plus clay content of the soil */
  double Ts = soilTemp.sand; /*  Sand content of the soil */
  double Tc = soilTemp.clay; /*  Clay content of the soil */

  double CN_structural = 150;
  double CN_surface;
  double CN_active ;
  double CN_slow ;
  double CN_passive ;
  
  double SN1, SN2, SN3, SN4, SN5, SN6, SN7, SN8, SN9;
  double SC1, SC2, SC3, SC4, SC5, SC6, SC7, SC8, SC9;
  double Abiot;
  double Na = 0.0, Nf = 0.0;
  double PlantN;

  double SC1_Leaf_Ln, SC1_Stem_Ln;
  double SC3_Root_Ln, SC3_Rhiz_Ln;

  /* Converting from Mg ha^-1 to g m^-2 */
  SC1 = SCs[0] * cf;
  SC2 = SCs[1] * cf;
  SC3 = SCs[2] * cf;
  SC4 = SCs[3] * cf;
  SC5 = SCs[4] * cf;
  SC6 = SCs[5] * cf;
  SC7 = SCs[6] * cf;
  SC8 = SCs[7] * cf;
  SC9 = SCs[8] * cf;

  double MinN = 0.0, Resp = 0.0;
  
  const double respC1_5 = 0.6;
  const double respC1_7 = 0.3;
  const double respC3_7 = 0.3;
  const double respC2_5 = 0.6;
  const double respC3_6 = 0.55;
  const double respC4_6 = 0.55;
  const double respC5_7 = 0.6;
  const double respC6 = 0.85 - 0.68 * T;
  const double respC7 = 0.55;
  const double respC8 = 0.55;

  double Ks[8];
  
  /*  Tm is the effect of soil texture on active SOM turnover */
  double Tm = 1 - 0.75 * T;

/*    The flow constants are taken from the paper 
      and Parton et al. 1993 Global Biogeochemistry pg 785 
      but 
      Parton et al. 1987 SSSJ 51:1173 
      is also relevant
    
      double Ksy[ ] = { 3.9, 4.9, 7.3, 6.0, 14.8, 18.5, 0.2, 0.0045 };     The units are year^-1  

      To get the rates per week we can divide by 52
      To get the rates poer day we can divide by 365
*/
/* It seems that the rates are the ones reported in Parton et al. 1993 Global Biogeochemical Cycles */
/* These are annual rates and to convert to other time steps the model should be run accordingly */

    /* Rprintf("Ks[3] : %f Ks[4] : %f \n",Ks[3],Ks[4]); */
    /* Rprintf("Ks[5] : %f Ks[6] : %f \n",Ks[5],Ks[6]); */

     if(timestep == 7){
       Ks[0] = Ks_cf[0] / 52 ;
       Ks[1] = Ks_cf[1] / 52 ;
       Ks[2] = Ks_cf[2] / 52 ;
       Ks[3] = Ks_cf[3] / 52 ;
       Ks[4] = Ks_cf[4] / 52 ;
       Ks[5] = Ks_cf[5] / 52 ;
       Ks[6] = Ks_cf[6] / 52 ;
       Ks[7] = Ks_cf[7] / 52 ;
     }else
     if(timestep == 1){   
       Ks[0] = Ks_cf[0] / 365 ; 
       Ks[1] = Ks_cf[1] / 365 ; 
       Ks[2] = Ks_cf[2] / 365 ; 
       Ks[3] = Ks_cf[3] / 365 ; 
       Ks[4] = Ks_cf[4] / 365 ; 
       Ks[5] = Ks_cf[5] / 365 ; 
       Ks[6] = Ks_cf[6] / 365 ; 
       Ks[7] = Ks_cf[7] / 365 ;  
     } 

/* Nitrogen processes 
    N deposition */

  Na = 0.21 + 0.0028 * precip * 0.10 ;  /*precipitation is entered in mm
					  but it is needed in cm for this
					   equation. Idem below */

  /*  N fixation */
  Nf = -0.18 + 0.014 * precip * 0.10 ; 

  /* The resulting N is in g N m^-2 yr^-1 
   Conversions
   g => Mg : multiply by 1e-6
   m^2 => ha : multiply by 1e4
   year to week : divide by 52 */
  if(timestep == 7){
     Na /=  52;
     Nf /=  52;
  }else
  if(timestep == 1){
     Na /=  365;
     Nf /=  365;
  }

/*   Rprintf("Na : %f \n",Na); */
/*   Rprintf("Nf : %f \n",Nf); */
/*   Nitrogen in the form of fertilizer */
/*   The input units should be in g N m^2 */

/*   Rprintf("Nfert : %f iMinN : %f \n",Nfert,iMinN); */
  MinN = Na + Nf + Nfert + iMinN;

/*   Rprintf("MinN 0: %f \n",MinN); */

  PlantN = *LeafL * LeafL_N + *StemL * StemL_N;

  CN_surface = 20 - PlantN * 5;
  CN_active = 15 - MinN * 6;
  CN_slow = 20 - MinN * 4;
  CN_passive = 10 - MinN * 1.5;

  if(PlantN > 2) CN_surface = 10;

  /*   Here 2 is g m^-2 */
  if(MinN > 2){
    CN_active = 3;
    CN_passive = 7;
    CN_slow = 12;
  }

  SN1 = SC1 / CN_structural; 
  SN2 = SC2 / CN_active;
  SN3 = SC3 / CN_structural;
  SN4 = SC4 / CN_active;
  SN5 = SC5 / CN_active;
  SN6 = SC6 / CN_active;
  SN7 = SC7 / CN_slow;
  SN8 = SC8 / CN_passive;
  SN9 = SC9 / CN_passive;

/*  Need to calculate the effect of temperature and moisture. */

  Abiot = AbiotEff(smoist, stemp);

/*    Calculate Fm and Lc separately for each component */
  FmLc_Leaf = FmLcFun(LeafL_Ln,LeafL_N);
  FmLc_Stem = FmLcFun(StemL_Ln,StemL_N);
  FmLc_Root = FmLcFun(RootL_Ln,RootL_N);
  FmLc_Rhiz = FmLcFun(RhizL_Ln,RhizL_N);

  /*  Surface Metabolic Carbon */
  SC2_Leaf = FmLc_Leaf.Fm * *LeafL;
  SC2_Stem = FmLc_Stem.Fm * *StemL;
  /*  Root Metabolic Carbon */
  SC4_Root = FmLc_Root.Fm * *RootL;
  SC4_Rhiz = FmLc_Rhiz.Fm * *RhizL;

  /*  Surface Structural Carbon */
  SC1_Leaf = (1 - FmLc_Leaf.Fm) * *LeafL;
  SC1_Stem = (1 - FmLc_Stem.Fm) * *StemL;
  /* Lignin content needs to be considered separately */
  SC1_Leaf_Ln = SC1_Leaf * LeafL_Ln;
  SC1_Stem_Ln = SC1_Stem * StemL_Ln;
  SC1_Leaf = SC1_Leaf - SC1_Leaf_Ln;
  SC1_Stem = SC1_Stem - SC1_Stem_Ln;

  /*  Root Structural Carbon */
  SC3_Root = (1 - FmLc_Root.Fm) * *RootL;
  SC3_Rhiz = (1 - FmLc_Rhiz.Fm) * *RhizL;
  /* Lignin content needs to be considered separately */
  SC3_Root_Ln = SC3_Root * RootL_Ln;
  SC3_Rhiz_Ln = SC3_Rhiz * RhizL_Ln;
  SC3_Root = SC3_Root - SC3_Root_Ln;
  SC3_Rhiz = SC3_Rhiz - SC3_Rhiz_Ln;

/*    T is silt plus clay content 
    Ls is fraction of structural C that is lignin 

    Structural Surface Litter C to Surface Microbe C 
    1 => 5 
    2 => 5 
    dC1/dt = Ki * Lc * A * Ci 
    Leaf */
  SC1_Leaf +=  0.3 * SC1;
  SC2_Leaf +=  0.3 * SC2;

  C1_5_Leaf = flow(&SC1_Leaf,CN_surface,Abiot,FmLc_Leaf.Lc,Tm,respC1_5,1,Ks);
  C2_5_Leaf = flow(&SC2_Leaf,CN_surface,Abiot,FmLc_Leaf.Lc,Tm,respC1_5,5,Ks);

  /*  Stem */
  SC1_Stem = SC1_Stem + 0.7 * SC1;
  SC2_Stem = SC2_Stem + 0.7 * SC2;

  C1_5_Stem = flow(&SC1_Stem,CN_surface,Abiot,FmLc_Stem.Lc,Tm,respC2_5,1,Ks);
  C2_5_Stem = flow(&SC2_Stem,CN_surface,Abiot,FmLc_Stem.Lc,Tm,respC2_5,5,Ks);

  SC1_Leaf = C1_5_Leaf.SC;
  SC2_Leaf = C2_5_Leaf.SC;
  SC1_Stem = C1_5_Stem.SC;
  SC2_Stem = C2_5_Stem.SC;

 /*   Adding the ligning content */
  C1_7_Leaf_Ln = flow(&SC1_Leaf_Ln,CN_surface,Abiot,FmLc_Leaf.Lc,Tm,respC1_7,1,Ks);
  C1_7_Stem_Ln = flow(&SC1_Stem_Ln,CN_surface,Abiot,FmLc_Stem.Lc,Tm,respC1_7,1,Ks);
  
  /* Flow from SC1.lignin to SC7 */
  SC7 += C1_7_Leaf_Ln.fC + C1_7_Stem_Ln.fC ;

  /*  Collect respiration */
  Resp = C1_5_Leaf.Resp + C2_5_Leaf.Resp +
         C1_5_Stem.Resp + C2_5_Stem.Resp +
         C1_7_Leaf_Ln.Resp + C1_7_Stem_Ln.Resp;
  /*  Collect mineralized Nitrogen */
  MinN += C1_5_Leaf.MinN + C2_5_Leaf.MinN +
          C1_5_Stem.MinN + C2_5_Stem.MinN +
          C1_7_Leaf_Ln.MinN + C1_7_Stem_Ln.MinN;

/*   Rprintf("MinN 1: %f \n",MinN); */
  /*  Updating the Soil Carbon Pools 1 and 2  */
  SC1 = C1_5_Leaf.SC + C1_5_Stem.SC + C1_7_Leaf_Ln.SC + C1_7_Stem_Ln.SC ;
  SC2 = C2_5_Leaf.SC + C2_5_Stem.SC ;

  /*  Updating the Nitrogen Carbon Pools 1 and 2 */
  SN1 = SC1 / CN_structural + SN1;
  SN2 = SC2 / CN_surface + SN2;
  
/*    Structural Root Litter C to Soil Microbe C */
/*    4 => 6 */
/*    3 => 6 */
/*    Root */
  SC3_Root = SC3_Root + 0.3 * SC3;
  SC4_Root = SC4_Root + 0.3 * SC4;
  C3_6_Root = flow(&SC3_Root,CN_active,Abiot,FmLc_Root.Lc,Tm,respC3_6,2,Ks);
  C4_6_Root = flow(&SC4_Root,CN_active,Abiot,FmLc_Root.Lc,Tm,respC3_6,6,Ks);
/*    Rhizome */
  SC3_Rhiz = SC3_Rhiz + 0.7 * SC3;
  SC4_Rhiz = SC4_Rhiz + 0.7 * SC4;
  C3_6_Rhiz = flow(&SC3_Rhiz,CN_active,Abiot,FmLc_Rhiz.Lc,Tm,respC4_6,2,Ks);
  C4_6_Rhiz = flow(&SC4_Rhiz,CN_active,Abiot,FmLc_Rhiz.Lc,Tm,respC4_6,6,Ks);

  SC3_Root = C3_6_Root.SC;
  SC4_Root = C4_6_Root.SC;
  SC3_Rhiz = C3_6_Rhiz.SC;
  SC4_Rhiz = C4_6_Rhiz.SC;

  C3_7_Root_Ln = flow(&SC3_Root_Ln,CN_slow,Abiot,FmLc_Root.Lc,Tm,respC3_7,2,Ks);
  C3_7_Rhiz_Ln = flow(&SC3_Rhiz_Ln,CN_slow,Abiot,FmLc_Rhiz.Lc,Tm,respC3_7,2,Ks);

  SC7 += C3_7_Root_Ln.fC + C3_7_Rhiz_Ln.fC ; 
 /*   Collect respiration */
  Resp += C3_6_Root.Resp + C4_6_Root.Resp +
           C3_6_Rhiz.Resp + C4_6_Rhiz.Resp;

/*    Collect mineralized Nitrogen */
  MinN += C3_6_Root.MinN + C4_6_Root.MinN +
           C3_6_Rhiz.MinN + C4_6_Rhiz.MinN + 
           C3_7_Root_Ln.MinN + C3_7_Rhiz_Ln.MinN;

/*   Rprintf("MinN 2: %f \n",MinN); */
/*   Updating the Soil Carbon Pools 3 and 4 */
  SC3 = C3_6_Root.SC + C3_6_Rhiz.SC + C3_7_Root_Ln.SC + C3_7_Rhiz_Ln.SC;
  SC4 = C4_6_Root.SC + C4_6_Rhiz.SC ;

/*   Updating the Nitrogen pools 3 and 4 */
  SN3 += SC3 / CN_structural ;
  SN4 += SC4 / CN_active ;

/*    Updating the Soil Carbon Pool 5 */
  SC5 += C1_5_Leaf.fC + C1_5_Stem.fC + C2_5_Leaf.fC + C2_5_Stem.fC ;

/*    Updating the Soil Nitrogen pool 5 */
  SN5 += SC5 / CN_surface ;
  
/*    Updating the Soil Carbon Pool 6 */
  SC6 += C3_6_Root.fC + C3_6_Rhiz.fC + C4_6_Root.fC + C4_6_Rhiz.fC ;

/*    Updating the Soil Nitrogen Pool 6 */
  SN6 += SC6 / CN_active ;

/*    Surface Microbe C to Slow C */
/*    5 => 7 */
  C5_7 = flow(&SC5,CN_slow,Abiot,0,0,respC5_7,4,Ks);

  Resp +=  C5_7.Resp;
  MinN +=  C5_7.MinN;

/*    Updating Surface Microbe C (pool 5) and slow (pool 7) */

  SC5 = C5_7.SC ;
  SC7 += C5_7.fC ;

/*    Updating Surface Microbe N pool */

  SN5 = SC5 / CN_surface;
   
/*   Soil Microbe C to intermediate stage C */
  C6 = flow(&SC6,CN_slow,Abiot,0,Tm,respC6,3,Ks);

/*   if(ISNAN(C6.fC) | ISNAN(C6.SC)){ */
/*     Rprintf("C6.fC %f and C6.SC %f and SC6 %f \n",C6.fC,C6.SC,SC6); */
/*   } */

  Resp +=  C6.Resp;
  MinN +=  C6.MinN;

/*    Updating carbon and nitrogen soil pools 6 */
  SC6 = C6.SC;
  SN6 = SC6 / CN_active;
  
  C_ap = 0.003 + 0.032 * Tc;   
  C_al = leachWater / 18.0 * (0.01 + 0.04 * Ts);
  
/*   Rprintf("C_al %f and C_ap %f Tc %f \n",C_al, C_ap, Tc); */
/*   Rprintf("leachWater %f and Ts %f \n",leachWater, Ts); */

  C6_8 = C6.fC * C_ap;
  C6_9 = C6.fC * C_al;
  C6_7 = C6.fC * (1 - C_ap - C_al); /* There is no need to subtract 0.55 since 
                                       this was already taken into account in 
                                       the flow equation */
  
 /*   Updating the Soil Carbon Pool 7, 8 and 9 */

  SC7 += C6_7 ;
  SC8 += C6_8 ;
  SC9 += C6_9 ;

/*   Rprintf("C6_9 %f \n",C6_9); */

/*  Updating the Soil Nitrogen Pool 7, 8 and 9 */

  SN7 += SC7 / CN_slow ;
  SN8 += SC8 / CN_passive ;
  SN9 += SC9 / CN_slow ;
  
/*    Slow Carbon to intermediate stage */

  C7 = flow(&SC7,CN_slow,Abiot,0,0,respC7,7,Ks);

  Resp += C7.Resp;
  MinN += C7.MinN;
  
  C_sp = 0.003 - 0.009 *Tc;
  C7_8 = C7.fC * C_sp;
  C7_6 = C7.fC * (1 - C_sp);  /* There is no need to subtract 0.55 since 
                                 this was already taken into account in 
                                 the flow equation */

/*    Updating the Soil Carbon Pools 6 and 8 */

  SC6 += C7_6 ;
  SC8 += C7_8 ;

/*    Updating the Soil Nitrogen Pools 6 and 8 */

  SN6 = SC6 / CN_active;
  SN8 = SN8 / CN_passive;

/*    Passive Carbon to Soil Microbe C */

  C8_6 = flow(&SC8,CN_passive,Abiot,0,0,respC8,8,Ks);

  Resp += C8_6.Resp;
  MinN += C8_6.MinN;

/*   Updating the Soil Microbe C 6 and 8 */
  
  SC8 = C8_6.SC ;
  SC6 += C8_6.fC ;

  SN6 = SC6 / CN_active;
  SN8 = SC8 / CN_passive;

  tmp.SNs[0] = SN1 / cf;
  tmp.SNs[1] = SN2 / cf ;
  tmp.SNs[2] = SN3 / cf ;
  tmp.SNs[3] = SN4 / cf ;
  tmp.SNs[4] = SN5 / cf ;
  tmp.SNs[5] = SN6 / cf ;
  tmp.SNs[6] = SN7 / cf ;
  tmp.SNs[7] = SN8 / cf ;
  tmp.SNs[8] = SN9 / cf ;

  tmp.SCs[0] = SC1 / cf;
  tmp.SCs[1] = SC2 / cf;
  tmp.SCs[2] = SC3 / cf;
  tmp.SCs[3] = SC4 / cf;
  tmp.SCs[4] = SC5 / cf;
  tmp.SCs[5] = SC6 / cf;
  tmp.SCs[6] = SC7 / cf;
  tmp.SCs[7] = SC8 / cf;
  tmp.SCs[8] = SC9 / cf;

  tmp.MinN = MinN;
  tmp.Resp = Resp;

/* Converting Leaf Litter to its original units */

  *LeafL /= cf;
  *StemL /= cf;
  *RootL /= cf;
  *RhizL /= cf;

  return(tmp);
}



struct FL_str FmLcFun(double Lig, double Nit){

  double Fm, Ls, Lc;
  struct FL_str tmp;

  Fm = 0.85 - 0.018 *(Lig/Nit);
  Ls = Lig / (1 - Fm);
  Lc = exp(-3 * Ls);

  tmp.Lc = Lc;
  tmp.Fm = Fm;

  return(tmp);

}

/*
 *SC = pointer to the source of carbon
 CNratio = Carbon to Nitrogen ratio of the source of carbon
 A = abiotic decomposition factor
 Lc = exp(-3*L_s)
 Tm = 1 - 0.75 * T
 resp = respiration fraction
 kno = rate of change K number
 Ks = vector with values of rate of change 
*/

struct flow_str flow(double *SC, double CNratio, double A, double Lc, double Tm, double resp, int kno, double Ks[8]){

  struct flow_str tmp;

  double Kf = 0.0, fC = 0.0;
  double Resp, MinN;

  if(kno < 3){
    Kf = Ks[kno-1] * Lc * A ;
    fC = Kf * *SC;
  }else
  if(kno == 3){
    Kf = Ks[kno-1] * A * Tm ;
    fC = Kf * *SC;
  }else
  if(kno > 3){
    Kf = Ks[kno-1] * A;
    fC = Kf * *SC;
  }

  if(Kf > 1){
    Rprintf("Kf greater than 1: %d %f %f %f %f \n", kno, Ks[kno-1], Kf, A, Tm);
  }

  Resp = fC * resp;

  /*  Mineralized N */
  MinN = Resp / CNratio;

  *SC = *SC - fC;
  
  fC = fC - Resp;
/*   fN = fC / CNratio; */

/*  It is important to keep track of C emissions because */
/*  I might need to validate it against Eddy flux data */
  tmp.SC = *SC;
  tmp.fC = fC;
  tmp.Resp = Resp;
  tmp.MinN = MinN;  

  return(tmp);

}
  

/* Function to calculate the abiotic effect */
double AbiotEff(double smoist, double stemp){

  double ans;
  double TempEff = 0.0, MoisEff;

  if(stemp < 35){
    TempEff = 1.0087 / (1 + (46.2 * exp(-0.1899 * stemp)));
  }else{
    TempEff = -0.0826 * stemp + 3.84;
  }
  
  MoisEff = 1.0267 / (1 + 14.7 * exp(-6.5 * smoist));

  ans = TempEff * MoisEff;

  return(ans);

}

