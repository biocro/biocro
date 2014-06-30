#ifndef GRID_H_
#define GRID_H_
#include "Point3D.h"
#include "Compound.h"
#include "Ray.h"
#include "Triangle.h"
#include "BBox.h"
#include <vector>
#include "LeafOptics.h"

// major function for ray tracing

class Grid{
public:
	
	double   ignor_Photon_Flux_threashold;  //umol.s-1  (default is 5e-7 when smallest distance between rays is 1e-3)

	Grid(double ignor_thres);
	virtual ~Grid();      

	void
	setup_cells(Point3D p0, Point3D p1);

	void
	add_triangle(Triangle* triangle);

	vector<Compound*>
	get_cells();

	vector<Triangle*>
	get_triangles();

	bool
		hit(Ray & ray, double& tmin, const int& hour_th, int& firstStep)const; // this hour_th is hour-0.5
	
	

private:
	vector<Compound*> cells;
	vector<Triangle*> triangles;
	
	BBox bbox;
	int nx, ny, nz;
	bool
	hit_scatter_rays(vector<Ray*> statter_rays, const int& hour_th)const;
	bool
	generate_scatter_rays(Ray& ray, Triangle* triangle_ptr, const int& hour_th)const;
	bool
		generate_scatter_rays_2(Ray& ray, Triangle* triangle_ptr, const int& hour_th)const;
	Point3D
	min_coordinates(void);
	Point3D
	max_coordinates(void);
	LeafOptics* leaf_optics;
	

};

#endif /* GRID_H_ */


