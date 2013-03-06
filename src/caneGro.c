/*
 *  BioCro/src/BioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2011
 *
 */

#include <R.h>
#include <math.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"
#include "Century.h"
#include "BioCro.h"
#include "caneGro.h"

SEXP caneGro(SEXP VECSIZE,             /* vector size              16 */
             SEXP TIMESTEP,            /* time step                15 */
	     SEXP PLANTINGDENSITY,     /* Planting density */
	     SEXP ILEAF,               /* Initial Fraction of Below-ground stem/seedcane to Leaves */
	     SEXP LAT,                /* Latitude                  1 */ 
	     SEXP DOY,                 /* Day of the year           2 */
	     SEXP HR,                  /* Hour of the day           3 */
	     SEXP SOLAR,               /* Solar Radiation           4 */
	     SEXP TEMP,                /* Temperature               5 */
	     SEXP RH,                  /* Relative humidity         6 */ 
	     SEXP WINDSPEED,           /* Wind Speed                7 */ 
	     SEXP PRECIP,              /* Precipitation             8 */
	     SEXP PHOTOP,              /* C4 Photo. Parameters  */
             SEXP CANOPYP,             /* Canopy Parameters     */
             SEXP PHENOP,              /* Phenology Parameters  */
             SEXP NITROP,              /* Leaf Nitrogen Parameters */
             SEXP SENEP,               /* Senescence Parameters   */
             SEXP SOILP,
             SEXP iWatCont,
	     SEXP SoilDepths
             )          
{

        int vecsize = INTEGER(VECSIZE)[0];        /*arg 1 */
	int timestep = INTEGER(TIMESTEP)[0];
        double iSeedcane=REAL(PLANTINGDENSITY)[0];
	double lat=REAL(LAT)[0];
        double ileaf=REAL(ILEAF)[0];

	double *newLeafcol=malloc(vecsize*sizeof(double));
                                  if(newLeafcol==NULL)
                                  {
                                    Rprintf("Out of Memory for newLeafcol\n");
                                    exit;
                                  }
        double *newStemcol=malloc(vecsize*sizeof(double));
                                  if(newStemcol==NULL)
                                  {
                                    Rprintf("Out of Memory for newStemcol\n");
                                    exit;
                                  }
        double *newRootcol=malloc(vecsize*sizeof(double));
                                  if(newRootcol==NULL)
                                  {
                                    Rprintf("Out of Memory for newRootcol\n");
                                    exit;
                                  }
        double *newSeedcanecol=malloc(vecsize*sizeof(double));
                                  if(newSeedcanecol==NULL)
                                  {
                                    Rprintf("Out of Memory for newSeedcanecoll\n");
                                    exit;
                                  }
         double *newSugarcol=malloc(vecsize*sizeof(double));
                                  if(newSugarcol==NULL)
                                  {
                                    Rprintf("Out of Memory for newSugarcol\n");
                                    exit;
                                  }	 	
	double vmax1=REAL(PHOTOP)[0];
	double alpha1=REAL(PHOTOP)[1];
	double kparm1=REAL(PHOTOP)[2];
	double theta=REAL(PHOTOP)[3];
	double beta=REAL(PHOTOP)[4];
	double Rd1=REAL(PHOTOP)[5];
	double Ca=REAL(PHOTOP)[6];
	double b01=REAL(PHOTOP)[7];
	double b11=REAL(PHOTOP)[8];
	double ws =REAL(PHOTOP)[9];
        double UPPERTEMP=REAL(PHOTOP)[10];
	double LOWERTEMP=REAL(PHOTOP)[11];

        double FieldC=REAL(SOILP)[0];
	double WiltP=REAL(SOILP)[1];
	double phi1=REAL(SOILP)[2];
	double phi2=REAL(SOILP)[3];
	double soilDepth=REAL(SOILP)[4];
//	int soilType=INTEGER(SOILP)[5];
	double soilType1=REAL(SOILP)[5];
	int soilType=(int)soilType1;
//	int soillayers=INTEGER(SOILP)[6];
	double soillayers1=REAL(SOILP)[6];
	int soillayers=(int)soillayers1;
//	int wsFun=INTEGER(SOILP)[7];
	double wsFun1=REAL(SOILP)[7];
        int wsFun=(int)wsFun1;
	double scsf=REAL(SOILP)[8];
	double transpRes=REAL(SOILP)[9];
	double leafPotTh=REAL(SOILP)[10];
//	int hydrDist=INTEGER(SOILP)[11];
	double hydrDist1=REAL(SOILP)[11];
        int hydrDist=(int)hydrDist1;
	double rfl=REAL(SOILP)[12];
	double rsec=REAL(SOILP)[13];
	double rsdf=REAL(SOILP)[14];
//	int optiontocalculaterootdepth=INTEGER(SOILP)[15];
	double optiontocalculaterootdepth1=REAL(SOILP)[15];
	int optiontocalculaterootdepth= (int) optiontocalculaterootdepth1;
	double rootfrontvelocity=REAL(SOILP)[16];
        double cwsVec[soillayers];
	double soildepths[soillayers];
        int ii;
	// Getting Soil Deptgs and Associated initial Soil Water content for multilayer soil Model
        for (ii=0;ii<=soillayers;ii++){
	cwsVec[ii]=REAL(iWatCont)[ii];
	soildepths[ii]=REAL(SoilDepths)[ii];
	}


        double waterCont=cwsVec[0]; /* In case we are using single layer soil water model */
        struct soilText_str soTexS; /* , *soTexSp = &soTexS; */
	soTexS = soilTchoose(soilType);

        double cwsVecSum = 0.0;
        double LeafPsim = 0.0; /* Parameter to calculate leaf water potential */

	double Sp=REAL(CANOPYP)[0];
	double SpD=REAL(CANOPYP)[1];
	int nlayers=INTEGER(CANOPYP)[2];
	double kd=REAL(CANOPYP)[3];
	double chil=REAL(CANOPYP)[4];
	double gRespCoeff=REAL(CANOPYP)[5];
	double Qleaf=REAL(CANOPYP)[6];
	double Qstem=REAL(CANOPYP)[7];
	double Qroot=REAL(CANOPYP)[8];
	double mRespleaf=REAL(CANOPYP)[9];
        double mRespstem=REAL(CANOPYP)[10];
        double mResproot=REAL(CANOPYP)[11];
	double hf=REAL(CANOPYP)[12];


	double TT0 = REAL(PHENOP)[0];
        double TTseed = REAL(PHENOP)[1];
        double Tmaturity = REAL(PHENOP)[2];
        double Rd = REAL(PHENOP)[3];
        double Alm = REAL(PHENOP)[4];
        double Arm = REAL(PHENOP)[5];
        double Clstem = REAL(PHENOP)[6];
        double Ilstem = REAL(PHENOP)[7];
        double Cestem = REAL(PHENOP)[8];
        double Iestem = REAL(PHENOP)[9];
        double Clsuc = REAL(PHENOP)[10];
        double Ilsuc = REAL(PHENOP)[11];
        double Cesuc = REAL(PHENOP)[12];
        double Iesuc = REAL(PHENOP)[13];
        double Tbase = REAL(PHENOP)[14];

        double SeneLeaf=REAL(SENEP)[0];
	double SeneStem= REAL(SENEP)[1];
	double SeneRoot=REAL(SENEP)[2];
	double SeneSeedcane=REAL(SENEP)[3];
	double leafrate=REAL(SENEP)[4];
	double rootrate=REAL(SENEP)[5];
	double stemrate=REAL(SENEP)[6];
	double leafremobilizefraction=REAL(SENEP)[7];
        double stemremobilizefraction=REAL(SENEP)[8];
        double rootremobilizefraction=REAL(SENEP)[9];
	double seedcaneremobilizefraction=REAL(SENEP)[10];
	// double to integer
	double option1=INTEGER(SENEP)[11];
        int option=(int)option1;
        double Tfrost1=REAL(SENEP)[12];
	double Tfrost2=REAL(SENEP)[13];
	double minleafN=REAL(SENEP)[14];
        

	double LeafN=REAL(NITROP)[0];
        double kLN=REAL(NITROP)[1];
	double vmaxb1=REAL(NITROP)[2];
	double vmaxb0=REAL(NITROP)[3];
	double alphab1=REAL(NITROP)[4];
	double alphab0=REAL(NITROP)[5];
	double kpLN=REAL(NITROP)[6];
	double lnb0=REAL(NITROP)[8];
	double lnb1=REAL(NITROP)[9];
	double lnFun1=REAL(NITROP)[7];
	int lnfun=(int)lnFun1;
	double LeafNmax=REAL(NITROP)[10];
	double LeafNmin=REAL(NITROP)[11];
	double dayofMaxLeafN=REAL(NITROP)[12];


	SEXP lists, names;
	SEXP DayofYear;
	SEXP Hour;
	SEXP CanopyAssim;
	SEXP CanopyTrans;
	SEXP Leafy;
	SEXP Stemy;
	SEXP Rooty;
	SEXP Sugary;
	SEXP Fibery;
	SEXP Seedcaney;
	SEXP LAIc;
	SEXP TTTc;
	SEXP SoilWatCont;
	SEXP StomatalCondCoefs;
	SEXP LeafReductionCoefs;
	SEXP LeafNitrogen;
	SEXP AboveLitter;
	SEXP BelowLitter;
	SEXP VmaxVec;
	SEXP AlphaVec;
	SEXP SpVec;
	SEXP SoilEvaporation;
	SEXP cwsMat;
	SEXP psimMat; /* Holds the soil water potential */
	SEXP rdMat;
	SEXP LeafPsimVec;
        SEXP LeafLittervec;
        SEXP StemLittervec;
	SEXP RootLittervec;
	SEXP Leafmaintenance;
        SEXP Stemmaintenance;
        SEXP Rootmaintenance;
        SEXP Seedcanemaintenance;
	SEXP kkleaf;
        SEXP kkroot;
        SEXP kkstem;
	SEXP kkseedcane;
	SEXP kkfiber;
        SEXP kksugar;
        SEXP dailyThermalT;
	SEXP moisturecontent;
	SEXP dayafterplanting;

	vecsize = length(DOY);
	PROTECT(lists = allocVector(VECSXP,41));
	PROTECT(names = allocVector(STRSXP,41));

	PROTECT(DayofYear = allocVector(REALSXP,vecsize));
	PROTECT(Hour = allocVector(REALSXP,vecsize));
	PROTECT(CanopyAssim = allocVector(REALSXP,vecsize));
	PROTECT(CanopyTrans = allocVector(REALSXP,vecsize));
	PROTECT(Leafy = allocVector(REALSXP,vecsize));
	PROTECT(Stemy = allocVector(REALSXP,vecsize));
	PROTECT(Rooty = allocVector(REALSXP,vecsize));
	PROTECT(Sugary = allocVector(REALSXP,vecsize));
	PROTECT(Fibery = allocVector(REALSXP,vecsize));
	PROTECT(Seedcaney = allocVector(REALSXP,vecsize));
	PROTECT(LAIc = allocVector(REALSXP,vecsize));
	PROTECT(TTTc = allocVector(REALSXP,vecsize));
	PROTECT(SoilWatCont = allocVector(REALSXP,vecsize));
	PROTECT(StomatalCondCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafReductionCoefs = allocVector(REALSXP,vecsize));
	PROTECT(LeafNitrogen = allocVector(REALSXP,vecsize));
	PROTECT(AboveLitter = allocVector(REALSXP,vecsize));
	PROTECT(BelowLitter = allocVector(REALSXP,vecsize));
	PROTECT(VmaxVec = allocVector(REALSXP,vecsize));
	PROTECT(AlphaVec = allocVector(REALSXP,vecsize));
	PROTECT(SpVec = allocVector(REALSXP,vecsize));
	PROTECT(SoilEvaporation = allocVector(REALSXP,vecsize));
	PROTECT(cwsMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(psimMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(rdMat = allocMatrix(REALSXP,soillayers,vecsize));
	PROTECT(LeafPsimVec = allocVector(REALSXP,vecsize));
        PROTECT(LeafLittervec = allocVector(REALSXP,vecsize));
	PROTECT(StemLittervec = allocVector(REALSXP,vecsize));
	PROTECT(RootLittervec = allocVector(REALSXP,vecsize));
	PROTECT(Leafmaintenance = allocVector(REALSXP,vecsize));
	PROTECT(Stemmaintenance = allocVector(REALSXP,vecsize));
	PROTECT(Rootmaintenance = allocVector(REALSXP,vecsize));
	PROTECT(Seedcanemaintenance = allocVector(REALSXP,vecsize));
	PROTECT(kkleaf = allocVector(REALSXP,vecsize));
	PROTECT(kkroot = allocVector(REALSXP,vecsize));
	PROTECT(kkstem = allocVector(REALSXP,vecsize));
	PROTECT(kkseedcane = allocVector(REALSXP,vecsize));
	PROTECT(kkfiber = allocVector(REALSXP,vecsize));
	PROTECT(kksugar = allocVector(REALSXP,vecsize));
	PROTECT(dailyThermalT = allocVector(REALSXP,vecsize));
	PROTECT(moisturecontent = allocVector(REALSXP,vecsize));
	PROTECT(dayafterplanting = allocVector(REALSXP,vecsize));


	double iSp, propLeaf;
	int i, i2, i3;
	int dailyvecsize;
	double Leaf, Stem, Root, LAI, Grain = 0.0;
	double TTc = 0.0,TT12c=0.0;
	double kLeaf = 0.0, kStem = 0.0, kRoot = 0.0, kRhizome = 0.0, kGrain = 0.0;
	double newLeaf, newStem = 0.0, newRoot, newRhizome, newGrain = 0.0;
	double StomWS = 1, LeafWS = 1;
	double CanopyA, CanopyT;
       	double Sugar,sugarstress,Seedcane;
	double laimax;
	double currentday;
	double soilEvap, TotEvap;
	double *sti , *sti2, *sti3, *sti4, *sti5; 
	double Remob;
	int k = 0, q = 0, m = 0, n = 0;
	int ri = 0;
	double LeafN_0=LeafN;
	double vmax0=vmax1,alpha0=alpha1;

	struct Can_Str Canopy,CanopyRd;
	struct ws_str WaterS;
	struct dbp_sugarcane_str dbps={0,0,0,0,0,0,0};
	struct cenT_str centS; 
	struct soilML_str soilMLS;

	propLeaf=REAL(ILEAF)[0];
	Leaf=iSeedcane*propLeaf;
	Stem = iSeedcane * 0.001;
	Root = iSeedcane* 0.001;
	LAI = Leaf * Sp;
	iSp = Sp;

/* Creating pointers to avoid calling functions REAL and INTEGER so much */
	int *pt_doy = INTEGER(DOY);
	int *pt_hr = INTEGER(HR);
	double *pt_solar = REAL(SOLAR);
	double *pt_temp = REAL(TEMP);
	double *pt_rh = REAL(RH);
	double *pt_windspeed = REAL(WINDSPEED);
	double *pt_precip = REAL(PRECIP);


	/* Creation of pointers outside the loop */
	sti = &newLeafcol[0]; /* This creates sti to be a pointer to the position 0
				 in the newLeafcol vector */
	sti2 = &newStemcol[0];
	sti3 = &newRootcol[0];
	sti4 = &newSeedcanecol[0];
        sti5 = &newSugarcol[0];
        
	double dailyassim=0.0;
	double dailytransp=0.0;
	double dailyLeafResp = 0.0;
        double dailyStemResp = 0.0;
	double dailyRootResp = 0.0;
	double dailyRhizomeResp = 0.0;
	double LeafResp,StemResp,RootResp,RhizomeResp,Litter;
	double LAIi;
	double delTT;
	double dap,moist;

	double rootlitter,deadroot,leaflitter,deadleaf,leafREMOB;
	double AboveLittery,BelowLittery,cswMaty,psiMaty;
	double Fiber,newFiber;
	double kSugar,kFiber,SeedcaneResp,newSeedcane,dailySeedcaneResp,kSeedcane;
	double LAIold,LAInew;
	double newSugar,ALitter,BLitter;
	int dayi=0; /* this is idnex for days */
            LAIi = iSeedcane* propLeaf*Sp;
	    LAIold=LAIi; /* Enter simulation with LAIold variable to track precious LAI to implement water stress */
      
 
	for(i=0;i<vecsize;i++)
	{	
                TTc += *(pt_temp+i) / (24/timestep); 
                if((*(pt_temp+i))<Tbase)
                {
			TT12c = TT12c;
			delTT=0.0;
		}
               
		if((*(pt_temp+i))>= Tbase)
                {
			TT12c = TT12c + (*(pt_temp+i)-Tbase) / (24/timestep);
			delTT=(*(pt_temp+i)- Tbase)/24.0;
		}

                REAL(TTTc)[i]=TT12c;

		//  Do the magic! Calculate growth
                //  Think about how to implement some parameters such as option to calculate leaf N dynamics

		Canopy = CanAC(LAI, *(pt_doy+i), *(pt_hr+i),
			       *(pt_solar+i), *(pt_temp+i),
			       *(pt_rh+i), *(pt_windspeed+i),
			       lat, nlayers,
			       vmax1,alpha1,kparm1,
			       theta,beta,Rd1,Ca,b01,b11,StomWS,
			       ws, kd,
			       chil, hf,
                               LeafN, kpLN, lnb0, lnb1, lnfun,UPPERTEMP,LOWERTEMP);

		// Collecting the results 
		CanopyA = Canopy.Assim * timestep;
		CanopyT = Canopy.Trans * timestep;
                
                 // summing up to get daily assimilation (NOT HOURLY)
		 dailyassim =  dailyassim + CanopyA;
	         dailytransp = dailytransp + CanopyT;

		 // Evaluate maintenance Respiration of different plant organs 
		   LeafResp=MRespiration(Leaf, Qleaf, mRespleaf, *(pt_temp+i), timestep);
		   REAL(Leafmaintenance)[i]=LeafResp;
		   StemResp=MRespiration(Fiber, Qstem, mRespstem, *(pt_temp+i), timestep);
                   REAL(Stemmaintenance)[i]=StemResp;
	           RootResp=MRespiration(Root, Qroot, mResproot, *(pt_temp+i), timestep);
		   REAL(Rootmaintenance)[i]=RootResp;
                   SeedcaneResp=MRespiration(Seedcane, Qstem, mRespstem, *(pt_temp+i), timestep);
		   REAL(Seedcanemaintenance)[i]=SeedcaneResp;
                
		   // summing up hourly maintenance Respiration to determine daily maintenance respiration of each plant organ 
		    dailyLeafResp =  dailyLeafResp + LeafResp;
		    dailyStemResp =  dailyStemResp + StemResp;
		    dailyRootResp =  dailyRootResp + RootResp;
		    dailySeedcaneResp =  dailySeedcaneResp + SeedcaneResp;
                   
	               // Inserting the multilayer model 
		       // Need to think about playing with additional option for root front velosity
			if(soillayers > 1){
			dap=(double)dayi; // converting dayi to double before passing to soilML to avoid rounding error
			soilMLS = soilML_rootfront(*(pt_precip+i), CanopyT, &cwsVec[0], soilDepth, soildepths, FieldC, WiltP,
					 phi1, phi2, soTexS, wsFun, soillayers, Root, 
					 LAI, 0.68, *(pt_temp+i), *(pt_solar), *(pt_windspeed+i), *(pt_rh+i), 
					 hydrDist, rfl, rsec, rsdf,optiontocalculaterootdepth,rootfrontvelocity,dap);

			StomWS = soilMLS.rcoefPhoto;
			LeafWS = soilMLS.rcoefSpleaf;
			soilEvap = soilMLS.SoilEvapo;
			for(i3=0;i3<soillayers;i3++){
				cwsVec[i3] = soilMLS.cws[i3];
				cwsVecSum += cwsVec[i3];
				REAL(cwsMat)[i3 + i*soillayers] = soilMLS.cws[i3];
				REAL(rdMat)[i3 + i*soillayers] = soilMLS.rootDist[i3];
			}

			waterCont = cwsVecSum / soillayers;
			cwsVecSum = 0.0;

		}else{

			soilEvap = SoilEvapo(LAI, 0.68, *(pt_temp+i), *(pt_solar+i), waterCont, FieldC, WiltP, 
                                             *(pt_windspeed+i), *(pt_rh+i), rsec);
			TotEvap = soilEvap + CanopyT;
			WaterS = watstr(*(pt_precip+i),TotEvap,waterCont,soilDepth,FieldC,WiltP,phi1,phi2,soilType, wsFun);   
			waterCont = WaterS.awc;
			StomWS = WaterS.rcoefPhoto ; 
			LeafWS = WaterS.rcoefSpleaf;
			REAL(cwsMat)[i] = waterCont;
			REAL(psimMat)[i] = WaterS.psim;
		}

			// check if new day begins to perform partitioning of biomass          	
			if( (i % 24) ==0) 
			{

				// daily loss of assimilated CO2 via growth respiration 
				// Here I am calling function to calculate Growth Respiration. 
				dailyassim=GrowthRespiration(dailyassim, gRespCoeff); // Plant growth continues during day and night

				// daily loss via  maintenance respirations of different plant organs 
				// dailyassim=dailyassim-dailyLeafResp-dailyStemResp- dailyRootResp- dailyRhizomeResp;
				// However, before performing above step, I am doing some checks
				// Below is how biomass is allocated to different fractions

		  
		/* call the biomass partitioning function */

				dbps = SUGARCANE_DBP_CUADRA(TT12c,TT0,TTseed,Tmaturity,Rd,Alm,Arm,Clstem,Ilstem,Cestem,Iestem,Clsuc,Ilsuc,Cesuc,Iesuc,*(pt_temp+i));
			  
				kLeaf = dbps.kLeaf;
				kStem = dbps.kStem;
				kRoot = dbps.kRoot;
				kSeedcane = dbps.kSeedcane;
				kSugar = dbps.kSugar;
				kFiber=dbps.kFiber;
		   

				REAL(kkleaf)[dayi] = dbps.kLeaf;
				REAL(kkstem)[dayi] = dbps.kStem;
				REAL(kksugar)[dayi]= dbps.kSugar;
				REAL(kkfiber)[dayi]= dbps.kFiber;
				REAL(kkroot)[dayi] = dbps.kRoot;
				REAL(kkseedcane)[dayi] = dbps.kSeedcane;
				REAL(dailyThermalT)[dayi]= TT12c;
		
		        	if(kLeaf >= 0)
				{
					newLeaf =  dailyassim * kLeaf;
					if(newLeaf>=dailyLeafResp)
					{
					 newLeaf=newLeaf-0;
					 // Leaf Respiration is not needed to be subtracted as shown in below comment line
					 // newLeaf=newLeaf-dailyLeafResp
					 // because CanopyAssimilation is already taking care of leaf maintenance respiration
					 // however in case of stem and other organs, we must subtract maintenance respiration
					 // associated with those fractions
					}
					else
					{
						newLeaf=0.0; // may be keep a tracker to simulate reduction in sugarcontent whenit has to provide to compensate for excess respiration
                                        }
					if(newLeaf>0) {
		      	                             *(sti+i) = newLeaf; 
                                                      } 
				}
				else
				{
					  newLeaf = Leaf * kLeaf ; /* This is not suppposed to happen for sugarcane */
					  warning("kleaf is negative");
                                }
					
                               if(TT12c < SeneLeaf)
				{
					   Leaf += newLeaf;
					   leaflitter=leaflitter+0.0;
				}
			       else
				{
					   deadleaf=(leafrate*Leaf)/100.0;  // biomass of dead leaf 
				           leaflitter=leaflitter+deadleaf;  // update litter vector
				           Leaf =Leaf+newLeaf-deadleaf;  // update leaf biomass
				           REAL(LeafLittervec)[dayi]= leaflitter; // Collecting the leaf litter  
			              
				 }

                               // may be move this part outside the daily loop
			       // or evaluate average daily temperature 
			       if((*(pt_temp+i))< Tfrost1)
				 {
					  if((*(pt_temp+i))< Tfrost2)
					  {
						  deadleaf=Leaf;
						  leaflitter=leaflitter+Leaf;
						  Leaf=Leaf-deadleaf;
					  }
					  else
					  {
					          deadleaf=(1-((*(pt_temp+i)-Tfrost2)/(Tfrost1-Tfrost2)))*Leaf;
						  leaflitter=leaflitter+deadleaf;
						  Leaf=Leaf-deadleaf;
					  }
                                    
				 }
				
			       // Here, I am checking for maximum LAI based on minimum SLN = 50 is required
			       // If LAI > LAI max then additional senescence of leaf take place
			       laimax=(-1)*log((minleafN/LeafN))/(kpLN);
				 if(LAI>laimax)
				   {
					 LAI=laimax;
					 deadleaf=(LAI-laimax)/Sp;  
				         Leaf=Leaf-deadleaf;
					 leaflitter=leaflitter+deadleaf;
				    }


				  if(kStem >= 0)
				  {
				        newStem =  dailyassim * kStem ;
					newSugar= dailyassim *kSugar;
					newFiber=dailyassim *kFiber;
					if(newStem>=dailyStemResp)
					{
					 newStem=newStem-dailyStemResp;
					}
					else  // may be keep a tracker to simulate reduction in sugarcontent whenit has to provide to compensate for excess respiration
					{
					  newStem=0.0;
					  newSugar= 0;
					  newFiber=0;
                                        }
				  }
				  else
				  {
					error("kStem should be positive");
				  }
                
               

				  if(TT12c < SeneStem)
				  {
				   Stem += newStem;
				   Fiber+=newFiber;
				   Sugar+=newSugar;
				  }
				  else  //Senescence reduces stem and fiber biomass but not sugar biomass
				  {
				       Stem=Stem+newStem-SeneStem;  
				       Fiber=Fiber+newFiber-SeneStem;
				       Sugar=Sugar+newSugar;
					  
				  }

			         if(kRoot >= 0)
		                 {
					newRoot =  dailyassim * kRoot ;
					if(newRoot>=dailyRootResp)
					{
					 newRoot=newRoot-dailyRootResp;
					}
					else
					{
					 newRoot=0.0; // tracker for reduction in sugar content
                                        }
			                  
		                 }
			       
				 if(TT12c < SeneRoot)
				 {
				   Root += newRoot;
                                   rootlitter=0.0;
				 }
				 else
				 {
				    deadroot=(rootrate*Root)/100.0;  /* biomass of dead root */
				    Root =Root+newRoot-deadroot;
				 }
	
				 
				 if(kSeedcane > 0)
		                    {
			            newSeedcane = dailyassim * kSeedcane ;	
				    if(newSeedcane>0)
				        {
					*(sti4+ri) = newSeedcane;
				        }
				       else
					{
					 newSeedcane=0;
					}
			            ri++;
				  }
				 else
				 {
				  if(Seedcane < 0)
				  {
				    Seedcane = 1e-4;
				    warning("Seedcane became negative");
				  }
				    newSeedcane = Seedcane * kSeedcane;
			            Root += kRoot * -newSeedcane ;
			            Stem += kStem * -newSeedcane ;
				    Leaf += kLeaf * -newSeedcane ;
	                            newSeedcane=newSeedcane-dailySeedcaneResp;	
				 }
                
				 /*	 newSugar=newStem*dbps.kSugar;
					 Sugar=Sugar+newSugar;   */
				 	
				 ALitter = REAL(LeafLittervec)[i] + REAL(StemLittervec)[i];
				 BLitter = REAL(RootLittervec)[i];

			       // daily update of leaf-N, vmac, and alpga  based on cyclic seasonal variation //
				currentday=*(pt_doy+i);
				LeafN=seasonal(LeafNmax,LeafNmin ,currentday,dayofMaxLeafN,365.0,lat);
				vmax1=vmaxb1*LeafN-vmaxb0;
				if(vmax1>vmax0) vmax1=vmax0;
				alpha1=alphab1*LeafN+alphab0;
				if(alpha1>alpha0)alpha1=alpha0;
				Rd1=Rd1;

//					LeafN = LeafN_0 * pow(Stem + Leaf,-kLN); 
// 	                         	if(LeafN > LeafN_0) LeafN = LeafN_0;
//				        vmax1 = (LeafN_0 - LeafN) * REAL(VMAXB1)[0] + REAL(VMAX)[0]; 
//					alpha1 = (LeafN_0 - LeafN) * REAL(ALPHAB1)[0] + REAL(ALPHA)[0]; 

		
				 /*daily update of specific leaf area*/
				 Sp = Sp - SpD;
				 if (Sp < 0.2)Sp=0.2;

				 /* new value of leaf area index */
				 LAInew = Leaf * Sp ;
				 /* apply leaf water stress based on rainfed (wsFun=0,1,2) or irrigated system (wsFun=3)*/

                                if(wsFun ==3)
				{ 
				 LAI=LAInew;
				}
				else
				{ 
				 LAI=LAIold+(LAInew-LAIold)*LeafWS;
				}
				LAIold=LAI;
		
				// reset daily assimilation and daily transpiratin to zero for the next day */
				dailyassim=0.0;
				dailytransp=0.0;

				/*reset daily maintenance respiration to zero for the next day */
				dailyLeafResp = 0.0;
				dailyStemResp = 0.0;
				dailyRootResp = 0.0;
				dailyRhizomeResp = 0.0;
			} //daily loop ends here

			REAL(DayofYear)[i] =  INTEGER(DOY)[i];   //1
			REAL(Hour)[i] =  INTEGER(HR)[i];         //2
			REAL(CanopyAssim)[i] =  CanopyA;         //3
			REAL(CanopyTrans)[i] =  CanopyT;//4 
			REAL(Leafy)[i] = Leaf;//5
			REAL(Stemy)[i] = Stem;//6
			REAL(Rooty)[i] =  Root;//7
			REAL(Sugary)[i] = Sugar;//8
			REAL(Fibery)[i] = Fiber;//9
			REAL(Seedcaney)[i] =  Seedcane;//10
			REAL(LAIc)[i] = LAI;//11
			REAL(TTTc)[i]=TT12c;//12
			REAL(SoilWatCont)[i] = waterCont;//13
			REAL(StomatalCondCoefs)[i] = StomWS;//14
			REAL(LeafReductionCoefs)[i] = LeafWS;//15
			REAL(LeafNitrogen)[i] = LeafN;//16
	                REAL(AboveLitter)[i]=AboveLittery;//17
			REAL(BelowLitter)[i]=BelowLittery;//18
			REAL(VmaxVec)[i] = vmax1;//19
			REAL(AlphaVec)[i] = alpha1;//20
			REAL(SpVec)[i] = Sp;//21
			REAL(SoilEvaporation)[i] = soilEvap;//22
//			REAL(cwsMat)[i]=cswMaty;//23
//			REAL(psiMat)[i]=psiMaty;//24
//			REAL(rdMat)[i]=rdMat;//25
			REAL(LeafPsimVec)[i] = LeafPsim;//26
			REAL(LeafLittervec)[i] = LeafPsim;//27
			REAL(StemLittervec)[i] = LeafPsim;//28
			REAL(RootLittervec)[i] = LeafPsim;//29
			REAL(Leafmaintenance)[i] = LeafPsim;//30
			REAL(Stemmaintenance)[i] = LeafPsim;//31
			REAL(Rootmaintenance)[i] = LeafPsim;//32
			REAL(Seedcanemaintenance)[i] = LeafPsim;//33
			REAL(kkleaf)[i] = LeafPsim;//34
			REAL(kkroot)[i] = LeafPsim;//35
			REAL(kkstem)[i] = LeafPsim;//36
			REAL(kkseedcane)[i] = LeafPsim;//37
			REAL(kkfiber)[i] = LeafPsim;//38
			REAL(kksugar)[i] = LeafPsim;//39
			REAL(dailyThermalT)[i] = LeafPsim;//40
			REAL(moisturecontent)[i] = LeafPsim;//41
			REAL(dayafterplanting)[i] = LeafPsim;//42	
	}

/* Populating the results of the Century model */


	SET_VECTOR_ELT(lists,0,DayofYear);
	SET_VECTOR_ELT(lists,1,Hour);
	SET_VECTOR_ELT(lists,2,CanopyAssim);
	SET_VECTOR_ELT(lists,3,CanopyTrans);
	SET_VECTOR_ELT(lists,4,Leafy);
	SET_VECTOR_ELT(lists,5,Stemy);
	SET_VECTOR_ELT(lists,6,Rooty);
	SET_VECTOR_ELT(lists,7,Sugary);
	SET_VECTOR_ELT(lists,8,Fibery);
	SET_VECTOR_ELT(lists,9,Seedcaney);
        SET_VECTOR_ELT(lists,10,LAIc);
	SET_VECTOR_ELT(lists,11,TTTc);
	SET_VECTOR_ELT(lists,12,SoilWatCont);
	SET_VECTOR_ELT(lists,13,StomatalCondCoefs);
	SET_VECTOR_ELT(lists,14,LeafReductionCoefs);
	SET_VECTOR_ELT(lists,15,LeafNitrogen);
	SET_VECTOR_ELT(lists,16,AboveLitter);
	SET_VECTOR_ELT(lists,17,BelowLitter);
	SET_VECTOR_ELT(lists,18,VmaxVec);
	SET_VECTOR_ELT(lists,19,AlphaVec);
	SET_VECTOR_ELT(lists,20,SpVec);
	SET_VECTOR_ELT(lists,21,SoilEvaporation);
//	SET_VECTOR_ELT(lists,22,cwsMat);
//	SET_VECTOR_ELT(lists,23,psiMat);
//	SET_VECTOR_ELT(lists,24,rdMat);
	SET_VECTOR_ELT(lists,25,LeafPsimVec);
	SET_VECTOR_ELT(lists,26,LeafLittervec);
	SET_VECTOR_ELT(lists,27,StemLittervec);
	SET_VECTOR_ELT(lists,28,RootLittervec);
	SET_VECTOR_ELT(lists,29,Leafmaintenance);
	SET_VECTOR_ELT(lists,30,Stemmaintenance);
	SET_VECTOR_ELT(lists,31,Rootmaintenance);
	SET_VECTOR_ELT(lists,32,Seedcanemaintenance);
	SET_VECTOR_ELT(lists,33,kkleaf);
	SET_VECTOR_ELT(lists,34,kkroot);
	SET_VECTOR_ELT(lists,35,kkstem);
	SET_VECTOR_ELT(lists,36,kkseedcane);
	SET_VECTOR_ELT(lists,37,kkfiber);
	SET_VECTOR_ELT(lists,38,kksugar);
	SET_VECTOR_ELT(lists,39,dailyThermalT);
	SET_VECTOR_ELT(lists,40,moisturecontent);
	SET_VECTOR_ELT(lists,41,dayafterplanting);

	SET_STRING_ELT(names,0,mkChar("DayofYear"));
	SET_STRING_ELT(names,1,mkChar("Hour"));
	SET_STRING_ELT(names,2,mkChar("CanopyAssim"));
	SET_STRING_ELT(names,3,mkChar("CanopyTrans"));
	SET_STRING_ELT(names,4,mkChar("Leafy"));
	SET_STRING_ELT(names,5,mkChar("Stemy"));
	SET_STRING_ELT(names,6,mkChar("Rooty"));
	SET_STRING_ELT(names,7,mkChar("Sugary"));
	SET_STRING_ELT(names,8,mkChar("Fibery"));
	SET_STRING_ELT(names,9,mkChar("Seedcanec"));
        SET_STRING_ELT(names,10,mkChar("LAIc"));
	SET_STRING_ELT(names,11,mkChar("TTTc"));
	SET_STRING_ELT(names,12,mkChar("SoilWatCont"));
	SET_STRING_ELT(names,13,mkChar("StomatalCondCoefs"));
	SET_STRING_ELT(names,14,mkChar("LeafReductionCoefs"));
	SET_STRING_ELT(names,15,mkChar("LeafNitrogen"));
	SET_STRING_ELT(names,16,mkChar("AboveLitter"));
	SET_STRING_ELT(names,17,mkChar("BelowLitter"));
	SET_STRING_ELT(names,18,mkChar("VmaxVec"));
	SET_STRING_ELT(names,19,mkChar("AlphaVec"));
	SET_STRING_ELT(names,20,mkChar("SpVec"));
	SET_STRING_ELT(names,21,mkChar("SoilEvaporation"));
	SET_STRING_ELT(names,22,mkChar("cwsMat"));
//	SET_STRING_ELT(names,23,mkChar("psiMat"));
//	SET_STRING_ELT(names,24,mkChar("rdMat"));
//	SET_STRING_ELT(names,25,mkChar("LeafPsimVec"));
	SET_STRING_ELT(names,26,mkChar("LeafLittervec"));
	SET_STRING_ELT(names,27,mkChar("StemLittervec"));
	SET_STRING_ELT(names,28,mkChar("RootLittervec"));
	SET_STRING_ELT(names,29,mkChar("Leafmaintenance"));
	SET_STRING_ELT(names,30,mkChar("Stemmaintenance"));
	SET_STRING_ELT(names,31,mkChar("Rootmaintenance"));
	SET_STRING_ELT(names,32,mkChar("Seedcanemaintenance"));
	SET_STRING_ELT(names,33,mkChar("kkleaf"));
	SET_STRING_ELT(names,34,mkChar("kkroot"));
	SET_STRING_ELT(names,35,mkChar("kkstem"));
	SET_STRING_ELT(names,36,mkChar("kkseedcane"));
	SET_STRING_ELT(names,37,mkChar("kkfiber"));
	SET_STRING_ELT(names,38,mkChar("kksugar"));
	SET_STRING_ELT(names,39,mkChar("dailyThermalT"));
	SET_STRING_ELT(names,40,mkChar("moisturecontent"));
	SET_STRING_ELT(names,41,mkChar("dayafterplanting"));
	setAttrib(lists,R_NamesSymbol,names);
	UNPROTECT(42);
	return(lists);
}

