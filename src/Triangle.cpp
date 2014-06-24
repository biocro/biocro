/*
 * Triangle.cpp
 *
 *  Created on: 2012-11-15
 *      Author: Administrator
 */

#include "Triangle.h"
#include "Maths.h"
#include "iostream"

Triangle::Triangle(double start_hour, double end_hour, double hour_interval)
	// TODO Auto-generated constructor stub
{
	int num = (int)((end_hour - start_hour)/hour_interval);
	for (int i=0; i<=num; i++){
		photonFlux_up_dir.push_back(0.0);
		photonFlux_down_dir.push_back(0.0);
		photonFlux_up_dff.push_back(0.0);
		photonFlux_down_dff.push_back(0.0);
		photonFlux_up_scat.push_back(0.0);
		photonFlux_down_scat.push_back(0.0);
	}
}

Triangle::Triangle(const Point3D& a, const Point3D& b, const Point3D& c, const double leafID, const double leafL, const double position,
		const double chlSPAD, const double kt, const double kr, const double nitrogenPerArea, double start_hour, double end_hour, double hour_interval)
{
	v0 = a;
	v1 = b;
	v2 = c;
	normal = (v1-v0) ^ (v2-v0);  //right hand law, that the a, b, c, normal is in a right hand system.
	normal.normalize();
	 /// --------------- ----------------- Qingfeng calculate the triangle area
//	area = 1;
	int num = (int)((end_hour - start_hour)/hour_interval);
//	cout<<"num: "<<num<<endl;
	for (int i=0; i<=num; i++){
		photonFlux_up_dir.push_back(0.0);
		photonFlux_down_dir.push_back(0.0);
		photonFlux_up_dff.push_back(0.0);
		photonFlux_down_dff.push_back(0.0);
		photonFlux_up_scat.push_back(0.0);
		photonFlux_down_scat.push_back(0.0);
	}
	//photonFlux = 0;  // ------------------------- Qingfeng set the PPFD default is 0
	nitrogenPerA = nitrogenPerArea;
	leID = leafID;
	leL = leafL;
	pos = position;
	chlSPA = chlSPAD;
	kLeafTransmittance = kt;
	kLeafReflectance =kr;

}


Triangle::~Triangle() {
	// TODO Auto-generated destructor stub
}

void method1(){

}
// ---------------------------------------------------------------- compute_normal

void
Triangle::compute_normal(void) {
	normal = (v1 - v0) ^ (v2 - v0);
	normal.normalize();
}

bool
Triangle::hit(const Ray& ray, double& tmin){

	double a = v0.x - v1.x, b=v0.x - v2.x, c = ray.d.x, d = v0.x - ray.o.x;
	double e = v0.y - v1.y, f=v0.y - v2.y, g = ray.d.y, h = v0.y - ray.o.y;
	double i = v0.z - v1.z, j=v0.z - v2.z, k = ray.d.z, l = v0.z - ray.o.z;

	double m = f*k - g*j, n = h*k - g*l, p = f*l - h*j;
	double q = g*i - e*k, s = e*j - f*i;

	double inv_denom = 1.0/(a*m + b*q + c*s);

	double e1 = d * m - b * n - c * p;
	double beta = e1*inv_denom;

	if(beta<0.0)
		return (false);

	double r = e*l - h*i;
	double e2 = a*n + d*q + c*r;
	double gamma = e2*inv_denom;

	if(gamma<0.0)
		return(false);

	if(beta+gamma>1.0)
		return(false);

	double e3 = a*p - b*r + d*s;
	double t = e3*inv_denom;

	if(t<kEpsilon)
		return(false);

	tmin = t;
//	cout<<"in Triangle : add ray --- > "<<ray.photonFlux2<<endl;

//	cout<<"in Triangle : triangle PPF --- > "<<photonFlux<<endl;
//	sr.normal = normal;
 	hit_point = ray.o + t*ray.d;  //update hit_point

//

	return(true);

}
BBox
Triangle::get_bounding_box(void){
	double delta = 0.000001; 
	
	return (BBox(min(min(v0.x, v1.x), v2.x) - delta, max(max(v0.x, v1.x), v2.x) + delta, 
				 min(min(v0.y, v1.y), v2.y) - delta, max(max(v0.y, v1.y), v2.y) + delta, 
				 min(min(v0.z, v1.z), v2.z) - delta, max(max(v0.z, v1.z), v2.z) + delta));
	
}














