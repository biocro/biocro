extern double SoilEvapo(double LAI, double k, double AirTemp, double IRad, double awc, double fieldc, double wiltp, double winds, double RelH, double rsec );

extern struct seqRD_str seqRootDepth(double to, int lengthOut);

extern struct rd_str rootDist(int layer, double rootDepth, double *depthsp, double rfl);
