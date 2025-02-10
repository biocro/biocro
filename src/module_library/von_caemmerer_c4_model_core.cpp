#include "von_caemmerer_c4_model_core.h"




// struct linpoly {
//     double c0, c1;

//     double operator()(double x) const {
//         return c0 + c1 * x;
//     }

//     linpoly& operator+=(const double rhs){
//         c0 += rhs;
//         return *this;
//     }

//     linpoly& operator+=(const linpoly rhs){
//         c0 += rhs.c0;
//         c1 += rhs.c1;
//         return *this;
//     }

//     linpoly& operator-=(const double rhs){
//         c0 -= rhs;
//         return *this;
//     }

//     linpoly& operator-=(const linpoly rhs){
//         c0 -= rhs.c0;
//         c1 -= rhs.c1;
//         return *this;
//     }

//     linpoly& operator*=(const double rhs){
//         c0 *= rhs;
//         c1 *= rhs;
//         return *this;
//     }

//     linpoly& operator/=(const double rhs){
//         c0 /= rhs;
//         c1 /= rhs;
//         return *this;
//     }

// };

// inline linpoly operator+(const linpoly& lhs, const double rhs){
//     linpoly result = lhs;
//     result += rhs;
//     return result;
// }

// inline linpoly operator+(const double lhs, const linpoly& rhs){
//     return rhs + lhs;
// }

// inline linpoly operator+(const linpoly& lhs, const linpoly& rhs){
//     linpoly result = lhs;
//     result += rhs;
//     return result;
// }

// inline linpoly operator-(const linpoly& lhs, const linpoly& rhs){
//     linpoly result = lhs;
//     result -= rhs;
//     return result;
// }

// inline linpoly operator-(const linpoly& lhs, const double rhs){
//     linpoly result = lhs;
//     result -= rhs;
//     return result;
// }

// inline linpoly operator-(const double lhs, const linpoly& rhs){
//     linpoly result = rhs;
//     result *= -1;
//     result += lhs;
//     return result;
// }

// inline linpoly operator*(const linpoly& lhs, const double rhs){
//     linpoly result = lhs;
//     result *= rhs;
//     return result;
// }

// inline linpoly operator*(const double lhs, const linpoly& rhs){return rhs * lhs; }

// inline linpoly operator/(const linpoly& lhs, const double rhs){
//     linpoly result = lhs;
//     result /= rhs;
//     return result;
// }


// double eq1(double A, double Vc, double Vo, double Rd){
//     double rhs = Vc - 0.5 * Vo - Rd;
//     return rhs - A;
// }

// double eq2(double Rd, double Rm, double Rs){
//     double rhs = Rm + Rs;
//     return rhs - Rd;
// }

double eq3(double A, double Vp, double L, double Rm){
    double rhs = Vp - L - Rm;
    return rhs - A;
}

// double eq4(double L, double gbs, double Cs, double Cm){
//     double rhs = gbs * (Cs - Cm);
//     return rhs - L;
// }

double leak_rate(double gbs, double Cs, double Cm){
    return gbs * (Cs - Cm);
}


// double get_Vc(double Cs, double Os, double Vcmax, K_type<double> K){
//     double denom = Cs + K.c * (1 + Os / K.o);
//     return (Cs / denom) *  Vcmax;
// }

// double eq8(double Vc, double Vo, double GammaStar, double Cs){
//     double rhs = 2 * GammaStar / Cs;
//     double lhs = Vc / Vo;
//     return rhs - lhs;
// }



double get_reduced_Vc(double Cs, double Os, double Vcmax, double Kc, double Ko, double gamma_star){
    double num = Vcmax * (Cs - gamma_star * Os);
    double denom = Cs + Kc * (1 + Os / Ko);
    return num / denom;
}

double get_Cs(double A, double Cm, double Vp, double Rm, double gbs){
    return Cm + (Vp - A - Rm) / gbs;
}

double check_Cs(double A, double Cm, double Vp, double Rm, double gbs){
    double Cs = get_Cs(A, Cm, Vp, Rm, gbs);
    double L = leak_rate(gbs , Cs, Cm);
    return eq3(A, Vp, L, Rm);
}

double get_go(double gbs, double ao){
    return gbs * ao;
}

double get_Os(double A, double Om, double alpha, double go){
    return alpha * (A / go) + Om;
}

double get_Vp(double Cm, double Vpmax, double Kp, double Vpr){
    double vp = Vpmax * (Cm / (Cm + Kp));
    return std::min(vp, Vpr);
}

// double eq10(double Ac, double reduced_Vc, double Rd){
//     return (reduced_Vc - Rd) - Ac;
// }

// double eq37(double Aj, double Vjmax, double Cs, double Os, double gamma_star, double Rd){
//     double num = Vjmax * (Cs - gamma_star * Os);
//     double denom = 3 * Cs + 7 * gamma_star * Os;
//     double rhs = num/ denom  - Rd;
//     return rhs - Aj ;
// }

double get_z(double f_cyc){
    constexpr double protons_per_atp = 4;
    return ((3 - f_cyc)/(1 - f_cyc) ) / protons_per_atp;
}

double get_rho(double f_cyc){
    return (1 - f_cyc)/(2 - f_cyc);
}


double solve_quadratic_balance(linpoly P, linpoly Q, double Rd){
    // A = P / Q - Rd
    // P(A) - Q(A) * (A + Rd) = 0 (quadratic in A)
    double f0 = P.c0 - Q.c0  * Rd ;
    double f1 = P.c1  -  Q.c0 - Rd * Q.c1;
    double f2 = - Q.c1;
    return quadratic_root_minus(f2, f1, f0);
}


