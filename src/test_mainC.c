#include <stdio.h>
#include <stdlib.h>

// defined the total triangle number
const int nrows = 2806;
// total columns. see comment of is_import_from_2DMatrix. 
const int ncols = 26;
// ********* when changing above two values: should change two const values in Constants.h. ***********
// they are: 
//const int nrows = 2806;
//const int ncols = 20;


void runFastTracer (int is_import_from_2DMatrix, char  filename[], double **m_3Dcanopy, double latitude, int day, double h, double Idir, double Idiff, double light_min_x, 
						double light_max_x, double light_min_y, double light_max_y, double light_min_z, double light_max_z);

int main(){

	int is_import_from_2DMatrix = 0;  //if 0: input from Canopy3D model file, the m_3Dcanopy contains zeros for getting results from fastTracer; 
									 // if 1: input from m_3Dcanopy, that means 3D structure model is already in the m_3Dcanopy.  

	char  filename[] = "E:\\Projects\\3DBioCro\\Sugarcane\\CM_SC.txt";  //this is the file name of input Canopy3D model file. 
	
	double   latitude   = 30;	// unit degree
	int      day        = 200;	 
	double   h          = 10;	
	double   Idir       = 1000; 
	double   Idiff      = 300; 

	// follows are extension of the ray tracing area
	double   light_min_x= -110; 
	double   light_max_x= 110;
	double   light_min_y= -20;
	double   light_max_y= 20;
	double   light_min_z= 0;
	double   light_max_z= 300;

	int i, n, m;

	double **m_3Dcanopy;   // format [total 20column]: column ID 0-8: 3D structure, 9: leafID, 10: leaf length, 11: distance from leaf base, 12: plant column id, 13: plant row id, 14: SPAD, 
						  //  15: transmittance, 16: reflectance, 17: leaf N content, 18: PPFD (umol.m-2.s-1), 19: cLAI. 


	m_3Dcanopy =  malloc(nrows * sizeof(double *));

	for(i = 0; i < nrows; i++)
	{
		m_3Dcanopy[i] =  malloc(ncols * sizeof(double));

	}
	
	// set to be zeros is needed when the nrows > real number of triangles, otherwise sorted by height of triangle will make error. 
	for (n=0; n<10; n++){
		for (m=0; m<20; m++){
			m_3Dcanopy[n][m] = 0;
		}
	}

	runFastTracer (is_import_from_2DMatrix,   filename, m_3Dcanopy,  latitude,  day,  h,  Idir,  Idiff,  light_min_x,
		light_max_x,  light_min_y,  light_max_y,  light_min_z,  light_max_z);
// PPFD, cLAI will be in column ID 18: PPFD (umol.m-2.s-1), 19: cLAI. 
//****	after ray tracing, the rows in m_3Dcanopy will be sorted by height from triangles in canopy from top to bottom. ****

	// for testing 
	for (n=0; n<2806; n++){

			printf("%f,%f,%f",m_3Dcanopy[n][0],m_3Dcanopy[n][1],m_3Dcanopy[n][19]);		
			printf("\n");
	}
	//

	return 0;
}


