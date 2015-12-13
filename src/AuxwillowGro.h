#ifndef AUXWILLOWGRO_H
#define AUXWILLOWGRO_H
/*
 *  /src/AuxwillowGro.h by Dandan Wang and Deepak Jaiswal  Copyright (C) 2010-2015
 *
 */

SEXP willowGro(
        SEXP LAT,              /* Latitude                            1 */
        SEXP DOY,              /* Day of the year                     2 */
        SEXP HR,               /* Hour of the day                     3 */
        SEXP SOLAR,            /* Solar Radiation                     4 */
        SEXP TEMP,             /* Temperature                         5 */
        SEXP RH,               /* Relative humidity                   6 */
        SEXP WINDSPEED,        /* Wind Speed                          7 */
        SEXP PRECIP,           /* Precipitation                       8 */
        SEXP KD,               /* K D (ext coeff diff)                9 */
        SEXP CHIL,             /* Chi, leaf angle distribution       10 */


        SEXP HEIGHTF,          /* Height factor                      11 */
        SEXP NLAYERS,          /* Number of layers int he canopy     12 */
        SEXP IPLANT,           /* Ini PLANT                          13 */
        SEXP IRTL,             /* i rhiz to leaf                     14 */
        SEXP SENCOEFS,         /* sene coefs                         15 */
        SEXP TIMESTEP,         /* time step                          16 */
        SEXP VECSIZE,          /* vector size                        17 */
        SEXP SPLEAF,           /* Spec Leaf Area                     18 */
        SEXP SPD,              /* Spec Lefa Area Dec                 19 */
        SEXP DBPCOEFS,         /* Dry Bio Coefs                      20 */
        SEXP THERMALP,         /* Themal Periods                     21 */
        SEXP TBASE,            /* Base temp  thermal time            22 */
        SEXP VMAX,             /* Vmax of photo                      23 */
        SEXP ALPHA,            /* Quantum yield                      24 */
        SEXP THETA,            /* theta param (photo)                26 */
        SEXP BETA,             /* beta param  (photo)                27 */
        SEXP RD,               /* Dark Resp   (photo)                28 */
        SEXP CATM,             /* CO2 atmosph                        29 */
        SEXP B0,               /* Int (Ball-Berry)                   30 */
        SEXP B1,               /* Slope (Ball-Berry)                 31 */
        SEXP SOILCOEFS,        /* Soil Coefficients                  32 */
        SEXP ILEAFN,           /* Ini Leaf Nitrogen                  33 */
        SEXP KLN,              /* Decline in Leaf Nitr               34 */
        SEXP VMAXB1,           /* Effect of N on Vmax                35 */
        SEXP ALPHAB1,          /* Effect of N on alpha               36 */
        SEXP MRESP,            /* Maintenance resp                   37 */
        SEXP SOILTYPE,         /* Soil type                          38 */
        SEXP WSFUN,            /* Water Stress Func                  39 */
        SEXP WS,               /* Water stress flag                  31 */
        SEXP CENTCOEFS,        /* Century coefficients               40 */
        SEXP CENTTIMESTEP,     /* Century timestep                   41 */
        SEXP CENTKS,           /* Century decomp rates               42 */
        SEXP SOILLAYERS,       /* # soil layers                      43 */
        SEXP SOILDEPTHS,       /* Soil Depths                        44 */
        SEXP CWS,              /* Current water status               45 */
        SEXP HYDRDIST,         /* Hydraulic dist flag                46 */
        SEXP SECS,             /* Soil empirical coefs               47 */
        SEXP KPLN,             /* Leaf N decay                       48 */
        SEXP LNB0,             /* Leaf N Int                         49 */
        SEXP LNB1,             /* Leaf N slope                       50 */
        SEXP LNFUN,            /* Leaf N func flag                   51 */
        SEXP UPPERTEMP,        /* Temperature Limitations photoParms 52 */
        SEXP LOWERTEMP,        /*                                    53 */
        SEXP JMAX,             /*                                    54 */
        SEXP JMAXB1,           /*                                    55 */
        SEXP O2,               /*                                    55 */
        SEXP GROWTHRESP,       /*                                    57 */
        SEXP STOMATAWS);       /*                                    58 */

struct ET_Str c3EvapoTrans(double Rad, 
        double Itot, 
        double Airtemperature, 
        double RH,
        double WindSpeed,
        double LeafAreaIndex, 
        double CanopyHeight, 
        double vcmax2, 
        double jmax2, 
        double Rd2, 
        double b02, 
        double b12,
        double Catm2,
        double O2,
        double theta2);

#endif

