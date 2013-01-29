## BioCro/R/eC4photo.R by Fernando Ezequiel Miguez Copyright (C) 2007-2008
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
## http://www.r-project.org/Licenses/ Start of the functions related to the c4
## photosynthesis model proposed by von Caemmerer This file will contain the
## functions in BioCro for and alternative photosynthesis model proposed by von
## Caemmerer This is the C4photo function based on von Caemmerer (2000)
eC4photo <- function(Qp, airtemp, rh, ca = 380, oa = 210, vcmax = 60, vpmax = 120, 
    vpr = 80, jmax = 400) {
    nqp = length(Qp)
    nat = length(airtemp)
    nrh = length(rh)
    if ((nqp != nat) || (nat != nrh)) {
        stop("Qp, airtemp and rh sould be of equal length")
    }
    if (length(c(ca, oa, vcmax, vpmax, vpr, jmax)) != 6) {
        stop("ca, oa, vcmax, vpmax, vpr, jmax should all be of length 1")
    }
    res <- .Call(eC4photo_sym, as.double(Qp), as.double(airtemp), as.double(rh), 
        as.double(ca), as.double(oa), as.double(vcmax), as.double(vpmax), as.double(vpr), 
        as.double(jmax))
    res
}
## Here I will add the Canopy Assimilation C routine
eCanA <- function(LAI, doy, hour, solarR, AirTemp, RH, WindS, Vcmax, Vpmax, Vpr, 
    Jmax, Ca = 380, Oa = 210, StomataWS = 1) {
    inputs <- c(LAI, doy, hour, solarR, AirTemp, RH, WindS, WindS, Vcmax, Vpmax, 
        Vpr, Jmax)
    if (length(inputs) != 12) 
        stop("The inputs should all be of length 1")
    res <- .Call(eCanA_sym, as.double(LAI), as.integer(doy), as.integer(hour), as.double(solarR), 
        as.double(AirTemp), as.double(RH), as.double(WindS), as.double(Ca), as.double(Oa), 
        as.double(Vcmax), as.double(Vpmax), as.double(Vpr), as.double(Jmax), as.double(StomataWS))
    res
}
MCMCEc4photo <- function(obsDat, niter = 30000, iCa = 380, iOa = 210, iVcmax = 60, 
    iVpmax = 120, iVpr = 80, iJmax = 400, thresh = 0.01, scale = 1) {
    assim <- obsDat[, 1]
    qp <- obsDat[, 2]
    temp <- obsDat[, 3]
    rh <- obsDat[, 4]
    if (iVpr != 80) 
        warning("\n Vpr is not optimized at the moment \n")
    res <- .Call(McMCEc4photo, as.double(assim), as.double(qp), as.double(temp), 
        as.double(rh), as.integer(niter), as.double(iCa), as.double(iOa), as.double(iVcmax), 
        as.double(iVpmax), as.double(iJmax), as.double(thresh), as.double(scale))
    res$resuBI <- res$resuBI[, 1:res$accept1]
    res$resuMC <- res$resuMC[, 1:res$accept2]
    res$niter <- niter
    if (I(res$accept2/res$niter) < 0.05) 
        stop("\nRun the chain again. Try increasing thresh.\n")
    structure(res, class = "MCMCEc4photo")
}
print.MCMCEc4photo <- function(x, level = 0.95, ...) {
    ul <- 1 - (1 - level)/2
    ll <- (1 - level)/2
    xMat <- t(x$resuMC[1:4, ])
    colnames(xMat) <- c("Vcmax", "Vpmax", "Vpr", "Jmax")
    cat("\n Markov chain Monte Carlo for the von Caemmerer c4 photosynthesis model")
    cat("\n Burn in period:")
    cat("\n R-squared:", x$RsqBI)
    cat("\n Coefficients:", x$CoefBI)
    cat("\n Iterations:", x$accept1, "\n")
    cat("\n Markov chain")
    cat("\n Length of the chain:", x$accept2)
    cat("\n Moves:", x$accept3, "Prop:", x$accept3/x$niter, "\n")
    cat("\n Summaries \n  Vcmax:\n")
    print(summary(x$resuMC[1, ]), ...)
    cat("\n Vpmax:\n")
    print(summary(x$resuMC[2, ]), ...)
    cat("\n Jmax:\n")
    print(summary(x$resuMC[3, ]), ...)
    cat("\n HPD region \n Vcmax:\n")
    print(quantile(x$resuMC[1, ], c(ll, ul)), ...)
    cat("\n Vpmax:\n")
    print(quantile(x$resuMC[2, ], c(ll, ul)), ...)
    cat("\n Jmax:\n")
    print(quantile(x$resuMC[3, ], c(ll, ul)), ...)
    cat("\n correlation matrix:\n")
    print(cor(xMat), ...)
    cat("\n R-squared range:", range(x$resuMC[4, ]), "\n")
    invisible(x)
}
plot.MCMCEc4photo <- function(x, x2 = NULL, x3 = NULL, type = c("trace", "density"), 
    ...) {
    type <- match.arg(type)
    ## This first code is to plot the first object only Ploting the trace
    if (missing(x2) && missing(x3)) {
        if (type == "trace") {
            plot1 <- xyplot(x$resuMC[1, ] ~ 1:x$accept2, xlab = "Iterations", type = "l", 
                ylab = expression(paste("Vcmax (", mu, mol, " ", m^-2, " ", s^-1, 
                  ")")), ...)
            plot2 <- xyplot(x$resuMC[2, ] ~ 1:x$accept2, xlab = "Iterations", type = "l", 
                ylab = expression(paste("Vpmax (", mu, mol, " ", m^-2, " ", s^-1, 
                  ")")), ...)
            plot3 <- xyplot(x$resuMC[3, ] ~ 1:x$accept2, xlab = "Iterations", type = "l", 
                ylab = expression(paste("Jmax (", mu, mol, " ", m^-2, " ", s^-1, 
                  ")")), ...)
            print(plot1, position = c(0, 0, 1, 0.333), more = TRUE)
            print(plot2, position = c(0, 0.333, 1, 0.666), more = TRUE)
            print(plot3, position = c(0, 0.666, 1, 1))
        } else if (type == "density") {
            ## Ploting the density
            plot1 <- densityplot(~x$resuMC[1, ], xlab = "Vcmax", ...)
            plot2 <- densityplot(~x$resuMC[2, ], xlab = "Vpmax", ...)
            plot3 <- densityplot(~x$resuMC[3, ], xlab = "Jmax", ...)
            print(plot1, position = c(0, 0, 1, 0.333), more = TRUE)
            print(plot2, position = c(0, 0.333, 1, 0.666), more = TRUE)
            print(plot3, position = c(0, 0.666, 1, 1))
        }
    } else if (missing(x3)) {
        ## This part of the code is to plot objects x and x2 Ploting the trace
        n1 <- x$accept2
        n2 <- x2$accept2
        minchainLength <- min(n1, n2)
        tmpvec11 <- x$resuMC[1, 1:minchainLength]
        tmpvec12 <- x2$resuMC[1, 1:minchainLength]
        tmpvec21 <- x$resuMC[2, 1:minchainLength]
        tmpvec22 <- x2$resuMC[2, 1:minchainLength]
        tmpvec31 <- x$resuMC[3, 1:minchainLength]
        tmpvec32 <- x2$resuMC[3, 1:minchainLength]
        if (type == "trace") {
            plot1 <- xyplot(tmpvec11 + tmpvec12 ~ 1:minchainLength, xlab = "Iterations", 
                type = "l", ylab = expression(paste("Vcmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), ...)
            plot2 <- xyplot(tmpvec21 + tmpvec22 ~ 1:minchainLength, xlab = "Iterations", 
                type = "l", ylab = expression(paste("Vpmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), ...)
            plot3 <- xyplot(tmpvec31 + tmpvec32 ~ 1:minchainLength, xlab = "Iterations", 
                type = "l", ylab = expression(paste("Jmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), ...)
            print(plot1, position = c(0, 0, 1, 0.333), more = TRUE)
            print(plot2, position = c(0, 0.333, 1, 0.666), more = TRUE)
            print(plot3, position = c(0, 0.666, 1, 1))
        } else if (type == "density") {
            ## ploting the density
            plot1 <- densityplot(~tmpvec11 + tmpvec12, xlab = "Vmax", plot.points = FALSE, 
                ...)
            plot2 <- densityplot(~tmpvec21 + tmpvec22, xlab = "Vpmax", plot.points = FALSE, 
                ...)
            plot3 <- densityplot(~tmpvec31 + tmpvec32, xlab = "Jmax", plot.points = FALSE, 
                ...)
            print(plot1, position = c(0, 0, 1, 0.25), more = TRUE)
            print(plot2, position = c(0, 0.25, 1, 0.5), more = TRUE)
            print(plot3, position = c(0, 0.75, 1, 1))
        }
    } else {
        n1 <- x$accept2
        n2 <- x2$accept2
        n3 <- x3$accept2
        minchainLength <- min(n1, n2, n3)
        tmpvec11 <- x$resuMC[1, 1:minchainLength]
        tmpvec12 <- x2$resuMC[1, 1:minchainLength]
        tmpvec13 <- x3$resuMC[1, 1:minchainLength]
        tmpvec21 <- x$resuMC[2, 1:minchainLength]
        tmpvec22 <- x2$resuMC[2, 1:minchainLength]
        tmpvec23 <- x3$resuMC[2, 1:minchainLength]
        tmpvec31 <- x$resuMC[3, 1:minchainLength]
        tmpvec32 <- x2$resuMC[3, 1:minchainLength]
        tmpvec33 <- x3$resuMC[3, 1:minchainLength]
        if (type == "trace") {
            plot1 <- xyplot(tmpvec11 + tmpvec12 + tmpvec13 ~ 1:minchainLength, xlab = "Iterations", 
                type = "l", ylab = expression(paste("Vmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), ...)
            plot2 <- xyplot(tmpvec21 + tmpvec22 + tmpvec23 ~ 1:minchainLength, xlab = "Iterations", 
                type = "l", ylab = expression(paste("Vpmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), ...)
            plot3 <- xyplot(tmpvec31 + tmpvec32 + tmpvec33 ~ 1:minchainLength, xlab = "Iterations", 
                type = "l", ylab = expression(paste("Jmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), ...)
            print(plot1, position = c(0, 0, 1, 0.333), more = TRUE)
            print(plot2, position = c(0, 0.333, 1, 0.666), more = TRUE)
            print(plot3, position = c(0, 0.666, 1, 1))
        } else if (type == "density") {
            plot1 <- densityplot(~tmpvec11 + tmpvec12 + tmpvec13, xlab = "Vmax", 
                plot.points = FALSE, ...)
            plot2 <- densityplot(~tmpvec21 + tmpvec22 + tmpvec23, xlab = "Vpmax", 
                plot.points = FALSE, ...)
            plot3 <- densityplot(~tmpvec31 + tmpvec32 + tmpvec33, xlab = "Jmax", 
                plot.points = FALSE, ...)
            print(plot1, position = c(0, 0, 1, 0.333), more = TRUE)
            print(plot2, position = c(0, 0.333, 1, 0.666), more = TRUE)
            print(plot3, position = c(0, 0.666, 1, 1))
        }
    }
} 
