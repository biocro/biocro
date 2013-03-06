met2010 <- read.csv("~/biocro-dan/data/met2010.csv")
names(met2010) <- c("year","obs","month","day","hour","minute","solorR","temp","RH","prec","ws")
met2010$RH=met2010$RH/100
met2010$lai=8
tmp2 <- matrix(ncol=5,nrow=8760)
tmp3 <- matrix(ncol=4,nrow=8760)
layers <- 8
dat2 <- null
met2010$day
for(i in 1:8760){
  lai <- met2010[i,12]
  doy <- met2010[i,4]
  hr  <- met2010[i,5]
  solar <- met2010[i,7]
  temp <- met2010[i,8]
  rh <- met2010[i,9]
  ws <- met2010[i,11]
  
  ##       tmp1 <- CanA(lai,doy,hr,solar,temp,rh,ws, StomataWS=1, nlayers=layers, lnControl=lnParms(LeafN=2,lnFun="linear",kpLN=0))
  tmp1 <- c3CanA(lai,doy,hr,solar,temp,rh, ws,)
  tmp2[i,1] <- tmp1$CanopyAssim
  tmp2[i,2] <- tmp1$CanopyTrans
  tmp2[i,3] <- tmp1$TranEpen
  tmp2[i,4] <- tmp1$TranEpries
  tmp2[i,5] <- tmp1$CanopyCond
  dat1 <- data.frame(hour=i,layer=1:layers, as.data.frame(tmp1$LayMat))
  dat2 <- rbind(dat2,dat1)
  }

met2011 <- read.csv("~/biocro-dan/data/met2011.csv") 


     ## Plot of Irradiance for the 10 layers
     xyplot(IDir + IDiff ~ hour | factor(layer),type="o",
      data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Irradiance (",mu,"mol ",m^-2," ",s^-1,")")))

     ## Plot of TempDiff for the 10 layers
     xyplot(DeltaSun + DeltaShade ~ hour | factor(layer),type="o",
      data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
            ylab="Delta temperature (Celsius)")


## Plot of Leaf area (sunlit and shaded) for the 10 layers
     xyplot(Leafsun + Leafshade ~ hour | factor(layer),type="o",
      data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Leaf Area (",m^2," ",m^-2,")")))


     ## Plot of Transpiration for the 10 layers
     xyplot(TransSun + TransShade ~ hour | factor(layer),type="o",
      data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Transpiration (mm ",H[2],"O ",m^-2," ",s^-1,")")))


     ## Plot of Assimilation for the 10 layers
     xyplot(AssimSun + AssimShade ~ hour | factor(layer),type="o",
      data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Assimilation (",mu,"mol ",m^-2," ",s^-1,")")))

     ## Plot of Assimilation for the 10 layers
     xyplot(CondSun + CondShade ~ hour | factor(layer),type="o",
      data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Conductance (mmol ",m^-2," ",s^-1,")")))

## Testing the effect of N distribution
##dat2.no <- dat2

LeafN.no <- dat2.no$LeafN
LeafN.li <- dat2$LeafN
## Plot of Leaf Nitrogen
pdf("LeafNitrogen.pdf")
     xyplot(LeafN.no + LeafN.li ~  layer,type="o", subset = hour == 12,
      data = dat2, xlab="layer",col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Leaf Nitrogen (g ",m^-2,")")))
dev.off()



Vmax.no <- dat2.no$Vmax
Vmax.li <- dat2$Vmax
## Plot of Vmax
pdf("Vmax.pdf")
     xyplot(Vmax.no + Vmax.li ~  layer,type="o", subset = hour == 12,
      data = dat2, xlab="layer",col=c("blue","green"),lwd=1.5,
            ylab=expression(paste("Vmax (",mu,"mol ",m^-2," ",s^-1,")")))
dev.off()


## Let's do some math 2g m^-2 times 8 = 16 g total
## This gives 
(Atot.no <- sum(dat2.no$AssimSun + dat2.no$AssimShade)) ## 1002
## How do I distribute the same ammount of N more efficiently?
## Let us say I start with 3g m^-2
sum(subset(dat2,hour==12)$LeafN)*(8/10) ## 16 the same total ammount of N
(Atot.li <- sum(dat2$AssimSun + dat2$AssimShade)) ## 1160



## Plot of Assimilation for the 10 layers
##pdf("Assim.pdf")
xyplot(I(dat2$AssimSun + dat2$AssimShade) +
       I(dat2.no$AssimSun + dat2.no$AssimShade) ~ hour | factor(layer),type="o",
       data = dat2, xlab="hour",layout=c(2,layers/2),col=c("blue","green"),lwd=1.5,
       ylab=expression(paste("Assimilation (",mu,"mol ",m^-2," ",s^-1,")")),
       key=list(text=list(c("exp","const")),lines=TRUE,points=TRUE,
         col=c("blue","green"),type="o",pch=21))
##dev.off()
