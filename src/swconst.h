/*  swconst.h */

#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define HOURS_PER_DAY 24
#define NDAY 366               /* dimension for daily arrays */
#define NMONTH 12              /* # of months in a year */
#define MAXLYR 21              /* Max # of soil layers (soil water model) */
#define CENTMAXLYR 10          /* Max # of century soil layers */
#define MAXSTLYR 200           /* Max # of soil temperature layers */
#define PARTDENS 2.65          /* Particle Density (g/cm3) */
#define PI 3.14159265
#define CONVLAI 80    /* biomass needed to produce an LAI of 1 (g/m**2) */

#define FNSOIL "soils.in"
#define FNSITE "sitepar.in"

#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))
