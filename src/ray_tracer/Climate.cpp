/*
 * Climate.cpp
 *
 *  Created on: 2014-3-19
 *      Author: Qingfeng
 */

#include "Climate.h"
#include "math.h"
#include "Constants.h"
#include <iostream>

Climate::Climate() {
	// TODO Auto-generated constructor stub

}
Climate::~Climate() {
	// TODO Auto-generated destructor stub
}
void
Climate::climate_calculation (double Latitude, double solarTimeNoon,double atmosphericTransmittance, int day,
double start_hour, double end_hour, double hour_interval, int silence){
	int num = (int)((end_hour - start_hour)/hour_interval);
	for (int i = 0; i<=num; i++){
		double hour = start_hour + i*hour_interval;

		double derta = -23.45*cos(2*PI*(day+10)/365)/180*PI;
		double Phi = Latitude/180*PI; //Latitude
		double tsn = solarTimeNoon; //12.15; //time of noon
		double h = 15*(hour-tsn)/180*PI; //time angle...
		double thetas = asin((cos(h)*cos(derta)*cos(Phi) + sin(derta) * sin(Phi)));
		if (silence == 0){
			cout << "thetas: " << thetas << endl;
			cout << "derta: " << derta << endl;
			cout << "Phi: " << Phi << endl;
			cout << "h: " << h << endl;
			//solar

		//	cout << "1: " << (sin(derta)*cos(Phi) - cos(h)*cos(derta)*sin(Phi)) / cos(thetas) << endl;
		}
		double temp = (float)(sin(derta)*cos(Phi)-cos(h)*cos(derta)*sin(Phi))/cos(thetas);

		//cout<<"temp: "<<temp<<endl;
		double phys;
		if(temp>=1){
			phys = 0;
		}else{
			if (temp < -1){ temp = -1; }
			phys = PI - acos(temp); //
		}
		if (silence == 0){
			cout << "phys: " << phys << endl;
		}
		if (h>0){
		    phys = -phys;
		}

		double elevationAngle = thetas;
		double azimuthAngle = phys;
		if (elevationAngle >= 0){
			Vector3D* v = new Vector3D;
			v->x = cos(elevationAngle)*cos(azimuthAngle);
			v->y = cos(elevationAngle)*sin(azimuthAngle);
			v->z = -sin(elevationAngle);
			direct_light_d_list.push_back(v);

			double ppfd1 = kSOLAR_constant*pow(atmosphericTransmittance, 1 / sin(elevationAngle))*sin(elevationAngle);
			double ppfd2 = 0.5*kSOLAR_constant*(1 - pow(atmosphericTransmittance, 1 / sin(elevationAngle)))*sin(elevationAngle);
			if (silence == 0){
				cout << ppfd1 << endl;
				cout << ppfd2 << endl;
			}
			ppfd_direct_list[i] = ppfd1; //umol.m-2.s-1
			ppfd_diffuse_list[i] = ppfd2; //umol.m-2.s-1
		}
		else{
			Vector3D* v = new Vector3D;
			v->x = 0;
			v->y = 0;
			v->z = 0;
			direct_light_d_list.push_back(v);

			double ppfd1 = 0; 
			double ppfd2 = 0; 
			if (silence == 0){
				cout << ppfd1 << endl;
				cout << ppfd2 << endl;
			}
			ppfd_direct_list[i] = ppfd1; //umol.m-2.s-1
			ppfd_diffuse_list[i] = ppfd2; //umol.m-2.s-1
		}
		
	}
}

