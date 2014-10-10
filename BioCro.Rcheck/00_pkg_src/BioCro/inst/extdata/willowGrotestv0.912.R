library(BioCro)
library(latticeExtra)
library(Agreement)
source("/home/dwng/willowfigure/validation.R")
# Define dataframe that will be used to compare model performancefor Stem and LAI prediction

###########################################################################
############initial density of 20000 stems per hectare converted to 1mg/ha; if missing,20000 is default
compareStem<-data.frame(Observed = numeric(0),Predicted = numeric(0),Irrigated=numeric(0),Plantcrop=numeric(0))
compareLAI<-data.frame(Observed = numeric(0),Predicted = numeric(0),Irrigated=numeric(0),Plantcrop=numeric(0))
compareLeaf<-data.frame(Observed = numeric(0),Predicted = numeric(0),Irrigated=numeric(0),Plantcrop=numeric(0))


canwillow<-willowcanopyParms(Sp=1.1,kd=0.37,GrowthRespFraction=0.3)

senwillow <- willowseneParms(senLeaf=1600,senStem=5500, senRoot=5500,senRhiz=5500,Tfrosthigh=5,Tfrostlow=0,leafdeathrate=5)


phewillow <- willowphenoParms(tp1=250, tp2=350, tp3=900, tp4=1200, tp5=3939, tp6=7000,
                              kStem1=0.01, kLeaf1=0.98, kRoot1=0.01, kRhizome1=-8e-4, 
                              kStem2=0.01, kLeaf2=0.98, kRoot2=0.003, kRhizome2=0.007, 
                              kStem3=0.7, kLeaf3=0.15, kRoot3=0.045, kRhizome3=0.105, 
                              kStem4=0.7, kLeaf4=0.15, kRoot4=0.045, kRhizome4=0.105, 
                              kStem5=0.7, kLeaf5=0.00001, kRoot5=0.15, kRhizome5=0.15, 
                              kStem6=0.7, kLeaf6=0.000001, kRoot6=0.15, kRhizome6=0.15)

soilwillow <- soilParms(phi2=2,WiltP=0.16,wsFun=c('linear'))

##########################################################################
##########validation site 1 Nordh 2004 not coppiced
###willow observed data from dan's spreadsheet
#soilwillow=soilParms(soilType=10)

weather1990 <- read.csv("~/weather/59.617-16.667/1990-1990/weather.csv")
weather1990<- weather1990[,2:12]
lat=59.617

weather19902 <- weachNEW(weather1990, lati = lat, ts = 1, 
                     temp.units="Celsius", rh.units="fraction", 
                     ws.units="mph", pp.units="in")
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19902,day1=100,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.9
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19902,day1=100,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19902,day1=100,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19902,day1=100,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
res=res2
DAP<-to.dap(res$DayofYear)
willowdata <- read.csv("~/willowfigure/willowdata.csv")
n=length(unique(willowdata$Study))

dat=willowdata[willowdata$Study==unique(willowdata$Study)[1],]
#obs<-data.frame(DAP=2900,Stem=mean(dat$yield))
obs<-data.frame(DAP=max(DAP),Stem=max(dat$yield))
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Nordh 2004",0)

#********************************************************************************
#*************************************************************************8*******
#*site Labrecque 2003, two cycles 1995-1997, 1998-2000
weather1998 <- read.csv("~/weather/45.083--74.333/1998-1998/weather.csv")
weather19982<- weather1998[,2:12]
lat=45.083
weather19982 <- weachNEW(weather19982, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")

iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19982,day1=130,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.9
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19982,day1=130,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19982,day1=130,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

res=res1
DAP<-to.dap(res$DayofYear)
dat=willowdata[willowdata$Study==unique(willowdata$Study)[2],]
obs <- data.frame(DAP=max(DAP), Stem=mean(dat$yield[1:8]))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Labrecque 2003",0)

#####second cycle
stem2=stem2*0.05
iplant0 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res0 <- willowGro(weather19982,day1=130,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.9
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19982,day1=130,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19982,day1=130,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

res=res1
DAP<-to.dap(res$DayofYear)
dat=willowdata[willowdata$Study==unique(willowdata$Study)[2],]
obs <- data.frame(DAP=max(DAP), Stem=mean(dat$yield[c(10,12,14,16)]))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Labrecque 2003",0)




##############################################################################
##############################################################################
###Christersson 1987 all data are irrigated and fertilized data are seprated by root age (3,4) and shoot age (1,2)
weather <- read.csv("~/weather/56.93-12.35/1982-1982/weather.csv")
weather19822<- weather[,2:12]
lat=56.93
weather19822 <- weachNEW(weather19822, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
day1=140
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

#stem0=stem0*0.05
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]


iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]


res=res2 #root age 3,shoot age2
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(c(32,26,24,27,34,33)))
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Christersson 1987",0)


iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
res=res3# root age 4, shoot age 3
DAP<-to.dap(res$DayofYear)
dat=willowdata[willowdata$Study==unique(willowdata$Study)[3],]
obs <- data.frame(DAP=max(DAP), Stem=mean(c(68,54,41,39)))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Christersson 1987",0)

####this is going to be coppiced at the second year
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

stem1=stem1*0.05
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

res=res2 #root age 3,shoot age1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(c(12,9,8,9,10,9,10,7)))
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Christersson 1987",0)

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
res=res3# root age 4, shoot age 2
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(c(42,26,25)))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Christersson 1987",0)


#########this is coppiced at the third year
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

stem2=stem2*0.05
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(c(9,14,16,13)))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Christersson 1987",0)



############################################################
################################################################
###############  Walker 2008 no coppicing from 2004 to 2006

weather <- read.csv("~/weather/51.47-22.07/2003-2003/weather.csv")
weather2003<- weather[,2:12]
lat=51.47
weather20032 <- weachNEW(weather2003, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
day1=130
iplant0 <-iwillowParms(iRhizome=1.0,iStem=2.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather20032,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

res=res0# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=14.7)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Walker 2008",0)

stem0=stem0*0.9
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather20032,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=25.6)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Walker 2008",0)

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19822,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=33.3)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Walker 2008",0)

#################################################################################
#################################################################################
###############Dusek 2006 #planting density is 58000/ha, 2.9 this could be used for calibration
weather <- read.csv("~/weather/49.15-15/1996-1996/weather.csv")
weather1996<- weather[,2:12]
lat=49
weather19962 <- weachNEW(weather1996, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
day1=130
iplant0 <-iwillowParms(iRhizome=1.0,iStem=2.9,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]
leaf0 <-max(res0$Leaf)

dat=willowdata[willowdata$Study==unique(willowdata$Study)[6],] 
obsyear1=dat$yield[1]
res=res0# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=11.2,Leaf=3.1)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Dusek 2006",0)


iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
obsyear2=dat$yield[2]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=56.9)

#the data maybe wrong
#tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
#tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
#compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,0,0)


iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=59.1)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Dusek 2006",0)


####################################################################################################
####################################################################################################
###########Szczukowski 2002 planting density is 40000/ha
#soilwillow=soilParms(wsFun=c("none"))
weather <- read.csv("~/weather/53.73-18.89/2000-2000/weather.csv")
weather1996<- weather[,2:12]
lat=49
weather19962 <- weachNEW(weather1996, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
day1=135
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
#day1= weather19962[weather19962$Temp >=6,]$doy[1] 
res0 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=14.9)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Szczukowski 2002",0)


iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=32.1)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Szczukowski 2002",0)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=64.65)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Szczukowski 2002",0)

####################################################################################################
####################################################################################################
#########Boehmel 2008
weather <- read.csv("~/weather/48.733-8.933/2002-2002/weather.csv")
weather1996<- weather[,2:12]
lat=49
weather19962 <- weachNEW(weather1996, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
dat=willowdata[willowdata$Study==unique(willowdata$Study)[9],] 
day1=130
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)

obsyear3=dat$yield[3]
res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat$yieldyearly[1]*3)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Boehmel 2008",0)

