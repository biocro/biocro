#include "Ray.h"

// ---------------------------------------------------------------- default constructor

Ray::Ray (void)
	: 	o(0.0), 
		d(0.0, 0.0, 1.0),
		photonFlux2(0)
{}

// ---------------------------------------------------------------- constructor

Ray::Ray (const Point3D& origin, const Vector3D& dir, const double pf)
	: 	o(origin), d(dir), photonFlux2(pf)
{}//photonFlux = photonFlux1;

// ---------------------------------------------------------------- copy constructor

Ray::Ray (const Ray& ray)
	: 	o(ray.o), 
		d(ray.d),
		photonFlux2(ray.photonFlux2)

{}

Ray::Ray(const Ray* ray_ptr)
	:   o(ray_ptr->o),
	    d(ray_ptr->d),
	    photonFlux2(ray_ptr->photonFlux2)
{}

// ---------------------------------------------------------------- assignment operator

Ray& 
Ray::operator= (const Ray& rhs) {
	
	if (this == &rhs)
		return (*this);
		
	o = rhs.o; 
	d = rhs.d; 

	return (*this);	
}

// ---------------------------------------------------------------- destructor

Ray::~Ray (void) {

}



