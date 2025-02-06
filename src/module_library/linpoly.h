#ifndef LINPOLY
#define LINPOLY


struct linpoly {
    double c0, c1;

    double operator()(double x) const {
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

#endif
