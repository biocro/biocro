## BioCro/R/c4photo.R by Fernando Ezequiel Miguez Copyright (C) 2007-2008
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
## http://www.r-project.org/Licenses/
c4photo <- function(Qp, Tl, RH, vmax = 39, alpha = 0.04, kparm = 0.7, theta = 0.83, 
    beta = 0.93, Rd = 0.8, Catm = 380, b0 = 0.08, b1 = 3, StomWS = 1, ws = c("gs", 
        "vmax")) {
    if ((max(RH) > 1) || (min(RH) < 0)) 
        stop("RH should be between 0 and 1")
    if (any(Catm < 150)) 
        warning("Stomatal conductance is not reliable for values of Catm lower than 150\n")
    if (any(Catm < 15)) 
        stop("Assimilation is not reliable for low (<15) Catm values")
    ws <- match.arg(ws)
    if (ws == "gs") 
        ws <- 1 else ws <- 0
    if (length(Catm) == 1) {
        Catm <- rep(Catm, length(Qp))
    } else {
        if (length(Catm) != length(Qp)) 
            stop("length of Catm should be either 1 or equal to length of Qp")
    }
    res <- .Call(c4photo_sym, as.double(Qp), as.double(Tl), as.double(RH), as.double(vmax), 
        as.double(alpha), as.double(kparm), as.double(theta), as.double(beta), as.double(Rd), 
        as.double(Catm), as.double(b0), as.double(b1), as.double(StomWS), as.integer(ws))
    res
}
MCMCc4photo <- function(data, niter = 20000, ivmax = 39, ialpha = 0.04, ikparm = 0.7, 
    itheta = 0.83, ibeta = 0.93, iRd = 0.8, Catm = 380, b0 = 0.08, b1 = 3, StomWS = 1, 
    ws = c("gs", "vmax"), scale = 1, sds = c(1, 0.005), prior = c(39, 10, 0.04, 0.02)) {
    if (ncol(data) != 4) 
        stop("ncol data should be 4")
    if (length(prior) != 4) 
        stop("length of prior should be 4")
    if (niter < 2) 
        stop("niter should be at least 2")
    assim <- data[, 1]
    qp <- data[, 2]
    temp <- data[, 3]
    rh <- data[, 4]
    ws <- match.arg(ws)
    if (ws == "gs") 
        ws <- 1 else ws <- 0
    res <- .Call(McMCc4photo, as.double(assim), as.double(qp), as.double(temp), as.double(rh), 
        as.integer(niter), as.double(ivmax), as.double(ialpha), as.double(ikparm), 
        as.double(itheta), as.double(ibeta), as.double(iRd), as.double(Catm), as.double(b0), 
        as.double(b1), as.double(StomWS), as.double(scale), as.double(sds[1]), as.double(sds[2]), 
        as.integer(ws), as.double(prior))
    res$resuMC <- t(res$resuMC)
    res$niter <- niter
    colnames(res$resuMC) <- c("Vcmax", "Alpha", "RSS")
    res$prior <- prior
    structure(res, class = "MCMCc4photo")
}
## Function for printing the MCMCc4photo objects
print.MCMCc4photo <- function(x, burnin = 1, level = 0.95, digits = 1, ...) {
    ul <- 1 - (1 - level)/2
    ll <- (1 - level)/2
    xMat <- x$resuMC[burnin:x$niter, 1:2]
    colnames(xMat) <- c("Vmax", "alpha")
    cat("\n Markov chain Monte Carlo for the Collatz C4 photosynthesis model")
    cat("\n Summary of the chain")
    cat("\n Moves:", x$accept, "Prop:", x$accept/x$niter, "\n")
    cat("\n Summaries for vmax and alpha:\n")
    sum1 <- summary(x$resuMC[burnin:x$niter, 1])
    sum2 <- summary(x$resuMC[burnin:x$niter, 2])
    nm <- names(sum1)
    mat <- matrix(rbind(sum1, sum2), nrow = 2, ncol = 6)
    colnames(mat) <- nm
    rownames(mat) <- c("vmax", "alpha")
    print(mat, ...)
    cat("\n", level * 100, "% Quantile Intervals for vmax and alpha:\n")
    qua1 <- quantile(x$resuMC[burnin:x$niter, 1], c(ll, ul))
    qua2 <- quantile(x$resuMC[burnin:x$niter, 2], c(ll, ul))
    mat2 <- rbind(qua1, qua2)
    rownames(mat2) <- c("vmax", "alpha")
    colnames(mat2) <- c(ll, ul)
    print(mat2, ...)
    cat("\n correlation matrix:\n")
    print(cor(xMat), ...)
    cat("\n RSS range:", range(x$resuMC[burnin:x$niter, 3]), "\n")
    invisible(x)
}
plot.MCMCc4photo <- function(x, x2 = NULL, x3 = NULL, plot.kind = c("trace", "density"), 
    type = c("l", "p"), burnin = 1, cols = c("blue", "green", "purple"), prior = FALSE, 
    pcol = "black", ...) {
    plot.kind <- match.arg(plot.kind)
    type <- match.arg(type)
    ## This first code is to plot the first object only Ploting the trace
    if (missing(x2) && missing(x3)) {
        if (plot.kind == "trace") {
            plot1 <- xyplot(x$resuMC[burnin:x$niter, 1] ~ burnin:x$niter, xlab = "Iterations", 
                type = type, col = cols[1], ylab = expression(paste("Vmax (", mu, 
                  mol, " ", m^-2, " ", s^-1, ")")), ...)
            plot2 <- xyplot(x$resuMC[burnin:x$niter, 2] ~ burnin:x$niter, xlab = "Iterations", 
                type = type, col = cols[1], ylab = expression(paste("alpha (", mol, 
                  " ", m^-1, ")")), ...)
            print(plot1, position = c(0, 0, 0.5, 1), more = TRUE)
            print(plot2, position = c(0.5, 0, 1, 1))
        } else if (plot.kind == "density") {
            ## Ploting the density
            if (prior == FALSE) {
                plot1 <- densityplot(~x$resuMC[burnin:x$niter, 1], xlab = "Vmax", 
                  col = cols[1], plot.points = FALSE, ...)
                plot2 <- densityplot(~x$resuMC[burnin:x$niter, 2], xlab = "alpha", 
                  col = cols[1], plot.points = FALSE, ...)
            } else {
                plot1 <- densityplot(~x$resuMC[burnin:x$niter, 1], xlab = "Vmax", 
                  col = cols[1], plot.points = FALSE, panel = function(xi, ...) {
                    panel.densityplot(xi, ...)
                    panel.mathdensity(dmath = dnorm, args = list(mean = x$prior[1], 
                      sd = x$prior[2]), col = pcol)
                  }, ...)
                plot2 <- densityplot(~x$resuMC[burnin:x$niter, 2], xlab = "alpha", 
                  col = cols[1], plot.points = FALSE, panel = function(xi, ...) {
                    panel.densityplot(xi, ...)
                    panel.mathdensity(dmath = dnorm, args = list(mean = x$prior[3], 
                      sd = x$prior[4]), col = pcol)
                  }, ...)
            }
            print(plot1, position = c(0, 0, 0.5, 1), more = TRUE)
            print(plot2, position = c(0.5, 0, 1, 1))
        }
    } else if (missing(x3)) {
        ## This part of the code is to plot objects x and x2 Ploting the trace
        n1 <- x$niter
        n2 <- x2$niter
        maxchainLength <- max(n1, n2)
        tmpvec11 <- x$resuMC[burnin:n1, 1]
        tmpvec12 <- x2$resuMC[burnin:n2, 1]
        tmpvec21 <- x$resuMC[burnin:n1, 2]
        tmpvec22 <- x2$resuMC[burnin:n2, 2]
        ymin1 <- min(c(tmpvec11, tmpvec12)) * 0.95
        ymax1 <- max(c(tmpvec11, tmpvec12)) * 1.05
        ymin2 <- min(c(tmpvec21, tmpvec22)) * 0.95
        ymax2 <- max(c(tmpvec21, tmpvec22)) * 1.05
        if (plot.kind == "trace") {
            plot1 <- xyplot(tmpvec11 ~ burnin:n1, xlim = c(I(burnin - 0.05 * maxchainLength), 
                I(maxchainLength * 1.05)), ylim = c(ymin1, ymax1), xlab = "Iterations", 
                type = "l", ylab = expression(paste("Vmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), panel = function(x, y, ...) {
                  panel.xyplot(x, y, col = cols[1], ...)
                  panel.xyplot(burnin:n2, tmpvec12, col = cols[2], ...)
                }, ...)
            plot2 <- xyplot(tmpvec21 ~ burnin:n2, xlim = c(I(burnin - 0.05 * maxchainLength), 
                I(maxchainLength * 1.05)), ylim = c(ymin2, ymax2), xlab = "Iterations", 
                type = "l", ylab = expression(paste("alpha (", mol, " ", m^-1, ")")), 
                panel = function(x, y, ...) {
                  panel.xyplot(x, y, col = cols[1], ...)
                  panel.xyplot(burnin:n2, tmpvec22, col = cols[2], ...)
                }, ...)
            print(plot1, position = c(0, 0, 0.5, 1), more = TRUE)
            print(plot2, position = c(0.5, 0, 1, 1))
        } else if (plot.kind == "density") {
            ## ploting the density
            plot1 <- densityplot(~tmpvec11 + tmpvec12, xlab = "Vmax", plot.points = FALSE, 
                col = cols[1:2], ...)
            plot2 <- densityplot(~tmpvec21 + tmpvec22, xlab = "alpha", plot.points = FALSE, 
                col = cols[1:2], ...)
            print(plot1, position = c(0, 0, 0.5, 1), more = TRUE)
            print(plot2, position = c(0.5, 0, 1, 1))
        }
    } else {
        n1 <- x$niter
        n2 <- x2$niter
        n3 <- x3$niter
        maxchainLength <- max(n1, n2, n3)
        tmpvec11 <- x$resuMC[burnin:n1, 1]
        tmpvec12 <- x2$resuMC[burnin:n2, 1]
        tmpvec13 <- x3$resuMC[burnin:n3, 1]
        tmpvec21 <- x$resuMC[burnin:n1, 2]
        tmpvec22 <- x2$resuMC[burnin:n2, 2]
        tmpvec23 <- x3$resuMC[burnin:n3, 2]
        ymin1 <- min(c(tmpvec11, tmpvec12, tmpvec13)) * 0.95
        ymax1 <- max(c(tmpvec11, tmpvec12, tmpvec13)) * 1.05
        ymin2 <- min(c(tmpvec21, tmpvec22, tmpvec23)) * 0.95
        ymax2 <- max(c(tmpvec21, tmpvec22, tmpvec23)) * 1.05
        if (plot.kind == "trace") {
            plot1 <- xyplot(tmpvec11 ~ burnin:n1, xlim = c(I(burnin - 0.05 * maxchainLength), 
                I(maxchainLength * 1.05)), ylim = c(ymin1, ymax1), xlab = "Iterations", 
                type = "l", ylab = expression(paste("Vmax (", mu, mol, " ", m^-2, 
                  " ", s^-1, ")")), panel = function(x, y, ...) {
                  panel.xyplot(x, y, col = cols[1], ...)
                  panel.xyplot(burnin:n2, tmpvec12, col = cols[2], ...)
                  panel.xyplot(burnin:n3, tmpvec13, col = cols[3], ...)
                }, ...)
            plot2 <- xyplot(tmpvec21 ~ burnin:n1, xlim = c(I(burnin - 0.05 * maxchainLength), 
                I(maxchainLength * 1.05)), ylim = c(ymin2, ymax2), xlab = "Iterations", 
                type = "l", ylab = expression(paste("alpha (", mol, " ", m^-1, ")")), 
                panel = function(x, y, ...) {
                  panel.xyplot(x, y, col = cols[1], ...)
                  panel.xyplot(burnin:n2, tmpvec22, col = cols[2], ...)
                  panel.xyplot(burnin:n3, tmpvec23, col = cols[3], ...)
                }, ...)
            print(plot1, position = c(0, 0, 0.5, 1), more = TRUE)
            print(plot2, position = c(0.5, 0, 1, 1))
        } else if (plot.kind == "density") {
            plot1 <- densityplot(~tmpvec11 + tmpvec12 + tmpvec13, xlab = "Vmax", 
                plot.points = FALSE, col = cols, ...)
            plot2 <- densityplot(~tmpvec21 + tmpvec22 + tmpvec23, xlab = "alpha", 
                plot.points = FALSE, col = cols, ...)
            print(plot1, position = c(0, 0, 0.5, 1), more = TRUE)
            print(plot2, position = c(0.5, 0, 1, 1))
        }
    }
} 
