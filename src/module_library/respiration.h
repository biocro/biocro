#ifndef RESPIRATION_H
#define RESPIRATION_H

struct respiration_outputs {
    double net_rate;          //!< New rate after subtracting respiratory losses
    double respiration_rate;  //!< Rate of respiratory losses
};

respiration_outputs resp(double comp, double grc, double temp);

#endif
