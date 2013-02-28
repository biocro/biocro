\name{sunML}
\alias{sunML}
\title{Sunlit shaded multi-layer model}

\description{
  Simulates the light microenvironment in the canopy based on
  the sunlit-shade model and the multiple layers. 
}
\usage{
sunML(Idir,Idiff,LAI=8,nlayers=8,cos.theta=0.5,kd=0.7,chi.l=1,heightf=3)
}

\arguments{

\item{I.dir}{ direct light (quantum flux), (\eqn{\mu mol \; m^{-2} \; s^{-1}}{micro mol /m2/s}).} 
\item{I.diff}{ indirect light (diffuse),  (\eqn{\mu mol \; m^{-2} \; s^{-1}}{micro mol /m2/s}).} 
\item{LAI}{ leaf area index, default 8.}
\item{nlayers}{ number of layers in which the canopy is partitioned, default 8.}
\item{cos.theta}{ cosine of \eqn{\theta}{theta}, solar zenith angle.}
\item{kd}{ extinction coefficient for diffuse light.}
\item{chi.l}{ The ratio of horizontal:vertical projected area of leaves in
  the canopy segment.}
\item{heightf}{height factor. simple empirical relationship between LAI
  and height of a canopy.}

}

\value{
  a \code{\link{data.frame}} structure with components

  row number equal to the number of layers.

\item{layIdir}{direct solar radiation.}
\item{layIdiff}{difusse solar radiation.}
\item{layItotal}{total solar radiation.}
\item{layFsun}{proportion of the leaf area in each layer which is in direct light.}
\item{layFshade}{proportion of the leaf area in each layer which is in indirect light.}
\item{layHeight}{layer height.}
  
}    

\examples{
res2 <- sunML(1500,200,3,10)

xyplot(layFsun + layFshade ~ c(1:10), data=res2,
       ylab="LAI",
       xlab="layer",
       type="l",lwd=2,col=c("blue","green"),
       lty=c(1,2),
       key=list(text=list(c("Direct","Diffuse")),lty=c(1,2),
                cex=1.2,lwd=2,lines=TRUE,x=0.7,y=0.5,
                col=c("blue","green")))
}

\keyword{models}