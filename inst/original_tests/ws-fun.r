## Looking at the watrer stress functions developed in Grassini et
## al. (2009) Agronomy Journal Vol. 101. 564-571.

WSF.LAI <- function(a=270, b=-32.2, FAWHC.avg){

  x <- 1/(1 + a*exp(b*FAWHC.avg))

  x

}

WSF.RUE <- function(a=9, b=-15.3, FAWHC.avg){

  x <- 1/(1 + a*exp(b*FAWHC.avg))

  x

}

w <- seq(0,0.4,0.01)

yy <- WSF.LAI(FAWHC.avg=w)
yy2 <- WSF.RUE(FAWHC.avg=w)

xyplot(yy + yy2 ~ w, auto.key=TRUE)
