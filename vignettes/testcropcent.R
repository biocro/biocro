
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
data20yr<-rbind(urbana2008to2012,urbana2008to2012,urbana2008to2012,urbana2008to2012)
result<-CropGro(WetDat=data20yr,day1=50,dayn=5000,lat=40.11,iRhizome=8,photoControl=list(alpha=0.04),
        soilControl=soilParms(wsFun="none",soilLayers=10,soilDepth=1),phenoControl=phenoParms(kLeaf1=0.35,kStem1=0.35),
        canopyControl=canopyParms(Sp=1.6))
result<-willowGro(WetDat=data20yr,day1=50,dayn=350,lat=40.11)
plot(result)
N<-length(result$GDD)

initialbiomass<-result$Stemd[1]+result$Rootd[1]+result$Rhizomed[1]+result$Leafd[1]
finalbiomass<-result$Stemd[N]+result$Rootd[N]+result$Rhizomed[N]+result$Leafd[N]
delbiomass=finalbiomass-initialbiomass

initialbiomasslitter<-result$Stemlitterd[1]+result$Rootlitterd[1]+result$Rhizomelitterd[1]+result$Leaflitterd[1]
finalbiomasslitter<-result$Stemlitterd[N]+result$Rootlitterd[N]+result$Rhizomelitterd[N]+result$Leaflitterd[N]
dellitter=finalbiomasslitter-initialbiomasslitter

harvestedstem<-sum(result$HarvestedStem)
harvestedLeaf<-sum(result$HarvestedLeaf)
totalGPP<-sum(result$GPP[1:N])
totalNPP<-sum(result$NPP[1:N])
totalResp<-sum(result$autoRESP[1:N])

balance1=totalGPP-totalNPP-totalResp
balance2=totalNPP-delbiomass-dellitter-harvestedstem-harvestedLeaf

xyplot(result$Stemd+result$Leafd+result$Rootd+result$Rhizomed+result$LAId~result$DayafterPlanting,auto.key=TRUE, type="l", main="alive biomass")
xyplot(result$Stemd+result$Leafd+result$Rootd+result$Rhizomed+result$LAId~result$GDD,auto.key=TRUE, type="p", main="alive biomass")
xyplot(result$Stemlitterd+result$Leaflitterd+result$Rootlitterd+result$Rhizomelitterd~result$GDD,auto.key=TRUE, type="p", main="dead biomass")

# carbonbalance<-data.frame(GPP=NA,NPP=NA,AutoResp=NA, Balance1=NA)
checkmassbalance<-function(result,plantingrate,dapharvest, stemfr)
{
  N<-length(result$GDD)
  Cbalance<-as.numeric(N)
  Cbalance<-0
  deltalive=(result$Stemd[1]+result$Leafd[1]+result$Rootd[1]+result$Rhizomed[1])-plantingrate
  deltadead=(result$Stemlitterd[1]+result$Leaflitterd[1]+result$Rootlitterd[1]+result$Rhizomelitterd[1])
  Cbalance[1]=result$NPP[1]-(deltalive+deltadead)
  carbonbalance$GPP=result$GPP[1]
  carbonbalance$NPP=result$NPP[1]
  carbonbalance$AutoResp=result$autoRESP[1]
  carbonbalance$Balance1=carbonbalance$GPP-carbonbalance$NPP-  carbonbalance$AutoResp
  carbonbalance$delrhiz=result$Rhizomed[1]-20.0
  carbonbalance$delstem=0
  carbonbalance$delleaf=result$Leafd[1]-0.0
  carbonbalance$delroot=0
  carbonbalance$delalivebiomass= carbonbalance$delrhiz+carbonbalance$delstem+carbonbalance$delleaf+carbonbalance$delroot
  carbonbalance$delrhizlitter=0.0
  carbonbalance$delstemlitter=0.0
  carbonbalance$delleaflitter=0.0
  carbonbalance$delrootlitter=0.0
  carbonbalance$delbiomasslitter= carbonbalance$delrhizlitter+carbonbalance$delstemlitter+carbonbalance$delleaflitter+carbonbalance$delrootlitter
  carbonbalance$Balance2=carbonbalance$delbiomasslitter+carbonbalance$delalivebiomass-carbonbalance$NPP
  write.table(carbonbalance,file="/home/djaiswal/Research/dev/biocro/tests/Cabalance.txt", row.names=FALSE,append=FALSE,col.names=TRUE)
  for (i in 2:N)
  {
    deltalive=(result$Stemd[i]+result$Leafd[i]+result$Rootd[i]+result$Rhizomed[i])-(result$Stemd[i-1]+result$Leafd[i-1]+result$Rootd[i-1]+result$Rhizomed[i-1])
    deltadead=(result$Stemlitterd[i]+result$Leaflitterd[i]+result$Rootlitterd[i]+result$Rhizomelitterd[i])-(result$Stemlitterd[i-1]+result$Leaflitterd[i-1]+result$Rootlitterd[i-1]+result$Rhizomelitterd[i-1])
    Cbalance[i]=result$NPP[i]-(deltalive+deltadead)
    carbonbalance$GPP=result$GPP[i]
    carbonbalance$NPP=result$NPP[i]
    carbonbalance$AutoResp=result$autoRESP[i]
    carbonbalance$Balance1=carbonbalance$GPP - carbonbalance$NPP -  carbonbalance$AutoResp
    
    carbonbalance$delrhiz=result$Rhizomed[i]-result$Rhizomed[i-1]
    carbonbalance$delstem=result$Stemd[i]-result$Stemd[i-1]
    carbonbalance$delleaf=result$Leafd[i]-result$Leafd[i-1]
    carbonbalance$delroot=result$Rootd[i]-result$Rootd[i-1]
    carbonbalance$delalivebiomass= carbonbalance$delrhiz+carbonbalance$delstem+carbonbalance$delleaf+carbonbalance$delroot
    carbonbalance$delrhizlitter=result$Rhizomelitterd[i]-result$Rhizomelitterd[i-1]
    carbonbalance$delstemlitter=result$Stemlitterd[i]-result$Stemlitterd[i-1]
    carbonbalance$delleaflitter=result$Leaflitterd[i]-result$Leaflitterd[i-1]
    carbonbalance$delrootlitter=result$Rootlitterd[i]-result$Rootlitterd[i-1]
    carbonbalance$delbiomasslitter= carbonbalance$delrhizlitter+carbonbalance$delstemlitter+carbonbalance$delleaflitter+carbonbalance$delrootlitter
    carbonbalance$Balance2=carbonbalance$delbiomasslitter+carbonbalance$delalivebiomass-carbonbalance$NPP
    
    write.table(carbonbalance,file="/home/djaiswal/Research/dev/biocro/tests/Cabalance.txt", row.names=FALSE,append=TRUE,col.names=FALSE)
  }
  harvested=result$Stemd[dapharvest-1]*(stemfr)+result$Leafd[dapharvest-1]*(leaffr)
 ##  Cbalance[dapharvest]= Cbalance[dapharvest]-harvested
  }
cbalance<-read.table(file="/home/djaiswal/Research/dev/biocro/tests/Cabalance.txt",header=TRUE)

xyplot(Cbalance~result$DayafterPlanting)

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
plot(result$autoRESP~result$GPP)
lm(result$autoRESP~result$GPP)
plot(result$NPP~result$GPP)

xyplot(I(result$RootMResp+result$RhizomeMResp)~result$GDD, auto.key=TRUE)
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

