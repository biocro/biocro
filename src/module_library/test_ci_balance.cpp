#include <cmath>                             // for pow, sqrt, std::abs
#include <algorithm>                         // for std::min
#include <limits>
#include "ball_berry_gs.h"                   // for ball_berry_gs
#include "FvCB_assim.h"                      // for FvCB_assim
#include "secant_method.h"
#include "../framework/constants.h"          // for dr_stomata, dr_boundary
//#include "conductance_limited_assim.h"       // for conductance_limited_assim
using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

// #include <Rcpp.h>
// using namespace Rcpp;



double check_assim_rate(double assim, double Ca, double gbw, double RH, double J){
  stomata_outputs BB_res = ball_berry_gs(
    assim * 1e-6,
    Ca * 1e-6,
    /* RH */ RH,
    0.008,
    10.6,
    gbw,
    25,
    25);
  
  double Gs = BB_res.gsw;  // mol / m^2 / s
  
  // Using the value of stomatal conductance,
  // Calculate Ci using the total conductance across the boundary layer
  // and stomata
  double Ci = Ca - assim *
    (dr_boundary / gbw + dr_stomata / Gs);  // micromol / mol
  
  
 FvCB_outputs FvCB_res = FvCB_assim(
    Ci,
    /* Gstar */ 38.6,
    /* J */ J,
    /* Kc */ 259,
    /* Ko */ 179,
    /* Oi*/ 200,
    /*Rd*/ 1,
    /*TPU*/ 11 ,
    /*Vcmax*/ 100,
    /* alpha_TPU */ 0,
    /* electrons_per_carboxylation */ 4,
    /* electrons_per_oxygenation  */ 4);
  
 
  return FvCB_res.An - assim;
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

// // [[Rcpp::export]]
// NumericVector check_assim(NumericVector x,double Ca, double gbw=0.2, double RH=0.3, double J = 170) {
//   NumericVector out(x.size());
//   for (int i = 0; i < x.size(); ++i){
//     out[i] = check_assim_rate(x[i], Ca, gbw, RH, J);
//   }
//   return out;
// }
// 
// // [[Rcpp::export]]
// double assim_ub(double Ca,double J){
//   return FvCB_assim(
//     Ca,
//     /* Gstar */ 38.6,
//     /* J */ J,
//     /* Kc */ 259,
//     /* Ko */ 179,
//     /* Oi*/ 200,
//     /*Rd*/ 1,
//     /*TPU*/ 11 ,
//     /*Vcmax*/ 100,
//     /* alpha_TPU */ 0,
//     /* electrons_per_carboxylation */ 4,
//     /* electrons_per_oxygenation  */ 4).An;
//     
// }
// 
// // [[Rcpp::export]]
// NumericVector check_bb(NumericVector x,double Ca,double RH, double gbw) {
//   NumericVector out(x.size());
//   stomata_outputs BB_res;
//   for (int i = 0; i < x.size(); ++i){
//       BB_res = ball_berry_gs(
//       x[i] * 1e-6,
//       Ca * 1e-6,
//       /* RH */ RH ,
//       0.008,
//       10.6,
//       gbw,
//       25,
//       25);
//     out[i] = BB_res.gsw;
//   }
//   return out;
// }