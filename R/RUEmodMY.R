
## This is a function I'm writing to handle multiple years of weather data
## The function computes the RUE model with all defaults.

## Order of input variables
##---------------------------------------------------------------------##
# FIRST COLUMN : YEAR
# SECOND COLUMN : DOY
# THIRD COLUMN : SOLAR RADIATION (MJ M-2 day-1)
# FOURTH COLUMN : AVERAGE AIR TEMPERATURE (FARENHEIT)

RUEmodMY <- function(weatherdatafile,doy.s=91,doy.f=227,...) {

X <- weatherdatafile

minY <- min(X[,1])
maxY <- max(X[,1])

nyrs1 <- maxY - minY + 1
ndys1 <- doy.f - doy.s + 1
nrs1 <- nyrs1 * ndys1 + 1

   res <- NULL
   for(i in minY:maxY){
     tmp1 <- X[X[,1] == i,]
     Rad <- X[,3]
     T.a <- X[,4]
     tmp2 <- RUEmod(Rad,T.a,...)
     tmp2.dat <- as.matrix(data.frame(year=i,tmp2))
     res <- rbind(res,tmp2.dat)
   }

data.frame(res)

}
