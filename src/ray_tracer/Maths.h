#ifndef __MATHS__
#define __MATHS__
using namespace std;

inline double
max(double x0, double x1);

inline double
max(double x0, double x1)
{
	return((x0 > x1) ? x0 : x1);
}

inline double
min(double x0, double x1);

inline double
min(double x0, double x1)
{
	return((x0 < x1) ? x0 : x1);
}

inline int
clamp(double a, int b, int c);

inline int
clamp(double a, int b, int c)
{

	return((int)max(b,min(a,c)));
}

#endif