########################################################################################################
#######################################################################################################
#########Hofmann-Schielle 1999
weather <- read.csv("~/weather/49.08-12.15/1983-1983/weather.csv")
weather1996<- weather[,2:12]
lat=49
weather19962 <- weachNEW(weather1996, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
dat=willowdata[willowdata$Study==unique(willowdata$Study)[10],] 
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)

obsyear3=dat$yield[3]
res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(dat$yield))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Hofmann-Schielle 1999",0)

####################################################################################
####################################################################################
#############################Kopp 2001 ########ten successive annual harvests at two nitrogen we are using unfertilized data,
#we are ignoring the first year data, as the establishement year, we are only simulating 5 cycles
weather <- read.csv("~/weather/42.792--76.125/1987-1987/weather.csv")
weather1996<- weather[,2:12]
lat=49
weather19962 <- weachNEW(weather1996, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
weather19962[weather19962$RH>1,]$RH<-1
dat=willowdata[willowdata$Study==unique(willowdata$Study)[11],] 
day1=130
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
dat2=dat[dat$Season==2 & dat$Fertilization==0,]

res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(dat2$yieldyearly))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Kopp 2001",0)

stem1=stem1*0.05

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)
stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
dat3=dat[dat$Season==3 & dat$Fertilization==0,]

res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(dat3$yieldyearly))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Kopp 2001",0)

stem2=stem2*0.05

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]
dat4=dat[dat$Season==4 & dat$Fertilization==0,]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(dat4$yieldyearly))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Kopp 2001",0)


stem3=stem3*0.05

iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]
dat5=dat[dat$Season==5 & dat$Fertilization==0,]

res=res4# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(dat5$yieldyearly))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Kopp 2001",0)

stem4=stem4*0.05

iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]
dat6=dat[dat$Season==6 & dat$Fertilization==0,]

res=res5# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=mean(dat6$yieldyearly))

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Kopp 2001",0)


#######################################################################################################
############################################################################################################
##RANDALL
weather <- read.csv("~/weather/44.08--93.51/2003-2003/weather.csv")
weather1996<- weather[,2:12]
lat=49
weather19962 <- weachNEW(weather1996, lati = lat, ts = 1, 
                         temp.units="Celsius", rh.units="fraction", 
                         ws.units="mph", pp.units="in")
dat=willowdata[willowdata$Study==unique(willowdata$Study)[12],] 
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(weather19962,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)

obsyear3=dat$yield[1]
res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=15.4)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Randall",0)



#######################################################################################################
############################################################################################################
#volk 2011 yields of willow biomass crops across a range of sites in north american
#observed data are from the top clone

##new york site
weather <- load("~/weather/willowtullyny.RData")

dat=c(26.7)
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","new york")

##vermont site

load("~/weather/willowburlingtonvt.RData")

dat=c(36.6)
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","vermont")


##arlingtonwi

load("~/weather/willowarlingtonwi.RData")

dat=c(46.8)
day1=100
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","arlington")


##queenstown, md

load("~/weather/willowquennstownmd.RData")

dat=c(42)
day1=120
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","queenstown")

##tullyny 2005

load("~/weather/willowtullyny2005.RData")

dat=c(32.4)
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","tullyny")

##wasecamn

load("~/weather/willowwasecamn.RData")

dat=c(30.3)
day1=120
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","wasemn")


##boisbriand qc

load("~/weather/willowboisbriandqc.RData")

dat=c(51.6)
day1=140
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","queen")


##birchhills sk

load("~/weather/willowbirchhillssk.RData")

dat=c(35.4)
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]

iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]

iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(willowgrowclimate,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)


res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=dat)

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"volk 2011","sk")


#########################################################################################
###########################################################################################
###################uk sites Aylott 2008
##balb
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/balbweather2.Rdata")

data=ukdata[ukdata$PLOT_NAME=="Balbirnie",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1),mean(dat$Year2),mean(dat$Year3),mean(dat$Year4),mean(dat$Year5),mean(dat$Year6))
day1=130
#data for average across clones
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

