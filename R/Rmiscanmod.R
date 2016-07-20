##   BioCro/R/Rmiscanmod.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2008


## This file will implement the more detailed version of miscanmod
## according to the Excel spreadsheet from Clifton-Brown
## Some changes have been made in order to run it with the available data
## This model, based on Monteith, is first described in Clifton-Brown et al. (2000)
## Industrial Crops and Products. 12. 97-109. It was later developed in later papers
## Clifton-Brown et al. (2004) Global Change Biology. 10. 509-518.

##############################################################################
#
#
# data should have
#
# column 1: year
# column 2: month
# column 3: day
# column 4: JD
# column 5: max T (Celsius)
# column 6: min T (Celsius)
# column 7: PPFD or solar radiation divided by 2 (MJ/m2)
# column 8: Potential evaporation
# column 9: precip (mm)
#
# Parameters
# ----------
# RUE = Radiation Use Efficiency (g/MJ)
# LER = Leaf Expansion Rate
# Tb = Leaf base Temperature (Celsius)
# k = light extinction coefficient (dimensionless)
# LAIdrd = LAI down regulation 
# RUEdrd = RUE down regulation 
# a = soil parameter
# b = soil parameter
# soildepth = soil depth
#
##############################################################################
##' RUE-based model to calculate miscanthus growth and yield.
##'
##' Simple model to calculate crop growth and yield based on MISCANMOD (see
##' references).
##'
##' The data.frame or matrix should contain:
##'
##' column 1: Year. column 2: Month. column 3: Day. column 4: JD. column 5: Max T
##' (Celsius). column 6: Min T (Celsius). column 7: PPFD or solar radiation
##' divided by 2 (MJ/m2). column 8: Potential evaporation. column 9: Precip (mm).
##'
##' @param data data.frame or matrix described in details.
##' @param RUE Radiation use efficiency (g/MJ).
##' @param LER Leaf expansion rate LAI/GDD.
##' @param Tb Base temperature (Celsius).
##' @param k Extinction coefficient of light in the canopy.
##' @param LAIdrd Leaf Area Index `down regulation decline'.
##' @param LAIStop Leaf Area Index `down regulation decline' threshold .
##' @param RUEdrd Radiation Use Efficieny `down regulation decline'.
##' @param RUEStop Radiation Use Efficieny `down regulation decline' threshold.
##' @param SMDdrd Soil Moisture Deficit `down regulation decline'.
##' @param SMDStop Soil Moisture Deficit `down regulation decline' threshold.
##' @param FieldC Soil field capacity.
##' @param iWatCont Initial water content.
##' @param a Soil parameter.
##' @param b Soil parameter.
##' @param soildepth Soil depth.
##' @export
##' @return Returns a list with these components:
##'
##' \item{PotEvp}{Potential Evaporation.}
##' \item{Deficitp}{Deficitp.}
##' \item{SMDp}{Soil Moisture Deficit (potential).}
##' \item{AE.PE}{Actual Evaporation / Potential Evaporation.}
##' \item{Deficitp2}{Deficitp2.}
##' \item{SMDa}{Soil Moisture Deficit (actual).}
##' \item{diffRainPE}{Difference between Rainfall and potential
##' evaporation.}
##' \item{H2oper}{H2O percent.}
##' \item{SoilMoist}{Soil Moisture.}
##' \item{SoilMatPot}{Soil Matric Potential.}
##' \item{WL.LER}{Water limited Leaf Expansion Rate.}
##' \item{WL.RUE}{Water limited Radiation Use Efficiency.}
##' \item{DDaTb}{Degree days above base Temperature.}
##' \item{DDcum}{Degree days (cumulative).}
##' \item{adjSumDD}{Adjusted sum of degree days.}
##' \item{LAI}{Leaf Area Index.}
##' \item{pLI}{Proportion of light intercepted.}
##' \item{Yield}{Yield (dry biomass) (g/m2); to convert to Mg/ha, divide by
##' 100.}
##'
##' @references Clifton-Brown, J. C., B. Neilson, I. Lewandowski, and M. B. Jones.
##' 2000.  The modelled productivity of Miscanthus x giganteus (GREEF et DEU) in
##' Ireland. \emph{Industrial Crops and Products} 12 (2): 97--109.
##'
##' Clifton-Brown, J. C., P. F. Stampfl, and M. B. Jones.  2004.  Miscanthus biomass
##' production for energy in Europe and its potential contribution to
##' decreasing fossil fuel carbon emissions.  \emph{Global Change Biology} 10 (4)
##' 509--518.
##'
##' @keywords models
##' @examples
##'
##'
##' ## Need to get an example data set and then run it
##' \dontrun{
##' data(WD1979)
##'
##' res <- Rmiscanmod(WD1979)
##'
##' ## convert to Mg/ha
##'
##' Yld <- res$Yield / 100
##'
##' lattice::xyplot(Yld ~ 1:365 ,
##'        xlab='doy',
##'        ylab='Dry biomass (Mg/ha)')
##'
##' ## although the default value for Field Capacity is 45
##' ## a more reasonable value is closer to 27
##'
##'
##' }
##'
##'


