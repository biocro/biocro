#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Structures.h"
#include "Functions.h"
#include "c3photo.h"

// Read parameters from parameter input file
void ComputeReadParameters(char *FileName,int *StartDay,int *EndDay,double *DeltaT,double *Latitude,
                           struct CanopyParameters *Canopy,double *TBase,double **StageThermalTime,
                           struct PhotosynthesisParameters *Photosynthesis,struct SoilParameters *Soil,
                           struct PlantParts *Senescence,struct PlantParts *InitialBiomass,int *Stages,
                           struct PlantParts **PartitionCoefficients,struct PlantParts *GRespirationFraction,
                           struct PlantParts *Q10Respiration,struct PlantParts *MRespirationBase){
    int Years,PartitionLoop;
    char Star;
    FILE *InputFile=fopen(FileName,"rb");
    fscanf(InputFile,"%c %i %i %i %lf %lf \n",&Star,&Years,StartDay,EndDay,DeltaT,Latitude); // Weather parameters
    fscanf(InputFile,"%c %lf %lf %i %lf %lf %lf %lf \n",&Star,&Canopy->SLAI,&Canopy->SLAIDecline,&Canopy->Layers,
           &Canopy->KDiffuse,&Canopy->Chi,&Canopy->Clump,&Canopy->HeightFactor); // Canopy parameters
    fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf %lf %i %lf \n",&Star,&Photosynthesis->Vcmax25,
           &Photosynthesis->Jmax25,&Photosynthesis->Theta,&Photosynthesis->Rd25,
           &Photosynthesis->BallBerryIntercept,&Photosynthesis->BallBerrySlope,
           &Photosynthesis->CO2,&Photosynthesis->O2,&Photosynthesis->WaterStressFunction,
           &Photosynthesis->WaterStressFactor); // Photosynthesis parameters
    fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf \n",&Star,&InitialBiomass->Leaf,&InitialBiomass->Stem,
           &InitialBiomass->Flower,&InitialBiomass->Grain,&InitialBiomass->Root,&InitialBiomass->Rhizome,
           &InitialBiomass->Nodule); // Initial biomass parameters
    fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf \n",&Star,&Senescence->Leaf,&Senescence->Stem,&Senescence->Flower,
           &Senescence->Grain,&Senescence->Root,&Senescence->Rhizome,&Senescence->Nodule); // Senescence parameters
    fscanf(InputFile,"%c %lf %i \n",&Star,TBase,Stages);
    *StageThermalTime = malloc((*Stages)*(sizeof(double)));
	*PartitionCoefficients = malloc((*Stages)*(sizeof(struct PlantParts)));
	for(PartitionLoop=0;PartitionLoop<*Stages;PartitionLoop++){
        fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf %lf \n",&Star,&(*StageThermalTime)[PartitionLoop],
               &(*PartitionCoefficients)[PartitionLoop].Leaf,&(*PartitionCoefficients)[PartitionLoop].Stem,
               &(*PartitionCoefficients)[PartitionLoop].Flower,&(*PartitionCoefficients)[PartitionLoop].Grain,
               &(*PartitionCoefficients)[PartitionLoop].Root,&(*PartitionCoefficients)[PartitionLoop].Rhizome,
               &(*PartitionCoefficients)[PartitionLoop].Nodule);} // Partition coefficients
    fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf \n",&Star,&GRespirationFraction->Leaf,&GRespirationFraction->Stem,
           &GRespirationFraction->Flower,&GRespirationFraction->Grain,&GRespirationFraction->Root,
           &GRespirationFraction->Rhizome,&GRespirationFraction->Nodule); // Growth respiration parameters
    fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf \n",&Star,&Q10Respiration->Leaf,&Q10Respiration->Stem,
           &Q10Respiration->Flower,&Q10Respiration->Grain,&Q10Respiration->Root,&Q10Respiration->Rhizome,
           &Q10Respiration->Nodule); // Q10 respiration parameters
    fscanf(InputFile,"%c %lf %lf %lf %lf %lf %lf %lf \n",&Star,&MRespirationBase->Leaf,&MRespirationBase->Stem,
           &MRespirationBase->Flower,&MRespirationBase->Grain,&MRespirationBase->Root,&MRespirationBase->Rhizome,
           &MRespirationBase->Nodule); // Maintenance base respiration parameters
    fclose(InputFile);
