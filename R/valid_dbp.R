##
##  BioCro/R/valid_dbp.R by Fernando Ezequiel Miguez  Copyright (C) 2009
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
##' Validate dry biomass partitioning coefficients
##' 
##' It attempts to check the requirements of the dry biomass partitioning
##' coefficients.
##' 
##' 
##' @param x Vector of length equal to 25 containing the dry biomass
##' partitioning coefficients for the 6 phenological stages as for example
##' produced by \code{\link{phenoParms}}.
##' @param tol Numerical tolerance passed to the \code{\link{all.equal}}
##' function.
##' @export
##' @return It will return the vector of coefficients unchanged if no errors
##' are detected.
##' @author Fernando E. Miguez
##' @seealso \code{\link{BioGro}}
##' @keywords utilities
##' @examples
##' 
##' xx <- as.vector(unlist(phenoParms())[7:31])
##' valid_dbp(xx)
##' 
valid_dbp <- function(x, tol=1e-3){
  if(!is.vector(x) || length(x) != 25)
    stop("x should be a vector of length equal to 25")
  ## First stage
  p1 <- x[1:4]
  if(!isTRUE(all.equal(sum(p1[p1 > 0]),1,tolerance=tol))){
    cat("phen stage 1:",p1[p1 > 0]," sum:",sum(p1[p1 > 0]),"\n")
    stop("Phen stage 1 is not valid")
  }
  if(length(p1[p1 < 0]) > 0){
    if(p1[p1 < 0] < -1)
      stop("Phen stage 1 negative coefficient is less than -1")
  }
  ## Second stage
  p2 <- x[5:8]
  if(!isTRUE(all.equal(sum(p2[p2 > 0]),1,tolerance=tol))){
    cat("phen stage 2:",p2[p2 > 0]," sum:",sum(p2[p2 > 0]),"\n")
    stop("Phen stage 2 is not valid")
  }
  if(length(p2[p2 < 0]) > 0){
    if(p2[p2 < 0] < -1)
      stop("Phen stage 2 negative coefficient is less than -1")
  }
  ## Third stage
  p3 <- x[9:12]
  if(!isTRUE(all.equal(sum(p3[p3 > 0]),1,tolerance=tol))){
    cat("phen stage 3:",p3[p3 > 0]," sum:",sum(p3[p3 > 0]),"\n")
    stop("Phen stage 3 is not valid")
  }
  if(length(p3[p3 < 0]) > 0){
    if(p3[p3 < 0] < -1)
      stop("Phen stage 3 negative coefficient is less than -1")
  }
  ## Fourth stage
  p4 <- x[13:16]
  if(!isTRUE(all.equal(sum(p4[p4 > 0]),1,tolerance=tol))){
    cat("phen stage 4:",p4[p4 > 0]," sum:",sum(p4[p4 > 0]),"\n")
    stop("Phen stage 4 is not valid")
  }
  if(length(p4[p4 < 0]) > 0){
    if(p4[p4 < 0] < -1)
      stop("Phen stage 4 negative coefficient is less than -1")
  }
  ## Fifth stage
  p5 <- x[17:20]
  if(!isTRUE(all.equal(sum(p5[p5 > 0]),1,tolerance=tol))){
    cat("phen stage 5:",p5[p5 > 0]," sum:",sum(p5[p5 > 0]),"\n")
    print.default(p5[p5 > 0],digits=10)
    print.default(sum(p5[p5 > 0]),digits=10)
    stop("Phen stage 5 is not valid")
  }
  if(length(p5[p5 < 0]) > 0){
    if(p5[p5 < 0] < -1)
      stop("Phen stage 5 negative coefficient is less than -1")
  }
  ## Sixth stage
  p6 <- x[21:25]
  if(!isTRUE(all.equal(sum(p6[p6 > 0]),1,tolerance=tol))){
    cat("phen stage 6:",p6[p6 > 0]," sum:",sum(p6[p6 > 0]),"\n")
    stop("Phen stage 6 is not valid")
  }
  if(length(p6[p6 < 0]) > 0){
    if(p6[p6 < 0] < -1)
      stop("Phen stage 6 negative coefficient is less than -1")
  }
  x
}