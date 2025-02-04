#include "von_caemmerer_c4_model_core.h"


template<typename T>
struct K_type {
    T c, o, p;
};

template<typename T>
struct R_type {
    T d, m;
};

template<typename T>
struct Vmax_type {
    T cmax, pmax, pr;
};

template<typename T>
class vc_c4_model {

    public:
    T alpha_psii;
    T ao;
    T Cm;
    T f_cyc;
    T gbs;
    T Om;
    T x_etr;
    K_type<T> K;
    R_type<T> R;
    Vmax_type<T> V;

    vc_c4_model(
        double alpha_psii,
        double ao,
        double Cm,
        double f_cyc,
        double gamma_star,
        double gbs,
        double J,
        double Kc,
        double Ko,
        double Kp,
        double Om,
        double Rd,
        double Rm,
        double Vcmax,
        double Vpmax,
        double Vpr,
        double x_etr
    ) :

    K{Kc, Ko, Kp},
    R{Rd, Rm},
    V{Vcmax, Vpmax, Vpr} {}



};

// template<typename T, int deg>
// class polynomial {
//     using container = std::array<T, deg + 1>;
//     container coef;

//     public:
//     polynomial() : coef{} {}
//     polynomial(container c) : coef{c} {}
//     polynomial(T c) : coef{c} {}

//     // using iterator = typename container::iterator;
//     // using const_iterator = typename container::const_iterator;

//     auto begin() { return coef.begin();}
//     auto end() { return coef.begin();}

//     const_iterator cbegin() const { return coef.cbegin();}
//     const_iterator cend() const { return coef.cend();}

//     iterator rbegin() { return coef.rbegin();}
//     iterator rend() { return coef.rend();}

//     const_iterator crbegin() const { return coef.crbegin();}
//     const_iterator crend() const { return coef.crend();}

//     T operator()(T x){
//         // Horner's method
//         return std::reduce(
//             coef.crbegin(), coef.crend(), T(0),
//             [x](T left, T right){return left * x + right;}
//         );
//     }

//     polynomial& operator+=(const polynomial rhs){
//         std::transform(coef.cbegin(), coef.cend(), rhs.cbegin(), coef.begin(), std::plus<>{});
//         return *this;
//     }

// };

// template<typename T>
// class linpoly : public polynomial<T, 1> {
//     std::array<T, 2> coef;

//     public:
//     linpoly() : polynomial<T, 1>{} {}
//     linpoly(T c) : polynomial<T, 1>{{c, c}} {}
//     linpoly(T c0, T c1) : polynomial<T, 1>{{c0, c1}} {}


// };


struct linpoly {
    double c0, c1;

    double operator()(double x){
        return c0 + c1 * x;
    }

    linpoly& operator+=(const double rhs){
        c0 += rhs;
        return *this;
    }

    linpoly& operator+=(const linpoly rhs){
        c0 += rhs.c0;
        c1 += rhs.c1;
        return *this;
    }

    linpoly& operator-=(const double rhs){
        c0 -= rhs;
        return *this;
    }

    linpoly& operator-=(const linpoly rhs){
        c0 -= rhs.c0;
        c1 -= rhs.c1;
        return *this;
    }

    linpoly& operator*=(const double rhs){
        c0 *= rhs;
        c1 *= rhs;
        return *this;
    }

    linpoly& operator/=(const double rhs){
        c0 /= rhs;
        c1 /= rhs;
        return *this;
    }

};

inline linpoly operator+(const linpoly& lhs, const double rhs){
    linpoly result = lhs;
    result += rhs;
    return result;
}

inline linpoly operator+(const double lhs, const linpoly& rhs){
    return rhs + lhs;
}

inline linpoly operator+(const linpoly& lhs, const linpoly& rhs){
    linpoly result = lhs;
    result += rhs;
    return result;
}

inline linpoly operator-(const linpoly& lhs, const linpoly& rhs){
    linpoly result = lhs;
    result -= rhs;
    return result;
}

