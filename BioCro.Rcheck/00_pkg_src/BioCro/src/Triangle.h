#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Ray.h"
#include "Point3D.h"
#include "Normal.h"
#include "Constants.h"
#include "BBox.h"
#include <vector>


class Triangle{
public:

	Point3D v0, v1, v2;
	Point3D hit_point;
	Normal normal;
//	double area;
	vector<double> photonFlux_up_dir;   // light from up side
	vector<double> photonFlux_down_dir; // light from down side
	vector<double> photonFlux_up_dff;   // light from up side
	vector<double> photonFlux_down_dff; // light from down side
	vector<double> photonFlux_up_scat;   // light from up side
	vector<double> photonFlux_down_scat; // light from down side

	double leID, leL, pos, chlSPA;
	double kLeafReflectance;
	double kLeafTransmittance;
	double nitrogenPerA;
	double id_row;
	double id_col;

	// --------------- PPFD1 2 3 4 ... 15 Qingfeng


	Triangle(double start_hour, double end_hour, double hour_interval);
	Triangle(const Point3D& a, const Point3D& b, const Point3D& c, const double leafID, const double leafL, const double position, const double chlSPAD, const double kt, const double kr, const double nitrogenPerArea,
		double start_hour, double end_hour, double hour_interval, int plantColID, int plantRowID);
	virtual ~Triangle();

	bool
	hit(const Ray& ray, double& tmin);
	
	BBox
	get_bounding_box(void);

	void
	compute_normal(void);

};

#endif /* TRIANGLE_H_ */
