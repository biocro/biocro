source("c4photoR.r")

Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- seq(0,1,0.1)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res <- c4photo(dat1[,1],dat1[,2],dat1[,3])

dat <- data.frame(Amu=res$Assim, Cappm=380, Temp=dat1[,2],RelH=dat1[,3])
ans <- test_ballBerry(dat, b0=0.08, b1=3)

xyplot(gs ~ Amu | factor(RelH),
       groups = Temp, auto.key=TRUE,
       data = ans)

## Now with lower intercept
ans <- test_ballBerry(dat, b0=0.01, b1=3)

xyplot(gs ~ Amu | factor(RelH),
       groups = Temp, auto.key=TRUE,
       data = ans)