inline linpoly operator-(const linpoly& lhs, const double rhs){
    linpoly result = lhs;
    result -= rhs;
    return result;
}

inline linpoly operator-(const double lhs, const linpoly& rhs){
    linpoly result = rhs;
    result *= -1;
    result += lhs;
    return result;
}

inline linpoly operator*(const linpoly& lhs, const double rhs){
    linpoly result = lhs;
    result *= rhs;
    return result;
}

inline linpoly operator*(const double lhs, const linpoly& rhs){return rhs * lhs; }

inline linpoly operator/(const linpoly& lhs, const double rhs){
    linpoly result = lhs;
    result /= rhs;
    return result;
}


double eq1(double A, double Vc, double Vo, double Rd){
    double rhs = Vc - 0.5 * Vo - Rd;
    return rhs - A;
}

double eq2(double Rd, double Rm, double Rs){
    double rhs = Rm + Rs;
    return rhs - Rd;
}

double eq3(double A, double Vp, double L, double Rm){
    double rhs = Vp - L - Rm;
    return rhs - A;
}

double eq4(double L, double gbs, double Cs, double Cm){
    double rhs = gbs * (Cs - Cm);
    return rhs - L;
}

double leak_rate(double gbs, double Cs, double Cm){
    return gbs * (Cs - Cm);
}

double eq7(double Vc, double Cs, double Os,
    double Vcmax, double Kc, double Ko)
{
    double denom = Cs + Kc * (1 + Os / Ko);
    double rhs = (Cs / denom) *  Vcmax;
    return rhs - Vc;
}

double get_Vc(double Cs, double Os, double Vcmax, K_type<double> K){
    double denom = Cs + K.c * (1 + Os / K.o);
    return (Cs / denom) *  Vcmax;
}

double eq8(double Vc, double Vo, double GammaStar, double Cs){
    double rhs = 2 * GammaStar / Cs;
    double lhs = Vc / Vo;
    return rhs - lhs;
}

double eq9(double gamma_star, double Vcmax, double Kc, double Vomax, double Ko){
    double rhs = 0.5 * (Vomax / Vcmax) * (Kc / Ko);
    return rhs - gamma_star;
}

