## BioCro/R/Rmiscanmod.R by Fernando Ezequiel Miguez Copyright (C) 2007-2008
## 
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or 3 of the License (at your option).
## 
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
## more details.
## 
## A copy of the GNU General Public License is available at
## http://www.r-project.org/Licenses/ This file will implement the more
## detailed version of miscanmod according to the Excel spreadsheet from
## Clifton-Brown Some changes have been made in order to run it with the
## available data This model, based on Monteith, is first described in
## Clifton-Brown et al. (2000) Industrial Crops and Products. 12. 97-109. It
## was later developed in later papers Clifton-Brown et al. (2004) Global
## Change Biology. 10. 509-518.
## 
## data should have
## 
## column 1: year column 2: month column 3: day column 4: JD column 5: max T
## (Celsius) column 6: min T (Celsius) column 7: PPFD or solar radiation
## divided by 2 (MJ/m2) column 8: Potential evaporation column 9: precip (mm)
## 
## Parameters ---------- RUE = Radiation Use Efficiency (g/MJ) LER = Leaf
## Expansion Rate Tb = Leaf base Temperature (Celsius) k = light extinction
## coefficient (dimensionless) LAIdrd = LAI down regulation RUEdrd = RUE down
## regulation a = soil parameter b = soil parameter soildepth = soil depth
Rmiscanmod <- function(data, RUE = 2.4, LER = 0.01, Tb = 10, k = 0.67, LAIdrd = 0.8, 
    LAIStop = 1.8, RUEdrd = 1.3, RUEStop = 2.5, SMDdrd = -30, SMDStop = -120, FieldC = 45, 
    iWatCont = 45, a = 6682.2, b = -0.33, soildepth = 0.6) {
    ## At this point the vairables are selected by their position (i.e. column) in
    ## the data.frame or matrix
    if (!inherits(data, "data.frame") || !inherits(data, "matrix")) 
        stop("data should be a data.frame or matrix")
    if (dim(data)[2] != 9) 
        stop("data should have 9 columns")
    data <- as.matrix(data)
    JD <- data[, 4]
    maxTemp <- data[, 5]
    minTemp <- data[, 6]
    Radiation <- data[, 7]
    PotEvp <- data[, 8]
    precip <- data[, 9]
    ## Now need to calculate the precipitation deficit
    Deficitp <- ifelse((precip - PotEvp) > 0, 0, precip - PotEvp)
    ## Calculate the soil moisture deficit
    SMDp <- numeric(length(Deficitp))
    SMDp[1] <- 0
    for (i in 2:length(Deficitp)) {
        if (Deficitp[i] < 0) {
            SMDp[i] <- Deficitp[i] + SMDp[i - 1]
        } else {
            if ((precip[i] + SMDp[i - 1]) < 0) {
                SMDp[i] <- precip[i] + SMDp[i - 1]
            } else {
                SMDp[i] <- 0
            }
        }
    }
    ## At the moment I don't quite get how they have calculated this but I will
    ## assume SMDp instead of SMDa for now this seems to be Actual Evaporation over
    ## Potential Evaporation
    aSMDStop <- abs(SMDStop)
    aSMDdrd <- abs(SMDdrd)
    AE.PE <- ifelse(SMDp < SMDStop, 0, ifelse(SMDp < SMDdrd, 1 + (aSMDdrd/(aSMDStop - 
        aSMDdrd) + (1/(aSMDStop + SMDdrd)) * SMDp), 1))
    PE.dr <- AE.PE * PotEvp
    Deficitp2 <- ifelse((precip - PE.dr) > 0, 0, precip - PE.dr)
    SMDa <- numeric(length(Deficitp2))
    SMDa[1] <- 0
    for (i in 2:length(Deficitp2)) {
        if (Deficitp2[i] < 0) {
            SMDa[i] <- Deficitp2[i] + SMDa[i - 1]
        } else {
            if ((precip[i] + SMDa[i - 1]) < 0) {
                SMDa[i] <- precip[i] + SMDa[i - 1]
            } else {
                SMDa[i] <- 0
            }
        }
    }
    diffRainPE <- precip - PE.dr
    H2Oper <- ((diffRainPE/1000)/soildepth) * 100
    tmp4 <- numeric(length(H2Oper))
    tmp4[1] <- iWatCont
    for (i in 2:length(H2Oper)) {
        if ((tmp4[i - 1] + H2Oper[i]) > FieldC) {
            tmp4[i] <- FieldC
        } else {
            tmp4[i] <- tmp4[i - 1] + H2Oper[i]
        }
    }
    SoilMoist <- tmp4
    SoilMatPot <- a * exp(b * SoilMoist)
    WL.LER <- ifelse(SoilMatPot < LAIdrd, 1, ifelse(SoilMatPot < LAIStop, 1 - ((SoilMatPot - 
        LAIdrd)/(LAIStop - LAIdrd)), 0))
    WL.RUE <- ifelse(SoilMatPot < RUEdrd, 1, ifelse(SoilMatPot < RUEStop, 1 - ((SoilMatPot - 
        RUEdrd)/(RUEStop - RUEdrd)), 0))
    ## Calculation of Radiation available
    half <- as.integer(length(minTemp)/2)
    minTemp1 <- minTemp[1:half]
    if (min(minTemp1) > 0) {
        day1 <- 30
    } else {
        minTemp1 <- minTemp1[which(minTemp1 < 0)]
        day1 <- max(minTemp1)
    }
    minTemp2 <- minTemp[half:length(minTemp)]
    if (min(minTemp2) > 0) {
        day1 <- 330
    } else {
        minTemp2 <- minTemp2[which(minTemp2 < 0)]
        dayn <- min(minTemp2)
    }
    ## I'm using the convention in MISCANMOD which is a bit strange to use a zero
    ## for no frost and 1 for frost
    DaysNoFrost <- ifelse((JD < day1) || (JD > dayn), 1, 0)
    RadAvail <- cumsum(I(DaysNoFrost * Radiation))
    ## Calculate the available degree days
    avgT <- apply(cbind(minTemp, maxTemp), 1, mean)
    # Replace the previous line with the average daily air temperature
    DDaTb <- ifelse(maxTemp < Tb, 0, ifelse(avgT < Tb, (maxTemp - Tb)/4, ifelse(minTemp < 
        Tb, (maxTemp - Tb)/2 - (Tb - minTemp)/4, avgT - Tb)))
    DDcum <- cumsum(I(DaysNoFrost * DDaTb))
    SoilEffDD <- DDaTb * WL.LER
    DDcum <- cumsum(I(DaysNoFrost * DDaTb))
    adjSumDD <- cumsum(I(DaysNoFrost * SoilEffDD)) * DaysNoFrost
    LAI <- adjSumDD * LER
    pLI <- (1 - exp(-k * LAI)) * 100
    LI <- (pLI/100) * Radiation
    DayYield <- LI * RUE * WL.RUE
    Yield <- cumsum(DayYield)
    list(PotEvp = PotEvp, Deficitp = Deficitp, SMDp = SMDp, AE.PE = AE.PE, Deficitp2 = Deficitp2, 
        SMDa = SMDa, diffRainPE = diffRainPE, H2Oper = H2Oper, SoilMoist = SoilMoist, 
        SoilMatPot = SoilMatPot, WL.LER = WL.LER, WL.RUE = WL.RUE, DDaTb = DDaTb, 
        DDcum = DDcum, adjSumDD = adjSumDD, LAI = LAI, LI = LI, pLI = pLI, Yield = Yield)
} 
