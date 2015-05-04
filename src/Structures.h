struct Weather{int DayOfYear;
               int Hour;
               double Solar;
               double Temperature;
               double RH;
               double Wind;
               double Precipitation;};

struct CanopyParameters{double SLAI;
                        double SLAIDecline;
                        int Layers;
                        double KDiffuse;
                        double Chi;
                        double Clump;
                        double HeightFactor;};

struct PhotosynthesisParameters{double Vcmax25;
                                double Jmax25;
                                double Theta;
                                double Rd25;
                                double BallBerryIntercept;
                                double BallBerrySlope;
                                double CO2;
                                double O2;
                                int WaterStressFunction;
                                double WaterStressFactor;};

struct SoilParameters{double FieldCapacity;
                      double WiltingPoint;
                      double SoilDepth;
                      int wsFun;
                      double scsf;
                      int transResp;
                      int leafPotTh;
                      int hydrDist;
                      double rfl;
                      double rsec;
                      double rsdf;};

struct PlantParts{double Leaf;
                  double Stem;
                  double Flower;
                  double Grain;
                  double Root;
                  double Rhizome;
                  double Nodule;
                  double Total;};

struct RadiationParts{double PAR;
                      double NIR;
                      double LW;};

struct CanopyStructure{double Assim;
                       double Trans;
                       double GrossAssim;};