return;}

// Compute thermal time
void ComputeThermalTime (double Temperature[],double TBase,int StartTime,int EndTime,double DeltaT,
                         double ThermalTime[],double DailyThermalTime[]){
    int TimeCount=StartTime;
    int DayCount;
    for(DayCount=(int)StartTime/24-1;DayCount<(int)EndTime/24+1;DayCount++){
        DailyThermalTime[DayCount] = 0;}
    DayCount = (int)StartTime/24;
    if(Temperature[TimeCount]>TBase){
        ThermalTime[TimeCount]=(Temperature[TimeCount]-TBase)/(24/DeltaT);}
    else{
        ThermalTime[TimeCount]=0.0;}
    for(TimeCount=StartTime+1;TimeCount<EndTime;TimeCount++){
        if(Temperature[TimeCount]>TBase){
            ThermalTime[TimeCount] = ThermalTime[TimeCount-1]+(Temperature[TimeCount]-TBase)/(24/DeltaT);}
        else{
            ThermalTime[TimeCount] = ThermalTime[TimeCount-1];}
        if(TimeCount%24==0){
            DailyThermalTime[DayCount] = ThermalTime[TimeCount];
            DayCount += 1;}}
        DailyThermalTime[DayCount] = ThermalTime[TimeCount-1];
return;}

// Compute weather data
void ComputeWeather(int DayOfYear,int Hour,double Solar,double Temperature,double RH,double Wind,
                    double Precipitation,struct Weather *WeatherData){
  WeatherData->DayOfYear = DayOfYear;
  WeatherData->Hour = Hour;
  WeatherData->Solar = Solar;
  WeatherData->Temperature = Temperature;
  WeatherData->RH = RH;
  WeatherData->Wind = Wind;
  WeatherData->Precipitation = Precipitation;
return;}

// Compute maintenance respiration and daily totals of maintenance respiration
void ComputeMRespiration(struct CanopyStructure Canopy,struct PlantParts Biomass,
                         struct PlantParts Q10Respiration,struct PlantParts MRespirationBase,
                         double Temperature,double DeltaT,struct PlantParts *MRespiration){
    //  Compute maintenance respiration
    MRespiration->Leaf = (Canopy.GrossAssim-Canopy.Assim)*DeltaT;
    MRespiration->Stem = ComputeMRespirationPart(Biomass.Stem,Q10Respiration.Stem,
                                                 MRespirationBase.Stem,Temperature,DeltaT);
    MRespiration->Flower = ComputeMRespirationPart(Biomass.Flower,Q10Respiration.Flower,
                                                   MRespirationBase.Flower,Temperature,DeltaT);
    MRespiration->Grain = ComputeMRespirationPart(Biomass.Grain,Q10Respiration.Grain,
                                                  MRespirationBase.Grain,Temperature,DeltaT);
    MRespiration->Root = ComputeMRespirationPart(Biomass.Root,Q10Respiration.Root,
                                                 MRespirationBase.Root,Temperature,DeltaT);
    MRespiration->Rhizome = ComputeMRespirationPart(Biomass.Rhizome,Q10Respiration.Rhizome,
                                                    MRespirationBase.Rhizome,Temperature,DeltaT);
    MRespiration->Nodule = ComputeMRespirationPart(Biomass.Nodule,Q10Respiration.Nodule,
                                                   MRespirationBase.Nodule,Temperature,DeltaT);
    MRespiration->Total = MRespiration->Leaf+MRespiration->Stem+MRespiration->Flower+MRespiration->Grain+
                         MRespiration->Root+MRespiration->Rhizome+MRespiration->Nodule;
return;}

// Compute maintenance respiration for each plant part
double ComputeMRespirationPart(double Biomass,double Q10Respiration,double MRespirationBase,double Temperature,double DeltaT){
    double Result;
    Result=Biomass*(MRespirationBase*DeltaT/24)*pow(Q10Respiration,(Temperature-25.0)/10.0); // Change MRespirationBase from 1/day to 1/hr for DeltaT=1 hr
return(Result);}

