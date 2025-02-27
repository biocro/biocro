#ifndef LINPOLY
#define LINPOLY

template<typename T>
struct linpoly {
    T c0, c1;

    T operator()(T x) const {
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

template<typename T>
inline linpoly<T> operator+(const linpoly<T>& lhs, const T rhs){
    linpoly<T> result = lhs;
    result += rhs;
    return result;
}

template<typename T>
inline linpoly<T> operator+(const T lhs, const linpoly<T>& rhs){
    return rhs + lhs;
}

template<typename T>
inline linpoly<T> operator+(const linpoly<T>& lhs, const linpoly<T>& rhs){
    linpoly<T> result = lhs;
    result += rhs;
    return result;
}

template<typename T>
inline linpoly<T> operator-(const linpoly<T>& lhs, const linpoly<T>& rhs){
    linpoly<T> result = lhs;
    result -= rhs;
    return result;
}

template<typename T>
inline linpoly<T> operator-(const linpoly<T>& lhs, const T rhs){
    linpoly<T> result = lhs;
    result -= rhs;
    return result;
}

template<typename T>
inline linpoly<T> operator-(const T lhs, const linpoly<T>& rhs){
    linpoly<T> result = rhs;
    result *= -1;
    result += lhs;
    return result;
}

template<typename T>
inline linpoly<T> operator*(const linpoly<T>& lhs, const T rhs){
    linpoly<T> result = lhs;
    result *= rhs;
    return result;
}

template<typename T>
inline linpoly<T> operator*(const T lhs, const linpoly<T>& rhs){return rhs * lhs; }

template<typename T>
inline linpoly<T> operator/(const linpoly<T>& lhs, const T rhs){
    linpoly<T> result = lhs;
    result /= rhs;
    return result;
}

#endif
