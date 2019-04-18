/*
 *  BioCro/src/CanA.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *
 */

#ifndef CANA_H
#define CANA_H

struct ET_Str EvapoTrans2(double Rad, double Iave, double Airtemperature, double RH,
        double WindSpeed,double LeafAreaIndex, double CanopyHeight, 
        double stomatacond, double leafw, int eteq);

#endif

