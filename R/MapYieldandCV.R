

MapYieldDifference<-function(first.year,last.year,ResultFolder1,ResultFolder2,mapfolder,shapefilelocation,yieldbreakpoints=NULL,cvbreakpoints=NULL,nclr=NULL,cvmax=NULL){
  library(maps)
library(sp)
library(maptools)
gpclibPermit()
library(RColorBrewer)
library(classInt)

ReadBioCroOutput<-function(ResultsFolder,first.year,last.year) {
       resUS<-NULL
       for(i in first.year:last.year){
           tmp <- try(read.table(paste(ResultsFolder,"/USresSC.",i,".txt",sep="")),silent=TRUE)
              if(class(tmp) != "try-error"){
              tmp<-summaryBy(V2 +V3 +V4+V5+V6+V7+V8+V9+V10+V11+V12~ V1,data=tmp,FUN=mean,keep.names=TRUE)
              resUS <- rbind(resUS,tmp)
                                          }
                                    }
              resUS
     }
  

EvaluateMeanAndCV<-function (resUS,varname) {
    if(varname=="StemDB1"){
         ttemp2<-summaryBy(Lat + Lon + StemDB1 ~ ID,data=resUS,FUN=sd)
         ttemp2<-na.omit(ttemp2)
         ttemp<-summaryBy(Lat + Lon + StemDB1 ~ ID,data=resUS,FUN=mean)
         res.dat <- data.frame(Lat=ttemp$Lat.mean,Lon=ttemp$Lon.mean,harvDB=ttemp$StemDB1.mean)
         res.dat <- na.omit(res.dat)
         harvDB1.sd<-ttemp2$StemDB1.sd
         ttemp2<-cbind(ttemp2,harvDB1.sd)
         res.dat<-cbind(res.dat,ttemp2$harvDB1.sd)
         cv<-res.dat[,4]/res.dat[,3]
         res.dat<-cbind(res.dat,cv) 
                           }
         res.dat
  }

# breakpoints<-c(0,5,10,15,20,25,30,35,40,45,50) 
DefineMapScheme<-function(datatoplot,breakpoints=NULL,nclr=NULL){

        if(!is.numeric(breakpoints)){
        if(!is.numeric(nclr)) nclr=10
            plotclr <- brewer.pal(nclr,"Spectral")
            plotclr <- plotclr[length(plotclr):1]
            class <- classIntervals(datatoplot, nclr, style="pretty")
            colcode <- findColours(class, plotclr)
             } else {
                nclr<-length(breakpoints)-1
                plotclr <- brewer.pal(nclr,"Spectral")
                plotclr <- plotclr[length(plotclr):1]
                class <- classIntervals(datatoplot, nclr, style="pretty")
                class$brks <-breakpoints
                colcode <- findColours(class, plotclr)
              }
           colcode
       }
       
plotcolorpoints<-function(shapefilelocation,datatoplot,mapfolder,outfile,colcode){
  shapefile<-readShapePoly(shapefilelocation)
  pts <- SpatialPoints(cbind(datatoplot$Lon,datatoplot$Lat))
  sze<-1000
  png(paste(mapfolder,outfile,sep=""), height = sze, width = sze, res = 120)
  par(mar=c(0,0,0,0),oma=c(0,0,0,0))
  plot(shapefile)
  plot(pts,col=colcode,cex=0.76,pch=8,add=TRUE)
  plot(shapefile,add=TRUE)
  legend("bottomright", legend=names(attr(colcode, "table")),
  fill=attr(colcode, "palette"), cex=0.9, bty="n")
  text(-100,53,labels=outfile)
  dev.off()
  return
}

  # Read From Folder 1 Results
  resUS1<-ReadBioCroOutput(ResultsFolder1,first.year,last.year)
colnames(resUS1) <- c("ID","Lat","Lon",
                     "StemDB1","LeafDB1",
                     "RootDB1","LAI","fieldc","wiltp",
                     "mdep","climLAT","climLON")
##Here, I am using a harvest factor of 0.8 [For Sugarcane only] to deduce harvesting losses or Harvesting Index Factor
resUS1$StemDB1<-resUS1$StemDB1*0.8
# I should make harvest Index as an Factor in BioCro to avoid this calculation here
# This is species dependent

    # Read From Folder 2 Results
  resUS2<-ReadBioCroOutput(ResultsFolder2,first.year,last.year)
colnames(resUS2) <- c("ID","Lat","Lon",
                     "StemDB1","LeafDB1",
                     "RootDB1","LAI","fieldc","wiltp",
                     "mdep","climLAT","climLON")
##Here, I am using a harvest factor of 0.8 [For Sugarcane only] to deduce harvesting losses or Harvesting Index Factor
resUS2$StemDB1<-resUS2$StemDB1*0.8
# I should make harvest Index as an Factor in BioCro to avoid this calculation here
# This is species dependent



#Create data framr of mean and CV of harvestable  stem Dry biomass only
res.dat1<-EvaluateMeanAndCV(resUS1,"StemDB1")
res.dat2<-EvaluateMeanAndCV(resUS2,"StemDB1")

# Evaluate Difference between Folder1 and Folder 2
  res.dat<-res.dat1
  res.dat$harvDB<-res.dat1$harvDB-res.dat2$harvDB
  res.dat$cv<-res.dat1$cv-res.dat2$cv
  
  
#Plotting Mean Yields Difference
colorforyield<-DefineMapScheme(datatoplot=res.dat$harvDB,breakpoints=yieldbreakpoints)
plotcolorpoints(shapefilelocation,res.dat,mapfolder,outfile="yield.png",colcode=colorforyield)

# Plotting CV of Yields Difference
if(!is.null(cvmax))res.dat[res.dat$cv>cvmax,]$cv=cvmax
colorforyield<-DefineMapScheme(datatoplot=res.dat$cv,breakpoints=cvbreakpoints)
plotcolorpoints(shapefilelocation,res.dat,mapfolder,outfile="cv.png",colcode=colorforyield)

return

}


