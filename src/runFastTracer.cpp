#include <cstdlib>
#include <iostream>
#include "Grid.h"
#include <fstream>
#include <sstream>
#include <time.h>
#include "Climate.h"
#include "Constants.h"
#include <iomanip>
#include "string.h"
#include <algorithm>


using namespace std;

void import_data_from2DMatrix(double** m_3Dcanopy, Grid* grid, double start_hour, double end_hour, double hour_interval);
void import_data_from_file   (char  filename[], Grid* grid, double start_hour, double end_hour, double hour_interval);
bool cmp(double *p,double *q);

// return 2D matrix: double** m_3Dcanopy_light; accessd by m_3Dcanopy_light[i][j], i is rowID, j is columnID
// output: columns are x1 y1 z1 x2 y2 z2 x3 y3 z3 leafID leafLength Position plant column id, plant row id, SPAD Kt Kr NitrogenPerArea FacetArea PPFD cLAI
extern "C" void runFastTracer (int is_import_from_2DMatrix, char  filename[], double **m_3Dcanopy_light, double latitude, int day, double h, double Idir, double Idiff, double light_min_x,
                        double light_max_x, double light_min_y, double light_max_y, double light_min_z, double light_max_z){
// input: 
//     is_import_from_2DMatrix: bool, true is "inport from 2D matrix and data in m_3Dcanopy"; if false, "import from file of filename"
//     m_3Dcanopy should be a 2D matrix with columns of:
//         x1 y1 z1 x2 y2 z2 x3 y3 z3 leafID leafLength Position SPAD Kt Kr NitrogenPerArea
//     lat: double, unit of degree, 0~90. 
//     day: int,    1~365.
//     hour (h): double, 0.0 ~ 24.0
//     Idir, Idiff: double, unit: umol/m2/s
//  xmin, xmax, ymin, ymax, zmin, zmax: double, unit of cm. 

//  char filename[100], outfilename[100];
	/*
	 * information for run software
	 */
//	if(argc == 0 ){
//		cout<<"lat sn at d h min_x max_x y y z z nd Model out leaf_t r modelType(1,2,3) isWholeDay(0,1) startHour endHour intervalHour silence"<<endl;
//		return 1;

	/*
	 * input parameters
	 */

//	double latitude     = atof( argv[1] );
	double solar_noon   = 12;
//	double atmo_t       = atof( argv[3] );
//	int    day          = atoi( argv[4] );
//	double h         = atof( argv[5] );

//	double light_min_x 	= atof( argv[6] );   //  0
//	double light_max_x 	= atof( argv[7] );   // 10.5
//	double light_min_y 	= atof( argv[8] );   //  0
//	double light_max_y 	= atof( argv[9] );   //  5.5
//	double light_min_z  = atof( argv[10]); 	 // 12
//	double light_max_z  = atof( argv[11]);   //light is a horizontal face

	double light_nearest_distance = 0.1 ;  //

//	strcpy(filename,    argv[13]);   // for VC++ strcpy_s
//	strcpy(outfilename, argv[14]);   // for VC++ strcpy_s
	

//	double leaf_t       = atof( argv[15] );
//	double leaf_r       = atof( argv[16] );
//	int modeltype = atoi ( argv [17] );
//	int isWholeDay = atoi ( argv [18] );
// ------------- static variables ---------
	double kt = 0;
	double kr = 0;
// ------------- for 2 min interval simulation @ Shanghai, Sep 18th

//	double start_hour = atof(argv[19]);
//	double end_hour = atof(argv[20]);
//	double hour_interval = atof(argv[21]);

//	int silence = atoi(argv[22]);

//	if (isWholeDay == 1){
	//	start_hour = 6; //input from argv[19]
	//	end_hour = 18;  //input from argv[20]
//		hour_interval = hour_interval; // input from argv[21]
//	}else if(isWholeDay == 0){
		double start_hour = h; // input h as the hour of single time point
		double end_hour = h;
		double hour_interval = 1; // no use
//	}

//----------------------------------  Climate -------------------------------

	Climate climate;
  double atmo_t = 0.7;   // the real direct light and diffuse light is used from BioCro
  int silence = 1;
	climate.climate_calculation(latitude,solar_noon,atmo_t,day, start_hour, end_hour, hour_interval,silence);

//---------------  model import to a grid  -------------
	Grid* grid = new Grid(ignor_PPFD_threashold * light_nearest_distance * light_nearest_distance * 1e-4); //umol.s-1
//	if (silence == 0){
//		cout << "new Grid OK, import data ..." << endl;
//	}
	
  if(is_import_from_2DMatrix == 1){
  import_data_from2DMatrix(m_3Dcanopy_light, grid, start_hour, end_hour, hour_interval);
  }else{
    import_data_from_file (filename, grid, start_hour, end_hour, hour_interval);
    
  }
  
//	if (silence == 0){
//		cout << "import data OK, total " << grid->get_triangles().size() << " triangles, setup cells ..." << endl;
//	}
//------------- set up triangles into cells in the grid   ----------------
	grid->setup_cells(Point3D(light_min_x,light_min_y,light_min_z), Point3D(light_max_x,light_max_y,light_max_z)); //setup Grid, setup the triangles to cells with each cell a triangleList
//	if (silence == 0){
//		cout << "setup cells OK, total " << grid->get_cells().size() << " cells (" << grid->get_triangles().size() << " triangles)" << endl;
//		cout << "tracing direct light ... ..." << endl;
//	}
int num = (int)((end_hour - start_hour)/hour_interval); // when isWholeDay == 0, num = 0 and the folloing loop only run one time
for (int i = 0; i<=num; i++){
	
  double hour = start_hour + i*hour_interval;//we choose the mid-point of each hour as simulation sample point

	double light_d_x 	= climate.direct_light_d_list[i]->x;
	double light_d_y 	= climate.direct_light_d_list[i]->y;
	double light_d_z 	= climate.direct_light_d_list[i]->z;

	double direct_light_ppfd = Idir; //climate.ppfd_direct_list[i];
	double diffuse_light_ppfd = Idiff; //climate.ppfd_diffuse_list[i];

//------------------ output run log  -------------------------
//	if (silence == 0){
//		cout << "latitude:   " << latitude << endl;
//		cout << "solar_noon: " << solar_noon << endl;
//		cout << "atmospheric transmittance: " << atmo_t << endl;
//		cout << "day:  " << day << endl;
//		cout << "hour: " << hour << endl;
//		cout << "direct light PPFD:  " << direct_light_ppfd << endl;
//		cout << "diffuse light PPFD: " << diffuse_light_ppfd << endl;
//		cout << "x : (" << light_min_x << ", " << light_max_x << ")" << endl;
//		cout << "y : (" << light_min_y << ", " << light_max_y << ")" << endl;
//		cout << "z : (" << light_min_z << ", " << light_max_z << ")" << endl;
//		cout << "light_nearest_distance : " << light_nearest_distance << endl;
//		cout << "direct light direction : (" << light_d_x << ", " << light_d_y << ", " << light_d_z << ")" << endl;
//	}

//	---------------  Time   -------------------------
	//time_t t1 = time(0);
	//char tmp[64];
	//struct tm *tmp = NULL;
	//localtime_s(tmp, &t1);
	//cout<<"--------"<<tmp<<"--------"<<endl;

//--------------------------------------     << Direct Light >>    -----------------------------------------------

//--------------- Variables of Ray and PPF of ray (direct light)   -------------------------
	Ray ray;
	double t = kHugeValue;
	double dir_pf = direct_light_ppfd * light_nearest_distance * light_nearest_distance * 1e-4; //umol.s-1
	ray.d = Vector3D(light_d_x,light_d_y,light_d_z);
	ray.photonFlux2 = dir_pf;
	double x,y;

//---------------  trace rays (direct light)   ----------------------
	double light_max_x_stop = light_max_x - light_nearest_distance;
	double light_max_y_stop = light_max_y - light_nearest_distance;
	int lightType1 = 1; //direct light
	if (ray.photonFlux2 > 0){
		for (x = light_min_x; x<light_max_x_stop; x += light_nearest_distance)
		for (y = light_min_y; y<light_max_y_stop; y += light_nearest_distance){
			ray.o = Point3D(x, y, light_max_z);
			grid->hit(ray, t, i, lightType1);//Grid -> hit(ray, tmin), if hit, the hit triangle will be add one time hit number
		}
	}
	
//-----------------------------  Time  ------------------------------
//	t1 = time(0);

//	localtime_s(tmp,&t1);
//	if (silence == 0){
//		cout << "finish!" << endl << "----------------" << endl << endl;
//		cout << "tracing diffuse light ... ..." << endl;
//	}
//--------------------------------------     << Diffuse Light >>    -----------------------------------------------

//--------------- update Variables for Ray and PPF of ray (diffuse light)  -------------------------

	double dif_pf = diffuse_light_ppfd * light_nearest_distance * light_nearest_distance * 1e-4;
	ray.photonFlux2 = dif_pf;

//---------------  trace rays (diffuse light)   -------------------------
	light_max_x_stop = light_max_x - light_nearest_distance;
	light_max_y_stop = light_max_y - light_nearest_distance;
	int lightType2 = 2; //diffuse light
	if (ray.photonFlux2 > 0){
		for (x = light_min_x; x < light_max_x_stop; x += light_nearest_distance)
		for (y = light_min_y; y < light_max_y_stop; y += light_nearest_distance){
			ray.o = Point3D(x, y, light_max_z);
			ray.d = Vector3D(true);
			grid->hit(ray, t, i, lightType2);//Grid -> hit(ray, tmin), if hit, the hit triangle will be add one time hit number
		}
	}

//---------------  Time   -------------------------
//	t1 = time(0);
//	localtime_s(tmp,&t1);
//	if (silence == 0){
//		cout << "finish!" << endl << "---------------" << endl << endl;
//	}
}//hours in a whole day from 0.5h to 23.5h

//--------------  output to file ---------------
//	ofstream myfile (outfilename);
//	if (myfile.is_open()){
//		if (silence == 0){
//			cout << "open output file OK" << endl;
//		}
//		myfile << "x1" << "\t" << "y1" << "\t" << "z1" << "\t";      // titles
//		myfile << "x2" << "\t" << "y2" << "\t" << "z2" << "\t";
//		myfile << "x3" << "\t" << "y3" << "\t" << "z3" << "\t";
//		myfile <<"leafID"<< "\t" <<"leafL"<<"\t"<<"position"<<"\t"<<"ChlSPAD"<<"\t"<<"Kt"<<"\t"<<"Kr"<<"\t"<<"NperArea"<<"\t"<<"facetArea"<<"\t";
//		for (int i=0; i<=num; i++){
//			double hour = start_hour + i*hour_interval;
//			myfile <<hour<<"h_up_dir"<<"\t";
//			myfile << hour << "h_up_dff" << "\t";
//			myfile << hour << "h_up_scat" << "\t";
//			myfile << hour << "h_bottom_dir" << "\t";
//			myfile << hour << "h_bottom_dff" << "\t";
//			myfile << hour << "h_bottom_scat" << "\t";
//		}
//		myfile <<"\n";


//    double** m_3Dcanopy_light = new double* [1000];  //!!! for 1000 triangles !!!
    int i_2DMatrix = 0;
		double area;
		double ppfd;
		int num_triangle = grid -> get_triangles().size();


//		myfile.setf(ios::fixed);
		vector<Triangle*> v = grid->get_triangles();
		vector<Triangle*>::iterator it;

		for (it = v.begin(); it != v.end(); it++){
      
//      m_3Dcanopy_light[i_2DMatrix] = new double [19];
      
			area = (((*it)->v1 - (*it)->v0) ^
				((*it)->v2 - (*it)->v0)).length() * 0.5; //cm2
			m_3Dcanopy_light[i_2DMatrix][0] = (*it)->v0.x; 
			m_3Dcanopy_light[i_2DMatrix][1] = (*it)->v0.y;
			m_3Dcanopy_light[i_2DMatrix][2] = (*it)->v0.z ;
			m_3Dcanopy_light[i_2DMatrix][3] =  (*it)->v1.x ;
			m_3Dcanopy_light[i_2DMatrix][4] =  (*it)->v1.y ;
			m_3Dcanopy_light[i_2DMatrix][5] =  (*it)->v1.z ;
			m_3Dcanopy_light[i_2DMatrix][6] =   (*it)->v2.x ;
			m_3Dcanopy_light[i_2DMatrix][7] =   (*it)->v2.y ;
			m_3Dcanopy_light[i_2DMatrix][8] =   (*it)->v2.z ;
			m_3Dcanopy_light[i_2DMatrix][9] =  (*it)->leID ;
			m_3Dcanopy_light[i_2DMatrix][10] =  (*it)->leL ;
			m_3Dcanopy_light[i_2DMatrix][11] =  (*it)->pos ;
			m_3Dcanopy_light[i_2DMatrix][12] =   (*it)->chlSPA ;
			m_3Dcanopy_light[i_2DMatrix][13] =   (*it)->kLeafTransmittance ;
			m_3Dcanopy_light[i_2DMatrix][14] =   (*it)->kLeafReflectance ;
			m_3Dcanopy_light[i_2DMatrix][15] =   (*it)->nitrogenPerA;
      
			m_3Dcanopy_light[i_2DMatrix][16] =   area ;
      


			vector<double> photonFlux_up_dir = (*it) ->photonFlux_up_dir;   // light from up side
			vector<double> photonFlux_up_dff = (*it)->photonFlux_up_dff;   // light from up side
			vector<double> photonFlux_up_scat= (*it)->photonFlux_up_scat;   // light from up side
			vector<double> photonFlux_down_dir = (*it)->photonFlux_down_dir; // light from down side
			vector<double> photonFlux_down_dff = (*it)->photonFlux_down_dff; // light from down side
			vector<double> photonFlux_down_scat = (*it)->photonFlux_down_scat; // light from down side
			vector<double>::iterator it1, it2, it3, it4, it5, it6;
			
			double area_factor = 1 / (area * 1e-4);

			it2 = photonFlux_up_dff.begin();
			it3 = photonFlux_up_scat.begin();
			it4 = photonFlux_down_dir.begin();
			it5 = photonFlux_down_dff.begin();
			it6 = photonFlux_down_scat.begin();
			
			for (it1 = photonFlux_up_dir.begin(); it1 != photonFlux_up_dir.end(); it1++){

				m_3Dcanopy_light[i_2DMatrix][18] =  ( (*it1) + (*it2) + (*it3) + (*it4) + (*it5) + (*it6) ) * area_factor;
				  
				it2++; it3++; it4++; it5++; it6++;
			}

		//	myfile << "\n";
    
    i_2DMatrix ++;
		}
		printf("inside tracer function --%d", i_2DMatrix);
    
    // after we get structure, area and PPFD into the matrix, here, add calculation of cLAI. 
      // first, sort the triangles by Z value "sum(z1, z2, z3)"

	sort(m_3Dcanopy_light, m_3Dcanopy_light + nrows, cmp);


      double totalLA = 0;
      double oneOverground_area = 1/(light_max_x - light_min_x) * (light_max_y - light_min_y); 

      for (int m = 0; m<nrows; m++){                   
        totalLA += m_3Dcanopy_light[m][16];   
        m_3Dcanopy_light[m][19] = totalLA *oneOverground_area;  
      }
      
      
      

//		for (int i=0; i<num_triangle; i++){
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v0.x <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v0.y <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v0.z <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v1.x <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v1.y <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v1.z <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v2.x <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v2.y <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->v2.z <<"\t";
//
//			myfile <<setprecision(0)<< grid -> get_triangles()[i]->leid <<"\t";  //int
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->lel <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->pos <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->chlspa <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->kleaftransmittance <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->kleafreflectance <<"\t";
//			myfile <<setprecision(2)<< grid -> get_triangles()[i]->nitrogenpera <<"\t";
//
////--------------------  triangle area and ppfd  ------------------------
//			area = ((grid->get_triangles()[i]->v1 - grid->get_triangles()[i]->v0) ^
//				    (grid->get_triangles()[i]->v2 - grid->get_triangles()[i]->v0)).length() * 0.5; //cm2
//			myfile <<setprecision(4)<< area <<"\t";
//
//			for (int j = 0; j<=num; j++){
//				ppfd = grid->get_triangles()[i]->photonflux_up_dir[j] / (area * 1e-4);                        //umol.m-2.s-1
//				myfile <<setprecision(2)<< ppfd << "\t";
//				ppfd = grid->get_triangles()[i]->photonflux_up_dff[j] / (area * 1e-4);                        //umol.m-2.s-1
//				myfile << setprecision(2) << ppfd << "\t";
//				ppfd = grid->get_triangles()[i]->photonflux_up_scat[j] / (area * 1e-4);                        //umol.m-2.s-1
//				myfile << setprecision(2) << ppfd << "\t";
//				ppfd = grid->get_triangles()[i]->photonflux_down_dir[j] / (area * 1e-4);                        //umol.m-2.s-1
//				myfile << setprecision(2) << ppfd << "\t";
//				ppfd = grid->get_triangles()[i]->photonflux_down_dff[j] / (area * 1e-4);                        //umol.m-2.s-1
//				myfile << setprecision(2) << ppfd << "\t";
//				ppfd = grid->get_triangles()[i]->photonflux_down_scat[j] / (area * 1e-4);                        //umol.m-2.s-1
//				myfile << setprecision(2) << ppfd << "\t";
//			}
//			myfile << "\n";
//
//		}
//		myfile.close();
//		if (silence == 0){
//			cout << "write data to output file Finish, total " << num_triangle << " triangles" << endl;
//			cout << "program End" << endl;
//			cout << "==================================================" << endl << endl;
//		}
//	}
//	else cout << "Unable to open file";
//	return 0;
}

