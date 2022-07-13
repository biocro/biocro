#ifndef CANA_H
#define CANA_H

struct ET_Str EvapoTrans2(double Rad, double Iave, double Airtemperature, double RH,
        double WindSpeed,double LeafAreaIndex, double CanopyHeight,
        double stomatacond, double leafw, double specific_heat_of_air, int eteq);

#endif

