#ifndef C3PHOTO_H
#define C3PHOTO_H

struct c3_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
};

struct c3_str c3photoC(
    double Qp,
    double Tleaf,
    double RH,
    double Vcmax0,
    double Jmax0,
    double TPU_rate_max,
    double Rd0,
    double bb0,
    double bb1,
    double Gs_min,
    double Ca,
    double AP,
    double O2,
    double thet,
    double StomWS,
    int water_stress_approach,
    double electrons_per_carboxylation,
    double electrons_per_oxygenation);

double solc(double LeafT);
double solo(double LeafT);

#endif
