// This file contains the definition of the class Vector3D

#include <math.h>
#include "Constants.h"
#include "Vector3D.h"
#include "Normal.h"
#include "Point3D.h"

// ---------------------------------------------------------- default constructor

Vector3D::Vector3D(void)
	 : x(0.0), y(0.0), z(0.0)							
{}

// ---------------------------------------------------------- constructor

Vector3D::Vector3D(bool isRandom)

{
	while(true){
			x = rand()*invRAND_MAX*2-1;  // 0-1        //random points in the cube
			y = rand()*invRAND_MAX*2-1;  // 0-1
			z = rand()*invRAND_MAX*2-1;  // 0-1
			if (z>=0 || x*x+y*y+z*z>1)                         //points should be inside of the sphere
				continue;                                      //points should be in the Hemisphere "same" side with normal
			normalize();                                //normalize
			break;
		}
}

Vector3D::Vector3D(double a)
	 : x(a), y(a), z(a)							
{}

// ---------------------------------------------------------- constructor

Vector3D::Vector3D(double _x, double _y, double _z)	 
	: x(_x), y(_y), z(_z)
{}

// ---------------------------------------------------------- copy constructor

Vector3D::Vector3D(const Vector3D& vector)
	: x(vector.x), y(vector.y), z(vector.z)
{}


// ---------------------------------------------------------- constructor
// constructs a vector from a normal

Vector3D::Vector3D(const Normal& n)	 
	: x(n.x), y(n.y), z(n.z)
{}

// ---------------------------------------------------------- constructor
// constructs a vector from a point
// this is used in the ConcaveHemisphere hit functions

Vector3D::Vector3D(const Point3D& p)	 
	: x(p.x), y(p.y), z(p.z)
{}


// ---------------------------------------------------------- destructor

Vector3D::~Vector3D (void) 							
{}



// ---------------------------------------------------------- assignment operator

Vector3D& 
Vector3D::operator= (const Vector3D& rhs) {
	if (this == &rhs)
		return (*this);

	x = rhs.x; y = rhs.y; z = rhs.z;

	return (*this);
}


// ----------------------------------------------------------- assignment operator
// assign a Normal to a vector

Vector3D& 
Vector3D::operator= (const Normal& rhs) {
	x = rhs.x; y = rhs.y; z = rhs.z;
	return (*this);
}


// ---------------------------------------------------------- assignment operator 
// assign a point to a vector

Vector3D& 												
Vector3D::operator= (const Point3D& rhs) {
	x = rhs.x; y = rhs.y; z = rhs.z;
	return (*this);
}


// ----------------------------------------------------------  length
// length of the vector

double													
Vector3D::length(void) {
	return (sqrt(x * x + y * y + z * z));
}


// ----------------------------------------------------------  normalize
// converts the vector to a unit vector

void 													
Vector3D::normalize(void) {	
	double length = sqrt(x * x + y * y + z * z);
	x /= length; y /= length; z /= length;
}


// ----------------------------------------------------------  hat
// converts the vector to a unit vector and returns the vector

Vector3D& 													
Vector3D::hat(void) {	
	double length = sqrt(x * x + y * y + z * z);
	x /= length; y /= length; z /= length;
	return (*this);
} 

/*
void
Vector3D::compute_theta_phi(void){
	theta = acos(z);
	phi = 0;
	if(x!=0){
		    phi = atan(y/x);
		if (x<0){
			phi = phi + PI;
		}else{
			if(y>0){
				phi = phi + TWO_PI;
			}
		}
	}else{
		if(y>0){
			phi = PI/2;
		}else{
			phi = 3/2*PI;
		}
	}

}
*/

// non-member function

// ----------------------------------------------------------  operator* 
// multiplication by a matrix on the left