//----------------------------------------------- Import model  --------------------------------------------------
void import_data_from_file(char filename[], Grid* grid, double start_hour, double end_hour, double hour_interval){

	//modeltype: 1, 1-9 colums are model; 2, 1-9 model, 16 and 17 are leaf transmittance and reflectance;
			//
	//	bool isCM3 = false;  // if the input model is CM3, including Kt, Kr (leaf transmittance, leaf reflectance), ture; if not, false;

	ifstream myfile (filename);
	string line;
	if (myfile.is_open())
	{
		double x1, y1, z1,
		  	   x2, y2, z2,
		  	   x3, y3, z3;
		double leafID = 0, leafL = 0, position =0, chlSPAD=0, plantColID = 0, plantRowID = 0;
		double kt, kr;
		double nitrogenPerArea=0;

	    while ( !myfile.eof() ) //
	    {

	    //	cout<<"OK9"<<endl;

	      getline (myfile,line);
	    //  cout<<"OK10"<<endl;
	      if(line.length()>3){  // NOT the end of file
	      istringstream istr(line);
	   //   cout<<"OK11"<<endl;

	      istr >> x1; istr >> y1; istr >> z1;
	      istr >> x2; istr >> y2; istr >> z2;
	      istr >> x3; istr >> y3; istr >> z3;
	   //   cout<<"OK1"<<endl;
	    //  if (modeltype == 1){        // 1-9 colums are model;
	   // 	  kt = ktInput; kr = krInput;     // input from commmand line
	   // 	  cout<<"OK2"<<endl;
	   //   }else if (modeltype == 2){  // 1-9 model, 14 and 15 are leaf transmittance and reflectance;

	    	  istr >> leafID;istr >> leafL;istr >> position;istr >> plantColID; istr >> plantRowID; istr >> chlSPAD;  // other contributes in model file
	    	  istr >> kt  ;istr >> kr;// input from model file
	    	  istr >> nitrogenPerArea;

	    //  }else if (modeltype == 3){  // 1-9 model, 10-13 parameters, input kt and kr instead of model input, and 16 is nitrogen per leaf area
	    //	  istr >> leafID;istr >> leafL;istr >> position;istr >> chlSPAD;  // other contributes in model file
	    //	  istr >> kt  ;istr >> kr;// not use
	    //	  kt = ktInput; kr = krInput;     // input from commmand line
	    //	  istr >> nitrogenPerArea;
	    //  }
	   //   cout<<"OK3"<<endl;


// --------------------- new a triangle and add into grid ------------------------------------
	      Triangle* triangle = new Triangle(Point3D(x1,y1,z1), Point3D(x2,y2,z2), Point3D(x3,y3,z3), leafID, leafL, position, chlSPAD, kt, kr, nitrogenPerArea,
	    		  start_hour, end_hour, hour_interval);
	   //   cout<<"OK4"<<endl;
	      triangle->compute_normal();
	      grid -> add_triangle(triangle);
	   //   cout<<"OK5"<<endl;
	      }
	  //    cout<<"OK6"<<endl;
	    }
	 //   cout<<"OK7"<<endl;
	    myfile.close();
	 //   cout<<"OK8"<<endl;
	}
	else { cout << "Unable to open file";}
//	cout<<"OK90"<<endl;

}


