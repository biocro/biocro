
#include "Grid.h"
#include "Maths.h"
#include "Constants.h"
#include "BBox.h"
#include <iostream>
#include <vector>
#include <math.h>
#include "LeafOptics.h"

using namespace std;

Grid::Grid(double ignor_thres) {
	ignor_Photon_Flux_threashold = ignor_thres;
	// TODO Auto-generated constructor stub
}

Grid::~Grid() {
	// TODO Auto-generated destructor stub
}

void
Grid::setup_cells(Point3D p0, Point3D p1){
	//find the minimum and maximum coordinates of the grid
	//store them in the bounding box
	bbox.x0 = p0.x-kEpsilon; bbox.y0 = p0.y-kEpsilon; bbox.z0 = p0.z-kEpsilon;
	bbox.x1 = p1.x+kEpsilon; bbox.y1 = p1.y+kEpsilon; bbox.z1 = p1.z+kEpsilon;

	//compute the number of cells in the x-, y-, and z-directions

	int num_triangles = triangles.size();
	float wx = p1.x - p0.x;
	float wy = p1.y - p0.y;
	float wz = p1.z - p0.z;
	float multiplier = 2.0;             //about 8 times (2*2*2) more cells than objects
	float temp333 = 0.3333333;
	float s = pow(wx * wy * wz / num_triangles, temp333);
	nx = (int) (multiplier * wx/s + 1);
	ny = (int) (multiplier * wy/s + 1);
	nz = (int) (multiplier * wz/s + 1);         //number of cells in directions of x-, y-, z-coordinates

	//set up the array of cells with null pointers

	int num_cells = nx*ny *nz;
	cells.reserve(num_triangles);

	for(int j=0; j<num_cells; j++)
		cells.push_back(NULL);

	// set up the temporary array to hold the number of objects stored in each cell

	vector<int> counts;
	counts.reserve(num_cells);

	for(int j=0; j<num_cells; j++)
		counts.push_back(0);

	//put objects into the cells

	BBox obj_bbox;                      // object's bounding box
	int index;                          // cell array index

	for(int j=0; j<num_triangles; j++){
		obj_bbox = triangles[j]-> get_bounding_box();

		//if triangle is out of the bbox of grid, do not setup into grid

		if(obj_bbox.x0<bbox.x0 || obj_bbox.y0<bbox.y0 || obj_bbox.z0<bbox.z0 || obj_bbox.x1>bbox.x1 || obj_bbox.y1>bbox.y1 || obj_bbox.z1>bbox.z1)
			continue;

		//compute the cell indices for the corners of the bounding box of the object

		int ixmin = clamp((obj_bbox.x0 - p0.x) * nx /(p1.x - p0.x), 0, nx-1);
		int iymin = clamp((obj_bbox.y0 - p0.y) * ny /(p1.y - p0.y), 0, ny-1);
		int izmin = clamp((obj_bbox.z0 - p0.z) * nz /(p1.z - p0.z), 0, nz-1);
		int ixmax = clamp((obj_bbox.x1 - p0.x) * nx /(p1.x - p0.x), 0, nx-1);
		int iymax = clamp((obj_bbox.y1 - p0.y) * ny /(p1.y - p0.y), 0, ny-1);
		int izmax = clamp((obj_bbox.z1 - p0.z) * nz /(p1.z - p0.z), 0, nz-1);

		//add the triangle to the cells

		for(int iz = izmin; iz<=izmax; iz++)
			for(int iy = iymin; iy<=iymax; iy++)
				for(int ix = ixmin; ix<=ixmax; ix++){

					index = ix + nx * iy + nx*ny*iz;

					if(counts[index] == 0){
						Compound* compound_ptr = new Compound;  //construct a compound object when the cell is empty
						cells[index] = compound_ptr; 
					}
					cells[index] -> add_triangle(triangles[j]); //add triangle into a cell
					counts[index] += 1;					
				}
		//
	}
//	triangles.erase(triangles.begin(), triangles.end());
//	counts.erase(counts.begin(), counts.end());

}

