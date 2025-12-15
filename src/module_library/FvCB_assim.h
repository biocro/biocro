#ifndef FVCB_ASSIM_H
#define FVCB_ASSIM_H

/**
 * @brief A simple structure for holding the output of FvCB calculations.
 */
struct FvCB_outputs {
    double An;  //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double Ac;  //!< Rubisco-determined net CO2 assimilation rate (micromol / m^2 / s)
    double Aj;  //!< RuBP-regeneration-determined net CO2 assimilation rate (micromol / m^2 / s)
    double Ap;  //!< TPU-determined net CO2 assimilation rate (micromol / m^2 / s)
    double Vc;  //!< RuBP carboxylation rate (micromol / m^2 / s)
    double Wc;  //!< Rubisco-limited RuBP carboxylation rate (micromol / m^2 / s)
    double Wj;  //!< RuBP-regeneration-limited RuBP carboxylation rate (micromol / m^2 / s)
    double Wp;  //!< TPU-limited RuBP carboxylation rate (micromol / m^2 / s)
};

FvCB_outputs FvCB_assim(
    double Ci,
    double Gstar,
    double J,
    double Kc,
    double Ko,
    double Oi,
    double RL,
    double TPU,
    double Vcmax,
    double alpha_TPU,
    double electrons_per_carboxylation,
    double electrons_per_oxygenation);

#endif
