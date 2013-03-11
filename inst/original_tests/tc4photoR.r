source("c4photoR.r")

Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- seq(0.2,1,0.1)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))

stmws <- seq(0,1,0.05)

## This first test is with no water stress and
## Ball-Berry intercept of 0.08
res <- test_c4photoR(dat1, 1, b0=0.08) 

xyplot(Assim ~ Qp | factor(Tl) , groups = RH, data = res,
        auto.key=TRUE)


if(sum(res$conv) > 0.1)
  stop("c4photo failed to converge with no water stress \n
        and Ball-Berry intercept 0.08")


## Now the same test with a lower value of the Ball-Berry intercept
res <- test_c4photoR(dat1, 1, b0=0.01) 

sum(res$conv)

xyplot(conv ~ Qp | factor(Tl) , groups = RH, data = res,
        auto.key=TRUE)

xyplot(Assim ~ Qp | factor(Tl) , groups = RH, data = res,
        auto.key=TRUE, type="l")



## What happens when we add water stress?
## First the C version
res <- test_c4photo(dat1, StomWS=0.8, b0=0.08)

xyplot(Assim ~ Qp | factor(Tl) , groups = RH, data = res,
        auto.key=TRUE, type="l")

## Second the R version
res <- test_c4photoR(dat1, StomWS=0.8, b0=0.08)

xyplot(Assim ~ Qp | factor(Tl) , groups = RH, data = res,
        auto.key=TRUE, type="l")

## Second the R version
res <- test_c4photoR(dat1, StomWS=0.3, b0=0.08)

xyplot(Assim ~ Qp | factor(Tl) , groups = RH, data = res,
        auto.key=TRUE, type="l")





## xyplot(conv ~ Qp | factor(Tl) , groups = RH, data = res,
##        auto.key=TRUE)

## xyplot(conv ~ Qp | factor(RH) , groups = Tl, data = res,
##        auto.key=TRUE)

## xyplot(iters ~ Qp | factor(Tl) , groups = RH, data = res,
##        auto.key=TRUE)


