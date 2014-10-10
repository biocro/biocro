## Using Simulated data

Qps <- seq(0,2000,100)
Tls <- seq(0,55,5)
rhs <- c(0.7)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH, alpha=0.05)$Assim ## default alpha = 0.04
res1 <- res1 + rnorm(nrow(dat1),0,2)
xyplot(res1 ~ Qp | factor(Tl) , data = dat1, type="o")

b.op <- Opc4photo(cbind(res1,dat1))

b.mc <- MCMCc4photo(cbind(res1,dat1) , 100000, scale=0.25)
