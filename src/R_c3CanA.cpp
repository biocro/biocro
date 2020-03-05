/*
 *  BioCro/src/c3CanA.c by Fernando Ezequiel Miguez  Copyright (C) 2010
 *
 */

#include <Rinternals.h>
#include "standalone_ss.h"

extern "C" {
	SEXP c3CanA(SEXP Lai,
			SEXP DOY_DBL,
			SEXP SOLAR,
			SEXP TEMP,
			SEXP ReH,
			SEXP WINDSPEED,
			SEXP LAT,
			SEXP NLAYERS, 
			SEXP PHOTOPS, 
			SEXP KD, 
			SEXP HEIGHTF, 
			SEXP LNPS,
			SEXP CHIL,
			SEXP STOMATAWS,
			SEXP GROWTH_RESPIRATION_FRACTION,
			SEXP WATER_STRESS_APPROACH,
			SEXP ELECTRONS_PER_CARBOXYLATION,
			SEXP ELECTRONS_PER_OXYGENATION)
	{
		// Collect the inputs
		double lai = REAL(Lai)[0];
		double doy_dbl = REAL(DOY_DBL)[0];
		double solar = REAL(SOLAR)[0];
		double temp = REAL(TEMP)[0];
		double rh = REAL(ReH)[0];
		double windspeed = REAL(WINDSPEED)[0];
		double lat = REAL(LAT)[0];
		double nlayers = REAL(NLAYERS)[0];
		double vmax = REAL(PHOTOPS)[0];
		double jmax = REAL(PHOTOPS)[1];
		double Rd = REAL(PHOTOPS)[2];
		double Catm = REAL(PHOTOPS)[3];
		double O2 = REAL(PHOTOPS)[4];
		double b0 = REAL(PHOTOPS)[5];
		double b1 = REAL(PHOTOPS)[6];
		double theta = REAL(PHOTOPS)[7];
		double kd = REAL(KD)[0];
		double heightf = REAL(HEIGHTF)[0];
		double LeafN = REAL(LNPS)[0];
		double kpLN = REAL(LNPS)[1];
		double lnb0 = REAL(LNPS)[2];
		double lnb1 = REAL(LNPS)[3];
		double lnfun = REAL(LNPS)[4];
		double chil = REAL(CHIL)[0];
		double StomataWS = REAL(STOMATAWS)[0];
		double growth_respiration_fraction = REAL(GROWTH_RESPIRATION_FRACTION)[0];
		double water_stress_approach = REAL(WATER_STRESS_APPROACH)[0];
		double electrons_per_carboxylation = REAL(ELECTRONS_PER_CARBOXYLATION)[0];
		double electrons_per_oxygenation = REAL(ELECTRONS_PER_OXYGENATION)[0];
		
		// Make objects to pass back to R
		SEXP lists;
		SEXP names;
		SEXP growth;
		SEXP trans;
		SEXP Ggrowth;
		
		// Allocate space
		PROTECT(lists = Rf_allocVector(VECSXP,5));
		PROTECT(names = Rf_allocVector(STRSXP,5));
		PROTECT(growth = Rf_allocVector(REALSXP,1));
		PROTECT(trans = Rf_allocVector(REALSXP,1));
		PROTECT(Ggrowth = Rf_allocVector(REALSXP,1));
		
		// Set up a standalone version of the c3 canopy module
		
		// Make the steady state module list
		std::vector<std::string> steady_state_modules = {
			"c3_canopy"
		};
		
		// Make pointers to the input parameters
		// Note: the list of required inputs was quickly determined by using the following R command
		//  with the BioCro library loaded:
		//  get_standalone_ss_info("c3_canopy")
		double* Catm_ptr = &Catm;
		double* LeafN_ptr = &LeafN;
		double* O2_ptr = &O2;
		double* Rd_ptr = &Rd;
		double* StomataWS_ptr = &StomataWS;
		double* b0_ptr = &b0;
		double* b1_ptr = &b1;
		double* chil_ptr = &chil;
		double* doy_dbl_ptr = &doy_dbl;
		double* electrons_per_carboxylation_ptr = &electrons_per_carboxylation;
		double* electrons_per_oxygenation_ptr = &electrons_per_oxygenation;
		double* growth_respiration_fraction_ptr = &growth_respiration_fraction;
		double* heightf_ptr = &heightf;
		double* jmax_ptr = &jmax;
		double* kd_ptr = &kd;
		double* kpLN_ptr = &kpLN;
		double* lai_ptr = &lai;
		double* lat_ptr = &lat;
		double* lnb0_ptr = &lnb0;
		double* lnb1_ptr = &lnb1;
		double* lnfun_ptr = &lnfun;
		double* nlayers_ptr = &nlayers;
		double* rh_ptr = &rh;
		double* solar_ptr = &solar;
		double* temp_ptr = &temp;
		double* theta_ptr = &theta;
		double* vmax_ptr = &vmax;
		double* water_stress_approach_ptr = &water_stress_approach;
		double* windspeed_ptr = &windspeed;
		
		// Make variables and pointers for the output parameters
		// Note: the list of required outputs was quickly determined by using the following R command
		//  with the BioCro library loaded:
		//  get_standalone_ss_info("c3_canopy")
		double GrossAssim;
		double canopy_assimilation_rate;
		double canopy_transpiration_rate;
		
		double* GrossAssim_ptr = &GrossAssim;
		double* canopy_assimilation_rate_ptr = &canopy_assimilation_rate;
		double* canopy_transpiration_rate_ptr = &canopy_transpiration_rate;
		
		// Set up the input parameter list to pass to the module
		// Note: the list of required inputs was quickly determined by using the following R command
		//  with the BioCro library loaded:
		//  get_standalone_ss_info("c3_canopy")
		std::unordered_map<std::string, const double*> input_param_ptrs {
			{"Catm", Catm_ptr},
			{"LeafN", LeafN_ptr},
			{"O2", O2_ptr},
			{"Rd", Rd_ptr},
			{"StomataWS", StomataWS_ptr},
			{"b0", b0_ptr},
			{"b1", b1_ptr},
			{"chil", chil_ptr},
			{"doy_dbl", doy_dbl_ptr},
			{"electrons_per_carboxylation", electrons_per_carboxylation_ptr},
			{"electrons_per_oxygenation", electrons_per_oxygenation_ptr},
			{"growth_respiration_fraction", growth_respiration_fraction_ptr},
			{"heightf", heightf_ptr},
			{"jmax", jmax_ptr},
			{"kd", kd_ptr},
			{"kpLN", kpLN_ptr},
			{"lai", lai_ptr},
			{"lat", lat_ptr},
			{"lnb0", lnb0_ptr},
			{"lnb1", lnb1_ptr},
			{"lnfun", lnfun_ptr},
			{"nlayers", nlayers_ptr},
			{"rh", rh_ptr},
			{"solar", solar_ptr},
			{"temp", temp_ptr},
			{"theta", theta_ptr},
			{"vmax", vmax_ptr},
			{"water_stress_approach", water_stress_approach_ptr},
			{"windspeed", windspeed_ptr}
		};
		
		// Set up the output parameter list to pass to the module
		// Note: the list of required outputs was quickly determined by using the following R command
		//  with the BioCro library loaded:
		//  get_standalone_ss_info("c3_canopy")
		std::unordered_map<std::string, double*> output_param_ptrs = {
			{"GrossAssim", GrossAssim_ptr},
			{"canopy_assimilation_rate", canopy_assimilation_rate_ptr},
			{"canopy_transpiration_rate", canopy_transpiration_rate_ptr},
		};
		
		// Now that the inputs are defined, make the standalone module and store a smart pointer to it
		std::shared_ptr<Standalone_SS> c3_module = std::shared_ptr<Standalone_SS>(new Standalone_SS(steady_state_modules, input_param_ptrs, output_param_ptrs));
		
		// Run the module
		c3_module->run();
		
		// Check the outputs
		if(ISNAN(canopy_assimilation_rate)) {
            Rf_error("Something is NA \n");
        }
		
		// Format the outputs properly for R
		REAL(growth)[0] = canopy_assimilation_rate;
		REAL(trans)[0] = canopy_transpiration_rate;
		REAL(Ggrowth)[0] = GrossAssim;

		SET_VECTOR_ELT(lists, 0, growth);
		SET_VECTOR_ELT(lists, 1, trans);
		SET_VECTOR_ELT(lists, 2, Ggrowth);

		SET_STRING_ELT(names, 0, Rf_mkChar("CanopyAssim"));
		SET_STRING_ELT(names, 1, Rf_mkChar("CanopyTrans"));
		SET_STRING_ELT(names, 2, Rf_mkChar("GrossCanopyAssim"));
		Rf_setAttrib(lists,R_NamesSymbol,names);

		UNPROTECT(5);
		return(lists);
	}
}	// extern "C"