Rmiscanmod <- function(data,RUE=2.4,LER=0.01,Tb=10,k=0.67,
                      LAIdrd=0.8,LAIStop=1.8,RUEdrd=1.3,RUEStop=2.5,
                      SMDdrd=-30,SMDStop=-120,FieldC=45,iWatCont=45,a=6682.2,b=-0.33,
                      soildepth=0.6){

  ## At this point the vairables are selected by their position (i.e. column)
  ## in the data.frame or matrix

  if(!inherits(data,"data.frame") || !inherits(data,"matrix"))
    stop("data should be a data.frame or matrix")

  if(dim(data)[2] != 9)
    stop("data should have 9 columns")

  data <- as.matrix(data)
  
  JD <- data[,4]
  maxTemp <- data[,5]
  minTemp <- data[,6]
  Radiation <- data[,7]
  PotEvp <- data[,8] 
  precip <- data[,9]
  
  ## Now need to calculate the precipitation deficit

  Deficitp <- ifelse((precip-PotEvp)>0,0,precip-PotEvp)

  ## Calculate the soil moisture deficit

  SMDp <- numeric(length(Deficitp))
  SMDp[1] <- 0
  
  for(i in 2:length(Deficitp)){

    if(Deficitp[i] < 0){
      
      SMDp[i] <- Deficitp[i] + SMDp[i-1]
      
    }else{
          if((precip[i] + SMDp[i-1])<0){
              SMDp[i] <- precip[i] + SMDp[i-1]
            }else{
              SMDp[i] <- 0
            }
        }
  }

  ## At the moment I don't quite get how they have calculated this but I will assume
  ## SMDp instead of SMDa for now
  ## this seems to be Actual Evaporation over Potential Evaporation


      aSMDStop <- abs(SMDStop)
      aSMDdrd <- abs(SMDdrd)
      
      AE.PE <- ifelse(SMDp < SMDStop,0,ifelse(SMDp < SMDdrd,
                                            1+(aSMDdrd/(aSMDStop-aSMDdrd)+(1/(aSMDStop+SMDdrd))*SMDp),1))

      PE.dr <- AE.PE * PotEvp

      Deficitp2 <- ifelse((precip-PE.dr)>0,0,precip-PE.dr)

      SMDa <- numeric(length(Deficitp2))
      SMDa[1] <- 0
  
        for(i in 2:length(Deficitp2)){
      
          if(Deficitp2[i] < 0){
        
            SMDa[i] <- Deficitp2[i] + SMDa[i-1]
        
          }else{
            if((precip[i] + SMDa[i-1])<0){
              SMDa[i] <- precip[i] + SMDa[i-1]
            }else{
              SMDa[i] <- 0
            }
          }
        }


      diffRainPE <- precip - PE.dr

      H2Oper <- ((diffRainPE/1000)/soildepth )*100

      tmp4 <- numeric(length(H2Oper))
      tmp4[1] <- iWatCont
  
        for(i in 2:length(H2Oper)){

          if((tmp4[i-1] + H2Oper[i])>FieldC){
            tmp4[i] <- FieldC
          }else{
            tmp4[i] <- tmp4[i-1] + H2Oper[i]
          }
        }

  SoilMoist <- tmp4

  SoilMatPot <- a * exp(b*SoilMoist)

  WL.LER <- ifelse(SoilMatPot<LAIdrd,1,ifelse(SoilMatPot<LAIStop,1-((SoilMatPot-LAIdrd)/(LAIStop-LAIdrd)),0))

  WL.RUE <- ifelse(SoilMatPot<RUEdrd,1,ifelse(SoilMatPot<RUEStop,1-((SoilMatPot-RUEdrd)/(RUEStop-RUEdrd)),0))

  ## Calculation of Radiation available
  
   half <- as.integer(length(minTemp)/2)
   minTemp1 <- minTemp[1:half]
        if (min(minTemp1) > 0) {
            day1 <- 30
        }
        else {
            minTemp1 <- minTemp1[which(minTemp1 < 0)]
            day1 <- max(minTemp1)
        }
   minTemp2 <- minTemp[half:length(minTemp)]
        if (min(minTemp2) > 0) {
            day1 <- 330
        }
        else {
            minTemp2 <- minTemp2[which(minTemp2 < 0)]
            dayn <- min(minTemp2)
        }

  ## I'm using the convention in MISCANMOD which is a bit strange
  ## to use a zero for no frost and 1 for frost
  DaysNoFrost <- ifelse((JD < day1)||(JD > dayn),1,0)
  
  RadAvail <- cumsum(I(DaysNoFrost*Radiation))
  
  ## Calculate the available degree days

  avgT <- apply(cbind(minTemp,maxTemp),1,mean)
# Replace the previous line with the average daily air temperature
  
  DDaTb <- ifelse(maxTemp < Tb,0,
                  ifelse(avgT < Tb,
                         (maxTemp-Tb)/4,
                         ifelse(minTemp < Tb,(maxTemp-Tb)/2 - (Tb - minTemp)/4,
                                avgT - Tb)))

  DDcum <- cumsum(I(DaysNoFrost*DDaTb))

  SoilEffDD <- DDaTb * WL.LER

  DDcum <- cumsum(I(DaysNoFrost*DDaTb))

  adjSumDD <- cumsum(I(DaysNoFrost*SoilEffDD)) * DaysNoFrost

  LAI <- adjSumDD * LER

  pLI <- (1 - exp(-k*LAI))*100

  LI <- (pLI/100) * Radiation

  DayYield <- LI * RUE * WL.RUE

  Yield <- cumsum(DayYield)

  list(PotEvp=PotEvp,Deficitp=Deficitp,SMDp=SMDp,AE.PE=AE.PE,Deficitp2=Deficitp2,SMDa=SMDa,
       diffRainPE=diffRainPE,H2Oper=H2Oper,SoilMoist=SoilMoist,SoilMatPot=SoilMatPot,WL.LER=WL.LER,
       WL.RUE=WL.RUE,DDaTb=DDaTb,DDcum=DDcum,adjSumDD=adjSumDD,LAI=LAI,LI=LI,pLI=pLI,Yield=Yield)

}
    