MapYieldandCV<-function(first.year,last.year,ResultFolder,mapfolder,shapefilelocation,yieldbreakpoints=NULL,cvbreakpoints=NULL,nclr=NULL,cvmax=NULL) {
library(maps)
library(sp)
library(maptools)
gpclibPermit()
library(RColorBrewer)
library(classInt)

ReadBioCroOutput<-function(ResultsFolder,first.year,last.year) {
       resUS<-NULL
       for(i in first.year:last.year){
           tmp <- try(read.table(paste(ResultsFolder,"/USresSC.",i,".txt",sep="")),silent=TRUE)
              if(class(tmp) != "try-error"){
              tmp<-summaryBy(V2 +V3 +V4+V5+V6+V7+V8+V9+V10+V11+V12~ V1,data=tmp,FUN=mean,keep.names=TRUE)
              resUS <- rbind(resUS,tmp)
                                          }
                                    }
              resUS
     }

EvaluateMeanAndCV<-function (resUS,varname) {
    if(varname=="StemDB1"){
         ttemp2<-summaryBy(Lat + Lon + StemDB1 ~ ID,data=resUS,FUN=sd)
         ttemp2<-na.omit(ttemp2)
         ttemp<-summaryBy(Lat + Lon + StemDB1 ~ ID,data=resUS,FUN=mean)
         ttemp<-na.omit(ttemp)
         res.dat <- data.frame(Lat=ttemp$Lat.mean,Lon=ttemp$Lon.mean,harvDB=ttemp$StemDB1.mean)
         res.dat <- na.omit(res.dat)
         harvDB1.sd<-ttemp2$StemDB1.sd
         ttemp2<-cbind(ttemp2,harvDB1.sd)
         res.dat<-cbind(res.dat,ttemp2$harvDB1.sd)
         cv<-res.dat[,4]/res.dat[,3]
         res.dat<-cbind(res.dat,cv) 
                           }
         res.dat
  }

# breakpoints<-c(0,5,10,15,20,25,30,35,40,45,50) 
DefineMapScheme<-function(datatoplot,breakpoints=NULL,nclr=NULL){

        if(!is.numeric(breakpoints)){
        if(!is.numeric(nclr)) nclr=10
            plotclr <- brewer.pal(nclr,"Spectral")
            plotclr <- plotclr[length(plotclr):1]
            class <- classIntervals(datatoplot, nclr, style="pretty")
            colcode <- findColours(class, plotclr)
             } else {
                nclr<-length(breakpoints)-1
                plotclr <- brewer.pal(nclr,"Spectral")
                plotclr <- plotclr[length(plotclr):1]
                class <- classIntervals(datatoplot, nclr, style="pretty")
                class$brks <-breakpoints
                colcode <- findColours(class, plotclr)
              }
           colcode
       }
       
plotcolorpoints<-function(shapefilelocation,datatoplot,mapfolder,outfile,colcode){
  shapefile<-readShapePoly(shapefilelocation)
  pts <- SpatialPoints(cbind(datatoplot$Lon,datatoplot$Lat))
  sze<-1000
  png(paste(mapfolder,outfile,sep=""), height = sze, width = sze, res = 120)
  par(mar=c(0,0,0,0),oma=c(0,0,0,0))
  plot(shapefile)
  plot(pts,col=colcode,cex=0.76,pch=8,add=TRUE)
  plot(shapefile,add=TRUE)
  legend("bottomright", legend=names(attr(colcode, "table")),
  fill=attr(colcode, "palette"), cex=0.9, bty="n")
  text(-100,53,labels=outfile)
  dev.off()
  return
}

  resUS<-ReadBioCroOutput(ResultsFolder,first.year,last.year)
colnames(resUS) <- c("ID","Lat","Lon",
                     "StemDB1","LeafDB1",
                     "RootDB1","LAI","fieldc","wiltp",
                     "mdep","climLAT","climLON")
##Here, I am using a harvest factor of 0.8 [For Sugarcane only] to deduce harvesting losses or Harvesting Index Factor
resUS$StemDB1<-resUS$StemDB1*0.8
# I should make harvest Index as an Factor in BioCro to avoid this calculation here
# This is species dependent


#Create data framr of mean and CV of harvestable  stem Dry biomass only
res.dat<-EvaluateMeanAndCV(resUS,"StemDB1")

#Plotting Mean Yields
colorforyield<-DefineMapScheme(datatoplot=res.dat$harvDB,breakpoints=yieldbreakpoints)
plotcolorpoints(shapefilelocation,res.dat,mapfolder,outfile="yield.png",colcode=colorforyield)

# Plotting CV of Yields
if(!is.null(cvmax))res.dat[res.dat$cv>cvmax,]$cv=cvmax
colorforyield<-DefineMapScheme(datatoplot=res.dat$cv,breakpoints=cvbreakpoints)
plotcolorpoints(shapefilelocation,res.dat,mapfolder,outfile="cv.png",colcode=colorforyield)

return

}