double get_J(double I2, double Jmax, double theta){
    return quadratic_root_minus(theta,  -(I2 + Jmax), I2 * Jmax);
}


template<typename T, typename U>
T reduced_vc_num(T Cs, T Os, U gamma_star){
    return  Cs - gamma_star * Os;
}

template<typename T, typename U>
T enzyme_limited_reduced_vc_denom(T Cs, T Os, U Kc, U Ko){
    return  Cs + Kc * (1 +  Os / Ko);
}

template<typename T, typename U>
T light_limited_reduced_vc_denom(T Cs, T Os, U gamma_star){
    return 3 *  Cs + 7 * gamma_star * Os;
}


double solve_Ac(linpoly Cs, linpoly Os, double gamma_star,
    double Kc, double Ko, double Rd, double Vcmax)
{

    linpoly P = Vcmax *  reduced_vc_num(Cs,  Os,  gamma_star);
    linpoly Q = enzyme_limited_reduced_vc_denom(Cs, Os, Kc, Ko);

    return solve_quadratic_balance(P, Q, Rd);
}

double solve_Aj(linpoly Cs, linpoly Os, double Vjmax, double gamma_star, double Rd)
{

    linpoly P = Vjmax * reduced_vc_num(Cs,  Os,  gamma_star);
    linpoly Q = light_limited_reduced_vc_denom(Cs, Os, gamma_star);

    return solve_quadratic_balance(P, Q, Rd);
}



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
){
    vc_c4_result out;
        double I2 = Qin * get_rho(f_cyc) * leaf_absorptance * spectral_correction;
        double J = get_J(I2, Jmax, theta);

        // enzyme limited rate
        double Vp_e = get_Vp(Cm, Vpmax, Kp, Vpr);

        linpoly A{0, 1};
        linpoly Cs_e = Cm + (Vp_e - A -  Rm) / gbs;
        linpoly Os = Om + (alpha_psii / get_go(gbs, ao)) * A;
        out.Ac = solve_Ac(Cs_e, Os, gamma_star, Kc, Ko, Rd, Vcmax);
        // linpoly P_e = Vcmax * (Cs_e - gamma_star * Os);
        // linpoly Q_e = Cs_e + Kc * (1 + Os / Ko);

        // out.Ac = solve_quadratic_balance(P_e, Q_e, Rd);


        double z = get_z(f_cyc);
        double J_atp =  J * z ;
        double Vp_j = 0.5 * J_atp * x_etr;
        double Vjmax = (1  - x_etr) * J_atp;
        linpoly Cs_j = Cm + (Vp_j - A - Rm) / gbs;
        // linpoly P_j = Vjmax * (Cs_j - gamma_star * Os);
        // linpoly Q_j = 3 * Cs_j + 7 * gamma_star * Os;

        out.Aj = solve_Aj(Cs_j, Os, Vjmax, gamma_star, Rd);
        out.An = std::min(out.Ac, out.Aj);
        return out;
}

class electron_transport {

    double theta;
    double Jmax;

    public:

    double operator()(double I2) const {
        return quadratic_root_minus(theta,  -(I2 + Jmax), I2 * Jmax);
    }

};


class vc_c4_biochemical {


    double alpha_psii;
    double ao;
    double Cm;
    double f_cyc;
    double gamma_star;
    double gbs;

    double Kc;
    double Ko;
    double Kp;
    double Om;

    double Qabs;
    electron_transport J;

    double Rd;
    double Rm;
    double Vcmax;
    double Vpmax;
    double Vpr;
    double x_etr;



    public:

    vc_c4_result result;

    void update_assim(){

        // enzyme limited rate
        double Vp_e = get_Vp(Cm, Vpmax, Kp, Vpr);

        linpoly A{0, 1};
        linpoly Cs_e = Cm + (Vp_e - A - Rm) / gbs;
        linpoly Os = Om + (alpha_psii / get_go(gbs, ao)) * A;
        result.Ac = solve_Ac(Cs_e, Os, gamma_star, Kc, Ko, Rd, Vcmax);

        double z = get_z(f_cyc);
        double J_atp = J(Qabs) * z;
        double Vp_j = 0.5 * J_atp * x_etr;
        double Vjmax = (1  - x_etr) * J_atp;
        linpoly Cs_j = Cm + (Vp_j - A - Rm) / gbs;

        result.Aj = solve_Aj(Cs_j, Os, Vjmax, gamma_star, Rd);
        result.An = std::min(result.Ac, result.Aj);
    }

    std::array<double, 2> solution_check(){
        std::array<double, 2> out;


        double Vp = get_Vp(Cm, Vpmax, Kp, Vpr);
        double Cs = Cm + (Vp - Ac - Rm)/gbs;
        double go = get_go(gbs, ao);
        double Os = Om + alpha_psii * Ac / go;


        double vc_num = reduced_vc_num(Cs, Os, gamma_star);
        double vc_denom = enzyme_limited_reduced_vc_denom(Cs, Os, Kc, Ko);
        double reduced_Vc = vc_num / vc_denom;
        out[0] = reduced_Vc - Rd - Ac; // (equation 10)

        double z = get_z(f_cyc);
        double J_atp = J(Qabs) * z;
        double Vp_j = 0.5 * J_atp * x_etr;
        double Vjmax = (1 - x_etr) * J_atp;
        Cs = Cm + (Vp_j - Aj - Rm) / gbs;
        Os = Om + alpha_psii * Aj  / go;
        vc_num = reduced_vc_num(Cs, Os, gamma_star);
        vc_denom = light_limited_reduced_vc_denom(Cs, Os, gamma_star);
        reduced_Vc = Vjmax * vc_num / vc_denom;
        out[1] = reduced_Vc - Rd - Aj; // (equation 37)
        return out;
    }
};
