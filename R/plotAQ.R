##  BioCro/R/plotAQ.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2009

##' Function to plot A/Q curves.
##
##' @title Plot A/Q curve
##' @param data Is assumed to have the following structure:
##' \preformatted{
##' col 1: trt.
##' col 2 (optional): Other treatment factor.
##' col 2: Assimilation.
##' col 3: Quantum flux.
##' col 4: Temperature.
##' col 5: Relative humidity.
##' col 6 (optional): Reference CO2.
##' }
##' @param fittd  FILL IN HERE
##' @param id.col  FILL IN HERE
##' @param trt.col  FILL IN HERE
##' @param ylab  FILL IN HERE
##' @param xlab  FILL IN HERE
##' @param by  FILL IN HERE
##' @param type  FILL IN HERE
##' @param ...  FILL IN HERE
##' @return NULL. Creates plot.
##' @export
##' @author Fernando E. Miguez
plotAQ <- function(data,fittd,
                   id.col=1,
                   trt.col=2,
                   ylab="CO2 Uptake",
                   xlab="Quantum flux",
                   by=c("trt","ID"),type="o",...){

  by <- match.arg(by)
  Acol <- trt.col + 1
  Qp.col <- trt.col + 2

  if(missing(fittd) || is.null(fittd)){
    if(by == "trt"){
      plot1 <- lattice::xyplot(data[,Acol] ~ data[,Qp.col] | factor(data[,trt.col]),
                      ylab = ylab,
                      xlab = xlab,
                      type = type, ...)
                      
      plot(plot1)
    }else{
      plot1 <- lattice::xyplot(data[,Acol] ~ data[,Qp.col] | factor(data[,id.col]),
                      ylab = ylab,
                      xlab = xlab,
                      type = type, ...)
      plot(plot1)
    }
  }else{
    stopifnot(length(fittd) == nrow(data))
    if(by == "trt"){
      plot1 <- lattice::xyplot(data[,Acol] + fittd ~ data[,Qp.col] | factor(data[,trt.col]),
                      ylab = ylab, 
                      xlab = xlab,
                      type = type, ...)
      plot(plot1)
    }else{
      plot1 <- lattice::xyplot(data[,Acol] + fittd ~ data[,Qp.col] | factor(data[,id.col]),
                      ylab = ylab, 
                      xlab = xlab,
                      type = type, ...)
      plot(plot1)
    }
  }

}


