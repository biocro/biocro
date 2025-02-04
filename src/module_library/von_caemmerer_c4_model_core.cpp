#include "von_caemmerer_c4_model_core.h"

namespace vc_c4_core {


struct A_t {double val;};
struct R_t {double val;};
struct V_t {double val;};
struct K_t {double val;};

struct K_type {
    double c, o, p;
};

struct R_type {
    double m, d;
};

struct Vmax_type {
    double cmax, pmax, pr;
};

struct enyzme_params{
    K_type K;
    R_type resp;
    Vmax_type V;
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

double get_Vc(double Cs, double Os, double Vcmax, K_type K){
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

double get_reduced_Vc(double Cs, double Os, double Vcmax, K_type K, double gamma_star){
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

double get_Ac(double Cm, double Om, double gbs,
    double alpha, double ao, double gamma_star,
    K_type K, R_type R, Vmax_type V)
{
    double Vp = get_Vp(Cm, V.pmax, K.p, V.pr);
    // Cs = c0 + c1 * A;
    linpoly Cs{Cm - (Vp - R.m) / gbs, - 1/gbs};
    // Os = o0 + o1 * A
    linpoly Os{Om, alpha / get_go(gbs, ao)};

    // Ac = P(A)/Q(A) - Rd  (eq 10)
    // P = P0 + P1 * A = Vcmax * (Cs - gamma_star * Os)
    linpoly P = V.cmax * (Cs - gamma_star * Os);

    // Q = Q0 + Q1 * A = Cs + Kc * (1 + Os / Ko)
    linpoly Q = Cs + K.c * (1 + Os / K.o);


    // solve P(A)/Q(A) - Rd - A = 0 for A;
    // P - Rd * Q - A * Q is quadratic in A;
    // P - Rd * Q - A * Q = f0 + f1 * A + f2 * A^2
    // one solution is extraneous;
    double f0 = P.c0 - Q.c0  * (R.d + 1);
    double f1 = P.c1 - R.d * Q.c1 - Q.c0;
    double f2 = 0.5 * Q.c1;

    // the solution closer to zero is the correct solution
    return quadratic_root_min(f2, f1, f0);

}


double get_Aj(double){
    return 0.;
}

} // end vc_c4_core