// Compute litter
void ComputeNewLitter(struct PlantParts StoreAddBiomass[],struct PlantParts Senescence,double DailyThermalTime[],
                      int TimeCount,int DayCount,struct PlantParts *SenescenceIndex,struct PlantParts *NewLitter){
    int Count,SeneCount;
    // Leaf
    Count = (int)(SenescenceIndex->Leaf);
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Leaf){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Leaf;SeneCount<Count;SeneCount++){
        NewLitter->Leaf += StoreAddBiomass[SeneCount].Leaf;}
    SenescenceIndex->Leaf = Count;
    // Stem
    Count = SenescenceIndex->Stem;
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Stem){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Stem;SeneCount<Count;SeneCount++){
        NewLitter->Stem += StoreAddBiomass[SeneCount].Stem;}
    SenescenceIndex->Stem = Count;
    // Flower
    Count = SenescenceIndex->Flower;
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Flower){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Flower;SeneCount<Count;SeneCount++){
        NewLitter->Flower += StoreAddBiomass[SeneCount].Flower;}
    SenescenceIndex->Flower = Count;
    // Grain
    Count = SenescenceIndex->Grain;
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Grain){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Grain;SeneCount<Count;SeneCount++){
        NewLitter->Grain += StoreAddBiomass[SeneCount].Grain;}
    SenescenceIndex->Grain = Count;
    // Root
    Count = SenescenceIndex->Root;
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Root){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Root;SeneCount<Count;SeneCount++){
        NewLitter->Root += StoreAddBiomass[SeneCount].Root;}
    SenescenceIndex->Root = Count;
    // Rhizome
    Count = SenescenceIndex->Rhizome;
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Rhizome){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Rhizome;SeneCount<Count;SeneCount++){
        NewLitter->Rhizome += StoreAddBiomass[SeneCount].Rhizome;}
    SenescenceIndex->Rhizome = Count;
    // Nodule
    Count = SenescenceIndex->Nodule;
    while (Count<DayCount && DailyThermalTime[DayCount]-DailyThermalTime[Count]>=Senescence.Nodule){
        Count += 1;}
    for(SeneCount=SenescenceIndex->Nodule;SeneCount<Count;SeneCount++){
        NewLitter->Nodule += StoreAddBiomass[SeneCount].Nodule;}
    SenescenceIndex->Nodule = Count;
return;}

// Compute total litter
void ComputeLitter(struct PlantParts NewLitter,struct PlantParts *Litter){
    Litter->Leaf = Litter->Leaf+NewLitter.Leaf;
    Litter->Stem = Litter->Stem+NewLitter.Stem;
    Litter->Flower = Litter->Flower+NewLitter.Flower;
    Litter->Grain = Litter->Grain+NewLitter.Grain;
    Litter->Root = Litter->Root+NewLitter.Root;
    Litter->Rhizome = Litter->Rhizome+NewLitter.Rhizome;
    Litter->Nodule = Litter->Nodule+NewLitter.Nodule;
return;}

// Compute phenology stage based on thermal time
void ComputePhenologyStage(int Stages,double ThermalTime,double StageThermalTime[],int *PhenologyStage){
    int Count=0,LoopCondition=0;
    while (Count<Stages && LoopCondition==0){
      if (ThermalTime<StageThermalTime[Count]){
        *PhenologyStage = Count;
        LoopCondition = 1;}
      else{
        Count += 1;}}
    if (ThermalTime>=StageThermalTime[Stages-1]){
      *PhenologyStage = Stages-1;}
return;}

// Compute biomass added/allocated based on phenology stage
void ComputeAddBiomass(double DailyCanopyNPP,struct PlantParts PartitionCoefficients[],int PhenologyStage,
                       struct PlantParts *AddBiomass){
    if (DailyCanopyNPP>0){
        AddBiomass->Leaf = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Leaf;
        AddBiomass->Stem = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Stem;
        AddBiomass->Flower = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Flower;
        AddBiomass->Grain = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Grain;
        AddBiomass->Root = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Root;
        AddBiomass->Rhizome = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Rhizome;
        AddBiomass->Nodule = DailyCanopyNPP*PartitionCoefficients[PhenologyStage].Nodule;
        AddBiomass->Total = DailyCanopyNPP;}
    else{
        AddBiomass->Leaf = 0;
        AddBiomass->Stem = 0;
        AddBiomass->Flower = 0;
        AddBiomass->Grain = 0;
        AddBiomass->Root = 0;
        AddBiomass->Rhizome = 0;
        AddBiomass->Nodule = 0;
        AddBiomass->Total = DailyCanopyNPP;}
return;}

