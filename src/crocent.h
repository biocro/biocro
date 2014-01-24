// structure to calculate daily climate
struct dailyclimate {
   int doy;
   double temp;
   double precip;
   double solar;
   double windspeed;
   double rh;
   double minimumTemp;
 };
// This structure is to define soil carbon content and types
struct carbon {
	double totalC;
	double unlablTOlabl;
};

// This structure is to define soil mineral content and types
struct minerals {
	double CN;
	double CP;
	double CS;
	double CK;
};

struct flowrestriction {
	double maxCN, maxCP, maxCS, maxCK;
	double minCN, minCP, minCS, minCK;
	double minN, minP, minS, minK;
};

struct flow {
	struct carbon C;
	struct minerals E;
};


struct TempEffectParms {
  double teff1;
  double teff2;
  double teff3;
  double teff4;
};

struct SoilWaterEffectParms {
  double a;
  double b;
  double c;
  double d;
};

struct AnaerobicParms {
  double ANEREF1;
  double ANEREF2;
  double ANEREF3;
};


struct PHParms {
  double a;
  double b;
  double c;
  double d;
};

struct SymbNFixationParms {
  double FXMCA;  // Intercept for effect of biomass on non--symbiotic soil N fixation
  double FXMCB;  // slope factor for efefct of biomass on non-symbiotc soil N fixation
  double FXMXS; // Maximum monthly non-symbiotic fixation rates ( can be reduced by N:P) 
  double FXNPB; /// Contrl of P on non-symbiotic fixation rate on availability of top soil N rate when NSNFIX=1.0
  int NSNFIX; // 0== based on annual precipitation, 1== based on N;P in mineral pool
  double NTSPM;
};

struct ErosionParms {
  double LHZF[3]; // loss of organic matter with erosion
  double EDEPTH; // Depth of soil layer for simulation of soil erosion
  double ENRICH; // enrichment factor for SOM losses with erosion
};

struct OrgLeachParms {
  double DailyThresholdFlow; // critical water flow for leaching of mineral to occur
  double OMLEACH[3];// parameters controlling leaching losses
  struct minerals som1c2toleach;
};

struct SoilTexture {
  double sand;
  double silt;
  double clay;
};

struct C13Parms {
  double DRESP; //discrimination factor for 13C during decomposition
  double DLIGDF; // difference in 13C for lignin compared to whole plant delta 13C
};
// This structure defines input going from BioCro to CropCent
struct InputToCropcent {
  struct carbon C;
  struct minerals E;
  double lignin;
  int surface;   // 1= surface litter (stem, leaf etc), 0= below ground litter (roots etc)
  int woody;  // 1 = woddy, 0= non-woody
};


struct BioCroToCropcentParms {
  double structometaSLOPE; // for structural and metabolic partitioning of incoming litter
  double structometaINTERCEP; // For structural and metabolic partitioning of incoming litter
  double surfacedamrN,surfacedamrP,surfacedamrS,surfacedamrK;
  double soildamrN,soildamrP,soildamrS,soildamrK;
  double mindamrN,mindamrP,mindamrS,mindamrK;
  double pabres;
};

struct cropcentEnvironment {
  double minN, minP,minS, minK;
  double surfaceTEMP,soilTEMP;
  double surfaceRELWC, soilRELWC,PET,AWC;
  double leachedWATER;
  double pH;
  double soilrad;
  struct SoilTexture SOILTEX;
  double drainage;
  struct ErosionParms EROSION;
  struct OrgLeachParms ORGLECH;
  struct C13Parms C13;
  struct SymbNFixationParms NFIX;
  struct BioCroToCropcentParms biocrotocentparms;
};


