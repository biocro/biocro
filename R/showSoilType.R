##  BioCro/R/showSoilType.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2011

SoilType <- function(soiltype){

  print(soiltype)

  if(soiltype == 0){
    nm <- c("sand soil")
    silt <- 0.05
    clay <- 0.03
    sand <- 0.92
    air.entry <- -0.7
    b <- 1.7
    Ks <- 5.8e-3
    satur <- 0.87
    fieldc <- 0.09
    wiltp <-  0.03
    
  } else
    
  if(soiltype == 1){
    nm <- c("loamy sand")
    silt <- 0.12
    clay <- 0.07
    sand <- 0.81
    air.entry <- -0.9
    b <- 2.1
    Ks <- 1.7e-3
    satur <- 0.72
    fieldc <- 0.13
    wiltp <- 0.06

  } else

  if(soiltype == 2){
    nm <- "sandy loam"
    silt <- 0.25
    clay <- 0.10
    sand <- 0.65
    air.entry <- -1.5
    b <- 3.1
    Ks <- 7.2e-4
    satur <- 0.57
    fieldc <- 0.21
    wiltp <- 0.10

  } else

  if(soiltype == 3){
    nm <- "loam"
    silt <- 0.40
    clay <- 0.18
    sand <- 0.52
    air.entry <- -1.1
    b <- 4.5
    Ks <- 3.7e-4
    satur <- 0.57
    fieldc <- 0.27
    wiltp <- 0.12

  } else

  if(soiltype == 4){
    nm <- "silt loam"
    silt <- 0.65
    clay <- 0.15
    sand <- 0.20
    air.entry <- -2.1
    b <- 4.7
    Ks <- 1.9e-4
    satur <- 0.59
    fieldc <- 0.33
    wiltp <- 0.13

  } else

  if(soiltype == 5){
    nm <- "sandy clay loam"
    silt <- 0.13
    clay <- 0.27
    sand <- 0.60
    air.entry <- -2.8
    b <- 4
    Ks <- 1.2e-4
    satur <- 0.48
    fieldc <- 0.26
    wiltp <- 0.15

  } else

  if(soiltype == 6){
    nm <- "clay loam"
    silt <- 0.34
    clay <- 0.34
    sand <- 0.32
    air.entry <- -2.6
    b <- 5.2
    Ks <- 6.4e-5
    satur <- 0.52
    fieldc <- 0.32
    wiltp <- 0.20

  } else

  if(soiltype == 7){
    nm <- "silty clay loam"
    silt <- 0.58
    clay <- 0.33
    sand <- 0.09
    air.entry <- -3.3
    b <- 6.6
    Ks <- 4.2e-5
    satur <- 0.52
    fieldc <- 0.37
    wiltp <- 0.21

  } else

  if(soiltype == 8){
    nm <- "sandy clay"
    silt <- 0.07
    clay <- 0.40
    sand <- 0.53
    air.entry <- -2.9
    b <- 6
    Ks <- 3.3e-5
    satur <- 0.51
    fieldc <- 0.34
    wiltp <- 0.24

  } else

  if(soiltype == 9){
    nm <- "silty clay"
    silt <- 0.45
    clay <- 0.45
    sand <- 0.10
    air.entry <- -3.4
    b <- 7.9
    Ks <- 2.5e-5
    satur <- 0.52
    fieldc <- 0.39
    wiltp <- 0.25

  } else

  if(soiltype == 10){
    nm <- "clay"
    silt <- 0.20 
    clay <- 0.60 
    sand <- 0.20
    air.entry <- -3.7
    b <- 7.6
    Ks <- 1.7e-5
    satur <- 0.53
    fieldc <- 0.4
    wiltp <- 0.27

  }

  ans <- list(type=nm, silt=silt,clay=clay, sand=sand, air.entry=air.entry, b=b, Ks=Ks, satur=satur, fieldc=fieldc, wiltp=wiltp) 
  return(ans)
}


