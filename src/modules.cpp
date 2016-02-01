

#include "modules.h"

vector<string> IModule::list_required_state() {
	return(this->_required_state);
}

vector<string> IModule::list_modified_state() {
	return(this->_modified_state);
}

map<string, double> IModule::operator()(map<string, double> state) {
	map<string, double> result;
	result = this->do_operation(state);
	return(result);
}

map<string, double> c4_canopy::do_operation(map<string, double> s) {
	struct Can_Str result;
	struct nitroParms nitroP; 
    map<string, double> new_state;

	nitroP.ileafN = s["ileafN"];
	nitroP.kln = s["kln"];
	nitroP.Vmaxb1 = s["Vmaxb1"];
	nitroP.Vmaxb0 = s["Vmaxb0"];
	nitroP.alphab1 = s["alphab1"];
	nitroP.alphab0 = s["alphab0"];
	nitroP.Rdb1 = s["Rdb1"];
	nitroP.Rdb0 = s["Rdb0"];
	nitroP.kpLN = s["kpLN"];
	nitroP.lnb0 = s["lnb0"];
	nitroP.lnb1 = s["lnb1"];
	nitroP.lnFun = (int)s["lnFun"];
	nitroP.maxln = s["maxln"];
	nitroP.minln = s["minln"];
	nitroP.daymaxln = s["daymaxln"];

	result = CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"), s.at("rh"), s.at("windspeed"),
			s.at("lat"), (int)s.at("nlayers"), s.at("vmax"), s.at("alpha"), s.at("kparm"),
			s.at("beta"), s.at("Rd"), s.at("Catm"), s.at("b0"), s.at("b1"), s.at("theta"),
			s.at("kd"), s.at("chil"), s.at("heightf"), s.at("LeafN"), s.at("kpLN"), s.at("lnb0"),
			s.at("lnb1"), (int)s.at("lnfun"), s.at("upperT"), s.at("lowerT"), nitroP, s.at("leafwidth"),
			(int)s.at("et_equation"), s.at("StomataWS"), (int)s.at("ws"));

    new_state["Assim"] = result.Assim;
    new_state["Trans"] = result.Trans;
    new_state["GrossAssim"] = result.GrossAssim;

	return(new_state);
}