##day1= balbweather2[balbweather2$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","balb")

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","balb")

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","balb")


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","balb")


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","balb")

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(balbweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","balb")


#########################################################################################
###########################################################################################
###################uk sites 
##loyt
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/loytweather2.Rdata")

data=ukdata[ukdata$PLOT_NAME=="Loyton Bampton",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1,na.rm=TRUE),mean(dat$Year2,na.rm=TRUE),mean(dat$Year3,na.rm=TRUE),mean(dat$Year4,na.rm=TRUE),mean(dat$Year5,na.rm=TRUE),mean(dat$Year6,na.rm=TRUE))
day1=130
#data for average across clones
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

#day1= loytweather2[loytweather2$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","loyt")

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","loyt")

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","loyt")


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","loyt")


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","loyt")

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(loytweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","loyt")

#########################################################################################
###########################################################################################
###################uk sites 
##tref
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/trefweather2.Rdata")

data=ukdata[ukdata$PLOT_NAME=="Trefeinon",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1,na.rm=TRUE),mean(dat$Year2,na.rm=TRUE),mean(dat$Year3,na.rm=TRUE),mean(dat$Year4,na.rm=TRUE),mean(dat$Year5,na.rm=TRUE),mean(dat$Year6,na.rm=TRUE))
day1=130
#data for average across clones
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

#day1= trefweather2[trefweather2$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008",'tref')

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008",'tref')

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008",'tref')


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008",'tref')


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008",'tref')

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(trefweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008",'tref')


#########################################################################################
###########################################################################################
###################uk sites 
##trum
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/trumweather2.Rdata")

data=ukdata[ukdata$PLOT_NAME=="Trumpington",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean,na.rm=TRUE)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1,na.rm=TRUE),mean(dat$Year2,na.rm=TRUE),mean(dat$Year3,na.rm=TRUE),mean(dat$Year4,na.rm=TRUE),mean(dat$Year5,na.rm=TRUE),mean(dat$Year6,na.rm=TRUE))

#data for average across clones
day1=120
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

day1= trumweather2[trumweather2$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","trum")

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","trum")

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","trum")


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","trum")


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","trum")

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(trumweather2,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","trum")


##########################################################
#################################################################33
##############################################################llan using NCEP data
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/llanwrst.RData")
lougweather=willowgrowclimate
data=ukdata[ukdata$PLOT_NAME=="Llanwrst",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1,na.rm=TRUE),mean(dat$Year2,na.rm=TRUE),mean(dat$Year3,na.rm=TRUE),mean(dat$Year4,na.rm=TRUE),mean(dat$Year5,na.rm=TRUE),mean(dat$Year6,na.rm=TRUE))
day1=150
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

#day1= lougweather[lougweather$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Llanwrst")

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Llanwrst")

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Llanwrst")


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Llanwrst")


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Llanwrst")

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Llanwrst")

#######################################################3
#######################################################
#########################################################
##Talybont NCEP DATA
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/talybont.RData")
lougweather=willowgrowclimate
data=ukdata[ukdata$PLOT_NAME=="Talybont",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1,na.rm=TRUE),mean(dat$Year2,na.rm=TRUE),mean(dat$Year3,na.rm=TRUE),mean(dat$Year4,na.rm=TRUE),mean(dat$Year5,na.rm=TRUE),mean(dat$Year6,na.rm=TRUE))
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

#day1= lougweather[lougweather$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Talybont")

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Talybont")

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Talybont")


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Talybont")


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Talybont")

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(lougweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","Talybont")

#########################################################################################
###########################################################################################
###################uk sites 
##alice holt
ukdata <- read.csv("/home/dwng/ukdata.csv",header=TRUE,na.string="")
load("~/weather/aliceholt.RData")
aliceweather=willowgrowclimate
data=ukdata[ukdata$PLOT_NAME=="Alice Holt",]
data1=data[data$CLONE  %in% c("Jorunn","Jorr","Orm","Q83","Bebbian","Bjorn","Dasyclados","Delamere","Germany","Spaethii","ST248155","Stott10","Stott11","Tora","Ulv","V789"),]
data2=data[data$CLONE=="Jorunn",]
dat=aggregate(x=data1,by=list(data1$CLONE),mean)
#data for Jorunn
obsdata=dat[6,]
obsdata=c(mean(dat$Year1,na.rm=TRUE),mean(dat$Year2,na.rm=TRUE),mean(dat$Year3,na.rm=TRUE),mean(dat$Year4,na.rm=TRUE),mean(dat$Year5,na.rm=TRUE),mean(dat$Year6,na.rm=TRUE))
day1=150
#data for average across clones
iplant0 <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=1.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)

