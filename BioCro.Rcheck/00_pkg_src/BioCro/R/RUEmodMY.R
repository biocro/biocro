
## This is a function I'm writing to handle multiple years of weather data
## The function computes the RUE model with all defaults.

## Order of input variables
##---------------------------------------------------------------------##
# FIRST COLUMN : YEAR
# SECOND COLUMN : DOY
# THIRD COLUMN : SOLAR RADIATION (MJ M-2 day-1)
# FOURTH COLUMN : AVERAGE AIR TEMPERATURE (FARENHEIT)
##' Radiation use efficiency based model
##'
##' Same as \code{\link{RUEmod}} but it handles multiple years.
##'
##'
##' @param weatherdatafile weather data file (see example).
##' @param doy.s first day of the growing season, default 91.
##' @param doy.f last day of the growing season, default 227.
##' @param ... additional arguments to be passed to the \code{\link{RUEmod}}
##' function.
##' @export
##' @return a \code{\link{data.frame}} structure with components
##' @returnItem year simulation year.
##' @returnItem doy day of the year.
##' @returnItem lai.cum cumulative leaf area index.
##' @returnItem AG.cum cumulative above ground dry biomass (Mg \eqn{ha^{-1}}).
##' @returnItem AGDD cumulative growing degree days.
##' @returnItem Int.e Intercepted solar radiation.
##' @keywords models
##' @examples
##'
##' ## weather data from Champaign, IL
##' data(cmiWet)
##' tmp1 <- RUEmodMY(cmiWet)
##'
##' xyplot(AG.cum ~ doy | factor(year), type='l', data = tmp1,
##'        lwd=2,
##'        ylab=expression(paste('dry biomass (Mg ',ha^-1,')')),
##'        xlab='DOY')
##'
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
