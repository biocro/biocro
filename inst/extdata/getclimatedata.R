library(BioCro)
library(RNCEP)
library(EnCro) # weachNEW require EnCro
# Fetch Urbana Climate Data
source("./InputForWeach.R")
clim2008<-InputForWeach(lat=40.11, lon=-88.208,year1=2008,year2=2008)
hourly2008<-weachNEW(clim2008,lat=40.11,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")
save(clim2008,file="clim2008.RData")
save(hourly2008,file="hourly2008.RData")

clim2009<-InputForWeach(lat=40.11, lon=-88.208,year1=2009,year2=2009)
hourly2009<-weachNEW(clim2009,lat=40.11,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")
save(clim2009,file="clim2009.RData")
save(hourly2009,file="hourly2009.RData")

clim2010<-InputForWeach(lat=40.11, lon=-88.208,year1=2010,year2=2010)
hourly2010<-weachNEW(clim2010,lat=40.11,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")
save(clim2010,file="clim2010.RData")
save(hourly2010,file="hourly2010.RData")

clim2011<-InputForWeach(lat=40.11, lon=-88.208,year1=2011,year2=2011)
hourly2011<-weachNEW(clim2011,lat=40.11,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")
save(clim2011,file="clim2011.RData")
save(hourly2011,file="hourly2011.RData")

clim2012<-InputForWeach(lat=40.11, lon=-88.208,year1=2012,year2=2012)
hourly2012<-weachNEW(clim2012,lat=40.11,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")
save(clim2012,file="clim2012.RData")
save(hourly2012,file="hourly2012.RData")