res0 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant0,soilControl=soilwillow)
stem0<-res0$Stem[length(res0$Stem)]
root0<-res0$Root[length(res0$Root)]
rhiz0<-res0$Rhizome[length(res0$Rhizome)]

stem0=stem0*0.05

#day1= aliceweather[aliceweather$Temp >=6,]$doy[1] 
iplant1 <-iwillowParms(iRhizome=rhiz0,iStem=stem0,iLeaf=0.0,iRoot=root0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res1 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant1,soilControl=soilwillow)
stem1<-res1$Stem[length(res1$Stem)]
root1<-res1$Root[length(res1$Root)]
rhiz1<-res1$Rhizome[length(res1$Rhizome)]
res=res1# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[1])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","aliceholt")

stem1=stem1*0.9
iplant2 <-iwillowParms(iRhizome=rhiz1,iStem=stem1,iLeaf=0.0,iRoot=root1,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res2 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant2,soilControl=soilwillow)

stem2<-res2$Stem[length(res2$Stem)]
root2<-res2$Root[length(res2$Root)]
rhiz2<-res2$Rhizome[length(res2$Rhizome)]
res=res2# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[2])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","aliceholt")

stem2=stem2*0.9
iplant3 <-iwillowParms(iRhizome=rhiz2,iStem=stem2,iLeaf=0.0,iRoot=root2,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res3 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant3,soilControl=soilwillow)
stem3<-res3$Stem[length(res3$Stem)]
root3<-res3$Root[length(res3$Root)]
rhiz3<-res3$Rhizome[length(res3$Rhizome)]

res=res3# root age 4, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[3])
tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","aliceholt")


########################harvested after third year
stem3=stem3*0.05
iplant4 <-iwillowParms(iRhizome=rhiz3,iStem=stem3,iLeaf=0.0,iRoot=root3,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res4 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant4,soilControl=soilwillow)
stem4<-res4$Stem[length(res4$Stem)]
root4<-res4$Root[length(res4$Root)]
rhiz4<-res4$Rhizome[length(res4$Rhizome)]

res=res4# root age 4, shoot age 1

DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[4])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","aliceholt")


stem4=stem4*0.9
iplant5 <-iwillowParms(iRhizome=rhiz4,iStem=stem4,iLeaf=0.0,iRoot=root4,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res5 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant5,soilControl=soilwillow)
stem5<-res5$Stem[length(res5$Stem)]
root5<-res5$Root[length(res5$Root)]
rhiz5<-res5$Rhizome[length(res5$Rhizome)]

res=res5# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[5])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","aliceholt")

