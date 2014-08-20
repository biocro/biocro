/*!
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2011
 *TEST DOXYGEN
 *
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "AuxcaneGro.h"
#include "crocent.h"
#include "soilwater.h"
#include "function_prototype.h"
#include "n2o_model.h"

void CropGro_c(double lat,          /* Latitude    ~           1 */ 
      int *pt_doy,                 /* Day of the year   -        2 */
      int *pt_hr,                  /* Hour of the day     -      3 */
      double *pt_solar,               /* Solar Radiation    -       4 */
	    double *pt_temp,                /* Temperature        -       5 */
	    double *pt_rh,                  /* Relative humidity   -      6 */ 
	    double *pt_windspeed,           /* Wind Speed -               7 */ 
	    double *pt_precip,              /* Precipitation     -        8 */
	    double kd,                  /* K D (ext coeff diff) -     9 */
	    double * chilhf,              /* NEED TO REVERT TO DOUBLE * Chi and Height factor  -  10 */
	    int nlayers,             /* # Lay canop   -           11 */
	    double Rhizome,             /* Ini Rhiz        -         12 */
      double propLeaf,                /* i rhiz to leaf    -       13 */
      double * sencoefs,            /* sene coefs   !!!!!!!MADE NEW VARIABLE ADJUST IN OTHER FILE    -        14 */
      int timestep,            /* time step       -         15 */
      int vecsize,             /* vector size        -      16 */
	    double Sp,              /* Spec Leaf Area     -      17 */
	   // SEXP SPD, UNUSED VARIABLE OMITTED FROM ARGS                 /* Spec Lefa Area Dec  -     18 */
	    double * dbpcoefs,            /* Dry Bio Coefs      -      19 */
	    double * thermalp,            /* Themal Periods   -        20 */
	    double vmax1,                /* Vmax of photo     -       21 */
	    double alpha1,               /* Quantum yield   -         22 */
	    double kparm1,               /* k parameter (photo) -     23 */
	    double theta,               /* theta param (photo)   -   24 */
	    double beta,                /* beta param  (photo) -     25 */
	    double Rd1,                  /* Dark Resp   (photo) -     26 */
	    double Ca,                /* CO2 atmosph       -       27 */
	    double b01,                  /* Int (Ball-Berry)  -       28 */
	    double b11,                  /* Slope (Ball-Berry)  -     29 */
	    int ws,                  /* Water stress flag   -     30 */
	    double * soilcoefs,           /* Soil Coefficients !!!!! MADE NEW VARIABLE ADJUST IN OTHER FILE  -     31 */
	    double LeafN_0,              /* Ini Leaf Nitrogen -       32 */
	    double kLN,                 /* Decline in Leaf Nitr -    33 */
	   // SEXP VMAXB1, UNUSED VAR OMMITTED FROM ARGS             /* Effect of N on Vmax   -   34 */
	   // SEXP ALPHAB1, UNUSED VAR OMMITTED FROM ARGS             /* Effect of N on alpha   -  35 */
	    double * mresp,               /* Maintenance resp !!!!!CHANGED VAR ADJUST IN OTHER FILE    -    36 */
	    int soilType,            /* Soil type       -         37 */
	    int wsFun,               /* Water Stress Func  -      38 */
	    double * centcoefs,           /* Century coefficients !!!!!!!!! MADE NeW VARiABLE ADJUST IN OTHER FILE  -  39 */
      int centTimestep,        /* Century timestep   -      40 */ 
	   // SEXP CENTKS, UNUSED VAR OMMITTED FROM ARGS             /* Century decomp rates-     41 */
	    int soillayers,          /* # soil layers        -    42 */
	    double * soildepths,          /* Soil Depths      -        43 */
	    double * cwsVec,                 /* Current water status !!!!! CHANGED var ADJST IN NEW  -   44 */
	    int hydrdist,            /* Hydraulic dist flag   -   45 */
	    double * secs,                /* Soil empirical coefs !!!MADE NEW VAR CHANGE IN OTHER  -   46 */
	    double kpLN,                /* Leaf N decay         -    47 */
	    double lnb0,                /* Leaf N Int        -       48 */
	    double lnb1,                /* Leaf N slope         -    49 */
      int lnfun,               /* Leaf N func flag      -   50 */
      double upperT,           /* Temperature Limitations photoParms- */
	    double lowerT,         //-
	    double * nnitrop, //-        MADE NEW VAR CHANGE IN OTHER
      double * sompoolsfromr, // I don't know if I should change this to double * Let me know
      double * soiltexturefromr,// MADE NEW VAR CHANGE IN OTHER)    
      double * LAId_c, /********Start of output vars******/
      double * RhizomeLitterD,  
      double * RootLitterD,  
      double * LeafLitterD,
      double * StemLitterD,
      double * RhizomeD,
      double * RootD,
      double * StemD,
      double * LeafD,
      double * leafDarkResp,
      double * rhizomeMResp,
      double * rootMResp,
      double * stemMResp,
      double * AutoResp,
      double * npp,
      double * gpp,
      int * dayafterplanting,
     
      double * leafpsimvec,
      double * soilevaporation,
      double * minnitrovec,
      double * spvec,
      double * alphavec,
      double * vmaxvec,
      double * leafreductioncoefs,
      double * stomatalcondcoefs,
      double * abovelitter,
      double * belowlitter,
      double * respvec,
      double * soilwatcont,
      double * leafnitrogen,
      double * GDD_c,
      double * TTTc_c,
      double * LAIc_c,
      double * grainy,
      double * leafy,
      double * stemy, 
      double * rooty, 
      double * rhizomey,
      double * canopytrans,
      double * canopyassim,
      double * dayofyear,
      double * hour,
      double ** cwsmatrix,
      double ** psimmatrix,
      double ** rdmatrix,
      double ** waterfluxmatrix,
     
      double * snPools,
      double * scPools
      )
{
  
    /****Local Variables***/  
    //double ** cwsmatrix, ** psimmatrix,** rdmatrix, ** waterfluxmatrix;
    int dailyvecsize;
    double TEMPdoubletoint;
    double iSp;
    int i, i2, i3;    
  //  double * TTTc_c;
    double waterCont;
    double chil;
    double CanopyA, CanopyT;
  	double Leaf, Stem, Root, LAI, Grain, TTc, kLeaf, kStem,kRoot, kRhizome, kGrain;
    double newLeaf, newStem, newRoot, newRhizome, newGrain;
    double LeafLitter, StemLitter, RootLitter, RhizomeLitter, LeafLitter_d, StemLitter_d, RootLitter_d, RhizomeLitter_d;
    double ALitter, BLitter;
    double mrc1, mrc2;
    double StomWS, LeafWS;
//Soil Params
    double FieldC, WiltP, phi1, phi2, soilDepth;
  	double LeafN;
  	double soilEvap, TotEvap;
    
    double cwsVecSum;
    double rfl, rsec, rsdf, scsf, transpRes, leafPotTh, LeafPsim;

    double MinNitro;
    int doyNfert;
    double Nfert;
    double SCCs[9];
    double Resp;
    double SeneLeaf, SeneStem, SeneRoot, SeneRhizome;
    double *sti , *sti2, *sti3, *sti4;
    double Remob;
    int x;
    int k, q, m, n, ri;
    int DayCentSoilTexture,SoilClassification;
    double  accumulatedGDD;
//    double *GDD_c;
    double hf;
    double LeafResp,StemResp,RootResp,RhizResp;
    double gRespCoeff, dailydelTT, delTT, Tbase;
    int dap;
    int emergence;
    int phototype;
    int FlagBiogeochem;
    
   /*********** CROCENT VARIABLES***********************/
   struct cropcentlayer CROPCENT;
   double *fake;    
    
    

      
        
    

     
   double oldstandingN, newstandingN, Ndemand;   
   struct crop_phenology cropdbp;
   struct miscanthus miscanthus, deltamiscanthus;
   
   
   

//   miscanthus.leafvec[vecsize].newbiomass=(double)vecsize;
//   Rprintf("%f, %i\n",miscanthus.leafvec[vecsize].newbiomass, vecsize);
   double dailynetassim, CanopyAGross, dailyGPP;
   double mrespLeaf, mrespStem, mrespRoot, mrespRhizome;
   struct senthermaltemp senthermaltemp;
   struct canopyparms canopyparms;
   struct frostParms frostparms;
   struct nitroParms nitroparms;
   struct respirationParms RESP;
   
   struct Can_Str Canopy;
   struct ws_str WaterS;
   struct dbp_str dbpS;
   struct cenT_str centS; 
   struct soilML_str soilMLS;
   struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
   struct dailyclimate dailyclimate;
  /***********Management Variables *************/
   struct management management;
   
   
   
   /********START OF DECLARATIONS********/
   
   dailynetassim=0.0;
   dailyGPP=0.0;
   mrespLeaf=0.0;
   mrespStem=0.0;
   mrespRoot=0.0;
   mrespRhizome=0.0;
   frostparms.leafT0=-8.0;
   frostparms.leafT100=-16.0;
   frostparms.stemT0=-16.0;
   frostparms.stemT100=-16.0;
   Grain = 0.0;
   TTc = 0.0;
   kLeaf = 0.0; 
   kStem = 0.0; 
   kRoot = 0.0;
   kRhizome = 0.0;
   kGrain = 0.0; 
   newStem = 0.0;
   newGrain = 0.0;
   LeafLitter_d = 0.0;
   StemLitter_d = 0.0;
   RootLitter_d = 0.0;
   RhizomeLitter_d = 0.0;
	 ALitter = 0.0;
   BLitter = 0.0;
   StomWS = 1;
   LeafWS = 1;
   LeafPsim = 0.0;
   Resp = 0.0;
   SeneRoot = 0.0;
   SeneRhizome = 0.0 ;
   k = 0;
   q = 0;
   m = 0;
   n = 0;
	 ri = 0;
   cwsVecSum = 0.0;
   gRespCoeff = 0.0;
   dailydelTT = 0.0;
   Tbase=0.0;
   dailynetassim=0.0;
   canopyparms.kN=0.1;
   canopyparms.SLA=0.1;
   canopyparms.remobFac=0.1;
   canopyparms.leafNsen=40;
   frostparms.leafT0=-20.0; //REAL(FROSTP)[0];
   frostparms.leafT100=-20.0;//REAL(FROSTP)[1];
   frostparms.stemT0=-20.0;//REAL(FROSTP)[2];
   frostparms.stemT100=-20.0;//REAL(FROSTP)[3];
   emergence=0;
   canopyparms.remobFac=0.5;
   dap=0;
   TTc=0.0;
   phototype=1;
   CROPCENT.ENV.minN=12.0; //120*1000/10000 g N/m2
//Useless variables    
   centS.SCs[0] = 0.0;
	 centS.SCs[1] = 0.0;
	 centS.SCs[2] = 0.0;
	 centS.SCs[3] = 0.0;
	 centS.SCs[4] = 0.0;
	 centS.SCs[5] = 0.0;
	 centS.SCs[6] = 0.0;
	 centS.SCs[7] = 0.0;
	 centS.SCs[8] = 0.0;
	 centS.Resp = 0.0;
   // Declaring Daily variables
   accumulatedGDD=0.0;
   /***Initialize Daily Variables *********/
   RESP.growth.stem=0.3;
   RESP.growth.root=0.3;
   RESP.growth.rhizome=0.3;
   RESP.maint.Qstem=2.0;
   RESP.maint.mstem=0.004;
   RESP.maint.Qroot=2.0;
   RESP.maint.mroot=0.002;
   RESP.maint.Qrhizome=2.0;
   RESP.maint.mrhizome=0.002;
  //  Resp.growth.stem=0.3;
   // Initializing daily soil layer flux to Zero
   for(i3=0;i3<soillayers;i3++)
          {
            soilMLS.dailyWflux[i3]=0.0;  
          }
  
  
  
  
   

   dailyvecsize = vecsize/24;
      
   
   
   
   
   
   /********************************************/
   
	/* This creates vectors which will collect the senesced plant
	   material. This is needed to calculate litter and therefore carbon
	   in the soil and then N in the soil. */



  /*Reading NitroP Variables */
	
	
	nitroparms.ileafN=nnitrop[0];
  nitroparms.kln=nnitrop[1];
	nitroparms.Vmaxb1=nnitrop[2];
	nitroparms.Vmaxb0=nnitrop[3];
	nitroparms.alphab1=nnitrop[4];
	nitroparms.alphab0=nnitrop[5];
  nitroparms.Rdb1=nnitrop[6];
	nitroparms.Rdb0=nnitrop[7];
	nitroparms.kpLN=nnitrop[8];
	nitroparms.lnb0=nnitrop[9];
	nitroparms.lnb1=nnitrop[10];
	TEMPdoubletoint=nnitrop[11];
	nitroparms.lnFun=(int)TEMPdoubletoint;
	nitroparms.maxln=nnitrop[12];
	nitroparms.minln=nnitrop[13];
	nitroparms.daymaxln=nnitrop[14];


///////////////////////////////////////////////////////////////////	



  
  
  
  

	/* Variables needed for collecting litter */
	LeafLitter = centcoefs[20];
  StemLitter = centcoefs[21];
	RootLitter = centcoefs[22];
  RhizomeLitter = centcoefs[23];
	
	/* Maintenance respiration */

	mrc1 = mresp[0];
	mrc2 = mresp[1]; 

	

  
  
	




  
 
  
	
	/* Some soil related empirical coefficients */
	rfl = secs[0];  /* root factor lambda */
	rsec = secs[1]; /* radiation soil evaporation coefficient */
	rsdf = secs[2]; /* root soil depth factor */
	scsf = soilcoefs[6]; /* stomatal conductance sensitivity factor */ /* Rprintf("scsf %.2f",scsf); */
	transpRes = soilcoefs[7]; /* Resistance to transpiration from soil to leaf */
	leafPotTh = soilcoefs[8]; /* Leaf water potential threshold */

        /* Parameters for calculating leaf water potential */


        /* Effect of Nitrogen */
	


	/* Century */
	MinNitro = centcoefs[19];
	doyNfert = centcoefs[18];


  
  


	
	soTexS = soilTchoose(soilType);
  soTexS.sand=soiltexturefromr[0];
  soTexS.silt=soiltexturefromr[1];
  soTexS.clay=soiltexturefromr[2];
  // We need to define DayCentSoilTexture before making a call to getsoilprop.
  //Getsoilprop is an old C function that need texture as a parameters
  // By default, we are setting it to medium texture value 2 as per definition in getsoilprop.c
  // This does not change anything because calculations are based on sand/silt/clay content
  //This step is necessary only to be consistent with function call and avoid error message
  //" Conditional jump or move depends on uninitialised value(s)" when running under valgrind
  DayCentSoilTexture=2; 
  //Based on Texture, Get Bulk Density, Field Capacity, and DayCent Soil Type
  getsoilprop(&soTexS.sand, &soTexS.silt, &soTexS.clay, &soTexS.bulkd,&soTexS.fieldc, &DayCentSoilTexture,&SoilClassification);
//  Filling_BioCro_SoilStructure(&soilMLS, &soTexS, soillayers,REAL(SOILDEPTHS));

	LeafN = LeafN_0; /* Initial value of N in the leaf */


	
 
	SeneLeaf = sencoefs[0];
	SeneStem = sencoefs[1];
	SeneRoot = sencoefs[2];
	SeneRhizome = sencoefs[3];

	/* Soil Parameters */
	FieldC = soilcoefs[0];
	WiltP = soilcoefs[1];
	phi1 = soilcoefs[2];
	phi2 = soilcoefs[3];
	soilDepth = soilcoefs[4];
	waterCont = soilcoefs[5];


  // Modify SOil Type Based on Texture Information entered
  soilType=SoilClassification; // DayCent Soil Type goes from 1-11, and BioCro from 0-10
  if(soilType<0 || soilType > 10)
  { 
    soilType=1;
  } // This is a temporary fix to avoid unknown SoilType

	SCCs[0] = centcoefs[0];
	SCCs[1] = centcoefs[1];
	SCCs[2] = centcoefs[2];
	SCCs[3] = centcoefs[3];
	SCCs[4] = centcoefs[4];
	SCCs[5] = centcoefs[5];
	SCCs[6] = centcoefs[6];
	SCCs[7] = centcoefs[7];
	SCCs[8] = centcoefs[8];


	chil = chilhf[0];
	hf = chilhf[1];


 
 
  
 // senthermaltemp.leafcriticalT=REAL(SENCOEFS)[0]; //change Made!!!
  senthermaltemp.leafcriticalT=SeneLeaf;
  senthermaltemp.stemcriticalT=SeneStem;
  senthermaltemp.rootcriticalT=SeneRoot;
  senthermaltemp.rhizomecriticalT=SeneRhizome;

 
  TTTc_c[0]=TTc;
  
  
  
 /**************************************/
 //    Rprintf("%i\n",vecsize);

   // Get Default parameters for miscanthus
     assignParms(&CROPCENT, fake);
  // Get Initial Values oof Pool for C and N
     assignPools(&CROPCENT, sompoolsfromr);
  // More parameters   
     GetBioCroToCropcentParms(&CROPCENT.BcroTOCentParms,fake);
   // Timestep is alreadt set to 1440.0 minutes (1 day) in the assignParms. We need to change the parameters to daily time step
     CROPCENTTimescaling(&CROPCENT);
   
   
   
   
   createNULLmiscanthus(&miscanthus,vecsize);
   

   assignManagement(&management);
  updateafteremergence(&miscanthus,&management);
  LAI = miscanthus.leaf.biomass*Sp;
  
  miscanthus.rhizome.biomass=management.emergenceparms.plantingrate;
  
  
for(i=0;i<vecsize;i++)
//    for(i=0;i<3;i++)
  {
		/* First calculate the elapsed Thermal Time*/
		/* The idea is that here I need to divide by the time step
		   to calculate the thermal time. For example, a 3 hour time interval
		   would mean that the division would need to by 8 */
//     delTT=*(pt_temp+i) / (24/timestep);
        if(i==0)
            {
              TTc=0.0;
              TTTc_c[i]=0.0;
            }
        else
            {
              delTT=getThermaltime(*(pt_temp+i), Tbase);
              delTT=delTT/24;
              TTc +=delTT;
              TTTc_c[i] =TTc;
            }
        if(emergence==0)
            {
            
            CanopyA = 0.0;
            CanopyAGross =0.0;
        		CanopyT = 0.0;
            miscanthus.autoresp.leafdarkresp=0;
            }
        else
            {
//         Rprintf("Before Canopy Function, Phototype = %i, i= %i, Assim=%f, Leaf=%f, LAI=%f, Specific Leaf Area = %f \n", phototype,i, Canopy.Assim, miscanthus.leaf.biomass, LAI,Sp);
  	        Canopy = CanAC(LAI, *(pt_doy+i), *(pt_hr+i),
			       *(pt_solar+i), *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,alpha1,kparm1,
			       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws, kd,
			       chil, hf,LeafN, kpLN, lnb0, lnb1, lnfun,upperT,lowerT,nitroparms);
        		CanopyA = Canopy.Assim * timestep;
            CanopyAGross =Canopy.GrossAssim*timestep;
        		CanopyT = Canopy.Trans * timestep;
            }
		/* Inserting the multilayer model */
		  if(soillayers > 1)
            {
        		soilMLS = soilML(*(pt_precip+i), CanopyT, &cwsVec[0], soilDepth, soildepths, FieldC, WiltP,
        	            phi1, phi2, soTexS, wsFun, soillayers, miscanthus.root.biomass, 
        					    LAI, 0.68, *(pt_temp+i), *(pt_solar), *(pt_windspeed+i), *(pt_rh+i), 
        					    hydrdist, rfl, rsec, rsdf);

            StomWS = soilMLS.rcoefPhoto;
            LeafWS = soilMLS.rcoefSpleaf;
            soilEvap = soilMLS.SoilEvapo;
              			for(i3=0;i3<soillayers;i3++)
                      {
              				cwsVec[i3] = soilMLS.cws[i3];
              				cwsVecSum += cwsVec[i3];
              				//REAL(cwsMat)[i3 + i*soillayers] = soilMLS.cws[i3];
                      cwsmatrix[i][i3] = soilMLS.cws[i3];
                      //REAL(rdMat)[i3 + i*soillayers] = soilMLS.rootDist[i3];
                      rdmatrix[i][i3] = soilMLS.rootDist[i3];
                      //REAL(waterfluxMat)[i3 + i*soillayers] = soilMLS.hourlyWflux[i3];
                      waterfluxmatrix[i][i3] = soilMLS.hourlyWflux[i3];
                      soilMLS.dailyWflux[i3]+=soilMLS.hourlyWflux[i3];
              			  }

			      waterCont = cwsVecSum / soillayers;
			      cwsVecSum = 0.0;

		      }
      else
          {

      			soilEvap = SoilEvapo(LAI, 0.68, *(pt_temp+i), *(pt_solar+i), waterCont, FieldC, WiltP, 
                                                   *(pt_windspeed+i), *(pt_rh+i), rsec);
      			TotEvap = soilEvap + CanopyT;
      			WaterS = watstr(*(pt_precip+i),TotEvap,waterCont,soilDepth,FieldC,WiltP,phi1,phi2,soilType, wsFun);   
      			waterCont = WaterS.awc;
      			StomWS = WaterS.rcoefPhoto ; 
      			LeafWS = WaterS.rcoefSpleaf;
      			//REAL(cwsMat)[i] = waterCont;
            cwsmatrix[i][0] = waterCont;
            //REAL(psimMat)[i] = WaterS.psim;
            psimmatrix[i][0] = WaterS.psim;
      		}
          
          
/**** this can be moved to a separate function **********************************/                                    
/****************Evaluating Daily Maintenance R espiration and Gross canopy assimilation******************/

  StemResp=MRespiration(miscanthus.stem.biomass, RESP.maint.Qstem, RESP.maint.mstem, *(pt_temp+i), timestep);
  miscanthus.autoresp.stemmaint+=StemResp;
  RootResp=MRespiration(miscanthus.root.biomass, RESP.maint.Qroot, RESP.maint.mroot, *(pt_temp+i), timestep);
  miscanthus.autoresp.rootmaint+=RootResp;
  RhizResp=MRespiration(miscanthus.rhizome.biomass, RESP.maint.Qrhizome, RESP.maint.mrhizome, *(pt_temp+i), timestep);
  miscanthus.autoresp.rhizomemaint+=RhizResp;
  miscanthus.autoresp.leafdarkresp+=(CanopyAGross-CanopyA);
  dailynetassim+=CanopyA;//Net Canopy Assimilation
  miscanthus.GPP+=CanopyAGross;



   if(i % 24== 0)
   {
     /*************************************************************************************************************
      * Now we are entering in the daily loop, for plant biomass updating as well as for biogeochemistry calculations
      * In the daily loop,we need
      * --(1) daily climate data for calculation of biogeochemical cycle.Function call to getdailyclimate is made to update structure dailyclimate
      * --(2) dailyThermalTime (GDD) is also updated, this is used in main dailygrowth function of miscanthus
      * --(3) dailymiscanthus function is called to update the biomass of plant for the current day, along with logical variable emergence
      * --(4) LAI is updated based on current biomass of miscanthus leaf and SLA
      * LAI CAN BE INCLUDED IN THE MISCANTHUS STRUCTURE TO AVOID AN EXTRA HANGING LINE OF CALCULATIONS, CALL TO CANOPY  MUST BE MADE USING 
      * MISCANTHUS->LAI INSTEAD OF SIMPLY LAI
      * *************************************************************************************************************/
     getdailyclimate(&dailyclimate, pt_doy,pt_solar,pt_temp, pt_rh, pt_windspeed,pt_precip,i,vecsize);  
     dailydelTT = (emergence ==1) ? getThermaltime(dailyclimate.temp, Tbase):0.0;
     accumulatedGDD+=dailydelTT; 
     dailymiscanthus(&miscanthus, dbpcoefs,thermalp,accumulatedGDD, *(pt_temp+i), dailynetassim,&senthermaltemp, &canopyparms,&frostparms,i,dailydelTT,&RESP,emergence); 
     LAI=miscanthus.leaf.biomass*Sp;
             /*****************************************************************************************************************
             * If plant is already emerged then 
             * --
             * --Test if today is harvest day, If no set back emergence to zero and update miscanthus structure[ usually leaving zero above ground biomass or a fraction]
             * Else
             * --
             * --We need to check if today is the emergence date based on logical value returned by CheckEmergence Function
             * -- (1)If today indeed is emergence date then initialuze leaf biomass by calling updateafteremergence
             * ---(2)also, emergence mean we need to set accumulated GDD back to zero so paritioning calendar is reset
             * -- (3) setting initial LAI based on initial fraction to leaf and SLA
             * ---(4) Fertilization on the date of emergence
             * THIS WHOLE IF ELSE LOOP CAN BE REPLACED BY A FUNCTION CALL WHICH TAKED CURRENT DAY,& SCHEDULE DATA FRAME (E.G. DATE OF HARVEST, FERTILIZATION)
             * TO UPDATE PLANT BIOMASS POOLS AND SOIL BIOGEOCHEMICAL POOLS
             * ***************************************************************************************************************/   
                        if(emergence==1)
                        {
                               if(dailyclimate.doy==management.harvestparms.harvestdoy)
                              {
                                emergence=0;                        //Emergence is set back to zero
                                GDD_c[dap]=0.0;                 //Set GDD back to zero to restart phenology from beginning
                                updateafterharvest(&miscanthus,&management); // Use harvest parameters to implement pracices such as removingor leaving residues 
                              }         
                        }
                      
                        else
                        {             
                                emergence=CheckEmergence(&dailyclimate,management.emergenceparms.emergenceTemp); 
                                if((dailyclimate.doy==120)&&(phototype==2))emergence=1;
                                if(emergence==1)
                                {
                                updateafteremergence(&miscanthus,&management);
                                accumulatedGDD=0.0;
                                TTc=0.0;
                                LAI = miscanthus.leaf.biomass* Sp;
                                CROPCENT.ENV.minN+=12.0; // adding fertilization on the emergence day
                                }
                        }
          
              /******************************************************************************************************************************
               *  Base on a logical [FlagBiogeochem=1], following four steps will be performed. Or, productivity will not be influenced by 
               *  by N availability and no output of soil C and GHG will be available [ all zeros will be output]
               * 
               * (1) I need to copy soil water profile from BioCro function to CROPCENT.soil 
               * 
               * (2) Assign Average Soil Properties to each layer of the soilprofile of cropcentlayer
               * 
               * (2) Now we have updated plant biomass and litter content of each component.We need to input litter (based on a user defined 
               *     falling rate) to soil biogeochemical cycle and perform decomposition of soil organic pool for today. Important thing is to 
               *     use correct C:N ratio of litter, call SCHEDULING FUNCTION to modify decomposition rates as per tillage implementation and 
               *     addition of external Fertilizers and manures etc. Output of this exercise  will be updated pools of soil organic carbon and
               *     newmineral N in the top 15 cm layer.
               * 
               * (3) I can use newmineral N pool for distributing it into multilayers then canclulate GHG emission and N leaching, and distribution
               *     of mineral N in different soil layers.
               * 
               * (4) I can calculate demand of N for today and accordingly modify mineral N content of different soil layers.
               * 
               * (5) If There is not enough mineral N then C:N ratio of plant is modified, which will eventually result in lower productivity
               *     because photosynthesis parameters are a linear function of leaf N content. Thus Limited N will reduce productivity
               *
               * *****************************************************************************************************************************/
                
                FlagBiogeochem =1;// MOVE THIS UP & ALLOW IT TO VARY FROM R ENVIRONMENT
 

                if(FlagBiogeochem==1)
                {
//                Assign_Soil_Properties_To_CropCent(bulkd,swclimit,fieldc,pH,tcoeff, baseflow,stormflow,frlechd,&CROPCENT);
//                Copy_SoilWater_BioCro_To_CropCent(&soilMLS,&CROPCENT);
//                Rprintf("soilMLS.dpthmn[1]=%f,CROPCENT.soilprofile.properties.dpthmn[1]=%f\n",soilMLS.dpthmn[1],CROPCENT.soilprofile.properties.dpthmn[1]);
//                CalculateBiogeochem(&miscanthus, &CROPCENT,&dailyclimate);
                }
              
               /******************* This part can go to a Separate Function - Nremobilization**************************************************/
               
               /*********This Part is to Model N response on Productivity**********************************************************************
               *   D=getTodayNDemand(&miscanthus) all positive partitioning for nutrients or non-decreasing N concentration
               *   S=getTodayInternalNSupply(&miscanthus) due to mobilization from senescing plant organs or phenology (Rhizome in the beginning, leaf & Stem in the end)
               *   if(S>D) satisfy all demand and remaining S will go to rhizome (storage organ)
               *   if(S<D) && (D-S)< soil Mineral N total
               *   Meet all the D and reduce soil mineral N by (D-S)
               *   if (S<D) && (D-S) > soil mineral N total
               *   make soil Mineral N = 0
               *   reduce all the N supply bu a factor k such that k*D-S= soil mineral
               *   Meet only a fraction(k) of demand D and reduce nutrient concentration of all the elements
               *  *******************************************************************************************************************************   
               * This reduced C:N ratio of leaf will eventually reduce photosynthesis as photosynthesis parameters are expressed in terms of SLN
               * *******************************************************************************************************************************/   
      
         for(i3=0;i3<soillayers;i3++)
          {
            soilMLS.dailyWflux[i3]=0.0;  
          }
                        
       /*printcropcentout(CROPCENT,
                        &REAL(totalSOC)[dap],
                        &REAL(strucc1)[dap],
                        &REAL(strucc2)[dap],
                        &REAL(metabc1)[dap],
                        &REAL(metabc2)[dap],  Commented Out because only being used for debugging. 
                        &REAL(som1c1)[dap],
                        &REAL(som1c2)[dap],
                        &REAL(som2c1)[dap],
                        &REAL(som2c2)[dap],
                        &REAL(som3c)[dap],
                        &REAL(minN)[dap]);
*/

/***************************************************************************/
   gpp[dap]=miscanthus.GPP;
   leafDarkResp[dap]=miscanthus.autoresp.leafdarkresp;
   stemMResp[dap]=miscanthus.autoresp.stemmaint;
   rootMResp[dap]=miscanthus.autoresp.rootmaint;
   rhizomeMResp[dap]=miscanthus.autoresp.rhizomemaint;
   AutoResp[dap]= miscanthus.autoresp.total;
   miscanthus.NPP=miscanthus.GPP-miscanthus.autoresp.total;
   npp[dap]=miscanthus.NPP;
   StemD[dap]=miscanthus.stem.biomass;
   LeafD[dap]=miscanthus.leaf.biomass;
   RootD[dap]=miscanthus.root.biomass;
   RhizomeD[dap]=miscanthus.rhizome.biomass;
   StemLitterD[dap]=miscanthus.stem.litter;
   LeafLitterD[dap]=miscanthus.leaf.litter;
   RootLitterD[dap]=miscanthus.root.litter;
   RhizomeLitterD[dap]=miscanthus.rhizome.litter;
   dayafterplanting[dap]=dap;  
   GDD_c[dap]=accumulatedGDD;
   LAId_c[dap]=LAI;
   
    miscanthus.autoresp.leafdarkresp=0.0;
    miscanthus.autoresp.stemmaint=0.0;
    miscanthus.autoresp.rootmaint=0.0;
    miscanthus.autoresp.rhizomemaint=0.0;  
    miscanthus.GPP=0;
    miscanthus.NPP=0;
    dailynetassim=0.0;
    dailyGPP=0.0;
    dap+=1;
		}



		MinNitro = centS.MinN; /* These should be kg / m^2 per week? */
		Resp = centS.Resp;
		SCCs[0] = centS.SCs[0];
		SCCs[1] = centS.SCs[1];
		SCCs[2] = centS.SCs[2];
		SCCs[3] = centS.SCs[3];
		SCCs[4] = centS.SCs[4];
		SCCs[5] = centS.SCs[5];
		SCCs[6] = centS.SCs[6];
		SCCs[7] = centS.SCs[7];
		SCCs[8] = centS.SCs[8];



		ALitter = LeafLitter + StemLitter;
		BLitter = RootLitter + RhizomeLitter;
    
		/* Here I could add a soil and nitrogen carbon component. I have soil
		   moisture, I have temperature and root and rhizome biomass */

		dayofyear[i] =  pt_doy[i];
		hour[i] =  pt_hr[i];
		canopyassim[i] =  CanopyA;
		canopytrans[i] =  CanopyT; 
		leafy[i] = miscanthus.leaf.biomass; //Leaf;
		stemy[i] = miscanthus.stem.biomass; // Stem;
		rooty[i] =  miscanthus.root.biomass; //Root;
		rhizomey[i] = miscanthus.rhizome.biomass; //Rhizome;
		grainy[i] = Grain;
		LAIc_c[i] = LAI;
	  soilwatcont[i] = waterCont;
		stomatalcondcoefs[i] = StomWS;
		leafreductioncoefs[i] = LeafWS;
	  leafnitrogen[i] = LeafN;
		abovelitter[i] = ALitter;
		belowlitter[i] = BLitter;
		vmaxvec[i] = vmax1;
	  alphavec[i] = alpha1;
		spvec[i] = Sp;
		minnitrovec[i] = MinNitro/ (24*centTimestep);
		respvec[i] = Resp / (24*centTimestep);
		soilevaporation[i] = soilEvap;
		leafpsimvec[i] = LeafPsim;
    
}

/* Populating the results of the Century model */

		scPools[0] = centS.SCs[0];
		scPools[1] = centS.SCs[1];
		scPools[2] = centS.SCs[2];
		scPools[3] = centS.SCs[3];
		scPools[4] = centS.SCs[4];
		scPools[5] = centS.SCs[5];
		scPools[6] = centS.SCs[6];
		scPools[7] = centS.SCs[7];
		scPools[8] = centS.SCs[8];

		snPools[0] = centS.SNs[0];
		snPools[1] = centS.SNs[1];
		snPools[2] = centS.SNs[2];
		snPools[3] = centS.SNs[3];
		snPools[4] = centS.SNs[4];
		snPools[5] = centS.SNs[5];
		snPools[6] = centS.SNs[6];
		snPools[7] = centS.SNs[7];
		snPools[8] = centS.SNs[8];
    

}
