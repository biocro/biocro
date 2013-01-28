#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"

/* Light Me function for Light Microenvironment */
void lightME(double lat, int DOY, int td)
{

	extern double tmp1[];
	double *ip1;
	ip1 = &tmp1[0];
	double omega, delta0, delta, deltaR;
	double tf, SSin, CCos, PPo;
	double CosZenithAngle, CosHour;
	double CosHourDeg;
	double Idir, Idiff, propIdir, propIdiff;
	const double DTR = M_PI/180;
	const double tsn = 12.0;
	const double alpha = 0.85;
	const double SolarConstant = 2650;
	const double atmP = 1e5;

	omega = lat * DTR;
	delta0 = 360.0 * ((DOY + 10)/365.0);
	delta = -23.5 * cos(delta0*DTR);
	deltaR = delta * DTR;

	tf = (15.0*(td - tsn))*DTR;
	SSin = sin(deltaR) * sin(omega);
	CCos = cos(deltaR) * cos(omega);

	CosZenithAngle = SSin + CCos * cos(tf);
	if(CosZenithAngle < pow(10,-10))
		CosZenithAngle = pow(10,-10);

	CosHour = -tan(omega) * tan(deltaR);
	CosHourDeg = (1/DTR)*CosHour;
	if(CosHourDeg < -57)
		CosHour = -0.994;

	PPo = 1e5 / atmP;
	Idir = SolarConstant * (pow(alpha,(PPo/CosZenithAngle))) * CosZenithAngle;
	Idiff = 0.5 * SolarConstant *(1 - pow(alpha,(PPo/CosZenithAngle))) * CosZenithAngle ;

	propIdir = Idir / (Idir + Idiff);
	propIdiff = Idiff / (Idir + Idiff);

	*ip1 = propIdir;
	*(ip1+1) = propIdiff;
	*(ip1+2) = CosZenithAngle;
}
