



void runFastTracer (int is_import_from_2DMatrix, char  filename[], double m_3Dcanopy[1000][19], double latitude, int day, double h, double Idir, double Idiff, double light_min_x, 
						double light_max_x, double light_min_y, double light_max_y, double light_min_z, double light_max_z);

int main(){

	int is_import_from_2DMatrix = 0;
	char  filename[] = "";
	double m_3Dcanopy[1000][19];
	double   latitude   = 30;
	int      day        = 200;
	double   h          = 10;
	double   Idir       = 1000;
	double   Idiff      = 300;
	double   light_min_x= 0;
	double   light_max_x= 100;
	double   light_min_y= 0;
	double   light_max_y= 100;
	double   light_min_z= 0;
	double   light_max_z= 100;

	runFastTracer (is_import_from_2DMatrix,   filename, m_3Dcanopy,  latitude,  day,  h,  Idir,  Idiff,  light_min_x,
		light_max_x,  light_min_y,  light_max_y,  light_min_z,  light_max_z);

//	m_3Dcanopy will be changed;
	return 0;
}

