
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
result<-CropGro(WetDat=urbana2008to2012,day1=1,dayn=1200,lat=40.11,iRhizome=0.2,photoControl=list(alpha=0.055),
        soilControl=soilParms(wsFun="none",soilLayers=10,soilDepth=1),phenoControl=phenoParms(kLeaf1=0.65,kStem1=0.05,tp1=900),
        canopyControl=canopyParms(Sp=1.6))
       
plot(result)
png("./inst/test/dailyTT.png")
xyplot(result$GDD~result$DayafterPlanting,type="l")
dev.off()
png("./inst/test/GPP.png")
xyplot(result$GPP~result$DayafterPlanting,type="l")
dev.off()
png("./inst/test/NPP.png")
xyplot(result$NPP~result$DayafterPlanting,type="l")
dev.off()
png("./inst/test/autoRESP.png")
xyplot(result$autoRESP~result$DayafterPlanting,type="l")
dev.off()
xyplot(result$LAI~result$DayafterPlanting,type="l")
if(result$GPP==0)result$GPP=result$autoRESP=1.0
if(result$GPP!=0){
Gpppercentlost<-100*result$autoRESP/result$GPP;
}else {
  Gpppercentlost=100
}
xyplot(Gpppercentlost~result$DayafterPlanting,type="l")
summary(Gpppercentlost)
png("./inst/test/stem.png")
xyplot(result$Stem~result$DayafterPlanting,type="l")
dev.off()
png("./inst/test/rhizome.png")
xyplot(result$Rhizome~result$DayafterPlanting,type="l")
dev.off()
png("./inst/test/Leaf.png")
xyplot(result$Leaf~result$DayafterPlanting,type="l")
dev.off()
