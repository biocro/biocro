 /*
 *  soyCro/src/BioCro.c by Venkatraman Srinivasan
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "soyGro.h"
#include "Structures.h"
#include "Functions.h"
#include "c3photo.h"

int soyGro(char *ParameterFileName,char *WeatherFileName,char *OutputFolderName){
    //***********************************************************************************************
    // Declare, allocate and initialize data, parameters and variables
    // Temporary index and variables
    int StartTime=0,EndTime=0,PhenologyStage=0,DayCount=0,TimeCount=0,LayerCount=0,Option=1;
    float HourCount=0;
    //***********************************************************************************************
    // Input variables
    double Latitude,DeltaT;
    int Years,StartDay,EndDay;
    struct CanopyParameters Canopy; // Canopy structure variables
    struct PhotosynthesisParameters Photosynthesis; // Photosynthesis variables
    struct PlantParts GRespirationFraction; // Growth respiration variables
    struct PlantParts Q10Respiration; // Maintenance respiration variables
    struct PlantParts MRespirationBase; // Maintenance respiration variables
    struct PlantParts Senescence; // Senescence thermal time variables [C Days]
    // Frost kill variables
    double TFrostHigh=5; // Upper temperature threshold for leaf frost kill [C]
    double TFrostLow=0; //  Upper temperature threshold for leaf frost kill [C]
    double dailyMinTemp=100.0; // Initialize minimum temperature for daily frost kill [C]
    double FrostFractionLow=5; // Percent of leaf dying per day [-/day]
    double RemobFraction=0.6; // Fraction of CH2O remobilized from leaf due to frost kill [-]
    double FrostFraction=0.0; // Initialize fraction of leaf frost kill [-]
    struct SoilParameters Soil; // Soil variables
    struct PlantParts Biomass; // Initial biomass variables [Mg/Ha]
    // Carbon partition variables
    double TBase; // Thermal time base temperature  [C]
    int Stages; // Number of partition stages
    double *StageThermalTime; StageThermalTime = NULL; // Thermal time for stage change [C days]
    struct PlantParts *PartitionCoefficients; PartitionCoefficients = NULL; // Partition coefficients
    if(Option==0){ // Specify values in code
        Latitude=40;
        DeltaT=1;
        Years=2002;
        StartDay=180;
        EndDay=300;
        Canopy=(struct CanopyParameters){3.75,0,10,0.37,1,1,1}; // Canopy structure variables;
        Photosynthesis=(struct PhotosynthesisParameters){100,180,0.76,0.8,0.008,10.6,550,210,3,1}; // Photosynthesis variables;
        GRespirationFraction=(struct PlantParts){0.3,0.3,0.3,0.3,0.3,0.3,0.3,0.3}; // Growth respiration variables;
        Q10Respiration=(struct PlantParts){2,2,2,2,2,2,2,2}; // Maintenance respiration variables;
        MRespirationBase=(struct PlantParts){0.0120,0.0016,0.000,0.0000,0.0036,0.0000,0.0000,0.0000}; // Maintenance respiration variables;
        Senescence=(struct PlantParts){1200,2000,9999,9999,2000,9999,9999}; // Senescence thermal time variables [C Days];
        Biomass=(struct PlantParts){0.25,0.25,0.00,0.00,0.25,0.00,0.00,0.75}; // Initial biomass variables [Mg/Ha];
        // Carbon partition variables
        TBase=0; // Thermal time base temperature  [C]
        Stages=6; // Number of partition stages
        StageThermalTime = malloc(sizeof(double)*Stages); // Thermal time for stage change [C days]
        memcpy(StageThermalTime,(double[6]){0,100,500,1000,2000,3000},sizeof(double)*6); // Thermal time for stage change [C days]
        PartitionCoefficients = malloc(sizeof(struct PlantParts)*Stages);// Partition coefficients
        PartitionCoefficients[0]=(struct PlantParts){0.3,0.5,0.0,0.0,0.2,0.0,0.0,1.0};
        PartitionCoefficients[1]=(struct PlantParts){0.3,0.5,0.0,0.0,0.2,0.0,0.0,1.0};
        PartitionCoefficients[2]=(struct PlantParts){0.3,0.5,0.0,0.0,0.2,0.0,0.0,1.0};
        PartitionCoefficients[3]=(struct PlantParts){0.3,0.6,0.0,0.0,0.1,0.0,0.0,1.0};
        PartitionCoefficients[4]=(struct PlantParts){0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0};
        PartitionCoefficients[5]=(struct PlantParts){0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0};}
    else{ // Model parameter data from parameter input file
        // ComputeReadXML(ParameterFileName,&StartDay,&EndDay,&DeltaT,&Latitude,&Canopy,&TBase,
        //                &StageThermalTime,&Photosynthesis,&Soil,&Senescence,&Biomass,&Stages,
        //                &PartitionCoefficients,&GRespirationFraction,&Q10Respiration,&MRespirationBase);}
        ComputeReadParameters(ParameterFileName,&StartDay,&EndDay,&DeltaT,&Latitude,&Canopy,&TBase,
                              &StageThermalTime,&Photosynthesis,&Soil,&Senescence,&Biomass,&Stages,
                              &PartitionCoefficients,&GRespirationFraction,&Q10Respiration,&MRespirationBase);}
    //***********************************************************************************************
    // Weather data
    FILE *DataInput;
    DataInput = fopen(WeatherFileName,"r");
    if (DataInput == NULL){printf("can not open file \n"); return 1;}
    int Size=365*24/DeltaT;
    int *Year; Year=malloc(sizeof(int)*Size);
    int *DayOfYear; DayOfYear=malloc(sizeof(int)*Size);
    int *Hour; Hour=malloc(sizeof(int)*Size);
    double *Solar; Solar=malloc(sizeof(double)*Size);
    double *Temperature; Temperature=malloc(sizeof(double)*Size);
    double *RH; RH=malloc(sizeof(double)*Size);
    double *Wind; Wind=malloc(sizeof(double)*Size);
    double *Precipitation; Precipitation=malloc(sizeof(double)*Size);
    for(TimeCount=0;TimeCount<Size;TimeCount++){
        fscanf(DataInput,"%i %i %i %lf %lf %lf %lf %lf \n",&Year[TimeCount],
              &DayOfYear[TimeCount],&Hour[TimeCount],&Solar[TimeCount],&Temperature[TimeCount],
              &RH[TimeCount],&Wind[TimeCount],&Precipitation[TimeCount]);}
    fclose(DataInput);
    struct Weather WeatherData;
    //***********************************************************************************************
    // Output variables
    // Canopy structure variables
    double *LeafAngle; // Leaf angle distribution
    LeafAngle=malloc(sizeof(double)*Canopy.Layers); memset(LeafAngle,0,Canopy.Layers);
    for(LayerCount=0;LayerCount<Canopy.Layers;LayerCount++){
        // LeafAngle[LayerCount] = 0.5+0.25*AngleCount;}
        LeafAngle[LayerCount] = Canopy.Chi;}
    double *SunlitAssimilation,*ShadedAssimilation;
    SunlitAssimilation=malloc(sizeof(double)*Canopy.Layers); memset(SunlitAssimilation,0,Canopy.Layers);
    ShadedAssimilation=malloc(sizeof(double)*Canopy.Layers); memset(ShadedAssimilation,0,Canopy.Layers);
    // Litter variables
    struct PlantParts SenescenceIndex={0,0,0,0,0,0,0,0};
    struct PlantParts NewLitter={0,0,0,0,0,0,0,0};
    struct PlantParts Litter={0,0,0,0,0,0,0,0};
    double AboveLitter=0.0,BelowLitter=0.0; // Above and below ground litter
    // Carbon partitioning variables
    int DaySize=(int)(DeltaT*Size/24);
    struct PlantParts AddBiomass,*StoreAddBiomass;
    StoreAddBiomass=malloc(sizeof(struct PlantParts)*DaySize); memset(StoreAddBiomass,0,DaySize);
    StoreAddBiomass[StartDay-1]=Biomass;
    AddBiomass=Biomass;
    // Canopy flux variables
    double dailyCanopyGPP=0.0,dailyCanopyNPP=0.0,dailyCanopyET=0.0;
    struct CanopyStructure Canopy1={0,0,0};
    // Canopy radiation variables
    double *LAIProfile,*FSun,*FShade;
    LAIProfile=malloc(sizeof(double)*Canopy.Layers); memset(LAIProfile,0,Canopy.Layers);
    FSun=malloc(sizeof(double)*Canopy.Layers); memset(FSun,0,Canopy.Layers);
    FShade=malloc(sizeof(double)*Canopy.Layers); memset(FShade,0,Canopy.Layers);
    double *SunlitAbsorbed,*ShadedAbsorbed;
    SunlitAbsorbed=malloc(sizeof(double)*Canopy.Layers); memset(SunlitAbsorbed,0,Canopy.Layers);
    ShadedAbsorbed=malloc(sizeof(double)*Canopy.Layers); memset(ShadedAbsorbed,0,Canopy.Layers);
    // Respiration variables
    struct PlantParts GRespiration;
    struct PlantParts MRespiration,DailyMRespiration={0,0,0,0,0,0,0,0};
    // Thermal time variables
    double *ThermalTime,*DailyThermalTime;
    ThermalTime=malloc(sizeof(double)*Size);
    DailyThermalTime=malloc(sizeof(double)*DaySize);
    for(TimeCount=0;TimeCount<Size;TimeCount++){ThermalTime[TimeCount]=0.0;}
    for(DayCount=0;DayCount<DaySize;DayCount++){DailyThermalTime[DayCount]=0.0;}
    // Leaf area index variables
    double LAI=Biomass.Leaf*Canopy.SLAI; // Initialize leaf area index [-]
    // Leaf nitrogen variables
    double LeafN_0 = 2.0; // Initial leaf nitrogen content [g/m2]
    double LeafN = LeafN_0;
    double kpLN=0.2; // XXX what is this?
    double lnb0=-5.0; // XXX what is this?
    double lnb1=18; // XXX what is this?
    int lnfun=1; // XXX what is this?
    // Output file variables
    char RespirationOutputFileName[200];strcpy(RespirationOutputFileName,OutputFolderName);
    char AllocationOutputFileName[200];strcpy(AllocationOutputFileName,OutputFolderName);
    char PartitionOutputFileName[200];strcpy(PartitionOutputFileName,OutputFolderName);
    char LitterOutputFileName[200];strcpy(LitterOutputFileName,OutputFolderName);
    char BiomassOutputFileName[200];strcpy(BiomassOutputFileName,OutputFolderName);
    char CFluxOutputFileName[200];strcpy(CFluxOutputFileName,OutputFolderName);
    char soyGroOutputFileName[200];strcpy(soyGroOutputFileName,OutputFolderName);
    FILE *RespirationOutputFile,*AllocationOutputFile,*PartitionOutputFile,
         *LitterOutputFile,*BiomassOutputFile,*CFluxOutputFile,*soyGroOutputFile;
    strcat(RespirationOutputFileName,"/RespirationOutput.txt");
    RespirationOutputFile=fopen(RespirationOutputFileName,"w");
    strcat(AllocationOutputFileName,"/AllocationOutput.txt");
    AllocationOutputFile=fopen(AllocationOutputFileName,"w");
    strcat(PartitionOutputFileName,"/PartitionOutput.txt");
    PartitionOutputFile=fopen(PartitionOutputFileName,"w");
    strcat(LitterOutputFileName,"/LitterOutput.txt");
    LitterOutputFile=fopen(LitterOutputFileName,"w");
    strcat(BiomassOutputFileName,"/BiomassOutput.txt");
    BiomassOutputFile=fopen(BiomassOutputFileName,"w");
    strcat(CFluxOutputFileName,"/CFluxOutput.txt");
    CFluxOutputFile=fopen(CFluxOutputFileName,"w");
    strcat(soyGroOutputFileName,"/soyGroOutput.txt");
    soyGroOutputFile=fopen(soyGroOutputFileName,"w");
    //***********************************************************************************************
    // Select day of year for simulation
    DayCount = StartDay;
    StartTime = DayCount*24-1;
    EndTime = StartTime+24*(EndDay+30-StartDay);
    //***********************************************************************************************
    // Compute cumulative thermal time
    ComputeThermalTime (Temperature,TBase,StartTime,EndTime,DeltaT,ThermalTime,DailyThermalTime);
    //***********************************************************************************************
    // Start time loop
    for(TimeCount=StartTime;TimeCount<EndTime;TimeCount++){
        //*********************************************************************************************
        //Compute weather
        ComputeWeather(DayOfYear[TimeCount],Hour[TimeCount],Solar[TimeCount],Temperature[TimeCount],
                       RH[TimeCount],Wind[TimeCount],Precipitation[TimeCount],&WeatherData);
        // Compute daily minimum temperature
        if (dailyMinTemp>Temperature[TimeCount]){
            dailyMinTemp = Temperature[TimeCount];}
        // Compute LAI profile
        for(LayerCount=0;LayerCount<Canopy.Layers;LayerCount++){
            LAIProfile[LayerCount] = LAI/Canopy.Layers;}
        //*********************************************************************************************
        // Compute canopy photosynthesis
        ComputeC3Canopy(TimeCount,LAI,LAIProfile,WeatherData,Latitude,Canopy,LeafAngle,
                        Photosynthesis,LeafN,kpLN,lnb0,lnb1,lnfun,
                        &Canopy1,FSun,FShade,SunlitAbsorbed,ShadedAbsorbed,
                        SunlitAssimilation,ShadedAssimilation);
        //***********************************************************************************************
        // Compute maintenance respiration
        ComputeMRespiration(Canopy1,Biomass,Q10Respiration,MRespirationBase,
                            Temperature[TimeCount],DeltaT,&MRespiration);
        //***********************************************************************************************
        // Compute daily totals of assimilation
        dailyCanopyGPP += Canopy1.GrossAssim*DeltaT;
        dailyCanopyET += Canopy1.Trans*DeltaT;
        dailyCanopyNPP += Canopy1.GrossAssim*DeltaT-MRespiration.Total; // Daily net assimilation including maintenance respiration
        //*************************************************************************************************
        // Compute allocation and biomass partitioning at a daily time scale
        if(TimeCount%24 == 0){
            //***********************************************************************************************
            // Compute senescence index and litter based on thermal time
            ComputeNewLitter(StoreAddBiomass,Senescence,DailyThermalTime,TimeCount,
                             DayCount,&SenescenceIndex,&NewLitter);
            // Compute additional leaf litter from frost damage
            if(DayCount>150 && dailyMinTemp<=TFrostLow){
                FrostFraction = 1.0;}
            else if(DayCount>150 && dailyMinTemp<=TFrostHigh){
                FrostFraction = 1-(1-FrostFractionLow/100)/(TFrostHigh-TFrostLow)*(dailyMinTemp-TFrostLow);}
            else{
                FrostFraction = 0.0;}
            FrostFraction = 0.0;
            //***********************************************************************************************
            // Compute total litter
            ComputeLitter(NewLitter,&Litter);
            // Compute total above and below ground litter
            AboveLitter = Litter.Leaf+Litter.Stem+Litter.Flower+Litter.Grain;
            BelowLitter = Litter.Root+Litter.Rhizome+Litter.Nodule;
            //***********************************************************************************************
            // Compute carbon available for partitioning
            dailyCanopyNPP += Biomass.Leaf*FrostFraction*RemobFraction; // Inputs from leaf frost remobilization
            // printf("Day = %d, Daily canopy NPP total = %f \n",DayCount,dailyCanopyNPP);
            //*********************************************************************************************
            // Compute phenology stage from thermal period
            ComputePhenologyStage(Stages,ThermalTime[TimeCount],StageThermalTime,&PhenologyStage);
            //*********************************************************************************************
            //  Compute carbon allocation (pre growth respiration)
            ComputeAddBiomass(dailyCanopyNPP,PartitionCoefficients,PhenologyStage,&AddBiomass);
            //*********************************************************************************************
            // Compute growth respiration and update carbon allocation (post growth respiration)
            ComputeGRespiration(GRespirationFraction,&GRespiration,&AddBiomass);
            StoreAddBiomass[DayCount] = AddBiomass;
            dailyCanopyNPP -= GRespiration.Total; // Update daily NPP
            //***********************************************************************************************
            // Compute new biomass
            ComputeBiomass(NewLitter,AddBiomass,FrostFraction,RemobFraction,&Biomass);
            //***********************************************************************************************
            // Update specific leaf area and LAI
            // ComputeBiomassToPlant(Biomass,&Plant)
            LAI = Biomass.Leaf*Canopy.SLAI;
            //***********************************************************************************************
            // Write output to a file
            fprintf(RespirationOutputFile,"%d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",DayCount,
                    dailyCanopyGPP,DailyMRespiration.Leaf,DailyMRespiration.Stem,DailyMRespiration.Flower,
                    DailyMRespiration.Grain,DailyMRespiration.Root,DailyMRespiration.Nodule,
                    DailyMRespiration.Rhizome,DailyMRespiration.Total);
            fprintf(AllocationOutputFile,"%d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",DayCount,LAI,
                    AddBiomass.Leaf,AddBiomass.Stem,AddBiomass.Flower,AddBiomass.Grain,
                    AddBiomass.Root,AddBiomass.Rhizome,AddBiomass.Nodule);
            fprintf(PartitionOutputFile,"%d\t %d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",DayCount,
                    PhenologyStage,DailyThermalTime[DayCount],LAI,PartitionCoefficients[PhenologyStage].Leaf,
                    PartitionCoefficients[PhenologyStage].Stem,PartitionCoefficients[PhenologyStage].Flower,
                    PartitionCoefficients[PhenologyStage].Grain,PartitionCoefficients[PhenologyStage].Root,
                    PartitionCoefficients[PhenologyStage].Rhizome,PartitionCoefficients[PhenologyStage].Nodule);
            fprintf(LitterOutputFile,"%d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",DayCount,LAI,
                    NewLitter.Leaf,NewLitter.Stem,NewLitter.Flower,NewLitter.Grain,NewLitter.Root,NewLitter.Rhizome,
                    NewLitter.Nodule,(Biomass.Leaf*FrostFraction*(1-RemobFraction)),ThermalTime[TimeCount]);
            fprintf(BiomassOutputFile,"%d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",DayCount,LAI,
                    Biomass.Leaf,Biomass.Stem,Biomass.Flower,Biomass.Grain,Biomass.Root,Biomass.Rhizome,Biomass.Nodule);
            //***********************************************************************************************
            // Reset daily totals
            NewLitter = (struct PlantParts){0,0,0,0,0,0,0,0};
            DailyMRespiration = (struct PlantParts){0,0,0,0,0,0,0,0};
            dailyCanopyNPP = 0.0;
            dailyCanopyGPP = 0.0;
            dailyCanopyET = 0.0;
            HourCount = 0;
            // Add new day
            DayCount += 1;}
        //***********************************************************************************************
        // Write output to a file
        fprintf(CFluxOutputFile,"%d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",TimeCount,ThermalTime[TimeCount],
                LAI,Canopy1.GrossAssim*DeltaT,MRespiration.Leaf,MRespiration.Stem,MRespiration.Flower,
                MRespiration.Grain,MRespiration.Root,MRespiration.Nodule,MRespiration.Rhizome,MRespiration.Total);
        fprintf(soyGroOutputFile,"%d\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\t %f\n",DayCount,HourCount,
                ThermalTime[TimeCount],LAI,Biomass.Leaf,Biomass.Stem,Biomass.Flower,Biomass.Grain,Biomass.Root,
                Biomass.Rhizome,Biomass.Nodule);
        HourCount += DeltaT;}
        // Close output file
    fclose(RespirationOutputFile);
    fclose(AllocationOutputFile);
    fclose(PartitionOutputFile);
    fclose(LitterOutputFile);
    fclose(BiomassOutputFile);
    fclose(CFluxOutputFile);
    fclose(soyGroOutputFile);
    //***********************************************************************************************
    // Free up memory for malloc statements
    free(Year);
    free(DayOfYear);
    free(Hour);
    free(Solar);
    free(Temperature);
    free(RH);
    free(Wind);
    free(Precipitation);

    free(StageThermalTime);
    free(PartitionCoefficients);

    free(LeafAngle);
    free(SunlitAssimilation);
    free(ShadedAssimilation);

    free(StoreAddBiomass);
    free(LAIProfile);
    free(FSun);
    free(FShade);
    free(SunlitAbsorbed);
    free(ShadedAbsorbed);

    free(ThermalTime);
    free(DailyThermalTime);

    printf("C program finished \n");
return 0;}
