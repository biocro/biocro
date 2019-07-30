#ifndef C3PHOTO_H
#define C3PHOTO_H
/*
 *  /src/c4photo.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 */

struct c3_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;

};

struct c3_str c3photoC(double Qp, double Tleaf, double RH, double Vcmax0, double Jmax0, 
        double Rd0, double bb0, double bb1, double Ca, double O2, double theta, double StomWS,int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation);
        
struct c3_str c3photoCdb(double Qp, double Tleaf, double RH, double Vcmax0, double Jmax0, 
        double Rd0, double bb0, double bb1, double Ca, double O2, double theta, double StomWS,int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation);

double solc(double LeafT);
double solo(double LeafT);

#endif

