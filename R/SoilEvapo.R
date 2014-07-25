## Function to calculate evaporation directly from the soil
##' Soil Evaporation
##'
##' Calculates soil evaporation
##'
##' The style of the code is \code{C} like because this is a prototype for the
##' underlying \code{C} (like so many other functions in this package). I leave
##' it here for future development.
##'
##' @param LAI Leaf Area Index.
##' @param k ~~Describe \code{k} here~~
##' @param AirTemp Air temperature.
##' @param IRad Incident radiation.
##' @param awc Available water content.
##' @param FieldC Field capacity.
##' @param WiltP Wilting point.
##' @param winds Wind speed.
##' @param RelH Relative humidty.
##' @export
##' @return Returns a single value of soil Evaporation in Mg H20 per hectare.
##' @author Fernando Miguez
##' @seealso Source code :)
##' @keywords models
##' @examples
##'
##'
##' SoilEvapo(LAI=3,k=0.68,AirTemp=20,IRad=1000,awc=0.3,FieldC=0.4,WiltP=0.2,winds=3,RelH=0.8)
##'
SoilEvapo <- function(LAI, k, AirTemp, IRad,
		  awc,  FieldC,  WiltP,  winds,  RelH ){

  method = 0;

   SoilClodSize = 0.04;
   SoilReflectance = 0.2;
   SoilTransmission = 0.01;
   SpecificHeat = 1010;
   kappa = 5.67e-8; #/* Stefan Boltzman Constant */

   cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 

#/* Let us assume a simple way of calculating the proportion of the
#     soil with direct radiation */
  SoilArea = exp(-k * LAI);
  
###   /* For now the temperature of the soil will be the same as the air.
### At a later time this can be made more accurate. I looked at the
### equations for this and the issue is that it is strongly dependent on
### depth. Since the soil model now has a single layer, this cannot be
### implemented correctly at the moment . */

  SoilTemp = AirTemp;

#  /* Let us use an idea of Campbell and Norman. Environmental
#     Biophysics. */
#  /* If relative available water content is */
  rawc = (awc - WiltP)/(FieldC - WiltP);

#  /* Page 142 */
#  /* Maximum Dimensionless Uptake Rate */
  Up = 1 - (1 + 1.3 * rawc)^-5;  
#  /* This is a useful idea because dry soils evaporate little water when dry*/

#  /* Total Radiation */
# /*' Convert light assuming 1 µmol PAR photons = 0.235 J/s Watts*/
  IRad = IRad * 0.2;
  TotalRadiation = IRad * 0.235;
 
  DdryA = TempToDdryA(AirTemp) ;
  LHV = TempToLHV(AirTemp) * 1e6 ; 
#/* Here LHV is given in MJ kg-1 and this needs to be converted
#   to Joules kg-1  */
  SlopeFS = TempToSFS(AirTemp) * 1e-3;
  SWVC = TempToSWVC(AirTemp) * 1e-3;

  PsycParam = (DdryA * SpecificHeat) / LHV;
  DeltaPVa = SWVC * (1 - RelH / 100);

  BoundaryLayerThickness = 4e-3 * sqrt(SoilClodSize / winds); 
  DiffCoef = 2.126e-5 * 1.48e-7 * SoilTemp;
  SoilBoundaryLayer = DiffCoef / BoundaryLayerThickness;

  Ja = 2 * TotalRadiation * ((1 - SoilReflectance - SoilTransmission) / (1 - SoilTransmission));
  rlc = 4 * kappa * (273 + SoilTemp)^3 * 0.5;

  PhiN = Ja - rlc; #/* Calculate the net radiation balance*/
  if(PhiN < 0) PhiN = 1e-7;

#  /* Priestly-Taylor */
  if(method == 0){
    Evaporation = 1.26 * (SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam));
  }else{
#  /* Penman-Monteith */
    Evaporation = (SlopeFS * PhiN + LHV * PsycParam * SoilBoundaryLayer * DeltaPVa) / (LHV * (SlopeFS + PsycParam));
  }
## /*  Report back the soil evaporation rate in Units mmoles/m2/s */
## /*     Evaporation = Evaporation * 1000:   ' Convert Kg H20/m2/s to g H20/m2/s */
## /*     Evaporation = Evaporation / 18:     ' Convert g H20/m2/s to moles H20/m2/s */
## /*     Evaporation = Evaporation * 1000:   ' Convert moles H20/m2/s to mmoles H20/m2/s */
    
## /*     If Evaporation <= 0 Then Evaporation = 0.00001: 
## ' Prevent any odd looking values which might get through at very low light levels */

  Evaporation = Evaporation* (1e6/18);
#  /* Adding the area dependence and the effect of drying */
#  /* Converting from m2 to ha (times 1e4) */
#  /* Converting to hour */
  Evaporation = Evaporation *  SoilArea * Up * cf2; 
  if(Evaporation < 0) Evaporation = 1e-6;

  return(Evaporation);
}



TempToDdryA <- function(Temp)
{
  DdryA = 1.295163636 + -0.004258182 * Temp;
  return(DdryA);
}

TempToLHV <- function(Temp)
  {
    LHV = 2.501 + -0.002372727 * Temp;
    return(LHV);
  }

##' Returns a value for SlopeFS from a function of temperature
##' 
##' Takes a value for Temp as defined by the SoilEvapo function and returns a value for SlopeFS
##' which helps define the Evaporation.
##' @param Temp Temperature
##' @export TempToSFS
TempToSFS <- function(Temp)
  {
    SlopeFS = 0.338376068 +  0.011435897 * Temp +  0.001111111 * Temp^2;
    return(SlopeFS);
  }

TempToSWVC <- function(Temp)
  {
    SWVC =  4.90820192 +   0.06387253 * Temp +    0.02745742 * Temp^2;
   return(SWVC);
  }
