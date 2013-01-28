extern void sunML(double Idir, double Idiff, double LAI, int nlayers, 
		  double cosTheta, double kd, double chil, double heighf);

extern void lightME(double lat, int DOY, int td);

extern void RHprof(double RH, int nlayers);


extern void WINDprof(double WindSpeed, double LAI, int nlayers);

extern void LNprof(double LeafN, double LAI, int nlayers, double kpLN);

extern struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH,double WindSpeed,double LeafAreaIndex, double CanopyHeight, double StomataWS, int ws,double vmax2, double alpha2, double kparm, double theta, double beta, double Rd2, double b02, double b12);
