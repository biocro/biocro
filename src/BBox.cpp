/*
 * BBox.cpp
 *
 *  Created on: 2012-11-30
 *      Author: Administrator
 */

#include "BBox.h"


BBox::BBox() {
	// TODO Auto-generated constructor stub
}

BBox::BBox (	const double _x0, const double _x1,			
				const double _y0, const double _y1, 
				const double _z0, const double _z1)
	: x0(_x0), x1(_x1), y0(_y0), y1(_y1), z0(_z0), z1(_z1)
{}

BBox::~BBox() {
	// TODO Auto-generated destructor stub
}

bool
BBox::hit(const Ray& ray) const{
	double kEpsilong = 0;
	double x0 = 0, y0 = 0, z0 = 0;
	double x1 = 0, y1 = 0, z1 = 0; //BBox

	double ox = ray.o.x; double oy = ray.o.y; double oz = ray.o.z;
	double dx = ray.d.x; double dy = ray.d.y; double dz = ray.d.z; //ray

	double tx_min, ty_min, tz_min;
	double tx_max, ty_max, tz_max;   // the min and max values at x-, y- and z-coordinates

	double a = 1.0/dx;
	if(a>=0){
		tx_min = (x0-ox) * a;
		tx_max = (x1-ox) * a;
	}else{
		tx_min = (x1-ox) * a;
		tx_max = (x0-ox) * a;
	}

	double b = 1.0/dy;
	if(b>=0){
		ty_min = (y0-oy) * b;
		ty_max = (y1-oy) * b;
	}else{
		ty_min = (y1-oy) * b;
		ty_max = (y0-oy) * b;
	}

	double c = 1.0/dz;
	if(c>=0){
		tz_min = (z0-oz)*c;
		tz_max = (z1-oz) *c;
	}else{
		tz_min = (z1-oz)*c;
		tz_max = (z0-oz)*c;
	}

	double t0,t1;

	//find largest entering t value

	if(tx_min>ty_min)
		t0 = tx_min;
	else
		t0 = ty_min;

	if(tz_min>t0)
		t0 = tz_min;

	//find smallest exiting t value
	if(tx_max<ty_max)
		t1 = tx_max;
	else
		t1 = ty_max;

	if(tz_max < t1)
		t1 = tz_max;

	return (t0<t1 && t1>kEpsilong);  // determine whether the ray hit the BBox by comparing the entering and exiting t

}
bool
BBox::inside(const Point3D& p) const {
	return ((p.x > x0 && p.x < x1) && (p.y > y0 && p.y < y1) && (p.z > z0 && p.z < z1));
}