template<typename T>
double get_reduced_Vc(double Cs, double Os, double Vcmax, K_type<T> K, double gamma_star){
    double num = Vcmax * (Cs - gamma_star * Os);
    double denom = Cs + K.c * (1 + Os / K.o);
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

double get_ao(double DO2, double SO2, double DCO2, double SCO2){
    return (DO2 / DCO2) * (SO2 / DCO2);
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

double eq10(double Ac, double reduced_Vc, double Rd){
    return (reduced_Vc - Rd) - Ac;
}

double eq37(double Aj, double Vjmax, double Cs, double Os, double gamma_star, double Rd){
    double num = Vjmax * (Cs - gamma_star * Os);
    double denom = 3 * Cs + 7 * gamma_star * Os;
    double rhs = num/ denom  - Rd;
    return rhs - Aj ;
}

double get_z(double f_cyc){
    constexpr double protons_per_atp = 4;
    return ((3 - f_cyc)/(1 - f_cyc) ) / protons_per_atp;
}

double solve_quadratic_balance(linpoly P, linpoly Q,  double Rd){
    // A = P / Q - Rd
    // P(A) - Q(A) * (A + Rd) = 0 (quadratic in A)
    double f0 = P.c0 - Q.c0  * Rd ;
    double f1 = P.c1  -  Q.c0 - Rd * Q.c1;
    double f2 = - Q.c1;
    return quadratic_root_minus(f2, f1, f0);
}






double get_Ac(double Cm, double Om, double gbs,
    double alpha_psii, double ao, double gamma_star,
    K_type<double> K, R_type<double> R, Vmax_type<double> V)
{
    double Vp = get_Vp(Cm, V.pmax, K.p, V.pr);
    // Cs = c0 + c1 * A;
    linpoly Cs{Cm + (Vp - R.m) / gbs, - 1/gbs};
    // Os = o0 + o1 * A
    linpoly Os{Om, alpha_psii / get_go(gbs, ao)};

    // Ac = P(A)/Q(A) - Rd  (eq 10)
    // P = P0 + P1 * A = Vcmax * (Cs - gamma_star * Os)
    linpoly P = V.cmax * (Cs - gamma_star * Os);

    // Q = Q0 + Q1 * A = Cs + Kc * (1 + Os / Ko)
    linpoly Q = Cs + K.c * (1 + Os / K.o);

    // the solution closer to zero is the correct solution
    return solve_quadratic_balance(P, Q, R.d);

}


double get_Aj(double J, double Cm, double Om, double gbs, double f_cyc, double alpha_psii, double ao, double gamma_star, R_type<double> R){
    double x_etr = 0.4; // partitioning factor
    double protons_per_atp = 4;
    double z = ((3 - f_cyc)/(1 - f_cyc) ) / protons_per_atp;
    double zJ = z * J;
    // Cs = c0 + c1 * A;
    linpoly Cs{Cm + (0.5 * zJ * x_etr   - R.m) / gbs, - 1/gbs};
    // Os = o0 + o1 * A
    linpoly Os{Om, alpha_psii / get_go(gbs, ao)};

    // Ac = P(A)/Q(A) - Rd  (eq 10)
    // P = P0 + P1 * A = Vcmax * (Cs - gamma_star * Os)
    linpoly P = zJ * (1 - x_etr) * (Cs - gamma_star * Os);

    // Q = Q0 + Q1 * A = Cs + Kc * (1 + Os / Ko)
    linpoly Q = 3 * Cs + 7 * gamma_star * Os;

    // solve P(A)/Q(A) - Rd - A = 0 for A;
    return solve_quadratic_balance(P, Q, R.d);
}

double check_Ac(
    double Ac,
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double J,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Rd,
    double Rm,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr
){
    K_type<double> K{Kc, Ko, Kp};
    double Vp = get_Vp(Cm, Vpmax, Kp, Vpr);
    double Cs = Cm + (Vp - Ac - Rm)/gbs;
    double Os = Om + alpha_psii * Ac / (ao * gbs);
    double reduced_Vc = get_reduced_Vc<double>(Cs, Os, Vcmax, K, gamma_star);
    return eq10(Ac, reduced_Vc, Rd);
}

double check_Aj(
    double Aj,
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double J,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Rd,
    double Rm,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr
){
    K_type<double> K{Kc, Ko, Kp};
    double z = get_z(f_cyc);
    double Cs = Cm + (0.5 *z*x_etr*J - Aj - Rm)/gbs;
    double Os = Om + alpha_psii * Aj / (ao * gbs);

    return eq37(Aj, z * (1 - x_etr) * J, Cs, Os, gamma_star, Rd);
}

vc_c4_result vc_c4_module_core(
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double J,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Rd,
    double Rm,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr
    ){

    vc_c4_result out;

    // enzyme limited rate
    double Vp_e = get_Vp(Cm, Vpmax, Kp, Vpr);

    linpoly A{0, 1};
    linpoly Cs_e = Cm + (Vp_e - A -  Rm) / gbs;
    linpoly Os = Om + (alpha_psii / get_go(gbs, ao)) * A;
    linpoly P_e = Vcmax * (Cs_e - gamma_star * Os);
    linpoly Q_e = Cs_e + Kc * (1 + Os / Ko);

    out.Ac = solve_quadratic_balance(P_e, Q_e, Rd);


    double z = get_z(f_cyc);
    double J_atp =  J * z ;
    double Vp_j = 0.5 * J_atp * x_etr;
    double Vjmax = (1  - x_etr) * J_atp;
    linpoly Cs_j = Cm + (Vp_j -  A - Rm) / gbs;
    linpoly P_j = Vjmax * (Cs_j - gamma_star * Os);
    linpoly Q_j = 3 * Cs_j + 7 * gamma_star * Os;

    out.Aj = solve_quadratic_balance(P_j, Q_j, Rd);
    out.An = std::min(out.Ac, out.Aj);
    return out;
}