// Compute growth respiration
void ComputeGRespiration(struct PlantParts GRespirationFraction,struct PlantParts *GRespiration,struct PlantParts *AddBiomass){
    // Compute growth respiration
    GRespiration->Leaf = AddBiomass->Leaf*GRespirationFraction.Leaf;
    GRespiration->Stem = AddBiomass->Stem*GRespirationFraction.Stem;
    GRespiration->Flower = AddBiomass->Flower*GRespirationFraction.Flower;
    GRespiration->Grain = AddBiomass->Grain*GRespirationFraction.Grain;
    GRespiration->Root = AddBiomass->Root*GRespirationFraction.Root;
    GRespiration->Rhizome = AddBiomass->Rhizome*GRespirationFraction.Rhizome;
    GRespiration->Nodule = AddBiomass->Nodule*GRespirationFraction.Nodule;
    GRespiration->Total = GRespiration->Leaf+GRespiration->Stem+GRespiration->Flower+GRespiration->Grain+
                         GRespiration->Root+GRespiration->Rhizome+GRespiration->Nodule;
    AddBiomass->Leaf -= GRespiration->Leaf;
    AddBiomass->Stem -= GRespiration->Stem;
    AddBiomass->Flower -= GRespiration->Flower;
    AddBiomass->Grain -= GRespiration->Grain;
    AddBiomass->Root -= GRespiration->Root;
    AddBiomass->Rhizome -= GRespiration->Rhizome;
    AddBiomass->Nodule -= GRespiration->Nodule;
    AddBiomass->Total -= GRespiration->Total;
return;}

// Compute new biomass
void ComputeBiomass(struct PlantParts NewLitter,struct PlantParts AddBiomass,double FrostFraction,
                    double RemobFraction,struct PlantParts *Biomass){
      Biomass->Leaf += AddBiomass.Leaf-NewLitter.Leaf-Biomass->Leaf*FrostFraction*RemobFraction;
      if (Biomass->Leaf<0){Biomass->Leaf = 0;}
      Biomass->Stem += AddBiomass.Stem-NewLitter.Stem;
      if (Biomass->Stem<0){Biomass->Stem = 0;}
      Biomass->Flower += AddBiomass.Flower-NewLitter.Flower;
      if (Biomass->Flower<0){Biomass->Flower = 0;}
      Biomass->Grain += AddBiomass.Grain-NewLitter.Grain;
      if (Biomass->Grain<0){Biomass->Grain = 0;}
      Biomass->Root += AddBiomass.Root-NewLitter.Root;
      if (Biomass->Root<0){Biomass->Root = 0;}
      Biomass->Rhizome += AddBiomass.Rhizome-NewLitter.Rhizome;
      if (Biomass->Rhizome<0){Biomass->Rhizome = 0;}
      Biomass->Nodule += AddBiomass.Nodule-NewLitter.Nodule;
      if (Biomass->Nodule<0){Biomass->Nodule = 0;}
return;}

// Compute zenith angle
 void ComputeZenith(double Lattitude,int DOY,int Hour,double *Zenith){
     double HourAngle;
     double Declination;
     double Pi = acos(0)*2.0;
     Lattitude = Lattitude*Pi/180;
     HourAngle = (Hour-12)/24.0*360*Pi/180;
     Declination = -asin(0.39779*cos((0.98565*((DOY-1)+10)+1.914*sin(0.98565*((DOY-1)-2)*Pi/180))*Pi/180));
     *Zenith = acos(sin(Lattitude)*sin(Declination)+cos(Lattitude)*cos(Declination)*cos(HourAngle));
return;}

// Compute fraction of diffuse shortwave radiation Spitters et al 1986_AFM_SeparatingDirectDiffuse
double ComputeDiffuseFraction(double SWRad,double Zenith,int DOY){
     double DiffuseFraction;
     double Elevation;
     double SolarConstant = 1370; // [J/m2/s]
     double Pi = acos(0)*2.0;
     double So,R,K; // Temporary variables for calculating diffuse fraction
     Elevation = Pi/2-Zenith;
     if (Elevation>0 && Elevation<Pi/2){
        So = SolarConstant*(1+0.033*cos(DOY/365*360*Pi/180))*sin(Elevation);
        R = 0.847-1.61*sin(Elevation)+1.04*pow(sin(Elevation),2);
        K = (1.47-R)/1.66;
        if (SWRad/So<=0.22){
            DiffuseFraction = 1;}
        else if (0.22<SWRad/So && SWRad/So<=0.35){
            DiffuseFraction = 1-6.4*pow((SWRad/So-0.22),2);}
        else if (0.35<SWRad/So && SWRad/So<=K){
            DiffuseFraction = 1.47-1.66*SWRad/So;}
        else if (K<SWRad/So){
            DiffuseFraction = R;}}
     else{
        DiffuseFraction = 1;}
return(DiffuseFraction);}

