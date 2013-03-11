library(c3photo)
## Testing the c3photo function
     ## First example: looking at the effect of changing Vcmax
      Qps <- seq(0,2000,10)
      Tls <- seq(0,55,5)
      rhs <- c(0.7)
      dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
      res1 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH) ## default vcmax = 100
      res2 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH,vcmax=120)

     ## Plot comparing alpha 0.04 vs. 0.06 for a range of conditions
     xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
            type="l",col=c("blue","green"),lwd=2,
            ylab=expression(paste("Assimilation (",
                 mu,mol," ",m^-2," ",s^-1,")")),
             xlab=expression(paste("Quantum flux (",
                 mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Vcmax 100","Vcmax 120")),
              lines=TRUE,col=c("blue","green"),lwd=2))

     ## For Stomatal conductance
     xyplot(res1$Gs + res2$Gs ~ Qp | factor(Tl) , data = dat1,
            type="l",col=c("blue","green"),lwd=2,
            ylab=expression(paste("Stomatal Conductance (",
                  mmol," ",m^-2," ",s^-1,")")),
             xlab=expression(paste("Quantum flux (",
                 mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Vcmax 100","Vcmax 120")),
              lines=TRUE,col=c("blue","green"),lwd=2))

     ## For intercellular CO2
     xyplot(res1$Ci + res2$Ci ~ Qp | factor(Tl) , data = dat1,
            type="l",col=c("blue","green"),lwd=2,
            ylab="Intercellular CO2",
             xlab=expression(paste("Quantum flux (",
                 mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Vcmax 100","Vcmax 120")),
              lines=TRUE,col=c("blue","green"),lwd=2))

     ## Second example: looking at the effect of changing Jmax
     ## Plot comparing Vmax 300 vs. 500 for a range of conditions

      res1 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH) ## default Jmax = 180
      res2 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH,jmax=100)

     xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
            type="l",col=c("blue","green"),lwd=2,
            ylab=expression(paste("Assimilation (",
                 mu,mol," ",m^-2," ",s^-1,")")),
             xlab=expression(paste("Quantum flux (",
                 mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Jmax 180","Jmax 100")),
              lines=TRUE,col=c("blue","green"),lwd=2))


## A/Ci curve

     Ca <- seq(0,800,1)

     resA1 <- numeric(length(Ca))
     resA2 <- numeric(length(Ca))
     resA3 <- numeric(length(Ca))
     resCi1 <- numeric(length(Ca))
     resCi2 <- numeric(length(Ca))
     resCi3 <- numeric(length(Ca))
     for(i in 1:length(Ca)){
       resA1[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 70)$Assim
       resA2[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 100)$Assim
       resA3[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 120)$Assim
       resCi1[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 70)$Ci
       resCi2[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 100)$Ci
       resCi3[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 120)$Ci
     }

     xyplot(resA1 + resA2 + resA3 ~ Ca ,type="l",lwd=2,
            col=c("blue","green","purple"),
            xlab=expression(paste("Reference ",CO[2]," (ppm)")),
            ylab=expression(paste("Assimilation (",
                mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Vcmax 70","Vcmax 100","Vcmax 120")),
              lines=TRUE,col=c("blue","green","purple"),
              lwd=2))

     xyplot(resA3 ~ resCi3 ,type="l",lwd=2,
            xlab=expression(paste("Intercellular ",CO[2]," (ppm)")),
            ylab=expression(paste("Assimilation (",
                mu,mol," ",m^-2," ",s^-1,")")),
            panel = function(x,y,...){
              panel.xyplot(x,y,col="purple",...)
              panel.xyplot(resCi1,resA1,col="blue",...)
              panel.xyplot(resCi2,resA2,col="green",...)
            },
          key=list(text=list(c("Vcmax 70","Vcmax 100","Vcmax 120")),
                             lines=TRUE,col=c("blue","green","purple"),
                             lwd=2))


## A/Ci curve with different b0 parameter

     Ca <- seq(0,800,1)

     resA1 <- numeric(length(Ca))
     resA2 <- numeric(length(Ca))
     resA3 <- numeric(length(Ca))
     resCi1 <- numeric(length(Ca))
     resCi2 <- numeric(length(Ca))
     resCi3 <- numeric(length(Ca))
     for(i in 1:length(Ca)){
       resA1[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 70 , Rd = 2)$Assim
       resA2[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 100, Rd = 2)$Assim
       resA3[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 120, Rd = 2)$Assim
       resCi1[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 70, Rd = 2)$Ci
       resCi2[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 100, Rd = 2)$Ci
       resCi3[i] <- c3photo(1500,25,0.7,Catm=Ca[i], vcmax = 120, Rd = 2)$Ci
     }

     xyplot(resA1 + resA2 + resA3 ~ Ca ,type="l",lwd=2,
            col=c("blue","green","purple"),
            xlab=expression(paste("Reference ",CO[2]," (ppm)")),
            ylab=expression(paste("Assimilation (",
                mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Vcmax 70","Vcmax 100","Vcmax 120")),
              lines=TRUE,col=c("blue","green","purple"),
              lwd=2))

     xyplot(resA3 ~ resCi3 ,type="l",lwd=2,
            xlab=expression(paste("Intercellular ",CO[2]," (ppm)")),
            ylab=expression(paste("Assimilation (",
                mu,mol," ",m^-2," ",s^-1,")")),
            panel = function(x,y,...){
              panel.xyplot(x,y,col="purple",...)
              panel.xyplot(resCi1,resA1,col="blue",...)
              panel.xyplot(resCi2,resA2,col="green",...)
            },
          key=list(text=list(c("Vcmax 70","Vcmax 100","Vcmax 120")),
                             lines=TRUE,col=c("blue","green","purple"),
                             lwd=2))
