\name{c4photo}
\alias{c4photo}
\title{Coupled photosynthesis-stomatal conductance simulation}
\usage{
c4photo(Qp, Tl, RH, vmax = 39, alpha = 0.04, kparm = 0.7, theta = 0.83,
        beta = 0.93, Rd = 0.8, uppertemp = 37.5, lowertemp = 3, Catm = 380,
        b0 = 0.08, b1 = 3, StomWS = 1, ws = c("gs", "vmax"))
}
\arguments{
    \item{Qp}{Quantum flux (direct light), (\ifelse{html}{\out{&mu;mol m<sup>-2</sup> s<sup>-1</sup>}}{\eqn{\mumol m^{-2} s^{-1}}{micromoles m^-2 s^-1}}.)}

    \item{Tl}{Temperature of the leaf (degrees Celsius).}

    \item{RH}{Relative humidity (fraction).}

    \item{vmax}{Maximum carboxylation of Rubisco according to the Collatz model.}

    \item{alpha}{alpha parameter according to the Collatz model. Initial slope of the response to irradiance.}

    \item{kparm}{k parameter according to the Collatz model. Initial slope of the response to carbon dioxide concentration.}

    \item{theta}{theta parameter of the Collatz model. Curvature of the photosynthetic light response curve.}

    \item{beta}{beta parameter of the Collatz model. Curvature of the photosynthetic carbon dioxide response curve.}

    \item{Rd}{Rd parameter of the Collatz model. Dark respiration.}

    \item{uppertemp}{Parameter controlling the upper temperature response.}

    \item{lowertemp}{Parameter controlling the lower temperature response.}

    \item{Catm}{Atmospheric carbon dioxide concentration (\ifelse{html}{\out{&mu;mol mol<sup>-1</sup>}}{micromoles \eqn{mol^-1}{micromoles mol^-1}}.)}

    \item{b0}{Intercept for the Ball-Berry stomatal conductance model.}

    \item{b1}{Slope for the Ball-Berry stomatal conductance model.}

    \item{StomWS}{Coefficient which controls the effect of water stress on stomatal conductance and assimilation.}

    \item{ws}{Option to control whether the water stress factor is applied to stomatal conductance (``gs'') or to Vmax (``vmax'').}
}
\value{
    A \code{\link{list}} structure with components

    \item{Gs}{Stomatal conductance (mmol \ifelse{html}{\out{m<sup>-2</sup> s<sup>-1</sup>}}{\eqn{m^{-2} s^{-1}}{m^-2 s^-1}}).}
    \item{Assim}{Net assimilation (\ifelse{html}{\out{&mu;mol m<sup>-2</sup> s<sup>-1</sup>}}{\eqn{\mumol m^{-2} s^{-1}}{micromoles m^-2 s^-1}}).}
    \item{Ci}{Leaf intercellular carbon dioxide concentration (\ifelse{html}{\out{&mu;mol mol<sup>-1</sup>}}{\eqn{\mumol mol^{-1}}{micromoles mol^-1}}).}
}

\description{
    The mathematical model is based on Collatz et al. 1992 (see references).
    Stomatal conductance is based on code provided by Joe Berry.
}
\author{
    Fernando E. Miguez.
    Contributions from Deepak Jaiswal for the temperature response.
}
\examples{

\dontrun{
## First example: looking at the effect of changing alpha
Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- c(0.7)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH) ## default alpha = 0.04
res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH,alpha=0.06)

## Plot comparing alpha 0.04 vs. 0.06 for a range of conditions
lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
                type='l',col=c('blue','green'),lwd=2,
                ylab=expression(paste('Assimilation (', mu,mol,' ',m^-2,' ',s^-1,')')),
                xlab=expression(paste('Quantum flux (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('alpha 0.04','alpha 0.06')), lines=TRUE,col=c('blue','green'),lwd=2))

## Second example: looking at the effect of changing vmax
## Plot comparing Vmax 39 vs. 50 for a range of conditions

res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH) ## default Vmax = 39
res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH,vmax=50)

lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
                type='l',col=c('blue','green'),lwd=2,
                ylab=expression(paste('Assimilation (', mu,mol,' ',m^-2,' ',s^-1,')')),
                xlab=expression(paste('Quantum flux (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('Vmax 39','Vmax 50')), lines=TRUE,col=c('blue','green'),lwd=2))

## Small effect of low RH on  Assim
Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- c(0.2,0.9)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH)
# plot for Assimilation and two RH
lattice::xyplot(res1$Assim ~ Qp | factor(Tl) , data = dat1,
                groups=RH, type='l',
                col=c('blue','green'),lwd=2,
                ylab=expression(paste('Assimilation (', mu,mol,' ',m^-2,' ',s^-1,')')),
                xlab=expression(paste('Quantum flux (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('RH 20\%','RH 90\%')), lines=TRUE,col=c('blue','green'), lwd=2))

## Effect of the previous runs on Stomatal conductance

lattice::xyplot(res1$Gs ~ Qp | factor(Tl) , data = dat1,
                type='l', groups=RH,
                col=c('blue','green'),lwd=2,
                ylab=expression(paste('Stomatal Conductance (', mu,mol,' ',m^-2,' ',s^-1,')')),
                xlab=expression(paste('Quantum flux (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('RH 20\%','RH 90\%')), lines=TRUE,col=c('blue','green'), lwd=2))


## A Ci curve for the Collatz model
## Assuming constant values of Qp, Temp, and RH
## Notice the effect of the different kparm
## The loop is needed because the length of Ca
## should be the same as Qp

Ca <- seq(15,400,5)

res1 <- numeric(length(Ca))
res2 <- numeric(length(Ca))
for(i in 1:length(Ca)){
    res1[i] <- c4photo(1500,25,0.7,Catm=Ca[i])$Assim
    res2[i] <- c4photo(1500,25,0.7,Catm=Ca[i],kparm=0.8)$Assim
}

lattice::xyplot(res1 + res2 ~ Ca ,type='l',lwd=2,
                col=c('blue','green'),
                xlab=expression(paste(CO[2],' (ppm)')),
                ylab=expression(paste('Assimilation (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('kparm 0.7','kparm 0.8')), lines=TRUE,col=c('blue','green'), lwd=2))

## Effect of Reduction in Assimilation due to
## water stress

Qps <- seq(0,2000,10)
Tls <- seq(0,55,5)
rhs <- c(0.7)
dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH) ## default StomWS = 1 No stress
res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH,StomWS=0.5)

## Plot comparing StomWS = 1 vs. 0.5 for a range of conditions
lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
                type='l',col=c('blue','green'),lwd=2,
                ylab=expression(paste('Assimilation (', mu,mol,' ',m^-2,' ',s^-1,')')),
                xlab=expression(paste('Quantum flux (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('StomWS 1','StomWS 0.5')), lines=TRUE,col=c('blue','green'),lwd=2))


## Effect on Stomatal Conductance
## Plot comparing StomWS = 1 vs. 0.5 for a range of conditions
lattice::xyplot(res1$Gs + res2$Gs ~ Qp | factor(Tl) , data = dat1,
                type='l',col=c('blue','green'),lwd=2,
                ylab=expression(paste('Stomatal Conductance (mmol ', m^-2,' ',s^-1,')')),
                xlab=expression(paste('Quantum flux (', mu,mol,' ',m^-2,' ',s^-1,')')),
                key=list(text=list(c('StomWS 1','StomWS 0.5')), lines=TRUE,col=c('blue','green'),lwd=2))
}

}
\references{
    G. Collatz, M. Ribas-Carbo, and J. Berry. 1992.  Coupled photosynthesis-stomatal conductance model for leaves of C4 plants.
    \emph{Australian Journal of Plant Physiology} 19 (5): 519--538.
}
\seealso{
}
\keyword{models}