// Compute fractions of sunlit and shaded leaves
void ComputeRadProfile(double DirectRad,double DiffuseRad,double LAIProfile[],int Layers,
                  double Zenith,double KDiffuse,double LeafAngle[],double Clump,
                  double Absorptivity,double FSun[],
                  double FShade[],double SunlitAbsorbed[],double ShadedAbsorbed[]){
    int LayerCount;
    double Pi=acos(0)*2.0;
    double CumulativeLAI=0;
    double *KBeam;
    KBeam=(double *)malloc(sizeof(double)*Layers);
    double DirectRadTransient,DownDiffuseRadTransient,UpDiffuseRadTransient;
    double Extinction,Intercepted,Unintercepted,Absorbed,Transmitted,Reflected;
    double *DownDirectRad,*DownDiffuseRad,*UpDiffuseRad;
    DownDirectRad=(double *)malloc(sizeof(double)*Layers);
    DownDiffuseRad=(double *)malloc(sizeof(double)*Layers);
    UpDiffuseRad=(double *)malloc(sizeof(double)*Layers);
    double Reflectivity,Transmissivity;
    double SkyLost=0;
    double SoilReflectivity=0.4,SoilAbsorptivity=0.6;
    double SoilAbsorbed,TotalAbsorbed;
    double Error=100;
    int ErrorCount=0,MaxErrorCount=10;
    // Compute Reflectivity and Transmissivity
    Reflectivity = (1-pow(Absorptivity,0.5))/(1+pow(Absorptivity,0.5));
    Transmissivity = 1-Absorptivity-Reflectivity;
    // Compute extinction coefficient for direct radiation
    for(LayerCount=0;LayerCount<Layers;LayerCount++){
        KBeam[LayerCount] = pow(pow(LeafAngle[LayerCount],2)+pow(tan(Zenith),2),0.5)/
                            (LeafAngle[LayerCount]+1.774*pow(LeafAngle[LayerCount]+1.182,-0.733));}
    // Direct SW radiation
    // Initialize above canopy
    DirectRadTransient = DirectRad;
    for(LayerCount=0;LayerCount<Layers;LayerCount++){
        SunlitAbsorbed[LayerCount] = 0;
        ShadedAbsorbed[LayerCount] = 0;}
    for(LayerCount=0;LayerCount<Layers;LayerCount++){
        CumulativeLAI += LAIProfile[LayerCount];
        // Sunlit and shaded leaf fractions
        if (Zenith>0 && Zenith<Pi/2){
            FSun[LayerCount] = exp(-KBeam[LayerCount]*Clump*CumulativeLAI);
            FShade[LayerCount] = 1-FSun[LayerCount];}
        else{
            FSun[LayerCount] = 0;
            FShade[LayerCount] = 1;}
        //Extinction
        Extinction = exp(-KBeam[LayerCount]*Clump*LAIProfile[LayerCount]);
        Intercepted = DirectRadTransient*(1-Extinction);
        Unintercepted = DirectRadTransient*Extinction;
        Absorbed = Absorptivity*Intercepted;
        Transmitted = Transmissivity*Intercepted;
        Reflected = Reflectivity*Intercepted;

        DownDirectRad[LayerCount] = Unintercepted;
        DownDiffuseRad[LayerCount] = Transmitted;
        UpDiffuseRad[LayerCount] = Reflected;

        DirectRadTransient = DownDirectRad[LayerCount];

        SunlitAbsorbed[LayerCount] = Absorbed;}
    // Soil reflectivity
    LayerCount = Layers-1;
    UpDiffuseRad[LayerCount] += SoilReflectivity*DownDirectRad[LayerCount];
    SoilAbsorbed = SoilAbsorptivity*DownDirectRad[LayerCount];

    // Initialize above canopy
    DownDiffuseRadTransient = DiffuseRad;
    while(fabs(Error)>=0.000001 && ErrorCount<=MaxErrorCount){
        // Diffuse SW radiation downward
        for(LayerCount=0;LayerCount<Layers;LayerCount++){
            // Extinction
            Extinction = exp(-KDiffuse*Clump*LAIProfile[LayerCount]);
            Intercepted = DownDiffuseRadTransient*(1-Extinction);
            Unintercepted = DownDiffuseRadTransient*Extinction;
            Absorbed = Absorptivity*Intercepted;
            Transmitted = Transmissivity*Intercepted;
            Reflected = Reflectivity*Intercepted;

            DownDiffuseRad[LayerCount] += Unintercepted+Transmitted;
            UpDiffuseRad[LayerCount] += Reflected;

            DownDiffuseRadTransient = DownDiffuseRad[LayerCount];

            SunlitAbsorbed[LayerCount] += Absorbed*FSun[LayerCount];
            ShadedAbsorbed[LayerCount] += Absorbed*FShade[LayerCount];}
        // Soil reflectivity
        LayerCount = Layers-1;
        UpDiffuseRadTransient = SoilReflectivity*DownDiffuseRad[LayerCount];
        SoilAbsorbed += SoilAbsorptivity*DownDiffuseRad[LayerCount];
        // Reset totals
        for(LayerCount=0;LayerCount<Layers;LayerCount++){
            DownDiffuseRad[LayerCount] = DownDiffuseRad[LayerCount]*0;}
        // Diffuse SW radiation upward
        for(LayerCount=(Layers-1);LayerCount>=0;LayerCount--){
            // Extinction
            Extinction = exp(-KDiffuse*Clump*LAIProfile[LayerCount]);
            Intercepted = UpDiffuseRadTransient*(1-Extinction);
            Unintercepted = UpDiffuseRadTransient*Extinction;
            Absorbed = Absorptivity*Intercepted;
            Transmitted = Transmissivity*Intercepted;
            Reflected = Reflectivity*Intercepted;

            UpDiffuseRad[LayerCount] += Unintercepted+Transmitted;
            DownDiffuseRad[LayerCount] += Reflected;

            DownDiffuseRadTransient = DownDiffuseRad[LayerCount];
            UpDiffuseRadTransient = UpDiffuseRad[LayerCount];

            SunlitAbsorbed[LayerCount] += Absorbed*FSun[LayerCount];
            ShadedAbsorbed[LayerCount] += Absorbed*FShade[LayerCount];}
        SkyLost += UpDiffuseRad[0];
        // Reset totals
        for(LayerCount=0;LayerCount<Layers;LayerCount++){
            UpDiffuseRad[LayerCount] = UpDiffuseRad[LayerCount]*0;}
        DownDiffuseRadTransient = 0;
        // Convergence criteria
        TotalAbsorbed = SoilAbsorbed;
        for(LayerCount=0;LayerCount<Layers;LayerCount++){
            TotalAbsorbed += SunlitAbsorbed[LayerCount]+ShadedAbsorbed[LayerCount];}
        Error = (((DirectRad+DiffuseRad)-TotalAbsorbed-SkyLost))
                /(DirectRad+DiffuseRad);
        ErrorCount += 1;}
    free(KBeam);
    free(DownDirectRad);
    free(DownDiffuseRad);
    free(UpDiffuseRad);
return;}

