#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "AuxBioCro.h"
#include "soil.h"




struct seqRD_str seqRootDepth(double to, int lengthOut){

	struct seqRD_str tmp;
	int i;
	double by;

	/* This is because in this case from is always zero */
	by = to / lengthOut;

	for(i=0;i<=lengthOut;i++){

		tmp.rootDepths[i] = i * by;

	}

	return(tmp);

}


struct rd_str rootDist(int layer, double rootDepth, double *depthsp, double rfl){

	struct rd_str tmp;  
	int i, j, k;
	double layerDepth = 0.0;
	double CumLayerDepth = 0.0;
	double CumRootDist = 1.0;
	double rootDist[layer];
	double ca = 0.0, a = 0.0;

	for(i=0;i<layer;i++){

		if(i == 0){
			layerDepth = depthsp[1];
		}else{
			layerDepth = depthsp[i] - depthsp[i-1];
		}

		CumLayerDepth += layerDepth;

		if(rootDepth > CumLayerDepth){
			CumRootDist++;
		}
	}

	for(j=0;j<layer;j++){
		if(j < CumRootDist){ 
			a = dpois(j+1,CumRootDist*rfl,0);
			rootDist[j] = a;
			ca += a;
		}else{
			rootDist[j] = 0;
		}
	}

	for(k=0;k<layer;k++){
		tmp.rootDist[k] = rootDist[k] / ca; 
	}

	return(tmp);
}
