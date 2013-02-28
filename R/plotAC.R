##
##  /R/plotAC.R by Fernando Ezequiel Miguez  Copyright (C) 2009
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 or 3 of the License
##  (at your option).
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  A copy of the GNU General Public License is available at
##  http://www.r-project.org/Licenses/
##
##


## Function to plot A/Ci curves
##
## data are assumed to have
##
## col 1: trt
## col 2 (optional): other treatment factor
## col 2: Assimilation
## col 3: Quantum flux
## col 4: Temperature
## col 5: Relative humidity
## col 6: Intercellular CO2
## col 7: Reference CO2
##' Plotting function for A/Ci curves
##'
##' A small helper function that can be used to easily plot multiple A/Ci
##' curves
##'
##'
##' @param data Input data in the format needed for the
##' \code{\link{mOpc4photo}}.
##' @param fittd Optional fitted values.
##' @param id.col Specify which column has the ids. Default is col 1.
##' @param trt.col Specify which column has the treatments. Default is col 2.
##' If no treatment is specified then use 1.
##' @param ylab Label for the y-axis.
##' @param xlab Label for the x-axis.
##' @param by Whether to plot by id or by treatment.
##' @param type this argument is passed to the \code{\link{xyplot}}. It changes
##' the plotting symbols behavior.
##' @export
##' @return NULL %% ~Describe the value returned %% If it is a LIST, use %%
##' \item{comp1 }{Description of 'comp1'} %% \item{comp2 }{Description of
##' 'comp2'} %% ...
##' @author Fernando E. Miguez
##' @seealso See Also \code{\link{xyplot}}.
##' @keywords hplot
##' @examples
##'
##' data(aci)
##' plotAC(aci, trt.col=1)
##'
##'
plotAC <- function(data,fittd,
                   id.col=1,
                   trt.col=2,
                   ylab="CO2 Uptake",
                   xlab="Ci",
                   by=c("trt","ID"),
                   type=c("p","smooth")){

  by <- match.arg(by)
  Acol <- trt.col + 1
  Ci.col <- trt.col + 5

  if(missing(fittd) || is.null(fittd)){
    if(by == "trt"){
      plot1 <- xyplot(data[,Acol] ~ data[,Ci.col] | factor(data[,trt.col]),
                      ylab = ylab, type = type,
                      xlab = xlab)
      plot(plot1)
    }else{
      plot1 <- xyplot(data[,Acol] ~ data[,Ci.col] | factor(data[,id.col]),
                      ylab = ylab, type = type,
                      xlab = xlab)
      plot(plot1)
    }
  }else{
    stopifnot(length(fittd) == nrow(data))
    if(by == "trt"){
      plot1 <- xyplot(data[,Acol] + fittd ~ data[,Ci.col] | factor(data[,trt.col]),
                      ylab = ylab, type = type,
                      xlab = xlab)
      plot(plot1)
    }else{
      plot1 <- xyplot(data[,Acol] + fittd ~ data[,Ci.col] | factor(data[,id.col]),
                      ylab = ylab, type = type,
                      xlab = xlab)
      plot(plot1)
    }
  }

}
