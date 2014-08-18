source("c4photoR.r")

Qps <- seq(0,2000,500)
Tls <- seq(0,55,10)
rhs <- seq(0,1,0.01)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))

res <- test_c4photo(dat1)

xyplot(Assim ~ RH | factor(Qps),
       groups = Tl, auto.key=TRUE,
       data = res, type="l")