struct strucc1 
{
  struct carbon C;
  struct minerals E;
  double lignin;
  struct Flux1 {
      struct flow strucc1TOsom1c1;
      struct flow strucc1TOsom2c1;
      struct flow strucc1TOstrucc1; // This represents flow from strucc1 to strucc1 (negative always?)
      struct flow strucc1TOmetabc1;// due to photo decomposition
      double hetresp;
      } Flux;
  struct parms1 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double strmx; //maximum amount that will decompose daily
        double pligst; //effect of lignin -structual ratio on decomposition
        double bioabsorp; // Litter biomass (gram biomass per m2) for full absorption of solar light
        double rsplig; // fraction of lignin lost to CO2 respiration
        double ps1co2; // Fraction of flow to som1c1 lost to respiration
        double pmetabco2; // Fraction of Flow to metabc1 pool lost as respiration
        double maxphoto;
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct PHParms PHEFF;
};
// Soil Structural Pool
struct strucc2
{
  struct carbon C;
  struct minerals E;
  double lignin;
  struct Flux2 {
      struct flow strucc2TOsom2c2;
      struct flow strucc2TOsom1c2;
      struct flow strucc2TOstrucc2; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms2 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double strmx; //maximum amount that will decompose daily
        double pligst; //effect of lignin -structual ratio on decomposition
        double rsplig; // fraction of lignin lost to CO2 respiration
        double ps1co2; // Fraction of flow to som1c2 lost to respiration
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct AnaerobicParms ANEREFF; // Anaerobic Factor is Effective in Soil Pool Decomposition
  struct PHParms PHEFF;
};

//  Wood Branches
struct wood1
{
  struct carbon C;
  struct minerals E;
  double lignin;
  struct Flux3 {
      struct flow wood1TOsom1c1;
      struct flow wood1TOsom2c1;
      struct flow wood1TOwood1; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms3 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double pligst; //effect of lignin -structual ratio on decomposition
        double rsplig; // fraction of lignin lost to CO2 respiration
        double ps1co2; // Fraction of flow to som1c1 lost to respiration      
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture 
  struct PHParms PHEFF;
};
// Large Dead Wood
struct wood2
{
  struct carbon C;
  struct minerals E;
   double lignin;
  struct Flux4 {
      struct flow wood2TOsom1c1;
      struct flow wood2TOsom2c1;
      struct flow wood2TOwood2; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms4 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double pligst; //effect of lignin -structual ratio on decomposition
        double rsplig; // fraction of lignin lost to CO2 respiration
        double ps1co2; // Fraction of flow to som1c1 lost to respiration   
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture 
  struct PHParms PHEFF;
};
// Coare Roots
struct wood3
{
  struct carbon C;
  struct minerals E;
  double lignin;
  struct Flux5 {
      struct flow wood3TOsom2c2;
      struct flow wood3TOsom1c2;
      struct flow wood3TOwood3; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms5 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double pligst; //effect of lignin -structual ratio on decomposition
        double rsplig; // fraction of lignin lost to CO2 respiration
        double ps1co2; // Fraction of flow to som1c2 lost to respiration
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture 
  struct AnaerobicParms ANEREFF; // Anaerobic Factor is Effective in Soil Pool Decomposition
  struct PHParms PHEFF;
};

struct metabc1
{
  struct carbon C;
  struct minerals E;
  struct Flux6 {
      struct flow metabc1TOsom1c1;
      struct flow metabc1TOmetabc1; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms6 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double pmco2; // Fraction of flow to som1c2 lost to respiration
        double a,b,x1,x2;
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moistur
  struct PHParms PHEFF;
};

struct metabc2
{
  struct carbon C;
  struct minerals E;
  struct Flux7 {
      struct flow metabc2TOsom1c2;
      struct flow metabc2TOmetabc2; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms7 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double pmco2; // Fraction of flow to som1c2 lost to respiration
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct AnaerobicParms ANEREFF; // Anaerobic Factor is Effective in Soil Pool Decomposition
  struct PHParms PHEFF;
};

struct som1c1
{
  struct carbon C;
  struct minerals E;
  struct Flux8 {
      struct flow som1c1TOsom2c1;
      struct flow som1c1TOsom1c1; // This represents flow from strucc1 to strucc1 (negative always?)
      double hetresp;
      } Flux;
  struct parms8 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double a,b,x1,x2;// Effect of incoming solar radiation on decom of metaboolic pool
        double p1co2a;
        double p1co2b; 
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct PHParms PHEFF;
  struct flowrestriction INFLOW;
};

struct som2c1
{
  struct carbon C;
  struct minerals E;
  struct Flux9 {
      struct flow som2c1TOsom1c1;
      struct flow som2c1TOsom2c1;
      struct flow som2c1TOsom2c2; // due to mixing
      double hetresp;
      } Flux;
  struct parms9 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double a,b,x1,x2;// Effect of incoming solar radiation on decom of metaboolic pool
        double p2co2; // Fraction of flow to som1c2 lost to respiration
	      double mix;
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct PHParms PHEFF;
  struct flowrestriction INFLOW;
};

struct som1c2
{
  struct carbon C;
  struct minerals E;
  struct Flux10 {
      struct flow som1c2TOsom2c2;
      struct flow som1c2TOsom3c; 
      struct flow som1c2TOleachate;
      struct flow som1c2TOsom1c2;
      double hetresp;
      } Flux;
  struct parms10 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year
        double peftxa,peftxb;
        double p1co2a,p1co2b; 
        double ps1s3[2];
        double animpt;
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct AnaerobicParms ANEREFF; // Anaerobic Factor is Effective in Soil Pool Decomposition
  struct PHParms PHEFF;
  struct flowrestriction INFLOW;
};
struct som2c2
{
  struct carbon C;
  struct minerals E;
  struct Flux11 {
      struct flow som2c2TOsom1c2;
      struct flow som2c2TOsom3c; 
      struct flow som2c2TOsom2c2;
      double hetresp;
      } Flux;
  struct parms11 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year
        double p2co2;
        double ps2s3[2];
        double animpt;
      } parms;    
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct AnaerobicParms ANEREFF; // Anaerobic Factor is Effective in Soil Pool Decomposition
  struct PHParms PHEFF;
  struct flowrestriction INFLOW;
};

struct som3c
{
  struct carbon C;
  struct minerals E;
  struct Flux12 {
      struct flow som3cTOsom1c2;
      struct flow som3cTOsom3c; 
      double hetresp;
      } Flux;
  struct parms12 {
        double k; // maximum annual decomposition rate
        double timestep; // time step to be used to modify k if it is not 1 year 
        double p3co2; // Fraction of flow to som1c2 lost to respiration
      } parms;
  struct TempEffectParms TEff; // temperature effect
  struct SoilWaterEffectParms SWEFF;  // effect of moisture
  struct AnaerobicParms ANEREFF; // Anaerobic Factor is Effective in Soil Pool Decomposition
  struct PHParms PHEFF;
  struct flowrestriction INFLOW;
};


struct cropcentlayer {
	struct strucc1 strucc1;
	struct strucc2 strucc2;
	struct metabc1 metabc1;
	struct metabc2 metabc2;
	struct som1c1 som1c1;
	struct som1c2 som1c2;
	struct som2c1 som2c1;
	struct som2c2 som2c2;
	struct som3c som3c;
	struct wood1 wood1;
	struct wood2 wood2;
	struct wood3 wood3;
	struct cropcentEnvironment ENV;
  struct BioCroToCropcentParms BcroTOCentParms;
};

void assignPools(struct cropcentlayer *CROPCENT, double *sompoolsfromR);
void assignParms(struct cropcentlayer *CROPCENT, double *somassignparmsfromR);
double timescaling (double k ,double t);
void CROPCENTTimescaling(struct cropcentlayer *CROPCENT);
void assignFluxRatios(struct cropcentlayer *CROPCENT);
void assignENV(struct cropcentlayer *CROPCENT,double *getsoiltexturefromR,double *getcropcentstatevarfromR, double *getbiocrotocropcentparmsfromR, double *geterosionparmsfromR,double *getc13parmsfromR,double *getleachingparmsfromR,double *getsymbnfixationparmsfromR);
void GetC13Parms(struct C13Parms *temp,double *getc13parmsfromR);

// Assignign pool to flow structure
void printcropcentout(struct cropcentlayer CROPCENT, double *totalSOC, double *strucc1, double *strucc2, 
                        double *metabc1, double *metabc2, double *som1c1, double *som1c2, double *som2c1, 
                        double *som2c2, double *som3c, double *minN);
// void assignpooltoflow(struct carbon *tmpC,struct minerals *tmpE,struct flow *tmpflow);
void GetBioCroToCropcentParms(struct BioCroToCropcentParms  *temp,double *getbiocrotocropcentparmsfromR );
void GetSymbNFixationParms(struct SymbNFixationParms *temp,double *getsymbnfixationparmsfromR);
void GetErosionParms(struct ErosionParms *temp,double *geterosionparmsfromR);
void GetSoilTexture( struct SoilTexture *temp, double *getsoiltexturefromR);
void GetCropCentStateVar(struct cropcentEnvironment *ENV, double *getcropcentstatevarfromR);
void GetInFlowCERatio(struct flowrestriction *INFLOWRESTRICT, struct minerals *output, struct cropcentEnvironment *ENV);
double GetMDR(double a,double b,double x1,double x2,double soilrad);
double GetMTI(double a,double b,double x1,double x2,double soilrad);
void replaceMineralStruct(struct minerals *FROM,struct minerals *TO);
struct flow GetLeachate (double tcflow, struct minerals *E, double leachedWATER, double sand, struct OrgLeachParms *temp);
double weightavg(double W1,double v1,double W2,double v2);
void UpdateDirectAbsorp(struct InputToCropcent *INCROCENT, struct BioCroToCropcentParms *parms, struct cropcentEnvironment *ENV);
void UpdateCropcentPoolsFromBioCro(struct cropcentlayer *CROPCENT, struct InputToCropcent *INCROPCENT);
// Function definition to decompose crocent layer
void decomposeCROPCENT(struct cropcentlayer *CROPCENT, int woody, int Eflag);
void BiocroToCrocent(double *stdedbc, double fallrate, double lignin, struct minerals *E, double isotoperatio, int surface, int woody,struct InputToCropcent *INCROCENT);

//Function Definition to determine if decomposition of a source pool will occur or not based on flow requirement and ENV
int CheckDecomposition(struct minerals *source, struct minerals *flow,struct cropcentEnvironment *ENV,int Eflag);
//evaluating variable which is linearly varying 
double line(double x,double x1,double y1,double x2,double y2);
double Getdefac(struct TempEffectParms *Temp, struct SoilWaterEffectParms *swc, double RELWC, double TEMP);
double GetPHfac(struct PHParms *PHEFF,double pH);
double GetAnerbFac(struct AnaerobicParms *ANEREFF, double PET, double AWC,double drainage);
void updateCEafterleachate(struct som1c2 *som1c2);
// Function definition to decompose strucc1 (surface structual pool)
void decomposeSTRUCC1(struct strucc1 *strucc1, struct cropcentEnvironment *ENV,  int flag, int Eflag);

// Function definition to decompose strucc2 (soil structual pool)
void decomposeSTRUCC2(struct strucc2 *strucc2, struct cropcentEnvironment *ENV, int flag,int Eflag);

// Function definition to decompose metabc1 (surface metabolic pool)
void decomposeMETABC1(struct metabc1 *metabc1, struct cropcentEnvironment *ENV, int flag, int Eflag);

// Function definition to decompose metabc2 (surface structual pool)
void decomposeMETABC2(struct metabc2 *metabc2, struct cropcentEnvironment *ENV, int flag, int Eflag);

// Function definition to decompose wood1 (dead branches)
void decomposeWOOD1(struct wood1 *wood1, struct cropcentEnvironment *ENV, int flag,int Eflag);

// Function definition to decompose wood2 (dead large woods)
void decomposeWOOD2(struct wood2 *wood2, struct cropcentEnvironment *ENV, int flag,int Eflag);

// Function definition to decompose wood3 (coarse root pool)
void decomposeWOOD3(struct wood3 *wood3, struct cropcentEnvironment *ENV, int flag,int Eflag);

// Function definition to decompose som1c1 (actve surfcae pool)
void decomposeSOM1C1(struct som1c1 *som1c1, struct cropcentEnvironment *ENV, int flag, int Eflag);

// Function definition to decompose som1c2 (actve soil pool)
void decomposeSOM1C2(struct som1c2 *som1c2, struct cropcentEnvironment *ENV, int flag, int Eflag);

// Function definition to decompose som2c1 (slow surfcae pool)
void decomposeSOM2C2(struct som2c2 *som2c2, struct cropcentEnvironment *ENV, int flag, int Eflag);

// Function definition to decompose som3c (passive soil  pool)
void decomposeSOM3C(struct som3c *som3c, struct cropcentEnvironment *ENV, int flag, int Eflag);

// Function definition to update crocent layer pools from the flow structures
void updatecropcentpools(struct cropcentlayer *CROPCENT);

// Updating an inddividual CE structure based on current status (pool) and Flux
void updateMineralStructure(struct carbon *toupdateC, struct minerals *toupdateE, struct carbon flowC, struct minerals flowE);
void updateCarbonStructure(struct carbon *toupdateC,struct carbon flow);


/**************************************************************/
/************ Structure Definitionfor CropGro********************/

struct leaf {
  double biomass, litter;
  struct minerals biomassE,litterE;
};

struct stem {
  double biomass, litter;
  struct minerals biomassE,litterE;
};

struct rhizome {
  double biomass, litter;
  double carbohydratefraction;
  struct minerals biomassE,litterE;
};

struct root {
  double biomass, litter;
  struct minerals biomassE,litterE;
};

struct dailyvec {
  double newbiomass;
  double newlitter;
  double ageinTT;
};

struct littervec {
  double biomass;
  double ageinTT;
};

struct DailyAutoResp {
  double leafdarkresp;
  double stemgrowth;
  double rootgrowth;
  double rhizomegrowth;
  double stemmaint;
  double rootmaint;
  double rhizomemaint;
  double total;
};

struct miscanthus {
  struct leaf leaf;
  struct stem stem;
  struct rhizome rhizome;
  struct root root;
  struct dailyvec *leafvec, *stemvec, *rootvec, *rhizomevec;
  struct DailyAutoResp autoresp;
  double GPP, NPP;
};

struct senthermaltemp {
  double leafcriticalT, leaffr;
  double stemcriticalT, stemfr;
  double rhizomecriticalT, rhizomefr;
  double rootcriticalT,rootfr;
};

struct canopyparms {
  double kN;
  double SLA;
  double remobFac;
  double leafNsen;
};

struct respirationParms
{
  struct growth {
    double stem;
    double root;
    double rhizome;
  } growth;
  
