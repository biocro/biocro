#ifndef EC4PHOTO_H
#define EC4PHOTO_H

double eC4photoC(double QP, double TEMP, double RH, double CA,
        double OA, double VCMAX, double VPMAX, double VPR,
        double JMAX);

double RsqeC4photo(double oAssim[], double oQp[],
        double oTemp[],  double oRH[],
        double COa, double O2a,  double Vcmax,
        double Vpmax, double Vpr, double Jmax, int enObs);

#endif

