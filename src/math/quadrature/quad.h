#ifndef QUADRATURE_H
#define QUADRATURE_H

namespace quadrature {



struct NewtonCotes {


    template<typename T = double, typename Integrand>
    T integrate(Integrand&& integrand, double a, double b) {
        T acc = integrand(a);
        int num = 10;
        double dx = (b - a) / num;
        double x;
        for (int n = 0; n < num; ++n) {
            x = n * dx;
            acc += integrand(x) * dx;
        }

        return acc;

    }
};


}

#endif
