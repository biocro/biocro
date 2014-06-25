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

using namespace std;

void import_data(int modeltype, char filename[], Grid* grid, double kt, double kr, double start_hour, double end_hour, double hour_interval);

// plan: change main to runFastTracer so that BioCro can call it
//

int main(int argc, char *argv[]){
	char filename[100], outfilename[100];
	/*
	 * information for run software
	 */
	if(argc == 0 ){
		cout<<"lat sn at d h min_x max_x y y z z nd Model out leaf_t r modelType(1,2,3) isWholeDay(0,1) startHour endHour intervalHour silence"<<endl;
		return 1;
	}else if(argc < 21){
		cout<<"error, not enough parameters !"<<endl;
		cout<<"lat sn at d h min_x max_x y y z z nd Model out leaf_t r modeltype(1,2,3) isWholeDay(0,1) startHour endHour intervalHour silence"<<endl;
		return 1;
	}

	/*
	 * input parameters
	 */

// we should use parameters input from call function

	double latitude     = atof( argv[1] );
	double solar_noon   = atof( argv[2] );
	double atmo_t       = atof( argv[3] );
	int    day          = atoi( argv[4] );
	double h         = atof( argv[5] );

	double light_min_x 	= atof( argv[6] );   //  0
	double light_max_x 	= atof( argv[7] );   // 10.5
	double light_min_y 	= atof( argv[8] );   //  0
	double light_max_y 	= atof( argv[9] );   //  5.5
	double light_min_z  = atof( argv[10]); 	 // 12
	double light_max_z  = atof( argv[11]);   //light is a horizontal face

	double light_nearest_distance = atof( argv[12] );  //0.1

	strcpy(filename,    argv[13]);   // for VC++ strcpy_s
	strcpy(outfilename, argv[14]);   // for VC++ strcpy_s
	

	double leaf_t       = atof( argv[15] );
	double leaf_r       = atof( argv[16] );
	int modeltype = atoi ( argv [17] );
	int isWholeDay = atoi ( argv [18] );
// ------------- static variables ---------
	double kt = leaf_t;
	double kr = leaf_r;
// ------------- for 2 min interval simulation @ Shanghai, Sep 18th

	double start_hour = atof(argv[19]);
	double end_hour = atof(argv[20]);
	double hour_interval = atof(argv[21]);

	int silence = atoi(argv[22]);

	if (isWholeDay == 1){
	//	start_hour = 6; //input from argv[19]
	//	end_hour = 18;  //input from argv[20]
//		hour_interval = hour_interval; // input from argv[21]
	}else if(isWholeDay == 0){
		start_hour = h; // input h as the hour of single time point
		end_hour = h;
		hour_interval = 1; // no use
	}

//----------------------------------  Climate -------------------------------

	Climate climate;
	climate.climate_calculation(latitude,solar_noon,atmo_t,day, start_hour, end_hour, hour_interval,silence);

//---------------  model import to a grid  -------------
	Grid* grid = new Grid(ignor_PPFD_threashold * light_nearest_distance * light_nearest_distance * 1e-4); //umol.s-1
	if (silence == 0){
		cout << "new Grid OK, import data ..." << endl;
	}
	import_data(modeltype,filename,grid, kt, kr, start_hour, end_hour, hour_interval);
	if (silence == 0){
		cout << "import data OK, total " << grid->get_triangles().size() << " triangles, setup cells ..." << endl;
	}
//------------- set up triangles into cells in the grid   ----------------
	grid->setup_cells(Point3D(light_min_x,light_min_y,light_min_z), Point3D(light_max_x,light_max_y,light_max_z)); //setup Grid, setup the triangles to cells with each cell a triangleList
	if (silence == 0){
		cout << "setup cells OK, total " << grid->get_cells().size() << " cells (" << grid->get_triangles().size() << " triangles)" << endl;
		cout << "tracing direct light ... ..." << endl;
	}
int num = (int)((end_hour - start_hour)/hour_interval); // when isWholeDay == 0, num = 0 and the folloing loop only run one time
for (int i = 0; i<=num; i++){
	double hour = start_hour + i*hour_interval;//we choose the mid-point of each hour as simulation sample point

	double light_d_x 	= climate.direct_light_d_list[i]->x;
	double light_d_y 	= climate.direct_light_d_list[i]->y;
	double light_d_z 	= climate.direct_light_d_list[i]->z;

	double direct_light_ppfd = climate.ppfd_direct_list[i];
	double diffuse_light_ppfd = climate.ppfd_diffuse_list[i];

//------------------ output run log  -------------------------
	if (silence == 0){
		cout << "latitude:   " << latitude << endl;
		cout << "solar_noon: " << solar_noon << endl;
		cout << "atmospheric transmittance: " << atmo_t << endl;
		cout << "day:  " << day << endl;
		cout << "hour: " << hour << endl;
		cout << "direct light PPFD:  " << direct_light_ppfd << endl;
		cout << "diffuse light PPFD: " << diffuse_light_ppfd << endl;
		cout << "x : (" << light_min_x << ", " << light_max_x << ")" << endl;
		cout << "y : (" << light_min_y << ", " << light_max_y << ")" << endl;
		cout << "z : (" << light_min_z << ", " << light_max_z << ")" << endl;
		cout << "light_nearest_distance : " << light_nearest_distance << endl;
		cout << "direct light direction : (" << light_d_x << ", " << light_d_y << ", " << light_d_z << ")" << endl;
	}

//---------------  Time   -------------------------
	time_t t1 = time(0);
  //  char tmp[64];
//	struct tm *tmp = NULL;
//	localtime_s(tmp, &t1);
//	cout<<"--------"<<tmp<<"--------"<<endl;

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
	int lightType1 = 1; //diffuse light
	if (ray.photonFlux2 > 0){
		for (x = light_min_x; x<light_max_x_stop; x += light_nearest_distance)
		for (y = light_min_y; y<light_max_y_stop; y += light_nearest_distance){
			ray.o = Point3D(x, y, light_max_z);
			grid->hit(ray, t, i, lightType1);//Grid -> hit(ray, tmin), if hit, the hit triangle will be add one time hit number
		}
	}
	
//-----------------------------  Time  ------------------------------
	t1 = time(0);

//	localtime_s(tmp,&t1);
	if (silence == 0){
		cout << "finish!" << endl << "----------------" << endl << endl;
		cout << "tracing diffuse light ... ..." << endl;
	}
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
	t1 = time(0);
//	localtime_s(tmp,&t1);
	if (silence == 0){
		cout << "finish!" << endl << "---------------" << endl << endl;
	}
}//hours in a whole day from 0.5h to 23.5h

//--------------  output to file ---------------
	ofstream myfile (outfilename);
	if (myfile.is_open()){
		if (silence == 0){
			cout << "open output file OK" << endl;
		}
		myfile << "x1" << "\t" << "y1" << "\t" << "z1" << "\t";      // titles
		myfile << "x2" << "\t" << "y2" << "\t" << "z2" << "\t";
		myfile << "x3" << "\t" << "y3" << "\t" << "z3" << "\t";
		myfile <<"leafID"<< "\t" <<"leafL"<<"\t"<<"position"<<"\t"<<"ChlSPAD"<<"\t"<<"Kt"<<"\t"<<"Kr"<<"\t"<<"NperArea"<<"\t"<<"facetArea"<<"\t";
		for (int i=0; i<=num; i++){
			double hour = start_hour + i*hour_interval;
			myfile <<hour<<"h_up_dir"<<"\t";
			myfile << hour << "h_up_dff" << "\t";
			myfile << hour << "h_up_scat" << "\t";
			myfile << hour << "h_bottom_dir" << "\t";
			myfile << hour << "h_bottom_dff" << "\t";
			myfile << hour << "h_bottom_scat" << "\t";
		}
		myfile <<"\n";

		double area;
		double ppfd;
		int num_triangle = grid -> get_triangles().size();


		myfile.setf(ios::fixed);
		vector<Triangle*> v = grid->get_triangles();
		vector<Triangle*>::iterator it;

		for (it = v.begin(); it != v.end(); it++){
			area = (((*it)->v1 - (*it)->v0) ^
				((*it)->v2 - (*it)->v0)).length() * 0.5; //cm2
			myfile << setprecision(2) << (*it)->v0.x << "\t"
				<< setprecision(2) << (*it)->v0.y << "\t"
				<< setprecision(2) << (*it)->v0.z << "\t"
				<< setprecision(2) << (*it)->v1.x << "\t"
				<< setprecision(2) << (*it)->v1.y << "\t"
				<< setprecision(2) << (*it)->v1.z << "\t"
				<< setprecision(2) << (*it)->v2.x << "\t"
				<< setprecision(2) << (*it)->v2.y << "\t"
				<< setprecision(2) << (*it)->v2.z << "\t"
				<< setprecision(0) << (*it)->leID << "\t"
				<< setprecision(2) << (*it)->leL << "\t"
				<< setprecision(2) << (*it)->pos << "\t"
				<< setprecision(2) << (*it)->chlSPA << "\t"
				<< setprecision(2) << (*it)->kLeafTransmittance << "\t"
				<< setprecision(2) << (*it)->kLeafReflectance << "\t"
				<< setprecision(2) << (*it)->nitrogenPerA << "\t"
				<< setprecision(4) << area << "\t";

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

				myfile << setprecision(2) << (*it1) * area_factor << "\t"
					<< setprecision(2) << (*it2) * area_factor << "\t"
					<< setprecision(2) << (*it3) * area_factor << "\t"
					<< setprecision(2) << (*it4) * area_factor << "\t"
					<< setprecision(2) << (*it5) * area_factor << "\t"
					<< setprecision(2) << (*it6) * area_factor << "\t";

				it2++; it3++; it4++; it5++; it6++;
			}
			myfile << "\n";
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
		myfile.close();
		if (silence == 0){
			cout << "write data to output file Finish, total " << num_triangle << " triangles" << endl;
			cout << "program End" << endl;
			cout << "==================================================" << endl << endl;
		}
	}
	else cout << "Unable to open file";
	return 0;
}