// Compute wind profile
void ComputeWindProfile(double WindSpeed,double LAIProfile[],int Layers,double WindProfile[]){
    int LayerCount;
    double KWind=0.7;
    double CumulativeLAI=0;
    for(LayerCount=0;LayerCount<Layers;LayerCount++){
        CumulativeLAI += LAIProfile[LayerCount];
        WindProfile[LayerCount] = WindSpeed*exp(-KWind*CumulativeLAI);}
return;}

// Compute RH profile
void ComputeRHProfile(double RH,double LAIProfile[],int Layers,double RHProfile[]){
    int LayerCount;
    double KRH=1-RH;
    double CumulativeLAI=0;
    for(LayerCount=0;LayerCount<Layers;LayerCount++){
        CumulativeLAI += LAIProfile[LayerCount];
        RHProfile[LayerCount] = RH*exp(-KRH*CumulativeLAI);  // XXX Negative sign before KRH missing in original
        if (RHProfile[LayerCount]>1){
            RHProfile[LayerCount]=0.99;}}
return;}

// Compute Leaf nitrogen profile
void ComputeLeafNProfile(double LeafN,double LAIProfile[],int Layers,double KLeafN,double LeafNProfile[]){
    int LayerCount;
    double CumulativeLAI=0;
    for(LayerCount=0;LayerCount<Layers;LayerCount++){
        CumulativeLAI += LAIProfile[LayerCount];
        LeafNProfile[LayerCount] = LeafN*exp(-KLeafN*CumulativeLAI);}
return;}