# Plotting Results from Rainfed conditions
ResultsFolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED/Results"
mapfolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED/maps/"
shapefilelocation<-"/home/djaiswal/Research/Simulations/maps/brazil/brazilsoil/BRA_adm1.shp"
last.year<-2011
first.year<-1948

MapYieldandCV(first.year=1948,last.year=2011,ResultFolder=ResultFolder,shapefilelocation=shapefilelocation,mapfolder=mapfolder,cvbreakpoints=NULL,yieldbreakpoints=NULL,cvmax=0.5)

# Plotting Results from Elevated Temperature Conditions (+1 C)
ResultsFolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED.T.PLUS.1/Results"
mapfolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED.T.PLUS.1/maps/"
shapefilelocation<-"/home/djaiswal/Research/Simulations/maps/brazil/brazilsoil/BRA_adm1.shp"
last.year<-2011
first.year<-1948

MapYieldandCV(first.year=1948,last.year=2011,ResultFolder=ResultFolder,shapefilelocation=shapefilelocation,mapfolder=mapfolder,cvbreakpoints=NULL,yieldbreakpoints=NULL,cvmax=0.5)


# Plotting Results from Elevated Temperature Conditions (+1 C) and Increased Co2 (500 ppm)
ResultsFolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED.T.PLUS1.CO2.500/Results"
mapfolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED.T.PLUS1.CO2.500/maps/"
shapefilelocation<-"/home/djaiswal/Research/Simulations/maps/brazil/brazilsoil/BRA_adm1.shp"
last.year<-2011
first.year<-1948

MapYieldandCV(first.year=1948,last.year=2011,ResultFolder=ResultFolder,shapefilelocation=shapefilelocation,mapfolder=mapfolder,cvbreakpoints=NULL,yieldbreakpoints=NULL,cvmax=0.5)

# Difference Map between Temperature and Current
ResultsFolder2<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED/Results"
ResultsFolder1<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED.T.PLUS.1/Results"
mapfolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/DIFFERENCEMAP/TEMPONLY/"
shapefilelocation<-"/home/djaiswal/Research/Simulations/maps/brazil/brazilsoil/BRA_adm1.shp"
last.year<-2011
first.year<-1948
MapYieldDifference(first.year=first.year,last.year=last.year,ResultFolder1=ResultFolder1,ResultFolder2=ResultFolder2,shapefilelocation=shapefilelocation,mapfolder=mapfolder,
                   yieldbreakpoints=c(-3.5,-3.0,-2.5,-2.0,-1.5,-1.0,-0.5,0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0),cvbreakpoints=NULL,cvmax=0.5)


# Difference Map between Temperature+co2 and Current
ResultsFolder2<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED/Results"
ResultsFolder1<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/RAINFED.T.PLUS1.CO2.500/Results"
mapfolder<-"/home/djaiswal/EBI-cluster/Simulations/Brazil/FUTURECLIMATE/DIFFERENCEMAP/TEMPANDCO2/"
shapefilelocation<-"/home/djaiswal/Research/Simulations/maps/brazil/brazilsoil/BRA_adm1.shp"
last.year<-2011
first.year<-1948
MapYieldDifference(first.year=first.year,last.year=last.year,ResultFolder1=ResultFolder1,ResultFolder2=ResultFolder2,shapefilelocation=shapefilelocation,mapfolder=mapfolder,
                   yieldbreakpoints=c(-3.5,-3.0,-2.5,-2.0,-1.5,-1.0,-0.5,0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0),cvbreakpoints=NULL,cvmax=0.5)