//----------------------------------------------- Import model  --------------------------------------------------
void import_data(int modeltype, char filename[], Grid* grid, double ktInput, double krInput, double start_hour, double end_hour, double hour_interval){

	//modeltype: 1, 1-9 colums are model; 2, 1-9 model, 14 and 15 are leaf transmittance and reflectance;
			//
	//	bool isCM3 = false;  // if the input model is CM3, including Kt, Kr (leaf transmittance, leaf reflectance), ture; if not, false;

	ifstream myfile (filename);
	string line;
	if (myfile.is_open())
	{
		double x1, y1, z1,
		  	   x2, y2, z2,
		  	   x3, y3, z3;
		double leafID = 0, leafL = 0, position =0, chlSPAD=0;
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
	      if (modeltype == 1){        // 1-9 colums are model;
	    	  kt = ktInput; kr = krInput;     // input from commmand line
	   // 	  cout<<"OK2"<<endl;
	      }else if (modeltype == 2){  // 1-9 model, 14 and 15 are leaf transmittance and reflectance;

	    	  istr >> leafID;istr >> leafL;istr >> position;istr >> chlSPAD;  // other contributes in model file
	    	  istr >> kt  ;istr >> kr;// input from model file
	    	  istr >> nitrogenPerArea;
	    	  kt = kt * 0.01;         // convert to 0 - 1
	    	  kr = kr * 0.01;
	      }else if (modeltype == 3){  // 1-9 model, 10-13 parameters, input kt and kr instead of model input, and 16 is nitrogen per leaf area
	    	  istr >> leafID;istr >> leafL;istr >> position;istr >> chlSPAD;  // other contributes in model file
	    	  istr >> kt  ;istr >> kr;// not use
	    	  kt = ktInput; kr = krInput;     // input from commmand line
	    	  istr >> nitrogenPerArea;
	      }
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



