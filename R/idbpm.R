##  BioCro/R/idbpm.R by Fernando Ezequiel Miguez  Copyright (C) 2012

##' This estimates initial dry biomass partitioning coefficients based on data for an annual grass
##' @title idbpm
##' @param data Data frame, e.g.:
##' \preformatted{
##'      ThermalT     Stem    Leaf    Root Grain     LAI
##' 1       0.211  0.00733 0.00104 0.00704     0 0.00119
##' 611   280.000  1.08019 0.95531 0.11618     0 1.62350
##' 1221  560.000  5.91862 2.08684 1.42061     0 3.54748
##' 1831  747.000 10.16707 2.36378 2.53192     0 4.01843
##' 2442  969.000 15.08485 2.42849 3.57410     0 4.12843
##' 3052 1080.000 18.56392 2.46765 4.32115     0 4.19501
##' 3662 1136.000 20.87121 2.04021 4.82178     0 3.46836
##' 4273 1452.000 22.05770 0.89954 5.20210     0 1.52921
##' }
##' @param MaizePhenoControl FILL IN HERE
##' @return Vector of biomass pools.
##' @author Fernando E. Miguez
idbpm <- function(data, MaizePhenoControl=list()){
  ## should have t
  if(any(is.na(data)))
    stop("missing data are not allowed")

  dnames <- c("ThermalT","Stem","Leaf","Root","Grain","LAI")
  if(any(is.na(unlist(lapply(names(data),pmatch,dnames)))))
    warning("data names and/or order might be wrong")

  maizephenoP <- MaizePhenoParms()
  maizephenoP[names(MaizePhenoControl)] <- MaizePhenoControl

  TT.V6 <- maizephenoP$phyllochron1 * 6
  TT.V12 <- maizephenoP$phyllochron1 * 12
  TT.R <- as.vector(unlist(maizephenoP)[6:11])
  TPcoefs <- c(TT.V6, TT.V12, TT.R[1], TT.R[6])
  
  n1dat <- numeric(4)
  for(i in 1:4) n1dat[i] <- nrow(data[data[,1] <= TPcoefs[i],])

  ## coefficients for the first stage
  if(n1dat[1] > 1){
    dat1 <- data[c(1,n1dat[1]),]
    delta.dat1 <- dat1[2,] - dat1[1,]
  }else{
    if(n1dat[1] == 1){
      warning("only one row for phen stage 1")
    }else{
      warning("zero rows for phen stage 1")
    }
    dat1 <- data[1,]
    delta.dat1 <- dat1
    c1s.r <- -1e-4
  }
  rsum.dat1 <- sum(delta.dat1[2:4]) 

  c1s <- c(delta.dat1[2:4]/rsum.dat1)
  c1s <- as.vector(unlist(c(c1s)))

  ## coefficients for the second stage
  dat2 <- data[n1dat[1:2],]
  delta.dat2 <- dat2[2,] - dat2[1,]
  rsum.dat2 <- sum(delta.dat2[2:4])
  c2s <- c(delta.dat2[2:4]/rsum.dat2)
  c2s <- as.vector(unlist(c(c2s)))

  ## coefficients for the third stage
  dat3 <- data[n1dat[2:3],];## print(dat3)
  delta.dat3 <- dat3[2,] - dat3[1,];## print(delta.dat3)
  rsum.dat3 <- sum(delta.dat3[2:5])
  c3s <- c(delta.dat3[2:4]/rsum.dat3)
  c3s <- as.vector(unlist(c(c3s)))

  ## coefficients for the fourth stage
  dat4 <- data[n1dat[3:4],]
  delta.dat4 <- dat4[2,] - dat4[1,]; ##print(delta.dat4)
  delta.dat4[delta.dat4 < 0] <- 1e-6
  rsum.dat4 <- sum(delta.dat4[2:5])
  c4s <- c(delta.dat4[2:5]/rsum.dat4)
  c4s <- as.vector(unlist(c(c4s)))

  cfs <- as.vector(unlist(c(c1s,c2s,c3s,c4s)))

  cfs

}
