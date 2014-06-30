/*
 * LeafOptics.h
 *
 *  Created on: 2012-11-23
 *      Author: Administrator
 */

#ifndef LEAFOPTICS_H_
#define LEAFOPTICS_H_
#include "Vector3D.h"
#include "Ray.h"
#include "Triangle.h"

class LeafOptics {
public:
	LeafOptics();
	virtual ~LeafOptics();

	Vector3D
	get_reflect_dir(Vector3D L, Vector3D N);
	void
		get_reflect_dir_2(Ray ray, Triangle* triangle_ptr, vector<Ray*> &scatter_rays, double ignor_Photon_Flux_threashold);
	Vector3D
	get_transmit_dir(Vector3D L, Vector3D N);

private:

	void
	randReflectRayDir (Vector3D N, Vector3D & r);
	void
	randThroughRayDir (Vector3D N, Vector3D & r);

	double
	getfr (double hv_wave_length, Vector3D V, Vector3D L, Vector3D N);
	Vector3D
	vMidLine (Vector3D A, Vector3D B);
	double
	vAngle (Vector3D A, Vector3D B);



};

#endif /* LEAFOPTICS_H_ */
