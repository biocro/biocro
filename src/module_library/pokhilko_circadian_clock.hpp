#ifndef POKHILKO_CIRCADIAN_CLOCK_H
#define POKHILKO_CIRCADIAN_CLOCK_H

#include "../modules.h"

// This module is based on the circadian clock gene network described in
//  Pokhilko, A. et al. The clock gene circuit in Arabidopsis includes a repressilator with additional feedback loops. Molecular Systems Biology 8, 574 (2012).
// Translation into c++ for BioCro was based on the code in "circadian_module.m", part of the "Arabadopsis Framework v2" model described in
//  Chew, Y. H. et al. Linking circadian time to growth rate quantitatively via carbon metabolism. bioRxiv 105437 (2017). doi:10.1101/105437
//  and available online from https://github.com/danielseaton/frameworkmodel
// Values for parameters were taken from the SBML model available in the supplemental information of the 2012 paper

class pokhilko_circadian_clock : public DerivModule {
	public:
		pokhilko_circadian_clock(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("pokhilko_circadian_clock"),
			// Get pointers to input parameters
			solar_ip(get_ip(input_parameters, "solar")),
			LHY_mRNA_ip(get_ip(input_parameters, "LHY_mRNA")),
			P_ip(get_ip(input_parameters, "P")),
			GI_ZTL_ip(get_ip(input_parameters, "GI_ZTL")),
			GI_ELF3_cytoplasm_ip(get_ip(input_parameters, "GI_ELF3_cytoplasm")),
			LHY_prot_ip(get_ip(input_parameters, "LHY_prot")),
			TOC1_mRNA_ip(get_ip(input_parameters, "TOC1_mRNA")),
			PRR9_prot_ip(get_ip(input_parameters, "PRR9_prot")),
			PRR5_NI_mRNA_ip(get_ip(input_parameters, "PRR5_NI_mRNA")),
			PRR5_NI_prot_ip(get_ip(input_parameters, "PRR5_NI_prot")),
			GI_prot_cytoplasm_ip(get_ip(input_parameters, "GI_prot_cytoplasm")),
			TOC1_prot_ip(get_ip(input_parameters, "TOC1_prot")),
			ZTL_ip(get_ip(input_parameters, "ZTL")),
			EC_ip(get_ip(input_parameters, "EC")),
			GI_mRNA_ip(get_ip(input_parameters, "GI_mRNA")),
			PRR9_mRNA_ip(get_ip(input_parameters, "PRR9_mRNA")),
			PRR7_mRNA_ip(get_ip(input_parameters, "PRR7_mRNA")),
			PRR7_prot_ip(get_ip(input_parameters, "PRR7_prot")),
			ELF4_mRNA_ip(get_ip(input_parameters, "ELF4_mRNA")),
			ELF4_prot_ip(get_ip(input_parameters, "ELF4_prot")),
			LHY_prot_modif_ip(get_ip(input_parameters, "LHY_prot_modif")),
			HY5_ip(get_ip(input_parameters, "HY5")),
			HFR1_ip(get_ip(input_parameters, "HFR1")),
			ELF3_mRNA_ip(get_ip(input_parameters, "ELF3_mRNA")),
			ELF3_cytoplasm_ip(get_ip(input_parameters, "ELF3_cytoplasm")),
			ELF3_nuclear_ip(get_ip(input_parameters, "ELF3_nuclear")),
			COP1_nuclear_night_ip(get_ip(input_parameters, "COP1_nuclear_night")),
			COP1_nuclear_day_ip(get_ip(input_parameters, "COP1_nuclear_day")),
			LUX_mRNA_ip(get_ip(input_parameters, "LUX_mRNA")),
			LUX_prot_ip(get_ip(input_parameters, "LUX_prot")),
			COP1_cytoplasm_ip(get_ip(input_parameters, "COP1_cytoplasm")),
			// Get pointers to output parameters
			LHY_mRNA_op(get_op(output_parameters, "LHY_mRNA")),
			P_op(get_op(output_parameters, "P")),
			GI_ZTL_op(get_op(output_parameters, "GI_ZTL")),
			GI_ELF3_cytoplasm_op(get_op(output_parameters, "GI_ELF3_cytoplasm")),
			LHY_prot_op(get_op(output_parameters, "LHY_prot")),
			TOC1_mRNA_op(get_op(output_parameters, "TOC1_mRNA")),
			PRR9_prot_op(get_op(output_parameters, "PRR9_prot")),
			PRR5_NI_mRNA_op(get_op(output_parameters, "PRR5_NI_mRNA")),
			PRR5_NI_prot_op(get_op(output_parameters, "PRR5_NI_prot")),
			GI_prot_cytoplasm_op(get_op(output_parameters, "GI_prot_cytoplasm")),
			TOC1_prot_op(get_op(output_parameters, "TOC1_prot")),
			ZTL_op(get_op(output_parameters, "ZTL")),
			EC_op(get_op(output_parameters, "EC")),
			GI_mRNA_op(get_op(output_parameters, "GI_mRNA")),
			PRR9_mRNA_op(get_op(output_parameters, "PRR9_mRNA")),
			PRR7_mRNA_op(get_op(output_parameters, "PRR7_mRNA")),
			PRR7_prot_op(get_op(output_parameters, "PRR7_prot")),
			ELF4_mRNA_op(get_op(output_parameters, "ELF4_mRNA")),
			ELF4_prot_op(get_op(output_parameters, "ELF4_prot")),
			LHY_prot_modif_op(get_op(output_parameters, "LHY_prot_modif")),
			HY5_op(get_op(output_parameters, "HY5")),
			HFR1_op(get_op(output_parameters, "HFR1")),
			ELF3_mRNA_op(get_op(output_parameters, "ELF3_mRNA")),
			ELF3_cytoplasm_op(get_op(output_parameters, "ELF3_cytoplasm")),
			ELF3_nuclear_op(get_op(output_parameters, "ELF3_nuclear")),
			COP1_nuclear_night_op(get_op(output_parameters, "COP1_nuclear_night")),
			COP1_nuclear_day_op(get_op(output_parameters, "COP1_nuclear_day")),
			LUX_mRNA_op(get_op(output_parameters, "LUX_mRNA")),
			LUX_prot_op(get_op(output_parameters, "LUX_prot")),
			COP1_cytoplasm_op(get_op(output_parameters, "COP1_cytoplasm"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* solar_ip;
		const double* LHY_mRNA_ip;
		const double* P_ip;
		const double* GI_ZTL_ip;
		const double* GI_ELF3_cytoplasm_ip;
		const double* LHY_prot_ip;
		const double* TOC1_mRNA_ip;
		const double* PRR9_prot_ip;
		const double* PRR5_NI_mRNA_ip;
		const double* PRR5_NI_prot_ip;
		const double* GI_prot_cytoplasm_ip;
		const double* TOC1_prot_ip;
		const double* ZTL_ip;
		const double* EC_ip;
		const double* GI_mRNA_ip;
		const double* PRR9_mRNA_ip;
		const double* PRR7_mRNA_ip;
		const double* PRR7_prot_ip;
		const double* ELF4_mRNA_ip;
		const double* ELF4_prot_ip;
		const double* LHY_prot_modif_ip;
		const double* HY5_ip;
		const double* HFR1_ip;
		const double* ELF3_mRNA_ip;
		const double* ELF3_cytoplasm_ip;
		const double* ELF3_nuclear_ip;
		const double* COP1_nuclear_night_ip;
		const double* COP1_nuclear_day_ip;
		const double* LUX_mRNA_ip;
		const double* LUX_prot_ip;
		const double* COP1_cytoplasm_ip;
		// Pointers to output parameters
		double* LHY_mRNA_op;
		double* P_op;
		double* GI_ZTL_op;
		double* GI_ELF3_cytoplasm_op;
		double* LHY_prot_op;
		double* TOC1_mRNA_op;
		double* PRR9_prot_op;
		double* PRR5_NI_mRNA_op;
		double* PRR5_NI_prot_op;
		double* GI_prot_cytoplasm_op;
		double* TOC1_prot_op;
		double* ZTL_op;
		double* EC_op;
		double* GI_mRNA_op;
		double* PRR9_mRNA_op;
		double* PRR7_mRNA_op;
		double* PRR7_prot_op;
		double* ELF4_mRNA_op;
		double* ELF4_prot_op;
		double* LHY_prot_modif_op;
		double* HY5_op;
		double* HFR1_op;
		double* ELF3_mRNA_op;
		double* ELF3_cytoplasm_op;
		double* ELF3_nuclear_op;
		double* COP1_nuclear_night_op;
		double* COP1_nuclear_day_op;
		double* LUX_mRNA_op;
		double* LUX_prot_op;
		double* COP1_cytoplasm_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> pokhilko_circadian_clock::get_inputs() {
	return {
		"solar",
		"LHY_mRNA",
		"P",
		"GI_ZTL",
		"GI_ELF3_cytoplasm",
		"LHY_prot",
		"TOC1_mRNA",
		"PRR9_prot",
		"PRR5_NI_mRNA",
		"PRR5_NI_prot",
		"GI_prot_cytoplasm",
		"TOC1_prot",
		"ZTL",
		"EC",
		"GI_mRNA",
		"PRR9_mRNA",
		"PRR7_mRNA",
		"PRR7_prot",
		"ELF4_mRNA",
		"ELF4_prot",
		"LHY_prot_modif",
		"HY5",
		"HFR1",
		"ELF3_mRNA",
		"ELF3_cytoplasm",
		"ELF3_nuclear",
		"COP1_nuclear_night",
		"COP1_nuclear_day",
		"LUX_mRNA",
		"LUX_prot",
		"COP1_cytoplasm"
	};
}

std::vector<std::string> pokhilko_circadian_clock::get_outputs() {
	return {
		"LHY_mRNA",
		"P",
		"GI_ZTL",
		"GI_ELF3_cytoplasm",
		"LHY_prot",
		"TOC1_mRNA",
		"PRR9_prot",
		"PRR5_NI_mRNA",
		"PRR5_NI_prot",
		"GI_prot_cytoplasm",
		"TOC1_prot",
		"ZTL",
		"EC",
		"GI_mRNA",
		"PRR9_mRNA",
		"PRR7_mRNA",
		"PRR7_prot",
		"ELF4_mRNA",
		"ELF4_prot",
		"LHY_prot_modif",
		"HY5",
		"HFR1",
		"ELF3_mRNA",
		"ELF3_cytoplasm",
		"ELF3_nuclear",
		"COP1_nuclear_night",
		"COP1_nuclear_day",
		"LUX_mRNA",
		"LUX_prot",
		"COP1_cytoplasm"
	};
}

void pokhilko_circadian_clock::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Unpack the solar radiantion
	double solar = *solar_ip;
	
	// Unpack the circadian clock components
	double LHY_mRNA = *LHY_mRNA_ip;
	double P = *P_ip;
	double GI_ZTL = *GI_ZTL_ip;
	double GI_ELF3_cytoplasm = *GI_ELF3_cytoplasm_ip;
	double LHY_prot = *LHY_prot_ip;
	double TOC1_mRNA = *TOC1_mRNA_ip;
	double PRR9_prot = *PRR9_prot_ip;
	double PRR5_NI_mRNA = *PRR5_NI_mRNA_ip;
	double PRR5_NI_prot = *PRR5_NI_prot_ip;
	double GI_prot_cytoplasm = *GI_prot_cytoplasm_ip;
	double TOC1_prot = *TOC1_prot_ip;
	double ZTL = *ZTL_ip;
	double EC = *EC_ip;
	double GI_mRNA = *GI_mRNA_ip;
	double PRR9_mRNA = *PRR9_mRNA_ip;
	double PRR7_mRNA = *PRR7_mRNA_ip;
	double PRR7_prot = *PRR7_prot_ip;
	double ELF4_mRNA = *ELF4_mRNA_ip;
	double ELF4_prot = *ELF4_prot_ip;
	double LHY_prot_modif = *LHY_prot_modif_ip;
	double HY5 = *HY5_ip;
	double HFR1 = *HFR1_ip;
	double ELF3_mRNA = *ELF3_mRNA_ip;
	double ELF3_cytoplasm = *ELF3_cytoplasm_ip;
	double ELF3_nuclear = *ELF3_nuclear_ip;
	double COP1_nuclear_night = *COP1_nuclear_night_ip;
	double COP1_nuclear_day = *COP1_nuclear_day_ip;
	double LUX_mRNA = *LUX_mRNA_ip;
	double LUX_prot = *LUX_prot_ip;
	double COP1_cytoplasm = *COP1_cytoplasm_ip;
	
	// Define the circadian clock parameters
	double n1 = 2.6;
	double n2 = 0.64;
	double n3 = 0.29;
	double n4 = 0.07;
	double n5 = 0.23;
	double n6 = 20;
	double n7 = 0.2;
	double n8 = 0.5;
	double n9 = 0.2;
	double n10 = 0.4;
	double n11 = 0.6;
	double n12 = 12.5;
	double n13 = 1.3;
	double n14 = 0.1;
	double g1 = 0.1;
	double g2 = 0.01;
	double g3 = 0.6;
	double g4 = 0.01;
	double g5 = 0.15;
	double g6 = 0.3;
	double g7 = 0.6;
	double g8 = 0.01;
	double g9 = 0.3;
	double g10 = 0.5;
	double g11 = 0.7;
	double g12 = 0.2;
	double g13 = 1;
	double g14 = 0.004;
	double g15 = 0.4;
	double g16 = 0.3;
	double m1 = 0.54;
	double m2 = 0.24;
	double m3 = 0.2;
	double m4 = 0.2;
	double m5 = 0.3;
	double m6 = 0.3;
	double m7 = 0.7;
	double m8 = 0.4;
	double m9 = 1.1;
	double m10 = 1;
	double m11 = 1;
	double m12 = 1;
	double m13 = 0.32;
	double m14 = 0.4;
	double m15 = 0.7;
	double m16 = 0.5;
	double m17 = 0.5;
	double m18 = 3.4;
	double m19 = 0.2;
	double m20 = 0.6;
	double m21 = 0.08;
	double m22 = 0.1;
	double m23 = 1.8;
	double m24 = 0.1;
	double m25 = 1.8;
	double m26 = 0.5;
	double m27 = 0.1;
	double m28 = 20;
	double m29 = 5;
	double m30 = 3;
	double m31 = 0.3;
	double m32 = 0.2;
	double m33 = 13;
	double m34 = 0.6;
	double m35 = 0.3;
	double m36 = 0.1;
	double m37 = 0.8;
	double m38 = 0.5;
	double m39 = 0.3;
	int a = 2;
	int b = 2;
	int c = 2;
	int d = 2;
	int e = 2;
	int f = 2;
	double p1 = 0.13;
	double p2 = 0.27;
	double p3 = 0.1;
	double p4 = 0.56;
	double p5 = 4;
	double p6 = 0.6;
	double p7 = 0.3;
	double p8 = 0.6;
	double p9 = 0.8;
	double p10 = 0.54;
	double p11 = 0.51;
	double p12 = 3.4;
	double p13 = 0.1;
	double p14 = 0.14;
	double p15 = 3;
	double p16 = 0.62;
	double p17 = 4.8;
	double p18 = 4;
	double p19 = 1;
	double p20 = 0.1;
	double p21 = 1;
	double p22 = 0.5;
	double p23 = 0.37;
	double p24 = 10;
	double p25 = 8;
	double p26 = 0.3;
	double p27 = 0.8;
	double p28 = 2;
	double p29 = 0.1;
	double p30 = 0.9;
	double p31 = 0.1;
	double q1 = 1.2;
	double q2 = 1.56;
	double q3 = 2.8;
	
	// Check whether the plant is illuminated
	// Rather than basing L(t) on specified values for dusk and dawn
	//  as in the original Pokhilko model, we use a logistic function
	//  based on solar radiation to determine when light is present.
	// This function is 0 for no sunlight and saturates to 1 when
	//  solar reaches ~150, which is well below the max on a typical day
	double L = 1.0 / (1.0 + exp(-0.058*(solar - 100.0)));
	
	// Calculate some parameters defined in the Pokhilko model
    double Gn = p28 * GI_prot_cytoplasm / (p29 + m19 + p17 * ELF3_nuclear);
    double EGn = (p18 * GI_ELF3_cytoplasm + p17 * ELF3_nuclear * Gn) / (m9 * COP1_nuclear_night + m10 * COP1_nuclear_day + p31);
    double e34 = p25 * ELF4_prot * ELF3_nuclear / (p26 * LUX_prot + p21 + m36 * COP1_nuclear_night + m37 * COP1_nuclear_day);
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
    update(LHY_mRNA_op, 1.0 * (q1 * L * P + n1 * pow(g1, a) / (pow(g1, a) + pow(PRR9_prot + PRR7_prot + PRR5_NI_prot + TOC1_prot, a))) - LHY_mRNA * (m1 * L + m2 * (1.0 - L)));
    update(P_op, p7 * (1.0 - L) * (1.0 - P) - m11 * P * L);
    update(GI_ZTL_op, p12 * L * ZTL * GI_prot_cytoplasm - p13 * GI_ZTL * (1.0 - L) - m21 * GI_ZTL);
    update(GI_ELF3_cytoplasm_op, p17 * ELF3_cytoplasm * GI_prot_cytoplasm - m9 * GI_ELF3_cytoplasm * COP1_cytoplasm - p18 * GI_ELF3_cytoplasm + p31 * EGn);
    update(LHY_prot_op, (p2 + p1 * L) * LHY_mRNA - m3 * LHY_prot - p3 * pow(LHY_prot, c) / (pow(LHY_prot, c) + pow(g3, c)));
    update(TOC1_mRNA_op, 1.0 * n2 * g4 / (g4 + EC) * (pow(g5, e) / (pow(g5, e) + pow(LHY_prot, e))) - TOC1_mRNA * m5);
    update(PRR9_prot_op, p8 * PRR9_mRNA - (m13 + m22 * (1.0 - L)) * PRR9_prot);
    update(PRR5_NI_mRNA_op, 1.0 * (n10 * pow(LHY_prot_modif, e) / (pow(g12, e) + pow(LHY_prot_modif, e)) + n11 * pow(PRR7_prot, b) / (pow(g13, b) + pow(PRR7_prot, b))) - m16 * PRR5_NI_mRNA);
    update(PRR5_NI_prot_op, p10 * PRR5_NI_mRNA - (m17 + m24 * (1.0 - L)) * PRR5_NI_prot);
    update(GI_prot_cytoplasm_op, p11 * GI_mRNA - m19 * GI_prot_cytoplasm - p12 * L * ZTL * GI_prot_cytoplasm + p13 * GI_ZTL * (1.0 - L) - p17 * ELF3_cytoplasm * GI_prot_cytoplasm - p28 * GI_prot_cytoplasm + p29 * Gn);
    update(TOC1_prot_op, p4 * TOC1_mRNA - m8 * TOC1_prot - (m6 + m7 * (1.0 - L)) * TOC1_prot * (p5 * ZTL + GI_ZTL));
    update(ZTL_op, 1.0 * p14 - m20 * ZTL - p12 * L * ZTL * GI_prot_cytoplasm + p13 * GI_ZTL * (1.0 - L));
    update(EC_op, p26 * LUX_prot * e34 - m36 * EC * COP1_nuclear_night - m37 * EC * COP1_nuclear_day - m32 * EC * (1.0 + p24 * L * pow(EGn + Gn, d) / (pow(g7, d) + pow(EGn + Gn, d))));
    update(GI_mRNA_op, 1.0 * (q2 * L * P + pow(g15, e) / (pow(g15, e) + pow(LHY_prot, e)) * g14 / (g14 + EC) * n12) - GI_mRNA * m18);
    update(PRR9_mRNA_op, 1.0 * q3 * L * P + g8 / (g8 + EC) * (n4 + n7 * pow(LHY_prot, e) / (pow(LHY_prot, e) + pow(g9, e))) - m12 * PRR9_mRNA);
    update(PRR7_mRNA_op, 1.0 * (n8 * pow(LHY_prot + LHY_prot_modif, e) / (pow(g10, e) + pow(LHY_prot + LHY_prot_modif, e)) + n9 * pow(PRR9_prot, f) / (pow(g11, f) + pow(PRR9_prot, f))) - m14 * PRR7_mRNA);
    update(PRR7_prot_op, p9 * PRR7_mRNA - PRR7_prot * (m15 + m23 * (1.0 - L)));
    update(ELF4_mRNA_op, n13 * (pow(g6, e) / (pow(g6, e) + pow(LHY_prot, e))) * g2 / (g2 + EC) - ELF4_mRNA * m34);
    update(ELF4_prot_op, p23 * ELF4_mRNA - m35 * ELF4_prot - p25 * ELF3_nuclear * ELF4_prot + p21 * e34);
    update(LHY_prot_modif_op, p3 * pow(LHY_prot, c) / (pow(LHY_prot, c) + pow(g3, c)) - m4 * LHY_prot_modif);
    update(HY5_op, p22 - m38 * HY5 * COP1_nuclear_day - m25 * HY5 * COP1_nuclear_night);
    update(HFR1_op, p30 - m28 * HFR1 * COP1_nuclear_night);
    update(ELF3_mRNA_op, 1.0 * n3 * pow(g16, e) / (pow(g16, e) + pow(LHY_prot, e)) - m26 * ELF3_mRNA);
    update(ELF3_cytoplasm_op, p16 * ELF3_mRNA - m9 * ELF3_cytoplasm * COP1_cytoplasm - p17 * ELF3_cytoplasm * GI_prot_cytoplasm - p19 * ELF3_cytoplasm + p20 * ELF3_nuclear);
    update(ELF3_nuclear_op, p19 * ELF3_cytoplasm - p20 * ELF3_nuclear - m29 * ELF3_nuclear * COP1_nuclear_night - m30 * ELF3_nuclear * COP1_nuclear_day - p25 * ELF3_nuclear * ELF4_prot + p21 * e34 - p17 * ELF3_nuclear * Gn);
    update(COP1_nuclear_night_op, p6 * COP1_cytoplasm - n6 * L * P * COP1_nuclear_night - n14 * COP1_nuclear_night - m27 * COP1_nuclear_night * (1.0 + p15 * L));
    update(COP1_nuclear_day_op, 1.0 * (n14 * COP1_nuclear_night + n6 * L * P * COP1_nuclear_night) - m31 * (1.0 + m33 * (1.0 - L)) * COP1_nuclear_day);
    update(LUX_mRNA_op, n13 * (pow(g6, e) / (pow(g6, e) + pow(LHY_prot, e))) * g2 / (g2 + EC) - LUX_mRNA * m34);
    update(LUX_prot_op, p27 * LUX_mRNA - m39 * LUX_prot - p26 * LUX_prot * e34);
    update(COP1_cytoplasm_op, 1.0 * n5 - p6 * COP1_cytoplasm - m27 * COP1_cytoplasm * (1.0 + p15 * L));
}

#endif
