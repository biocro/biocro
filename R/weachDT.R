weachDT <- function(X, lati, ts=1){

    tint <- 24/ts
    tseq <- seq(0,23,ts)
    
    Y <-  X[,list(solarR = rep((0.12 * dswrf.MJ) * 2.07 * 10^6 / 3600, each = tint) )]

    
    tmp <- X[,lightME(DOY = day, t.d = tseq, lat = 40), by = c("year", "day")]
    tmp$Itot <- tmp[,list(I.dir + I.diff)]
    tmp$resC2 <- tmp[,list((Itot - min(Itot)) / max(Itot))] 
    Y <- cbind(Y, tmp)
    SolarR <- Y[,list(solarR * resC2)]

    Temp <- X[,list(tmin + (sin(2*pi*(tseq-10)/tint) + 1)/2 * (tmax - tmin)), by = c("year", "day")]
    RH <- X[,list(rhmin + (cos(2 * pi * (tseq - 10)/tint) + 1) / 2), by = c("year", "day")]
    WS <- X[,rep(wnd, each = tint)]
    precip <- rep(X$precip/tint, each = tint)
    hour <- 1:24

    time <- Y[,list(year, doy = day, hour = 1:24)]
    ans <- cbind(time, SolarR, Temp, RH, WS, precip)
    return(ans)
}
    
