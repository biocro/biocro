#include <math.h>
#include "convergence_iteration.hpp"
#include "collatz_photo.hpp"
#include "ball_berry.h"

state_map collatz_leaf::do_operation (state_map const& s) const
{
    //check_state s = check_state(ss);
    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double const incident_irradiance = s.at("incident_irradiance");
    double const incident_par = s.at("incident_par");
    double const air_temperature = s.at("temp");
    double const vmax = s.at("vmax");
    double const alpha = s.at("alpha");
    double const kparm = s.at("kparm");
    double const theta = s.at("theta");
    double const beta = s.at("beta");
    double const Rd = s.at("Rd");
    double const upperT = s.at("upperT");
    double const lowerT = s.at("lowerT");
    double const k_Q10 = s.at("k_Q10");
    double const Catm = s.at("Catm");
    double const rh = s.at("rh");
    double const b0 = s.at("b0");
    double const b1 = s.at("b1");
    double const layer_wind_speed = s.at("layer_wind_speed");
    double const leafwidth = s.at("leafwidth");
    double const water_vapor_pressure = s.at("water_vapor_pressure");
    double const stefan_boltzman = s.at("stefan_boltzman");

    // Initial guesses
    double intercelluar_co2_molar_fraction = Catm * 0.4;  // micromole / mol
    double leaf_temperature = air_temperature;
    struct collatz_result r = collatz_photo(incident_par, leaf_temperature, vmax,
            alpha, kparm, theta, beta, Rd,
            upperT, lowerT, k_Q10, intercelluar_co2_molar_fraction);

    double gs = ball_berry(r.assimilation * 1e-6, Catm * 1e-6, rh, b0, b1);  // mmol / m^2 / s
    double conductance_in_m_per_s = gs * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double boundary_layer_conductance = leaf_boundary_layer_conductance(layer_wind_speed,
                                leafwidth, air_temperature, leaf_temperature - air_temperature, conductance_in_m_per_s, water_vapor_pressure);  // m / s

    double const absorbed_irradiance = incident_irradiance * (1 - s.at("leaf_reflectance") - s.at("leaf_transmittance")) / (1 - s.at("leaf_transmittance"));  // W / m^2. Leaf area basis.

    state_map leaf_state(s);
    double& leaf_net_irradiance = leaf_state["leaf_net_irradiance"];

    double const water_stress_approach = s.at("water_stress_approach");
    double const StomataWS = s.at("StomataWS");

    // Convergence loop
    {
        double black_body_radiation = 0;  // W / m^2. Leaf area basis.
        unsigned int constexpr max_iterations = 50;
        for (unsigned int n = 0; n < max_iterations; ++n) {
            leaf_net_irradiance = absorbed_irradiance - black_body_radiation;  // W / m^2. Leaf area basis.

            leaf_state["leaf_assimiliation_rate"] = r.assimilation;
            leaf_state["leaf_boundary_layer_conductance"] = boundary_layer_conductance;
            leaf_state["leaf_stomatal_conductance"] = gs;
            leaf_state["leaf_temperature"] = leaf_temperature;
            //Rprintf("Loop %i\n", n);
            //output_map(leaf_state);

            double const previous_assimilation = r.assimilation;  // micromole / m^2 / s

            leaf_temperature = this->leaf_temperature_module->run(leaf_state).at("leaf_temperature");

            double constexpr ratio = 1.6;  // The ratio of the diffusivities of H2O and CO2 in air.
            intercelluar_co2_molar_fraction = Catm - r.assimilation / gs * 1e3 * ratio;  // micromole / mol
            intercelluar_co2_molar_fraction = fmax(0, intercelluar_co2_molar_fraction);

            r = collatz_photo(incident_par, leaf_temperature, vmax,
                    alpha, kparm, theta, beta, Rd,
                    upperT, lowerT, k_Q10, intercelluar_co2_molar_fraction);


            if (water_stress_approach == 0) r.assimilation *= StomataWS;

            gs = ball_berry(r.assimilation * 1e-6, Catm * 1e-6, rh, b0, b1);  // mmol / m^2 / s

            if (water_stress_approach == 1) gs *= StomataWS;

            if (n > max_iterations - 10)
                gs = s.at("b0") * 1e3;  // mmol / m^2 / s. If it has gone through this many iterations, the convergence is not stable. This convergence is inapproriate for high water stress conditions, so use the minimum gs to try to get a stable system.

            //Rprintf("n %i; incident_par %f; ci %f; r.assimilation %f; gs %f; air_temperature %f; leaf_temperature %f\n", n, incident_par, intercelluar_co2_molar_fraction, r.assimilation, gs, air_temperature, leaf_temperature);

            conductance_in_m_per_s = gs * 1e-3 * volume_of_one_mole_of_air;  // m / s

            boundary_layer_conductance = leaf_boundary_layer_conductance(layer_wind_speed, leafwidth, air_temperature, leaf_temperature - air_temperature, conductance_in_m_per_s, water_vapor_pressure);  // m / s

            double delta_t = leaf_temperature - air_temperature;
            black_body_radiation = 4 * stefan_boltzman * pow(273.15 + air_temperature, 3) * delta_t;  // W / m^2. Leaf area basis.

            /* black_body_radiation = net long wave radiation emittted per second = radiation emitted per second - radiation absorbed per second = sigma * (Tair + deltaT)^4 - sigma * Tair^4
             * To make it a linear function of deltaT, do a Taylor series about deltaT = 0 and keep only the zero and first order terms.
             * black_body_radiation = sigma * (Tair - 0)^4 - sigma * Tair^4 + deltaT * (4 * sigma * Tair^3) = 4 * sigma * Tair^3 * deltaT
             * where 4 * sigma * Tair^3 is the derivative of sigma * (Tair + deltaT)^4 evaluated at deltaT = 0,
             */


            double constexpr tol = 0.01;  // micromole / m^2 / s
            double const diff = fabs(previous_assimilation - r.assimilation);
            if (diff <= tol) break;
        };
        //if (n > 49)
            //Rprintf("n %i; StomataWS %f; incident_par %f; ci %f; r.assimilation %f; gs %f; air_temperature %f; leaf_temperature %f\n", n, StomataWS, incident_par, intercelluar_co2_molar_fraction, r.assimilation, gs, air_temperature, leaf_temperature);
    }

    return {
        { "leaf_assimilation_rate", r.assimilation },
        { "leaf_stomatal_conductance", gs },
        { "leaf_boundary_layer_conductance", boundary_layer_conductance },
        { "leaf_temperature", leaf_temperature },
        { "ci", intercelluar_co2_molar_fraction },
        { "leaf_net_irradiance", leaf_net_irradiance },
    };
}

