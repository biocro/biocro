#ifndef C3_TEMPERATURE_RESPONSE_H
#define C3_TEMPERATURE_RESPONSE_H

struct c3_temperature_response_parameters {
    double Gstar_c;     //!< Arrhenius scaling parameter for Gstar in micromol / mol
    double Gstar_Ea;    //!< Arrhenius activation energy for Gstar (J / mol)
    double Jmax_c;      //!< Arrhenius scaling parameter for Jmax (normalized to its value at 25 degrees C)
    double Jmax_Ea;     //!< Arrhenius activation energy for Jmax (J / mol)
    double Kc_c;        //!< Arrhenius scaling parameter for Kc in micromol / mol
    double Kc_Ea;       //!< Arrhenius activation energy for Kc (J / mol)
    double Ko_c;        //!< Arrhenius scaling parameter for Ko in mmol / mol
    double Ko_Ea;       //!< Arrhenius activation energy for Ko (J / mol)
    double phi_PSII_0;  //!< Order 0 coefficient of polynomial response for theta (dimensionless)
    double phi_PSII_1;  //!< Order 1 coefficient of polynomial response for theta ((degrees C)^(-1))
    double phi_PSII_2;  //!< Order 2 coefficient of polynomial response for theta ((degrees C)^(-2))
    double Rd_c;        //!< Arrhenius scaling parameter for Rd (normalized to its value at 25 degrees C)
    double Rd_Ea;       //!< Arrhenius activation energy for Rd (J / mol)
    double theta_0;     //!< Order 0 coefficient of polynomial response for theta (dimensionless)
    double theta_1;     //!< Order 1 coefficient of polynomial response for theta ((degrees C)^(-1))
    double theta_2;     //!< Order 2 coefficient of polynomial response for theta ((degrees C)^(-2))
    double Tp_c;        //!< Johnson, Eyring, & Williams scaling parameter for Tp (normalized to its value at 25 degrees C)
    double Tp_Ha;       //!< Johnson, Eyring, & Williams enthalpy of activation for Tp
    double Tp_Hd;       //!< Johnson, Eyring, & Williams enthalpy of deactivation for Tp
    double Tp_S;        //!< Johnson, Eyring, & Williams entropy parameter for Tp
    double Vcmax_c;     //!< Arrhenius scaling parameter for Vcmax (normalized to its value at 25 degrees C)
    double Vcmax_Ea;    //!< Arrhenius activation energy for Vcmax (J / mol)
};

struct c3_param_at_tleaf {
    double Gstar;       //!< CO2 compensation point in the absence of Rd (micromol / mol)
    double Jmax_norm;   //!< Jmax normalized to its value at 25 degrees C (dimensionless)
    double Kc;          //!< Michaelis-Menten constant for Rubisco carboxylation (micromol / mol)
    double Ko;          //!< Michaelis-Menten constant for Rubisco oxygenation (mmol / mol)
    double phi_PSII;    //!< Dark-adapted operating efficiency of photosystem II (dimensionless)
    double Rd_norm;     //!< Rd normalized to its value at 25 degrees C (dimensionless)
    double theta;       //!< Linear light response factor (dimensionless)
    double Tp_norm;     //!< Tp normalized to its value at 25 degrees C (dimensionless)
    double Vcmax_norm;  //!< Vcmax normalized to its value at 25 degrees C (dimensionless)
};

double johnson_eyring_williams_response(
    double c,           // dimensionless
    double Ha,          // J / mol
    double Hd,          // J / mol
    double S,           // J / K / mol
    double temperature  // K
);

double polynomial_response(
    double c0,          // output units
    double c1,          // output units * (degrees C)^(-1)
    double c2,          // output units * (degrees C)^(-2)
    double temperature  // degrees C
);

c3_param_at_tleaf c3_temperature_response(
    c3_temperature_response_parameters param,
    double Tleaf  // degrees C
);

#endif
