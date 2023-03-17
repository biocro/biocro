#ifndef C3PHOTO_H
#define C3PHOTO_H

struct c3_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
    double Assim_conductance;
    int iterations;
};

struct c3_str c3photoC(
    double const absorbed_ppfd,
    double const Tleaf,
    double const RH,
    double const Vcmax0,
    double const Jmax0,
    double const TPU_rate_max,
    double const Rd0,
    double const bb0,
    double const bb1,
    double const Gs_min,
    double Ca,
    double const AP,
    double const O2,
    double const thet,
    double const StomWS,
    int const water_stress_approach,
    double const electrons_per_carboxylation,
    double const electrons_per_oxygenation,
    double const beta_PSII,
    double const gbw);

double solc(double LeafT);
double solo(double LeafT);

#endif