  struct maint {
    double Qstem, mstem;
    double Qroot, mroot;
    double Qrhizome, mrhizome;
  } maint;
};
double CalculateGrowthResp(double newbiomass,double growthRespFactor);

void dailymiscanthus(struct miscanthus *miscanthus,double coefs[25],double TherPrds[6], double TherTime, double Temp,double dailynetassim,
struct senthermaltemp *senparms, struct canopyparms *canopyparms, struct frostParms *frostparms, int N, double delTT,
struct respirationParms *RESP, int emergence);

double getThermalSenescence(double criticalTT, double currentTT, double biomass, double dailyfractionalloss);
double canopyNsenescence(struct leaf *leaf, double SLA, double kN,  double leafNsen);
double getStemSenescence(struct stem *stem, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT);
double getRootSenescence(struct root *root, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT);
double getRhizomeSenescence(struct rhizome *rhizome, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT);
double getLeafSenescence(struct leaf *leaf, double criticalTT, double senefracion,  double Temp, struct frostParms *frostparms, double TT,struct canopyparms *canopyparm);
void updatelittervec(struct littervec *littervec,double newbiomass,int N,double delTT);
void updatebiomass(double *res,double toadd,double toremove);
void dailymiscanthusupdate(struct miscanthus *miscanthus,struct miscanthus *deltamiscanthus);
double newbiomass(double AcanopyNet,double k, double GrowthRespCoeff);
double getThermaltime (double temp, double Tbase);
void UpdateStandingbiomass (double *standing, double newbiomass);
void UpdateStandingLeaf(struct leaf *leaf, double newbiomass, double deadleaf, double remobFactor);
void UpdateStandingStem(struct stem *stem, double newbiomass, double deadstem, double remobFactor);
void UpdateStandingRoot(struct root *root, double newbiomass, double deadroot, double remobFactor);
void UpdateStandingRhizome(struct rhizome *rhizome, double newbiomass, double deadrhizome, double remobFactor);
void updatedailyvec(struct dailyvec *dailyvec,double newbiomass,double newlitter, int N,double delTT);
void createNULLmiscanthus(struct miscanthus *miscanthus,int vecsize);
void getdailyclimate(struct dailyclimate *dailyclimate, int *doy,double *solar,double *temp, double *rh, double *windspeed, double*precip, int currenthour, int vecsize);
int CheckEmergence(struct dailyclimate *dailyclimate, double EmergTemperature);
void updatedormantstage(struct miscanthus *miscanthus);

/*********Structure and Functions for C3Tree*******************/
struct c3tree {
  struct leaf leaf;
  struct stem stem;
  struct root root;
  struct rhizome rhizome; // rhizome is mimicing corase roots here
  struct dailyvec *leafvec, *stemvec, *rootvec, *rhizomevec;
  struct DailyAutoResp autoresp;
  double GPP, NPP;
};

void dailyC3tree(struct c3tree *c3tree,double coefs[25],double TherPrds[6], double TherTime, double Temp,double dailynetassim,
struct senthermaltemp *senparms, struct canopyparms *canopyparms, struct frostParms *frostparms, struct respirationParms *RESP);

/*********************************************************************/
/*********************************************************************/
 

/*******Management Assigning Functions and Structures******************/

struct management
{
  struct harvestparms {
    int harvestdoy;
    double frleaf,frleaflitter; // fration of green leaf biomass and leaf litter removed with harvest 
    double frstem,frstemlitter;  // fraction of stem and stem litter removed with harvest
    double frdeadroot; // fraction of roots that die on harvest
    double frdeadrhizome; // fraction of rhizome that dies on harvest
  } harvestparms;
  
  struct emergenceparms {
    int minimumdoy;
    double emergenceTemp;
    double StoragetoLeaffraction;
    double StemtoLeaffraction;
    double plantingrate;
  } emergenceparms;  
};

void assignManagement(struct management *management);
void updateafterharvest(struct miscanthus *miscanthus,struct management *management);
void updateafteremergence(struct miscanthus *miscanthus,struct management *management);
void getsenescenceparms(struct senthermaltemp *senparms);
void getfrostparms(struct frostParms *frostparms);

// Willow Specific Functions
void UpdateC3treeAfterEmergence(struct c3tree *willow,struct management *management);
void UpdateWillowAfterHarvest(struct c3tree *willow,struct management *management);