idbpm <- function(data, MaizePhenoControl=list()){
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
