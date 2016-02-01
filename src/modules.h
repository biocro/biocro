#ifndef MODULES_H
#define MODULES_H

#include <map>
#include <string>
#include <vector>
#include "BioCro.h"
#include "c3photo.h"

using std::vector;
using std::string;
using std::map;

class IModule {
    public:
        IModule(const vector<string> required_state, const vector<string> modified_state) :
            _required_state(required_state),
            _modified_state(modified_state)
    {}
        vector<string> list_required_state();
        vector<string> list_modified_state();
        map<string, double> operator() (map<string, double> state);
        bool state_requirements_are_met();
        virtual ~IModule() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class.
    private:
        const vector<string> _required_state;
        const vector<string> _modified_state; 
        virtual map<string, double> do_operation(map<string, double> state) = 0;
        bool requirements_are_met;
};

inline IModule::~IModule() {};  // A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header.

class Leaf_photosynthesis_module : public IModule {
    public:
        Leaf_photosynthesis_module(const vector<string> required_state, const vector<string> modified_state)
            : IModule(required_state, modified_state)
        {}
        virtual struct c3_str assimilation(struct Model_state) = 0;
};

class c3_leaf : public Leaf_photosynthesis_module {
    public:
        c3_leaf()
            : Leaf_photosynthesis_module(vector<string> {"Qp", "Tleaf"} , vector<string> {})
        {} 
        double operator() (map<string, double> s);
        struct c3_str assimilation(map<string, double> s);
};


struct c3_str c3_leaf::assimilation(map<string, double> s) {
    struct c3_str result = {0, 0, 0, 0};
    result = c3photoC(s.at("Qp"), s.at("Tleaf"), s.at("RH"), s.at("Vcmax0"), s.at("Jmax"), s.at("Rd0"), s.at("bb0"), s.at("bb1"), s.at("Ca"), s.at("O2"), s.at("thet"), s.at("StomWS"), s.at("ws"));
    return(result);
}


class Canopy_photosynthesis_module : public IModule {
    public:
        Canopy_photosynthesis_module(const vector<string> required_state, const vector<string> modified_state)
            : IModule(required_state, modified_state)
        {}
};

class c4_canopy : public IModule {
    public:
        c4_canopy()
            : IModule(vector<string> {}, vector<string> {})
        {}
    private:
        virtual map<string, double> do_operation (map<string, double> s);
};

class c3_canopy : public IModule {
    public:
        c4_canopy()
            : IModule(vector<string> {}, vector<string> {})
        {}
    private:
        virtual map<string, double> do_operation (map<string, double> s);
};

/* Testing code
    struct Model_state mystate;
    map<string, double> myparameters;

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

	map<string, double> parms;

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