showSoilType <- function(soiltype){

if((soiltype < 0) || (soiltype > 10))
  stop("Incorrect soiltype")

soiltype <- as.character(soiltype)

switch(soiltype,

       `0` = {
         cat("sand soil \n")
         cat("silt = ",0.05, "\n")
         cat("clay = ",0.03, "\n")
         cat("sand = ",0.92, "\n")
         cat("air entry = ",-0.7,"\n")
         cat("b = ", 1.7, "\n")
         cat("Ks = ", 5.8e-3, "\n")
         cat("satur = ", 0.87, "\n")
         cat("fieldc = ", 0.09, "\n")
         cat("wiltp = ", 0.03, "\n")
       },

       `1` = {
         cat(" loamy sand  \n")
         cat("silt = ", 0.12, "\n")
         cat("clay = ", 0.07, "\n")
         cat("sand = ", 0.81, "\n")
         cat("air entry = ",-0.9,"\n")
         cat("b = ", 2.1, "\n")
         cat("Ks = ", 1.7e-3, "\n")
         cat("satur = ", 0.72, "\n")
         cat("fieldc = ", 0.13, "\n")
         cat("wiltp = ", 0.06, "\n")
       },

       `2` = {
         cat(" sandy loam  \n")
         cat("silt = ", 0.25, "\n")
         cat("clay = ", 0.10, "\n")
         cat("sand = ", 0.65, "\n")
         cat("air entry = ",-1.5,"\n")
         cat("b = ", 3.1, "\n")
         cat("Ks = ", 7.2e-4, "\n")
         cat("satur = ", 0.57, "\n")
         cat("fieldc = ", 0.21, "\n")
         cat("wiltp = ", 0.10, "\n")
       },

       `3` = {
         cat(" loam  \n")
         cat("silt = ", 0.40, "\n")
         cat("clay = ", 0.18, "\n")
         cat("sand = ", 0.52, "\n")
         cat("air entry = ",-1.1,"\n")
         cat("b = ", 4.5, "\n")
         cat("Ks = ", 3.7e-4, "\n")
         cat("satur = ", 0.57, "\n")
         cat("fieldc = ", 0.27, "\n")
         cat("wiltp = ", 0.12, "\n")
       },

       `4` = {
         cat(" silt loam  \n")
         cat("silt = ", 0.65, "\n")
         cat("clay = ", 0.15, "\n")
         cat("sand = ", 0.20, "\n")
         cat("air entry = ",-2.1,"\n")
         cat("b = ", 4.7, "\n")
         cat("Ks = ", 1.9e-4, "\n")
         cat("satur = ", 0.59, "\n")
         cat("fieldc = ", 0.33, "\n")
         cat("wiltp = ", 0.13, "\n")
       },

       `5` = {
         cat(" sandy clay loam  \n")
         cat("silt = ", 0.13, "\n")
         cat("clay = ", 0.27, "\n")
         cat("sand = ", 0.60, "\n")
         cat("air entry = ",-2.8,"\n")
         cat("b = ", 4, "\n")
         cat("Ks = ", 1.2e-4, "\n")
         cat("satur = ", 0.48, "\n")
         cat("fieldc = ", 0.26, "\n")
         cat("wiltp = ", 0.15, "\n")
       },

       `6` = {
         cat(" clay loam  \n")
         cat("silt = ", 0.34, "\n")
         cat("clay = ", 0.34, "\n")
         cat("sand = ", 0.32, "\n")
         cat("air entry = ",-2.6,"\n")
         cat("b = ", 5.2, "\n")
         cat("Ks = ", 6.4e-5, "\n")
         cat("satur = ", 0.52, "\n")
         cat("fieldc = ", 0.32, "\n")
         cat("wiltp = ", 0.20, "\n")
       },

       `7` = {
         cat(" silty clay loam  \n")
         cat("silt = ", 0.58, "\n")
         cat("clay = ", 0.33, "\n")
         cat("sand = ", 0.09, "\n")
         cat("air entry = ",-3.3,"\n")
         cat("b = ", 6.6, "\n")
         cat("Ks = ", 4.2e-5, "\n")
         cat("satur = ", 0.52, "\n")
         cat("fieldc = ", 0.37, "\n")
         cat("wiltp = ", 0.21, "\n")
       },

       `8` = {
         cat(" sandy clay  \n")
         cat("silt = ", 0.07, "\n")
         cat("clay = ", 0.40, "\n")
         cat("sand = ", 0.53, "\n")
         cat("air entry = ",-2.9,"\n")
         cat("b = ", 6, "\n")
         cat("Ks = ", 3.3e-5, "\n")
         cat("satur = ", 0.51, "\n")
         cat("fieldc = ", 0.34, "\n")
         cat("wiltp = ", 0.24, "\n")
       },

       `9` = {
         cat(" silty clay  \n")
         cat("silt = ", 0.45, "\n")
         cat("clay = ", 0.45, "\n")
         cat("sand = ", 0.10, "\n")
         cat("air entry = ",-3.4,"\n")
         cat("b = ", 7.9, "\n")
         cat("Ks = ", 2.5e-5, "\n")
         cat("satur = ", 0.52, "\n")
         cat("fieldc = ", 0.39, "\n")
         cat("wiltp = ", 0.25, "\n")
       },

       `10` = {
         cat(" clay  \n")
         cat("silt = ", 0.20, "\n")
         cat("clay = ", 0.60, "\n")
         cat("sand = ", 0.20, "\n")
         cat("air entry = ",-3.7,"\n")
         cat("b = ", 7.6, "\n")
         cat("Ks = ", 1.7e-5, "\n")
         cat("satur = ", 0.53, "\n")
         cat("fieldc = ", 0.4, "\n")
         cat("wiltp = ", 0.27, "\n")
       })

}
