#include <cmath>                             // for pow, sqrt, std::abs
#include <algorithm>                         // for std::min
#include <limits>
#include "ball_berry_gs.h"                   // for ball_berry_gs
#include "FvCB_assim.h"                      // for FvCB_assim
//#include "conductance_limited_assim.h"       // for conductance_limited_assim

#include <Rcpp.h>
using namespace Rcpp;


double ci_fun(double Ci, double Ca, double gbw){
  FvCB_outputs FvCB_res = FvCB_assim(
    Ci,
    /* Gstar */ 38.6,
    /* J */ 170,
    /* Kc */ 259,
    /* Ko */ 179,
    /* Oi*/ 200,
    /*Rd*/ 1,
    /*TPU*/ 11 ,
    /*Vcmax*/ 100,
    /* alpha_TPU */ 0,
    /* electrons_per_carboxylation */ 4,
    /* electrons_per_oxygenation  */ 4);
  
  stomata_outputs BB_res = ball_berry_gs(
    FvCB_res.An * 1e-6,
    Ca * 1e-6,
    /* RH */ 0.3 ,
    0.008,
    10.6,
    0.005,
    25,
    25);
  
  double g_total = 1/(1.37 / gbw + 1.6 / BB_res.gsw);
  return g_total/1.6 * ( Ca - Ci) - FvCB_res.An;
}



// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

// [[Rcpp::export]]
NumericVector cbal(NumericVector x, double Ca, double gbw = 0.01) {
  NumericVector out(x.size());
  for (int i = 0; i < x.size(); ++i){
    out[i] = ci_fun(x[i], Ca, gbw);
  }
  return out;
}