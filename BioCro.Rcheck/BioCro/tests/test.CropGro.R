
# This chunk of code downloads and save the climate data for Urbana from 2008 to 2012 
# I have commentd out this section because we can now simply use saved data in the data folder

################################################################################################################################
# Obtaining Climate data for Urbana from 2008 to 2012
# urbanaclim<-InputForWeach(lat=40.11, lon=-88.20,year1=2008,year2=2012)

#Formating data to use in CropGro
# urbana2008to2012<-weachNEW(urbanaclim,lat=40.11,ts=1, temp.units="Celsius", rh.units="fraction",ws.units="mph",pp.units="in")

# saving the data
# save(urbana2008to2012, file ="/home/djaiswal/Desktop/CropCent/biocro/data/urbana2008to2012.rda")

################################################################################################################################
# Testing for multiyear Simulation
data(urbana2008to2012)
result <- CropGro(WetDat = urbana2008to2012,
                  day1 = 1,
                  dayn = 360,
                  lat = 40.11,
                  iRhizome = 8,
                  photoControl = list(alpha=0.05),
                  soilControl = soilParms(wsFun = "none", soilLayers = 10,
                      soilDepth = 1),
                  phenoControl = phenoParms(kLeaf1 = 0.35, kStem1 = 0.35),
                  canopyControl = canopyParms(Sp = 1.6))

xyplot(result$Stemd+result$Leafd+result$Rootd+result$Rhizomed~result$DayafterPlanting, type="l",auto.key=TRUE)
xyplot(result$Stemlitterd+result$Leaflitterd+result$Rootlitterd+result$Rhizomelitterd~result$DayafterPlanting, type="l",auto.key=TRUE)
plot(result)
## test_that("")
## xyplot(result$Stemd+result$Leafd+result$Rootd+result$Rhizomed+result$LAId~result$DayafterPlanting,auto.key=TRUE, type="l", main="alive biomass")



## xyplot(result$Stemd+result$Leafd+result$Rootd+result$Rhizomed+result$LAId~result$GDD,auto.key=TRUE, type="p", main="alive biomass")
## xyplot(result$Stemlitterd+result$Leaflitterd+result$Rootlitterd+result$Rhizomelitterd~result$GDD,auto.key=TRUE, type="p", main="dead biomass")


## checkmassbalance<-function(result,plantingrate,dapharvest, stemfr)
## {
##   N<-length(result$GDD)
##   Cbalance<-as.numeric(N)
##   Cbalancee<-0
##   deltalive=(result$Stemd[1]+result$Leafd[1]+result$Rootd[1]+result$Rhizomed[1])-plantingrate
##   deltadead=(result$Stemlitterd[1]+result$Leaflitterd[1]+result$Rootlitterd[1]+result$Rhizomelitterd[1])
##   Cbalance[1]=result$NPP[1]-(deltalive+deltadead)
##   for (i in 2:N)
##   {
##     deltalive=(result$Stemd[i]+result$Leafd[i]+result$Rootd[i]+result$Rhizomed[i])-(result$Stemd[i-1]+result$Leafd[i-1]+result$Rootd[i-1]+result$Rhizomed[i-1])
##     deltadead=(result$Stemlitterd[i]+result$Leaflitterd[i]+result$Rootlitterd[i]+result$Rhizomelitterd[i])-(result$Stemlitterd[i-1]+result$Leaflitterd[i-1]+result$Rootlitterd[i-1]+result$Rhizomelitterd[i-1])
##     Cbalance[i]=result$NPP[i]-(deltalive+deltadead)
##   }
##   harvested=result$Stemd[dapharvest-1]*(stemfr)+result$Leafd[dapharvest-1]*(leaffr)
##  ##  Cbalance[dapharvest]= Cbalance[dapharvest]-harvested
##   }


## plot(result)
## png("./inst/test/dailyTT.png")
## xyplot(result$GDD~result$DayafterPlanting,type="l")
## dev.off()
## png("./inst/test/GPP.png")
## xyplot(result$GPP~result$DayafterPlanting,type="l")
## dev.off()
## png("./inst/test/NPP.png")
## xyplot(result$NPP~result$DayafterPlanting,type="l")
## dev.off()
## png("./inst/test/autoRESP.png")
## xyplot(result$autoRESP~result$DayafterPlanting,type="l")
## dev.off()
## xyplot(result$LAI~result$DayafterPlanting,type="l")
## plot(result$autoRESP~result$GPP)
## lm(result$autoRESP~result$GPP)
 
## xyplot(I(result$RootMResp+result$RhizomeMResp)~result$GDD, auto.key=TRUE)
## xyplot(Gpppercentlost~result$DayafterPlanting,type="l")
## summary(Gpppercentlost)
## png("./inst/test/stem.png")
## xyplot(result$Stem~result$DayafterPlanting,type="l")
## dev.off()
## png("./inst/test/rhizome.png")
## xyplot(result$Rhizome~result$DayafterPlanting,type="l")
## dev.off()
## png("./inst/test/Leaf.png")
## xyplot(result$Leaf~result$DayafterPlanting,type="l")
## dev.off()

