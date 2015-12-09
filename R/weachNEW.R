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
for(i in seq_len(No.Of.Year))
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
    
