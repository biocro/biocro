library(gdata)
sitebalb <- read.csv("/home/dwng/Met - BALB.csv", header=TRUE, na.string="")
balb = sitebalb[63:5029,]
load("/home/dwng/weather/loyton.RData")
X=strptime(balb$DATE, "%m/%d/%Y")
balb$doy=as.numeric(julian(X,origin="2003-01-01",tz=GMT))
balb$doy=round(balb$doy,digits=0)
hour=rep(0:23,207)
balb$hour=hour[1:4967]
balb$year=2003
balb$tavg=(balb$TMIN+balb$TMAX)/2
balbbiocro=balb[,c(26,24,25,18,27,8,6,4)]
names(balbbiocro)=names(willowgrowclimate)
balbbiocro$RH=balbbiocro$RH/100
RH=mean(balbbiocro$RH,na.rm=TRUE)
WS=mean(balbbiocro$WS,na.rm=TRUE)
TEMP=mean(balbbiocro$Temp,na.rm=TRUE)
balbbiocro$RH[is.na(balbbiocro$RH)]<-""
balbbiocro[balbbiocro$RH=="",]$RH=RH
balbbiocro$SolarR[is.na(balbbiocro$SolarR)]<-0
balbbiocro$precip[is.na(balbbiocro$precip)]<-0
balbbiocro$Temp[is.na(balbbiocro$Temp)]<-TEMP
balbbiocro$WS[is.na(balbbiocro$WS)]<-WS
balbbiocro$RH=as.numeric(balbbiocro$RH)

write.csv(balbbiocro,file="/home/dwng/weather/balbbiocro.csv")

balbweather <- read.csv("/home/dwng/weather/balbbiocro.csv",header=TRUE)
balbweather=balbweather[,2:9]
willowgrowclimate[1201:6167,]=balbweather
balbweather2=willowgrowclimate
a<-willowGro(willowgrowclimate)
save(balbweather2,file="/home/dwng/weather/balbweather2.Rdata")


######
siteloyt <- read.csv("/home/dwng/Met - LOYT.csv", header=TRUE, na.string="")
loyt = siteloyt[881:6760,]

X=strptime(loyt$DATE, "%m/%d/%Y")
loyt$doy=as.numeric(julian(X,origin="2003-01-01",tz=GMT))
loyt$doy=round(loyt$doy,digits=0)
hour=rep(0:23,245)
loyt$hour=hour
loyt$year=2003
loyt$Tavg=(loyt$TMIN+loyt$TMAX)/2
loytbiocro=loyt[,c(26,24,25,16,27,10,6,4)]
names(loytbiocro)=names(willowgrowclimate)
loytbiocro$RH=loytbiocro$RH/100
RH=mean(loytbiocro$RH,na.rm=TRUE)
WS=mean(loytbiocro$WS,na.rm=TRUE)
TEMP=mean(loytbiocro$Temp,na.rm=TRUE)
loytbiocro$RH[is.na(loytbiocro$RH)]<-""
loytbiocro[loytbiocro$RH=="",]$RH=RH
loytbiocro$SolarR[is.na(loytbiocro$SolarR)]<-0
loytbiocro$precip[is.na(loytbiocro$precip)]<-0
loytbiocro$Temp[is.na(loytbiocro$Temp)]<-TEMP
loytbiocro$WS[is.na(loytbiocro$WS)]<-WS
loytbiocro$RH=as.numeric(loytbiocro$RH)

write.csv(loytbiocro,file="/home/dwng/weather/loytbiocro.csv")

load("/home/dwng/weather/loyton.RData")
willowgrowclimate[2881:8760,4:8]=loytbiocro[,4:8]
loytweather2=willowgrowclimate
save(loytweather2,file="/home/dwng/weather/loytweather2.Rdata")


######
sitetref <- read.csv("/home/dwng/Met - TREF.csv", header=TRUE, na.string="")
tref = sitetref[8:7063,]

X=strptime(tref$DATE, "%m/%d/%Y")
tref$doy=as.numeric(julian(X,origin="2003-01-01",tz=GMT))
tref$doy=round(tref$doy,digits=0)
hour=rep(0:23,294)
tref$hour=hour
tref$year=2003
tref$Tavg=(tref$TMIN+tref$TMAX)/2
trefbiocro=tref[,c(25,24,26,16,27,10,6,4)]
names(trefbiocro)=names(willowgrowclimate)
trefbiocro$RH=trefbiocro$RH/100
RH=mean(trefbiocro$RH,na.rm=TRUE)
WS=mean(trefbiocro$WS,na.rm=TRUE)
TEMP=mean(trefbiocro$Temp,na.rm=TRUE)
trefbiocro$RH[is.na(trefbiocro$RH)]<-""
trefbiocro[trefbiocro$RH=="",]$RH=RH
trefbiocro$SolarR[is.na(trefbiocro$SolarR)]<-0
trefbiocro$precip[is.na(trefbiocro$precip)]<-0
trefbiocro$Temp[is.na(trefbiocro$Temp)]<-TEMP
trefbiocro$WS[is.na(trefbiocro$WS)]<-WS
trefbiocro$RH=as.numeric(trefbiocro$RH)

write.csv(trefbiocro,file="/home/dwng/weather/trefbiocro.csv")

