#ifndef GROWTH_RESP_H
#define GROWTH_RESP_H

double growth_resp(
    double const base_rate,
    double const grc);

double growth_resp_Q10(
    double const base_rate,
    double const grc0,
    double const Tleaf,
    double const Tref);

double maintenance_resp_Q10(
    double const tissue_mass,
    double const mrc0,
    double const Tleaf,
    double const Tref);

#endif
