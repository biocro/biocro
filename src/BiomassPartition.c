#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"


struct dbp_str sel_dbp_coef(double coefs[25], double TherPrds[6], double TherTime){

	struct dbp_str tmp;

	tmp.kLeaf = 0.0;
	tmp.kStem = 0.0;
	tmp.kRoot = 0.0;
	tmp.kRhiz = 0.0;
	tmp.kGrain = 0.0; /* kGrain is always zero except for the last thermal period */


	if(TherTime < TherPrds[0])
	{
		tmp.kLeaf = coefs[0];
		tmp.kStem = coefs[1];
		tmp.kRoot = coefs[2];
		tmp.kRhiz = coefs[3];

	} else
		if( TherTime < TherPrds[1] )
		{
			tmp.kLeaf = coefs[4];
			tmp.kStem = coefs[5];
			tmp.kRoot = coefs[6];
			tmp.kRhiz = coefs[7];

		} else
			if( TherTime < TherPrds[2])
			{
				tmp.kLeaf = coefs[8];
				tmp.kStem = coefs[9];
				tmp.kRoot = coefs[10];
				tmp.kRhiz = coefs[11];

			} else
				if(TherTime < TherPrds[3])
				{
					tmp.kLeaf = coefs[12];
					tmp.kStem = coefs[13];
					tmp.kRoot = coefs[14];
					tmp.kRhiz = coefs[15];

				} else
					if(TherTime < TherPrds[4])
					{
						tmp.kLeaf = coefs[16];
						tmp.kStem = coefs[17];
						tmp.kRoot = coefs[18];
						tmp.kRhiz = coefs[19];

					} else
						if(TherTime < TherPrds[5])
						{
							tmp.kLeaf = coefs[20];
							tmp.kStem = coefs[21];
							tmp.kRoot = coefs[22];
							tmp.kRhiz = coefs[23];
							tmp.kGrain = coefs[24];
						}else{
							Rprintf("TherPrds[5]: %.1f TherTime %.1f \n",TherPrds[5],TherTime);
							error("Thermal time larger than thermal periods");
						}


	return(tmp);

}