stem5=stem5*0.9
iplant6 <-iwillowParms(iRhizome=rhiz5,iStem=stem5,iLeaf=0.0,iRoot=root5,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
res6 <- willowGro(aliceweather,day1=day1,dayn=330,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,
                  iPlantControl=iplant6,soilControl=soilwillow)
stem6<-res6$Stem[length(res6$Stem)]
root6<-res6$Root[length(res6$Root)]
rhiz6<-res6$Rhizome[length(res6$Rhizome)]

res=res6# root age 5, shoot age 1
DAP<-to.dap(res$DayofYear)
obs <- data.frame(DAP=max(DAP), Stem=obsdata[6])

tmpobs<-data.frame(DAP=obs$DAP,Stem=obs$Stem)
tmppred<-data.frame(DAP=DAP,Stem=res$Stem)
compareStem<-SelectPredictStem(tmpobs,tmppred,compareStem,"Aylott 2008","aliceholt")



save(compareStem,file="/home/dwng/willowfigure/v091.RData")
library(ggplot2)
#compareStem=compareStem[11:66,]
df=compareStem[,c(1,2)]
names(df)=c("x","y")

lm_eqn = function(df){
  m = lm(y ~ x, df);
  eq <- substitute(italic(y) == a + b %.% italic(x)*","~~italic(r)^2~"="~r2,
                   list(a = format(coef(m)[1], digits = 2), 
                        b = format(coef(m)[2], digits = 2), 
                        r2 = format(summary(m)$r.squared, digits = 3)))
  #p = format(summary(m)$coefficients[8],digits=2))
  as.character(as.expression(eq));                 
}

stemplot <-ggplot(compareStem,aes(Observed,Predicted))+geom_point(aes(colour=factor(Irrigated)),size=3.5,alpha=1)+stat_smooth(method="lm")+
  scale_x_continuous("Observed willow yield (Mg/ha)") + # have tick marks for each session
  scale_y_continuous("Predicted willow yield (Mg/ha)") +# rescale Y axis slightly
  
  theme_bw() + # make the theme black-and-white rather than grey (do this before font changes, or it overrides them)
  opts(axis.title.x = theme_text(face="bold", size=14),
       axis.title.y = theme_text(face="bold", size=14, angle=90),
       panel.grid.major = theme_blank(), # switch off major gridlines
       panel.grid.minor = theme_blank(),
       legend.position='none')
stemplot <- stemplot+geom_text(aes(x = 14, y = 45, label = lm_eqn(df)), parse = TRUE,colour="black")+geom_text(aes(x = 4, y = 48, label = "n=74"))+geom_text(aes(x = 6, y = 42, label = "RMSE=6.9"))
stemplot=stemplot+geom_abline(intercep=0,slope=1,colour="green")
jpeg("/home/dwng/willowfigure/095.jpg", width=6.5, height=5, units="in", res=600) 
stemplot
dev.off()
library(hydroGOF)
rmse=rmse(df$x, df$y, na.rm=TRUE)

#get the RMSE
r.rmse <- sqrt(mean(r$residuals^2))

library(nlme)
r <- lme(conc ~ age, data=IGF)

#get the RMSE
r.rmse <- sqrt(mean(r$residuals^2))

stemuk = compareStem[33:74,]
names(stemuk)=c("Observed","Predicted","citation","site")
stemuk$year=rep(1:6,7)
stemuk[stemuk$site=='Talybont',]$site="Taly"
stemuk[stemuk$site=='Llanwrst',]$site="Llan"
stemuk[stemuk$site=='tref',]$site="Tref"
stemuk[stemuk$site=='trum',]$site="Trum"
stemuk[stemuk$site=='aliceholt',]$site="Alic"
stemuk[stemuk$site=='balb',]$site="Balb"
stemuk[stemuk$site=='loyt',]$site="Loyt"
stemuk$rotation=rep(1:3,14)
stemuk$pred=stemuk$Predicted/stemuk$rotation
stemuk$obs=stemuk$Observed/stemuk$rotation
rmse(stemuk$obs,stemuk$pred)
fitannual=lm(stemuk$obs~stemuk$pred)
jpeg("/home/dwng/willowfigure/validationuk.jpg", width=8, height=5, units="in", res=600) 
xyplot(Observed+Predicted~year| factor(site),type="o", data=stemuk, xlab="year",col=c("black","grey"),lwd=1.5,pch=19,
       ylab="Yield (Mg/ha)", key =
         list(text=list(c("Observed","Predicted")),space = "top", points = TRUE, lines = FALSE,col=c("black","grey"),cex=1,pch=19))
dev.off()
mean(stemuk[stemuk$year==c(1,2,3),]$obs)
mean(stemuk[stemuk$year==c(4,5,6),]$obs)
mean(stemuk[stemuk$year==c(1,2,3),]$pred)
mean(stemuk[stemuk$year==c(4,5,6),]$pred)