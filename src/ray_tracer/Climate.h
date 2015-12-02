/*
 * Climate.h
 *
 *  Created on: 2012-11-25
 *      Author: Administrator
 */

#ifndef CLIMATE_H_
#define CLIMATE_H_
#include "Vector3D.h"
#include <vector>
using namespace std;

class Climate {
public:
    //for a whole day from 0.5h to 23.5h
	vector<Vector3D*> direct_light_d_list;
	double ppfd_direct_list[368], ppfd_diffuse_list[368];

	Climate();
	virtual ~Climate();
	void
		climate_calculation(double Latitude, double solarTimeNoon, double atmosphericTransmittance, int day, double start_hour, double end_hour, double hour_interval, int silence);
};

#endif /* CLIMATE_H_ */