// use for read data from 3D matrix transfer from BioCro. 
void import_data_from2DMatrix(double** m_3Dcanopy, Grid* grid, double start_hour, double end_hour, double hour_interval){

  //modeltype: 1, 1-9 colums are model; 2, 1-9 model, 14 and 15 are leaf transmittance and reflectance;
			//
	//	bool isCM3 = false;  // if the input model is CM3, including Kt, Kr (leaf transmittance, leaf reflectance), ture; if not, false;

		double x1, y1, z1,
		  	   x2, y2, z2,
		  	   x3, y3, z3;
		double leafID = 0, leafL = 0, position =0, chlSPAD=0;
		double kt, kr;
		double nitrogenPerArea=0;

    for (int i=0; i<nrows; i++){
      x1 = m_3Dcanopy[i][0]; y1 = m_3Dcanopy[i][1]; z1 = m_3Dcanopy[i][2]; 
      x2 = m_3Dcanopy[i][3]; y2 = m_3Dcanopy[i][4]; z2 = m_3Dcanopy[i][5]; 
      x3 = m_3Dcanopy[i][6]; y3 = m_3Dcanopy[i][7]; z3 = m_3Dcanopy[i][8]; 
      
      leafID = m_3Dcanopy[i][9]; leafL = m_3Dcanopy[i][10]; position = m_3Dcanopy[i][11];
      chlSPAD = m_3Dcanopy[i][14];kt = m_3Dcanopy[i][15];kr = m_3Dcanopy[i][16];
      nitrogenPerArea = m_3Dcanopy[i][17];

// --------------------- new a triangle and add into grid ------------------------------------
	      Triangle* triangle = new Triangle(Point3D(x1,y1,z1), Point3D(x2,y2,z2), Point3D(x3,y3,z3), leafID, leafL, position, chlSPAD, kt, kr, nitrogenPerArea,
	    		  start_hour, end_hour, hour_interval);

	      triangle->compute_normal();
	      grid -> add_triangle(triangle);

	    }

}

//comparison function for sort
bool cmp( double *p, double *q)
{
	if ( q[2] < p[2]) {
		return true;
	}
	else {
		return false;
	}
    // averaged value of z for a triangle, sort by decreasing z, from top to bottom in canopy
}