load("/home/dwng/weather/trefeinon.RData")
willowgrowclimate[1681:8736,4:8]=trefbiocro[,4:8]
trefweather2=willowgrowclimate
save(trefweather2,file="/home/dwng/weather/trefweather2.Rdata")


######
sitetrum <- read.csv("/home/dwng/Met - TRUM.csv", header=TRUE, na.string="")
trum = sitetrum[1549:7404,]

X=strptime(trum$DATE, "%m/%d/%Y")
trum$doy=as.numeric(julian(X,origin="2003-01-01",tz=GMT))
trum$doy=round(trum$doy,digits=0)
hour=rep(0:23,244)
trum$hour=hour
trum$year=2003
trum$Tavg=(trum$TMIN+trum$TMAX)/2
trumbiocro=trum[,c(26,24,25,20,27,18,6,4)]
names(trumbiocro)=names(willowgrowclimate)
trumbiocro$RH=trumbiocro$RH/100
RH=mean(trumbiocro$RH,na.rm=TRUE)
WS=mean(trumbiocro$WS,na.rm=TRUE)
TEMP=mean(trumbiocro$Temp,na.rm=TRUE)
trumbiocro$RH[is.na(trumbiocro$RH)]<-""
trumbiocro[trumbiocro$RH=="",]$RH=RH
trumbiocro$SolarR[is.na(trumbiocro$SolarR)]<-0
trumbiocro$precip[is.na(trumbiocro$precip)]<-0
trumbiocro$Temp[is.na(trumbiocro$Temp)]<-TEMP
trumbiocro$WS[is.na(trumbiocro$WS)]<-WS
trumbiocro$RH=as.numeric(trumbiocro$RH)

write.csv(trumbiocro,file="/home/dwng/weather/trumbiocro.csv")

load("/home/dwng/weather/trumpington.RData")
willowgrowclimate[2137:7992,4:8]=trumbiocro[,4:8]
trumweather2=willowgrowclimate
save(trumweather2,file="/home/dwng/weather/trumweather2.Rdata")


uksummary <- read.csv("/home/dwng/MET_SUMMARY.csv",header=TRUE,na.string="")
uksummary$TAVG=(uksummary$DRYMIN+uksummary$DRYMAX)/2
uksummary$DATE2=strptime(uksummary$DATE, format="%m/%d/%Y") 
uksummary$year=as.numeric(format(uksummary$DATE2, format = "%Y"))
uksummary$day=1





RH=mean(uksummary$RELHUM,na.rm=TRUE)
WS=mean(uksummary$WSPEED,na.rm=TRUE)
TMAX=mean(uksummary$DRYMAX,na.rm=TRUE)
TMIN=mean(uksummary$DRYMIN,na.rm=TRUE)
PAR=mean(uksummary$PAR,na.rm=TRUE)
uksummary$RELHUM[is.na(uksummary$RELHUM)]<-""
uksummary[uksummary$RELHUM=="",]$RELHUM=RH
uksummary$RELHUM=as.numeric(uksummary$RELHUM)





uksummary$PAR[is.na(uksummary$PAR)]<-PAR
uksummary$RAIN[is.na(uksummary$RAIN)]<-0
uksummary$DRYMAX[is.na(uksummary$DRYMAX)]<-TMAX
uksummary$DRYMIN[is.na(uksummary$DRYMIN)]<-TMIN
uksummary$WSPEED[is.na(uksummary$WSPEED)]<-WS



uksummary[uksummary$year==1995,]$day=rep(1:365,46)
uksummary[uksummary$year==1996,]$day=rep(1:366,46)
uksummary[uksummary$year==1997,]$day=rep(1:365,46)


uk1997=uksummary[uksummary$year==1997,]
uk1995=uksummary[uksummary$year==1995,]
uk1996=uksummary[uksummary$year==1996,]

uk1997$RHMAX=uk1997$RELHUM
uk1997$RHMIN=uk1997$RELHUM
uk19972=uk1997[,c("year","day","PAR","DRYMAX","DRYMIN","TAVG","RHMAX","RHMIN","RELHUM","WSPEED","RAIN")]
names(uk19972)=names(climdata)
willowuk1997<-weachNEW(uk19972,lat=43.345,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")
willowuk1997$RH=willowuk1997$RH/100
save(willowuk1997,file="/home/dwng/weather/willowuk1997.RData")
willowuk1997$SITE=rep(uk1997$SITE_ID,each=24)


weather=willowuk1997[willowuk1997$SITE=="LLAN",]
weather=willowuk1997[willowuk1997$SITE=="LOUG",]
weather=willowuk1997[willowuk1997$SITE=="TEAN",]
weather=willowuk1997[willowuk1997$SITE=="TALY",]

ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
ukwillow=ukdata[ukdata$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
sitemean=aggregate(x=ukwillow,by=list(ukwillow$PLOT_NAME),mean,na.rm=TRUE)
sitemean$SITE=c("ALIC","ALLE","BALB","BIGB","BONY","BORE","CARR","CAST","CERE","CHFM","CHWD","CRAI","DELA","ROTH","DEMON","DUNN","DUNS","FRIA",
                "GILD","GREA","HARP","HAYB","LAWR","LLANG","LLAN","LOND","LONG","LOSE","LOUG","LOYT","MAWD","MORA","MOSC","MYER","OYNE","ROVE",
                "SLEB","SOHA","SUNN","TAIR","TALY","TEAN","THOR","TREF","TRUM","TWEE","WENS","WOOD","WRIT")
