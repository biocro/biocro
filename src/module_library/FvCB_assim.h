#ifndef FVCB_ASSIM_H
#define FVCB_ASSIM_H

struct FvCB_str {
    double An;  // micromol / m^2 / s
    double Ac;  // micromol / m^2 / s
    double Aj;  // micromol / m^2 / s
    double Ap;  // micromol / m^2 / s
    double Vc;  // micromol / m^2 / s
    double Wc;  // micromol / m^2 / s
    double Wj;  // micromol / m^2 / s
    double Wp;  // micromol / m^2 / s
};

FvCB_str FvCB_assim(
    double Ci,
    double Gstar,
    double J,
    double Kc,
    double Ko,
    double Oi,
    double Rd,
    double TPU,
    double Vcmax,
    double alpha_TPU,
    double electrons_per_carboxylation,
    double electrons_per_oxygenation);

#endif