// ---------------------------------------------------------------- hit

// The following grid traversal code is based on the pseudo-code in Shirley (2000)
// The first part is the same as the code in BBox::hit

bool
Grid::hit(Ray& ray, double& t, const int& hour_th, int& lightType) const {

	double ox = ray.o.x;
	double oy = ray.o.y;
	double oz = ray.o.z;
	double dx = ray.d.x;
	double dy = ray.d.y;
	double dz = ray.d.z;

	double x0 = bbox.x0;
	double y0 = bbox.y0;
	double z0 = bbox.z0;
	double x1 = bbox.x1;
	double y1 = bbox.y1;
	double z1 = bbox.z1;

	double tx_min, ty_min, tz_min;
	double tx_max, ty_max, tz_max;

	// the following code includes modifications from Shirley and Morley (2003)

	double a = 1.0 / dx;
	if (a >= 0) {
		tx_min = (x0 - ox) * a;
		tx_max = (x1 - ox) * a;
	}
	else {
		tx_min = (x1 - ox) * a;
		tx_max = (x0 - ox) * a;
	}

	double b = 1.0 / dy;
	if (b >= 0) {
		ty_min = (y0 - oy) * b;
		ty_max = (y1 - oy) * b;
	}
	else {
		ty_min = (y1 - oy) * b;
		ty_max = (y0 - oy) * b;
	}

	double c = 1.0 / dz;
	if (c >= 0) {
		tz_min = (z0 - oz) * c;
		tz_max = (z1 - oz) * c;
	}
	else {
		tz_min = (z1 - oz) * c;
		tz_max = (z0 - oz) * c;
	}

	double t0, t1;

	if (tx_min > ty_min)
		t0 = tx_min;
	else
		t0 = ty_min;

	if (tz_min > t0)
		t0 = tz_min;

	if (tx_max < ty_max)
		t1 = tx_max;
	else
		t1 = ty_max;

	if (tz_max < t1)
		t1 = tz_max;

	if (t0 > t1)
		return(false);

	// initial cell coordinates

	int ix, iy, iz;

	if (bbox.inside(ray.o)) {  			// does the ray start inside the grid?
		ix = clamp((ox - x0) * nx / (x1 - x0), 0, nx - 1);
		iy = clamp((oy - y0) * ny / (y1 - y0), 0, ny - 1);
		iz = clamp((oz - z0) * nz / (z1 - z0), 0, nz - 1);
	}
	else {
		Point3D p = ray.o + t0 * ray.d;  // initial hit point with grid's bounding box
		ix = clamp((p.x - x0) * nx / (x1 - x0), 0, nx - 1);
		iy = clamp((p.y - y0) * ny / (y1 - y0), 0, ny - 1);
		iz = clamp((p.z - z0) * nz / (z1 - z0), 0, nz - 1);
	}

	// ray parameter increments per cell in the x, y, and z directions

	double dtx = (tx_max - tx_min) / nx;
	double dty = (ty_max - ty_min) / ny;
	double dtz = (tz_max - tz_min) / nz;

	double 	tx_next, ty_next, tz_next;
	int 	ix_step, iy_step, iz_step;
	int 	ix_stop, iy_stop, iz_stop;
	int     ix_start, iy_start;

	if (dx > 0) {
		tx_next = tx_min + (ix + 1) * dtx;
		ix_step = +1;
		ix_start = 0;
		ix_stop = nx;
	}
	else {
		tx_next = tx_min + (nx - ix) * dtx;
		ix_step = -1;
		ix_start = nx-1;
		ix_stop = -1;
	}

	if (dx == 0.0) {
		tx_next = kHugeValue;
		ix_step = -1;
		ix_start = nx-1;
		ix_stop = -1;
	}


	if (dy > 0) {
		ty_next = ty_min + (iy + 1) * dty;
		iy_step = +1;
		iy_start = 0;
		iy_stop = ny;
	}
	else {
		ty_next = ty_min + (ny - iy) * dty;
		iy_step = -1;
		iy_start = ny-1;
		iy_stop = -1;
	}

	if (dy == 0.0) {
		ty_next = kHugeValue;
		iy_step = -1;
		iy_start = ny-1;
		iy_stop = -1;
	}

	if (dz > 0) {
		tz_next = tz_min + (iz + 1) * dtz;
		iz_step = +1;
		iz_stop = nz;
	}
	else {
		tz_next = tz_min + (nz - iz) * dtz;
		iz_step = -1;
		iz_stop = -1;
	}

	if (dz == 0.0) {
		tz_next = kHugeValue;
		iz_step = -1;
		iz_stop = -1;
	}


//----------------------------------- traverse the grid -------------------------------------------------------
	bool hitS;
	int j_hit = -1;
	int updown = 0;

	while (true) {
		Compound* compound_ptr = cells[ix + nx * iy + nx * ny * iz];

		if (tx_next < ty_next && tx_next < tz_next) {
			if (compound_ptr && compound_ptr->hit(ray, t, j_hit, updown) && t < tx_next) {
				
				if (lightType == 1){ // direct 
					if (updown == 1){
						compound_ptr->get_triangleList()[j_hit]->photonFlux_up_dir[hour_th] += (ray.photonFlux2 *
							(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
							compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));   //the hour_th hour
					}
					if (updown == -1){
						compound_ptr->get_triangleList()[j_hit]->photonFlux_down_dir[hour_th] += (ray.photonFlux2 *
							(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
							compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));   //the hour_th hour
					}
				}
				else if (lightType == 2){ //diffuse
					if (updown == 1){
						compound_ptr->get_triangleList()[j_hit]->photonFlux_up_dff[hour_th] += (ray.photonFlux2 *
							(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
							compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));   //the hour_th hour
					}
					if (updown == -1){
						compound_ptr->get_triangleList()[j_hit]->photonFlux_down_dff[hour_th] += (ray.photonFlux2 *
							(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
							compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));   //the hour_th hour
					}
				}else{              // scatter light lightType == 3
					if (updown == 1){
						compound_ptr->get_triangleList()[j_hit]->photonFlux_up_scat[hour_th] += (ray.photonFlux2 *
							(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
							compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));   //the hour_th hour
					}
					if (updown == -1){
						compound_ptr->get_triangleList()[j_hit]->photonFlux_down_scat[hour_th] += (ray.photonFlux2 *
							(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
							compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));   //the hour_th hour
					}
				}


			//----------------------------------------  scatter rays -------------------------------------------------
				hitS = generate_scatter_rays(ray, compound_ptr->get_triangleList()[j_hit],hour_th);

			//---------------------------------------- hit the scatter rays -------------------------------------------------
				return hitS;
			}
			tx_next += dtx;
			ix += ix_step;

			if (ix == ix_stop){
				// ----------------- move ray back the size of x extension
				ray.o.x = ray.o.x - ix_step*(bbox.x1 - bbox.x0);
				// ----------------- next cell is the start value
				ix = ix_start; //
				continue;
			}
		}
		else {
			if (ty_next < tz_next) {
				if (compound_ptr && compound_ptr->hit(ray, t, j_hit, updown) && t < ty_next) {
					if (lightType == 1){ // direct 
						if (updown == 1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_up_dir[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
						if (updown == -1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_down_dir[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
					}
					else if(lightType == 2){// diffuse
						if (updown == 1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_up_dff[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
						if (updown == -1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_down_dff[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
					}
					else{ // scatter
						if (updown == 1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_up_scat[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
						if (updown == -1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_down_scat[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
					}
					hitS = generate_scatter_rays(ray, compound_ptr->get_triangleList()[j_hit],hour_th);
					return hitS;
				}
				ty_next += dty;
				iy += iy_step;
				if (iy == iy_stop){
					// ----------------- move ray back the size of x extension
					ray.o.y = ray.o.y - iy_step*(bbox.y1 - bbox.y0);
					// ----------------- next cell is the start value
					iy = iy_start; //
					continue;
				}
		 	}
		 	else {
				if (compound_ptr && compound_ptr->hit(ray, t, j_hit, updown) && t < tz_next) {
					if (lightType == 1){ // direct 
						if (updown == 1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_up_dir[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
						if (updown == -1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_down_dir[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
					}
					else if (lightType == 2){ // diffuse
						if (updown == 1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_up_dff[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
						if (updown == -1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_down_dff[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
					}
					else{  //scatter
						if (updown == 1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_up_scat[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
						if (updown == -1){
							compound_ptr->get_triangleList()[j_hit]->photonFlux_down_scat[hour_th] += (ray.photonFlux2 *
								(1 - compound_ptr->get_triangleList()[j_hit]->kLeafReflectance -
								compound_ptr->get_triangleList()[j_hit]->kLeafTransmittance));
						}
					}
					hitS = generate_scatter_rays(ray, compound_ptr->get_triangleList()[j_hit],hour_th);
					return hitS;
				}
				tz_next += dtz;
				iz += iz_step;
				if (iz == iz_stop)
					return (false); // hit the soil ----------- Qingfeng
		 	}
		}
	}
}	// end of hit

//------------------------------------------------ hit scatter rays -----------------------------------------------

bool
Grid::hit_scatter_rays(vector<Ray*> scatter_rays, const int& hour_th)const{

	return true;
}

//vector<Ray*>
bool
Grid::generate_scatter_rays(Ray& ray, Triangle* triangle_ptr,const int& hour_th)const{
	vector<Ray*> scatter_rays;
	triangle_ptr ->compute_normal(); // ??? delete?QF 2014.2.18
	Vector3D normal_triangle (triangle_ptr ->normal);

//	cout<<"ray.d: "<<ray.d.x<<"  "<<ray.d.y<<"  "<<ray.d.z<<endl;
//	cout<<"norm_of_triangle: "<<normal_triangle.x<<"  "<<normal_triangle.y<<"  "<<normal_triangle.z<<endl;

	//check if L and N is opposite direction. and if not, opposite the N direction
	if(normal_triangle * ray.d > 0)
		normal_triangle = -normal_triangle;
//	normal_triangle.compute_theta_phi();

	LeafOptics* leaf_optics = new LeafOptics();

	double pf = ray.photonFlux2  * triangle_ptr->kLeafReflectance;
	if (pf > ignor_Photon_Flux_threashold){
		Vector3D reflect_d = leaf_optics ->get_reflect_dir(-ray.d, normal_triangle);
		scatter_rays.push_back(new Ray(triangle_ptr->hit_point,reflect_d,pf));
	}
	double pf2 = ray.photonFlux2 * triangle_ptr->kLeafTransmittance;
	if (pf2 > ignor_Photon_Flux_threashold){
		Vector3D transmit_d = leaf_optics ->get_transmit_dir(-ray.d, normal_triangle);
		scatter_rays.push_back(new Ray(triangle_ptr->hit_point,transmit_d,pf2));
	}

	delete leaf_optics;
// why after delete, still can access the pointer ???? --------------***************************
//	cout<<"TEST: ------ "<<ray_r_ptr->photonFlux2<<endl;

//	return statter_rays;
	int lightType3 = 3;//scatter light
	for (unsigned int k=0; k<scatter_rays.size(); k++){
	//			cout<<"go Back"<<endl;
		double t = kHugeValue; //when hit, the t life end
		Ray ray = Ray(scatter_rays[k]);
		this->hit(ray,t,hour_th,lightType3);
	}

	//delete the vector
	for (vector<Ray*>::iterator iter = scatter_rays.begin(); iter != scatter_rays.end();++iter) {
//		cout<<"delete *iter"<<endl;
		delete *iter;
	}
	scatter_rays.erase(scatter_rays.begin(),scatter_rays.end());

	return true;
}


void
Grid::add_triangle(Triangle* triangle_ptr){

	triangles.push_back(triangle_ptr);  //Qingfeng

}
vector<Compound*>
Grid::get_cells(){
	return cells;
}

vector<Triangle*>
Grid::get_triangles(){
	return triangles;

}

