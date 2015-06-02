void ComputeReadParameters(char *FileName,int *StartDay,int *EndDay,double *DeltaT,double *Latitude,
                           struct CanopyParameters *Canopy,double *TBase,double **StageThermalTime,
                           struct PhotosynthesisParameters *Photosynthesis,struct SoilParameters *Soil,
                           struct PlantParts *Senescence,struct PlantParts *InitialBiomass,int *Stages,
                           struct PlantParts **PartitionCoefficients,struct PlantParts *GRespirationFraction,
                           struct PlantParts *Q10Respiration,struct PlantParts *MRespirationBase);

void ComputeReadXML(char *FileName,int *StartDay,int *EndDay,double *DeltaT,double *Latitude,
                    struct CanopyParameters *Canopy,double *TBase,double **StageThermalTime,
                    struct PhotosynthesisParameters *Photosynthesis,struct SoilParameters *Soil,
                    struct PlantParts *Senescence,struct PlantParts *InitialBiomass,int *Stages,
                    struct PlantParts **PartitionCoefficients,struct PlantParts *GRespirationFraction,
                    struct PlantParts *Q10Respiration,struct PlantParts *MRespirationBase);

void ComputeThermalTime (double Temperature[],double TBase,int StartTime,int EndTime,double DeltaT,
                         double ThermalTime[],double DailyThermalTime[]);

void ComputeWeather(int DayOfYear,int Hour,double Solar,double Temperature,double RH,double Wind,
                    double Precipitation,struct Weather *WeatherData);

void ComputeMRespiration(struct CanopyStructure Canopy,struct PlantParts Biomass,struct PlantParts Q,
                         struct PlantParts mResp,double Temperature,double DeltaT,
                         struct PlantParts *MRespiration);

double ComputeMRespirationPart(double Biomass,double Q,double mResp,double Temperature,double DeltaT);

void ComputeNewLitter(struct PlantParts StoreAddBiomass[],struct PlantParts Senescence,double DailyThermalTime[],
                      int TimeCount,int DayCount,struct PlantParts *SenescenceIndex,struct PlantParts *NewLitter);

void ComputeLitter(struct PlantParts NewLitter,struct PlantParts *Litter);

void ComputePhenologyStage(int Stages,double ThermalTime,double StageThermalTime[],int *PhenologyStage);

void ComputeAddBiomass(double DailyCanopyNPP,struct PlantParts PartitionCoefficients[],int PhenologyStage,
                       struct PlantParts *AddBiomass);

void ComputeGRespiration(struct PlantParts GRespirationFraction,struct PlantParts *GRespiration,
                         struct PlantParts *AddBiomass);

void ComputeBiomass(struct PlantParts NewLitter,struct PlantParts AddBiomass,double FrostFraction,
                    double RemobFraction,struct PlantParts *Biomass);

void ComputeC3Canopy(int TimeCount,double LAI,double LAIProfile[],struct Weather WeatherData,
                     double Lattitude,struct CanopyParameters Canopy,double LeafAngle[],
                     struct PhotosynthesisParameters Photosynthesis,
                     double LeafN,double KLeafN,double lnb0,double lnb1,int lnfun,
                     struct CanopyStructure *Canopy1,double FSun[],double FShade[],
                     double SunlitAbsorbed[],double ShadedAbsorbed[],
                     double SunlitAssimilation[],double ShadedAssimilation[]);
