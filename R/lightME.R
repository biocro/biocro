##' Simulates the light macro environment
##' 
##' Simulates light macro environment based on latitude, day of the year.
##' Other coefficients can be adjusted.
##' 
##' 
##' @param lat the latitude, default is 40 (Urbana, IL, U.S.).
##' @param DOY the day of the year (1--365), default 190.
##' @param t.d time of the day in hours (0--23), default 12.
##' @param t.sn time of solar noon, default 12.
##' @param atm.P atmospheric pressure, default 1e5 (kPa).
##' @param alpha atmospheric transmittance, default 0.85.
##' @export
##' @return a \code{\link{list}} structure with components
##' @returnItem I.dir Direct radiation (\eqn{\mu} mol \eqn{m^{-2}}
##' \eqn{s^{-1}}).
##' @returnItem I.diff Indirect (diffuse) radiation (\eqn{\mu} mol \eqn{m^{-2}}
##' \eqn{s^{-1}}).
##' @returnItem cos.th cosine of \eqn{\theta}, solar zenith angle.
##' @returnItem propIdir proportion of direct radiation.
##' @returnItem propIdir proportion of indirect (diffuse) radiation.
##' @keywords models
##' @examples
##' 
##' 
##' ## Direct and diffuse radiation for DOY 190 and hours 0 to 23
##' 
##' res <- lightME(t.d=0:23)
##' 
##' xyplot(I.dir + I.diff ~ 0:23 , data = res,
##' type="o",xlab="hour",ylab="Irradiance")
##' 
##' x11();xyplot(propIdir + propIdiff ~ 0:23 , data = res,
##' type="o",xlab="hour",ylab="Irradiance proportion")
##' 
##' 
##' 
lightME <- function(lat=40,DOY=190,t.d=12,t.sn=12,atm.P=1e5,alpha=0.85) {
Dtr <- (pi/180)
omega <- lat * Dtr
delta0 <- 360 * (DOY + 10)/365
delta <- -23.5 * cos(delta0*Dtr)
deltaR <- delta * Dtr
t.f <- (15*(t.d-t.sn))*Dtr
SSin <- sin(deltaR) * sin(omega)
CCos <- cos(deltaR) * cos(omega)
CosZenithAngle0 <- SSin + CCos * cos(t.f)
CosZenithAngle <- ifelse(CosZenithAngle0 <= 10 ^ -10, 1e-10, CosZenithAngle0)
CosHour <-  -tan(omega) * tan(deltaR)
CosHourDeg <- (1/Dtr)*(CosHour)
CosHour <- ifelse(CosHourDeg < -57,-0.994,CosHour)
Daylength <- 2 * (1/Dtr)*(acos(CosHour)) / 15
SunUp <- 12 - Daylength / 2
SunDown <- 12 + Daylength / 2
SinSolarElevation <- CosZenithAngle
SolarElevation <- (1/Dtr)*(asin(SinSolarElevation))
PP.o <- 10^5 / atm.P
Solar_Constant <- 2650
I.dir <- Solar_Constant * (alpha ^ ((PP.o) / CosZenithAngle)) * CosZenithAngle
I.diff <- 0.5 * Solar_Constant * (1 - alpha ^ ((PP.o) / CosZenithAngle)) * CosZenithAngle
propIdir = I.dir / (I.dir+I.diff);
propIdiff = I.diff / (I.dir+I.diff);
list(I.dir=I.dir,I.diff=I.diff,cos.th=CosZenithAngle,propIdir=propIdir,propIdiff=propIdiff)
}