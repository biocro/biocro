#ifndef __RAY__
#define __RAY__

#include "Point3D.h"
#include "Vector3D.h"

class Ray {
	public:
	
		Point3D			o;  	// origin 
		Vector3D		d; 		// direction 
		double          photonFlux2; // photon flux, which equals to the photon flux density * area of one ray presents

		Ray(void);			
		
		Ray(const Point3D& origin, const Vector3D& dir, const double pf);
		
		Ray(const Ray& ray);

		Ray(const Ray* ray_ptr);
		
		Ray& 						
		operator= (const Ray& rhs);
		 								
		~Ray(void);
};

#endif
