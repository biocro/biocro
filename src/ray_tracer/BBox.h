
#ifndef BBOX_H_
#define BBOX_H_
#include "Ray.h"

class BBox{
public:
	double x0, y0, z0;  // the min point
	double x1, y1, z1;  // the max point

	BBox();
	BBox(	const double x0, const double x1,			
				const double y0, const double y1, 
				const double z0, const double z1);
	
	
	bool hit (const Ray& ray ) const;
	bool inside(const Point3D& p) const;
	
	virtual ~BBox();
};

#endif /* BBOX_H_ */

