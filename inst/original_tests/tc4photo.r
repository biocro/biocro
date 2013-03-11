## Testing the effect of CO2 increase on c4 photosynthesis

co2s <- c(300, 400, 500, 600)

Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- c(0.7)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH, Catm = co2s[1]) ## default alpha = 0.04
res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH, Catm = co2s[2])
res3 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH, Catm = co2s[3])
res4 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH, Catm = co2s[4])

## Assimilation
xyplot(res1$Assim + res2$Assim + res3$Assim + res4$Assim
       ~ Qp | factor(Tl) , data = dat1,
                 type="l",col=c("blue","green","red","purple"),lwd=2,
                 ylab=expression(paste("Assimilation (",
                      mu,mol," ",m^-2," ",s^-1,")")),
                  xlab=expression(paste("Quantum flux (",
                      mu,mol," ",m^-2," ",s^-1,")")),
                 key=list(text=list(c("CO2 300","CO2 400", "CO2 500", "CO2 600")),
                   lines=TRUE,col=c("blue","green","red","purple"),lwd=2))

## Somatal Conductance
xyplot(res1$Gs + res2$Gs + res3$Gs + res4$Gs
       ~ Qp | factor(Tl) , data = dat1,
                 type="l",col=c("blue","green","red","purple"),lwd=2,
                ylab=expression(paste("Stomatal Conductance (",
                                mu,mol," ",m^-2," ",s^-1,")")),
                  xlab=expression(paste("Quantum flux (",
                      mu,mol," ",m^-2," ",s^-1,")")),
                 key=list(text=list(c("CO2 300","CO2 400", "CO2 500", "CO2 600")),
                   lines=TRUE,col=c("blue","green","red","purple"),lwd=2))

data(weather05)
ans.bg <- BioGro(weather05)
photo.ll <- photoParms(Catm = 550)
ans2.bg <- BioGro(weather05, photoControl = photo.ll)

xyplot(ans.bg$CanopyT + ans2.bg$CanopyT ~ ans.bg$ThermalT)
sum(ans.bg$CanopyT)
sum(ans2.bg$CanopyT)

## Without water stress
soil.ll <- soilParms(wsFun = "none")
ans.bg <- BioGro(weather05, soilControl = soil.ll)
photo.ll <- photoParms(Catm = 550)
ans2.bg <- BioGro(weather05, photoControl = photo.ll, soilControl = soil.ll)

xyplot(ans.bg$CanopyT + ans2.bg$CanopyT ~ ans.bg$ThermalT)
sum(ans.bg$CanopyT)
sum(ans2.bg$CanopyT)

## Script relevant to
data(weather05)
ans <- capture.output(BioGro(weather05, day1=100, dayn=120))

ans <- ans[1:I(length(ans)-21)]

lans <- lapply(ans,function(x)
               c(as.numeric(strsplit(x," ")[[1]][1]),
                 as.numeric(strsplit(x," ")[[1]][2]),
                 as.numeric(strsplit(x," ")[[1]][3]),
                 as.numeric(strsplit(x," ")[[1]][4])))

llans <- unlist(lans)
mlans <- matrix(llans,ncol=4,byrow=TRUE)

for(i in 1:nrow(mlans)){

  print(i)
  res <- c4photo(mlans[i,1],mlans[i,2],mlans[i,3],StomWS=mlans[i,4])
  
}
