lightME <- function(lat=40, DOY=190, t.d=12, t.sn=12, atm.P=1e5, alpha=0.85) {
  Dtr <- pi / 180
  
  omega <- lat * Dtr
  
  delta0 <- 360 * (DOY + 10) / 365
  delta <- -23.5 * cos(delta0 * Dtr)
  deltaR <- delta * Dtr
  t.f <- 15 * (t.d - t.sn) * Dtr
  SSin <- sin(deltaR) * sin(omega)
  CCos <- cos(deltaR) * cos(omega)
  CosZenithAngle0 <- SSin + CCos * cos(t.f)
  CosZenithAngle <- ifelse(CosZenithAngle0 <= 10^-10, 1e-10, CosZenithAngle0)
  
  CosHour <-  -tan(omega) * tan(deltaR)
  CosHourDeg <- (1 / Dtr) * (CosHour)
  CosHour <- ifelse(CosHourDeg < -57, -0.994, CosHour)
  Daylength <- 2 * (1 / Dtr) * (acos(CosHour)) / 15
  SunUp <- 12 - Daylength / 2
  SunDown <- 12 + Daylength / 2
  SinSolarElevation <- CosZenithAngle
  SolarElevation <- (1 / Dtr) * (asin(SinSolarElevation))
  
  PP.o <- 10^5 / atm.P
  Solar_Constant <- 2650
  ## Notice the difference with the website for the eq below
  I.dir <- Solar_Constant * (alpha^((PP.o) / CosZenithAngle))
  I.diff <- 0.3 * Solar_Constant * (1 - alpha^((PP.o) / CosZenithAngle)) * CosZenithAngle
  propIdir <- I.dir / (I.dir + I.diff)
  propIdiff <- I.diff / (I.dir + I.diff)
  
  list(I.dir=I.dir, I.diff=I.diff, cos.th=CosZenithAngle, propIdir=propIdir, propIdiff=propIdiff)
}