// Compute C3 photosynthesis
void ComputeC3Canopy(int TimeCount,double LAI,double LAIProfile[],struct Weather WeatherData,
                     double Lattitude,struct CanopyParameters Canopy,double LeafAngle[],
                     struct PhotosynthesisParameters Photosynthesis,
                     double LeafN,double KLeafN,double lnb0,double lnb1,int lnfun,
                     struct CanopyStructure *Canopy1,double FSun[],double FShade[],
                     double SunlitAbsorbed[],double ShadedAbsorbed[],
                     double SunlitAssimilation[],double ShadedAssimilation[]){
    if (LAI>0){
        // Temporary variables
        int LayerCount;
        int Layers=Canopy.Layers;
        // Canopy radiation variables
        double Zenith,DiffuseFraction;
        double Absorptivity = 0.8;
        double Reflectivity,Transmissivity;
        double DirectSWRad,DiffuseSWRad;
        // Scalar canopy micro-environment variables
        double *WindProfile,*RHProfile,*LeafNProfile;
        WindProfile=(double *)malloc(sizeof(double)*Layers);
        RHProfile=(double *)malloc(sizeof(double)*Layers);
        LeafNProfile=(double *)malloc(sizeof(double)*Layers);
        double TotalHeight=1.0,CanopyHeight;
        // Local photosynthesis and transpiration variables
        double GPP=0,NPP=0,Transpiration=0;
        // struct ET_Str SunlitTranspiration,ShadedTranspiration;
        struct c3_str SunlitPhotosynthesis,ShadedPhotosynthesis;
        double LeafTemp;
        // Conversion factors
        double BiomassConvert=3600*1e-6*30*1e-6*10000; // Conversion factor for u moles CO2/m2/s to Mg/Ha of biomass
        double WaterConvert=3600*1e-3*18*1e-6*10000; // Conversion factor for u moles H2O/m2/s to Mg/Ha of water

        // Compute Zenith angle
        ComputeZenith(Lattitude,WeatherData.DayOfYear,WeatherData.Hour,&Zenith);
        // Compute fraction of diffuse radiation
        DiffuseFraction = ComputeDiffuseFraction(WeatherData.Solar,Zenith,WeatherData.DayOfYear);
        DirectSWRad = (1-DiffuseFraction)*WeatherData.Solar;
        DiffuseSWRad = DiffuseFraction*WeatherData.Solar;

        // Sunlit and shaded multi layer model
        if (DirectSWRad>0.1 || DiffuseSWRad>0.1){
            ComputeRadProfile(DirectSWRad,DiffuseSWRad,LAIProfile,Layers,Zenith,Canopy.KDiffuse,
                                LeafAngle,Canopy.Clump,Absorptivity,FSun,FShade,
                                SunlitAbsorbed,ShadedAbsorbed);}
        else{
            for(LayerCount=0;LayerCount<Layers;LayerCount++){
                FSun[LayerCount] = 0.0;
                FShade[LayerCount] = 1.0;
                SunlitAbsorbed[LayerCount] = 0.0;
                ShadedAbsorbed[LayerCount] = 0.0;}}
        // Compute wind speed profile
        ComputeWindProfile(WeatherData.Wind,LAIProfile,Layers,WindProfile);
        // Compute RH profile
        ComputeRHProfile(WeatherData.RH,LAIProfile,Layers,RHProfile);
        // Compute leaf nitrogen profile
        ComputeLeafNProfile(LeafN,LAIProfile,Layers,KLeafN,LeafNProfile);
        // Compute photosynthesis and transpiration
        for(LayerCount=0;LayerCount<Layers;LayerCount++){
            CanopyHeight = TotalHeight/Layers*(Layers-LayerCount);
            // Sunlit leaves
            // SunlitTranspiration = c3EvapoTrans(SunlitAbsorbed[LayerCount],SunlitAbsorbed[LayerCount],
            //                       WeatherData.Temp,RHProfile[LayerCount],WindProfile[LayerCount,
            //                       LAIProfile[LayerCount],CanopyHeight,
            //                       Vmax,Jmax,Rd,b0,b1,Catm,O2,theta,StomWS,ws);
            // LeafTemp = WeatherData.Temp+SunlitTranspiration.Deltat;
            LeafTemp = WeatherData.Temperature;
            SunlitPhotosynthesis = c3photoC(SunlitAbsorbed[LayerCount]/LAIProfile[LayerCount],
                                            LeafTemp,RHProfile[LayerCount],
                                            Photosynthesis.Vcmax25,Photosynthesis.Jmax25,
                                            Photosynthesis.Rd25,Photosynthesis.BallBerryIntercept,
                                            Photosynthesis.BallBerrySlope,Photosynthesis.CO2,
                                            Photosynthesis.O2,Photosynthesis.Theta,
                                            Photosynthesis.WaterStressFactor,Photosynthesis.WaterStressFunction);
            // SunlitAssimilation[LayerCount] = LAIProfile[LayerCount]*FSun[LayerCount]*SunlitPhotosynthesis.Assim;
            SunlitAssimilation[LayerCount] = SunlitPhotosynthesis.Assim;
            // Shaded leaves
            // ShadedTranspiration = c3EvapoTrans(ShadedAbsorbed[LayerCount],ShadedAbsorbed[LayerCount],
            //                       WeatherData.Temp,RHProfile[LayerCount],WindProfile[LayerCount,
            //                       LAIProfile[LayerCount],CanopyHeight,
            //                       Vmax,Jmax,Rd,b0,b1,Catm,O2,theta,StomWS,ws);
            // LeafTemp = WeatherData.Temp+ShadedTranspiration.Deltat;
            ShadedPhotosynthesis = c3photoC(ShadedAbsorbed[LayerCount]/LAIProfile[LayerCount],
                                            LeafTemp,RHProfile[LayerCount],
                                            Photosynthesis.Vcmax25,Photosynthesis.Jmax25,
                                            Photosynthesis.Rd25,Photosynthesis.BallBerryIntercept,
                                            Photosynthesis.BallBerrySlope,Photosynthesis.CO2,
                                            Photosynthesis.O2,Photosynthesis.Theta,
                                            Photosynthesis.WaterStressFactor,Photosynthesis.WaterStressFunction);
            // ShadedAssimilation[LayerCount] = LAIProfile[LayerCount]*FShade[LayerCount]*ShadedPhotosynthesis.Assim;
            ShadedAssimilation[LayerCount] = ShadedPhotosynthesis.Assim;
            // Total canopy
            GPP += LAIProfile[LayerCount]*FSun[LayerCount]*SunlitPhotosynthesis.GrossAssim+
                   LAIProfile[LayerCount]*FShade[LayerCount]*ShadedPhotosynthesis.GrossAssim;
            NPP += LAIProfile[LayerCount]*FSun[LayerCount]*SunlitPhotosynthesis.Assim+
                   LAIProfile[LayerCount]*FShade[LayerCount]*ShadedPhotosynthesis.Assim;
            // Transpiration += LAIProfile[LayerCount]*FSun[LayerCount]*SunlitTranspiration.TransR+
            //                  LAIProfile[LayerCount]*FShade[LayerCount]*ShadedTranspiration.TransR;
            Transpiration = 0.0;}

        Canopy1->GrossAssim = BiomassConvert*GPP;
        Canopy1->Assim = BiomassConvert*NPP;
        Canopy1->Trans = WaterConvert*Transpiration;
        if (Canopy1->GrossAssim<0 || Canopy1->GrossAssim<Canopy1->Assim){
            printf("Problem at time step = %d, Canopy GPP = %f\t canopy NPP = %f\n",
                   TimeCount,Canopy1->GrossAssim,Canopy1->Assim);
            Canopy1->GrossAssim = 0.0;
            Canopy1->Assim = 0.0;
            Canopy1->Trans = 0.0;}
        free(WindProfile);
        free(RHProfile);
        free(LeafNProfile);}
    else{
        Canopy1->GrossAssim = 0.0;
        Canopy1->Assim = 0.0;
        Canopy1->Trans = 0.0;}
return;}
