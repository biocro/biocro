#ifndef MODULES_H
#define MODULES_H

#include <map>
#include <string>
#include "BioCro.h"
#include "c3photo.h"

class Module {
    public:
        Module(std::map<std::string, double> parameters)
            : p(parameters)
        {}
        virtual ~Module() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class.
    protected:
        std::map<std::string, double> p;
};

inline Module::~Module() {};  // A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header.

class Leaf_photosynthesis_module : public Module {
    public:
        Leaf_photosynthesis_module(std::map<std::string, double> parameters)
            : Module(parameters)
        {}
        virtual struct c3_str assimilation(struct Model_state) = 0;
};

class c3_leaf : public Leaf_photosynthesis_module {
    public:
       c3_leaf(std::map<std::string, double> parameters)
          : Leaf_photosynthesis_module(parameters)
       {} 
       double operator() (struct Model_state);
       struct c3_str assimilation(struct Model_state s);
};

struct c3_str c3_leaf::assimilation(struct Model_state s) {
    struct c3_str result = {0, 0, 0, 0};
    result = c3photoC(p.at("Qp"), p.at("Tleaf"), p.at("RH"), p.at("Vcmax0"), p.at("Jmax"), p.at("Rd0"), p.at("bb0"), p.at("bb1"), p.at("Ca"), p.at("O2"), p.at("thet"), p.at("StomWS"), p.at("ws"));
    return(result);
}

class Canopy_photosynthesis_model : public Module {
    public:
        Canopy_photosynthesis_model(std::map<std::string, double> parameters)
            : Module(parameters)
        {}
        virtual struct Can_Str assimilation(struct Model_state) = 0;
};

class c4_canopy : public Canopy_photosynthesis_model {
    public:
        c4_canopy(std::map<std::string, double> parameters)
            : Canopy_photosynthesis_model(parameters)
        {}
        struct Can_Str assimilation(struct Model_state);
};

struct Can_Str c4_canopy::assimilation(struct Model_state s) {
    struct Can_Str result;
    struct nitroParms nitroP; 

nitroP.ileafN = p["ileafN"];
nitroP.kln = p["kln"];
nitroP.Vmaxb1 = p["Vmaxb1"];
nitroP.Vmaxb0 = p["Vmaxb0"];
nitroP.alphab1 = p["alphab1"];
nitroP.alphab0 = p["alphab0"];
nitroP.Rdb1 = p["Rdb1"];
nitroP.Rdb0 = p["Rdb0"];
nitroP.kpLN = p["kpLN"];
nitroP.lnb0 = p["lnb0"];
nitroP.lnb1 = p["lnb1"];
nitroP.lnFun = (int)p["lnFun"];
nitroP.maxln = p["maxln"];
nitroP.minln = p["minln"];
nitroP.daymaxln = p["daymaxln"];

    result = CanAC(s.lai, s.doy, s.hour, s.solar, s.temp, s.rh, s.windspeed,
            p.at("lat"), (int)p.at("nlayers"), s.vmax,s.alpha, p.at("kparm"),
            p.at("beta"), p.at("Rd"), p.at("Catm"), p.at("b0"), p.at("b1"), p.at("theta"),
            p.at("kd"), p.at("chil"), p.at("heightf"), s.LeafN, p.at("kpLN"), p.at("lnb0"),
            p.at("lnb1"), (int)p.at("lnfun"), p.at("upperT"), p.at("lowerT"), nitroP, p.at("leafwidth"),
            (int)p.at("et_equation"), s.StomataWS, (int)p.at("ws"));
    return(result);
}








class Canopy_assimilation : public Module {
    public:
       Canopy_assimilation(std::map<std::string, double> parameters)
          : Module(parameters)
       {} 
       double operator() (struct Model_state);
};

double Canopy_assimilation::operator() (struct Model_state) {
    return (p["vcmax"]);
}


/* Testing code
    struct Model_state mystate;
    std::map<std::string, double> myparameters;

myparameters["Qp"] = 1000;
myparameters["Tleaf"] = 25;
myparameters["RH"] = 80;
myparameters["Vcmax0"] = 100;
myparameters["Jmax"] = 180;
myparameters["Rd0"] = 1.1;
myparameters["bb0"] = 0.08;
myparameters["bb1"] = 5;
myparameters["Ca"] = 380;
myparameters["O2"] = 210;
myparameters["thet"] = 0.7;
myparameters["StomWS"] = 1;
myparameters["ws"] = 0;

    Canopy_assimilation can(myparameters);
    c3_leaf myphoto(myparameters);
    c3_canopy myc3can(myparameters);
    Rprintf("can: %.2f, myphoto: %.2f\n", can(mystate), myc3can.assimilation(mystate).Assim);

	std::map<std::string, double> parms;

	parms["lat"] = lat;
	parms["nlayers"] = nlayers;
	parms["kparm"] = kparm;
	parms["beta"] = beta;
	parms["Rd"] = Rd;
	parms["Catm"] = Catm;
	parms["b0"] = b0;
	parms["b1"] = b1;
	parms["theta"] = theta;
	parms["kd"] = kd;
	parms["chil"] = chil;
	parms["heightf"] = heightf;
	parms["kpLN"] = kpLN;
	parms["lnb0"] = lnb0;
	parms["lnb1"] = lnb1;
	parms["lnfun"] = lnfun;
	parms["upperT"] = upperT;
	parms["lowerT"] = lowerT;

	parms["ileafN"] = nitroP.ileafN;
	parms["kln"] = nitroP.kln;
	parms["Vmaxb1"] = nitroP.Vmaxb1;
	parms["Vmaxb0"] = nitroP.Vmaxb0;
	parms["alphab1"] = nitroP.alphab1;
	parms["alphab0"] = nitroP.alphab0;
	parms["Rdb1"] = nitroP.Rdb1;
	parms["Rdb0"] = nitroP.Rdb0;
	parms["kpLN"] = nitroP.kpLN;
	parms["lnb0"] = nitroP.lnb0;
	parms["lnb1"] = nitroP.lnb1;
	parms["lnFun"] = nitroP.lnFun;
	parms["maxln"] = nitroP.maxln;
	parms["minln"] = nitroP.minln;
	parms["daymaxln"] = nitroP.daymaxln;


	parms["leafwidth"] = leafwidth;
	parms["et_equation"] = et_equation;
	parms["ws"] = ws;

	c4_canopy mycanopy(parms);

		struct Can_Str temp_canopy;
		temp_canopy = mycanopy.assimilation(current_state);

		if ( temp_canopy.Assim != Canopy.Assim || temp_canopy.Trans != Canopy.Trans || temp_canopy.GrossAssim != Canopy.GrossAssim)
			Rprintf("%.2f, %.2f, %.2f\n%.2f, %.2f, %.2f\n\n", Canopy.Assim, Canopy.Trans, Canopy.GrossAssim, temp_canopy.Assim, temp_canopy.Trans, temp_canopy.GrossAssim);

    */

# endif

