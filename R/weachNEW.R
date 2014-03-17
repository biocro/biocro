##' algorithm that decides weather weach365 or weach366 will be used based on the output of CheckLeapYear
##' @param X a matrix (or data frame) containing weather information.  The
##' input format is strict but it is meant to be used with the data usually
##' obtained from weather stations in Illinois. The data frame should have 11
##' columns (see details).
##' @param lati latitude at the specific location
##' @param ts timestep for the simulation of sub-daily data from daily. For
##' example a value of 3 would return data every 3 hours. Only divisors of 24
##' work (i.e. 1,2,3,4, etc.).
##' @param temp.units Option to specify the units in which the temperature is
##' entered. Default is Farenheit.
##' @param rh.units Option to specify the units in which the relative humidity
##' is entered. Default is percent.
##' @param ws.units Option to specify the units in which the wind speed is
##' entered. Default is miles per hour.
##' @param pp.units Option to specify the units in which the precipitation is
##' entered. Default is inches.
##' @param list() additional arguments to be passed to \code{\link{lightME}}
##' @export weachNEW
weachNEW<- function(X,lati,ts=1,temp.units=c("Farenheit","Celsius"),rh.units=c("percent","fraction"),ws.units=c("mph","mps"),pp.units=c("in","mm"),...)
{

  if(missing(lati))
    stop("latitude is missing")
  
  if((ts<1)||(24%%ts != 0))
    stop("ts should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")
  
  if(dim(X)[2] != 11)
    stop("X should have 11 columns")

  UniqueYears<- unique(X[,1])  # get unique years
  No.Of.Year <-length(UniqueYears)

output<-as.data.frame(cbind(year=numeric(0), doy=numeric(0),hour=numeric(0),SolarR=numeric(0),Temp=numeric(0),RH=numeric(0),WS=numeric(0),precip=numeric(0)))
for(i in 1:No.Of.Year)
  {
    tempX<-X[X[,1]==UniqueYears[i],]
    LeapYearIndicator<-CheckLeapYear(UniqueYears[i])
    if(LeapYearIndicator==1)
      {
        tempXOUT<-weach366(tempX,lati=lati,ts=ts,temp.units=temp.units,rh.units=rh.units,ws.units=ws.units,pp.units=pp.units)
        output<-rbind(output,tempXOUT)
      }
    if(LeapYearIndicator==0)
      {
        tempXOUT<-weach365(tempX,lati=lati,ts=ts,temp.units=temp.units,rh.units=rh.units,ws.units=ws.units,pp.units=pp.units)
        output<-rbind(output,tempXOUT)
      }
  }
output
}
    
