## /R/plotAC.R by Fernando Ezequiel Miguez Copyright (C) 2009
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
## http://www.r-project.org/Licenses/ Function to plot A/Ci curves
## 
## data are assumed to have
## 
## col 1: trt col 2 (optional): other treatment factor col 2: Assimilation col
## 3: Quantum flux col 4: Temperature col 5: Relative humidity col 6:
## Intercellular CO2 col 7: Reference CO2
plotAC <- function(data, fittd, id.col = 1, trt.col = 2, ylab = "CO2 Uptake", xlab = "Ci", 
    by = c("trt", "ID"), type = c("p", "smooth")) {
    by <- match.arg(by)
    Acol <- trt.col + 1
    Ci.col <- trt.col + 5
    if (missing(fittd) || is.null(fittd)) {
        if (by == "trt") {
            plot1 <- xyplot(data[, Acol] ~ data[, Ci.col] | factor(data[, trt.col]), 
                ylab = ylab, type = type, xlab = xlab)
            plot(plot1)
        } else {
            plot1 <- xyplot(data[, Acol] ~ data[, Ci.col] | factor(data[, id.col]), 
                ylab = ylab, type = type, xlab = xlab)
            plot(plot1)
        }
    } else {
        stopifnot(length(fittd) == nrow(data))
        if (by == "trt") {
            plot1 <- xyplot(data[, Acol] + fittd ~ data[, Ci.col] | factor(data[, 
                trt.col]), ylab = ylab, type = type, xlab = xlab)
            plot(plot1)
        } else {
            plot1 <- xyplot(data[, Acol] + fittd ~ data[, Ci.col] | factor(data[, 
                id.col]), ylab = ylab, type = type, xlab = xlab)
            plot(plot1)
        }
    }
} 
