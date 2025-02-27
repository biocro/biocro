#ifndef VON_CAEMMERER_C4_MODEL_CORE
#define VON_CAEMMERER_C4_MODEL_CORE

#include <algorithm> // std::min
#include <array>
#include <numeric>
#include "../framework/quadratic_root.h" //
#include "linpoly.h"


struct vc_c4_result { double Ac, Aj, An;};
vc_c4_result vc_c4_biochemical_fun(
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double Jmax,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Qin,
    double Rd,
    double Rm,
    double theta,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr,
    double leaf_absorptance,
    double spectral_correction
    );


// data types


struct leaf_environment {
    double temp;
    double co2;
    double light;
    double rh;
    double wind_speed;
};

struct C4_pep_parameters {
    double Vpmax;
    double Vpr;
    double Rm;
    double gbs;
    double Kp;
};

struct C4_carboxylation_parameters {
    double Vcmax;
    double Kc;
    double Ko;
    double gamma_star;
};

struct electron_transport {


    double Jmax;
    double theta;
    double alpha;

    electron_transport(double Jmax, double theta,
        double f_cyc, double leaf_absorptance, double spectral_correction) :
        Jmax{Jmax}, theta{theta},
        alpha{get_rho(f_cyc) * leaf_absorptance * spectral_correction}
     {
     }

    double operator()(double incident_ppfd) const {
        double I2 = incident_ppfd * alpha;
        return quadratic_root_minus(theta,  -(I2 + Jmax), I2 * Jmax);
    }

};


class vc_c4_biochemical {
    double alpha_psii;
    double ao;

    double f_cyc;
    double gamma_star;
    double gbs;

    double Kc;
    double Ko;
    double Kp;

    electron_transport J;

    double Rd;
    double Rm;
    double Vcmax;
    double Vpmax;
    double Vpr;
    double x_etr;
    double z;

    public:
    vc_c4_biochemical() {}

    vc_c4_result get_assim(double Qi, double Ci, double temp) const;
    double assim(double Qi, double Ci, double temp) const;

};


class electron_transport {

};

template<typename response_type = constant_response>
class temp_parameter {
    private:
        double ref_value_;
        double ref_temp_;
        response_type response_;

    public:
        template<typename... Args>
        temp_parameter(
            double ref_value, double ref_temp, Args... args) :
        ref_value_{ref_value}, ref_temp_{ref_temp}, response_type(args) {}

        double operator()(double temp){
           return response_(ref_value_, temp, ref_temp_);
        }
};

class temperature_response {
    public:
        virtual ~temperature_response() = default;
        virtual double operator()(double ref_value, double temp, double ref_temp) const = 0;
};

class constant_response : public temperature_response {
    public:
    constant_response() {}
    double operator()(double ref_value, double temp, double ref_temp) const override {
        return ref_value;
    }
};




#endif
