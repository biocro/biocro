\name{eC4photo}
\alias{eC4photo}
\title{C4 photosynthesis simulation (von Caemmerer model)}
\usage{
  eC4photo(Qp, airtemp, rh, ca = 380, oa = 210, vcmax = 60,
    vpmax = 120, vpr = 80, jmax = 400)
}
\arguments{
  \item{Qp}{quantum flux (\eqn{\mu}{micro} mol
  \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).}

  \item{airtemp}{air temperature (Celsius).}

  \item{rh}{relative humidity in proportion (e.g. 0.7).}

  \item{ca}{atmospheric carbon dioxide concentration (ppm
  or \eqn{\mu}{micro}bar) (e.g. 380).}

  \item{oa}{atmospheric oxygen concentration (mbar) (e.g.
  210).}

  \item{vcmax}{Maximum rubisco activity (\eqn{\mu}{micro}
  mol \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).}

  \item{vpmax}{Maximum PEP carboxylase activity
  (\eqn{\mu}{micro} mol \eqn{m^{-2}}{m-2}
  \eqn{s^{-1}}{s-1}).}

  \item{vpr}{PEP regeneration rate (\eqn{\mu}{micro} mol
  \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).}

  \item{jmax}{Maximal electron transport rate
  (\eqn{\mu}{micro}mol electrons \eqn{m^{-2}}{m-2}
  \eqn{s^{-1}}{s-1}).}
}
\value{
  results of call to C function eC4photo_sym

  a \code{\link{list}} structure with components
}
\description{
  Simulation of C4 photosynthesis based on the equations
  proposed by von Caemmerer (2000).  At this point
  assimilation and stomatal conductance are not coupled and
  although, for example a lower relative humidity will
  lower stomatal conductance it will not affect
  assimilation.  Hopefully, this will be improved in the
  future.
}
\details{
  The equations are taken from von Caemmerer (2000) for the
  assimilation part and stomatal conductance is based on
  FORTRAN code by Joe Berry (translated to C).
}
\examples{
\dontrun{
## A simple example for the use of eC4photo
## This is the model based on von Caemmerer
## First we can compare the effect of varying
## Jmax. Notice that this is different from
## varying alpha in the Collatz model

Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- c(0.7)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res1 <- eC4photo(dat1$Qp,dat1$Tl,dat1$RH)
res2 <- eC4photo(dat1$Qp,dat1$Tl,dat1$RH,jmax=700)

## Plot comparing Jmax 400 vs. 700 for a range of conditions
xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
            type="l",col=c("blue","green"),lwd=2,
            ylab=expression(paste("Assimilation (",
                 mu,mol," ",m^-2," ",s^-1,")")),
             xlab=expression(paste("Quantum flux (",
                 mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(c("Jmax 400","Jmax 700")),
              lines=TRUE,col=c("blue","green"),lwd=2))

## Second example is the effect of varying Vcmax

Qps <- seq(0,2000,10)
Tls <- seq(0,35,5)
rhs <- 0.7
vcmax <- seq(0,40,5)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs,vcmax=vcmax))
res1 <- numeric(nrow(dat1))
for(i in 1:nrow(dat1)){
res1[i] <- eC4photo(dat1$Qp[i],dat1$Tl[i],dat1$RH[i],vcmax=dat1$vcmax[i])$Assim
}

## Plot comparing different Vcmax
cols <- rev(heat.colors(9))
xyplot(res1 ~ Qp | factor(Tl) , data = dat1,col=cols,
            groups=vcmax,
            type="l",lwd=2,
            ylab=expression(paste("Assimilation (",
                 mu,mol," ",m^-2," ",s^-1,")")),
             xlab=expression(paste("Quantum flux (",
                 mu,mol," ",m^-2," ",s^-1,")")),
            key=list(text=list(as.character(vcmax)),
              lines=TRUE,col=cols,lwd=2))


}
}
\references{
  Susanne von Caemmerer (2000) Biochemical Models of Leaf
  Photosynthesis. CSIRO Publishing. (In particular chapter
  4).
}
\keyword{models}

