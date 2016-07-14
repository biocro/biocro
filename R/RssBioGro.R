## Very simple function to compare the distance between simulated
## and observed data for the BioGro function
## Need to add an argument such as pc.sigmas "plant component sigmas"
## If variability of the plant component is known
##' Residual sum of squares for BioGro.
##'
##' Computes residual sum of squares for the \code{\link{BioGro}} function.
##'
##'
##' @param obs Observed data.
##' @param sim Simulated data.
##' @export
##' @return Atomic vector with the residual sum of squares.
##' @author Fernando E. Miguez
##' @seealso See also \code{\link{BioGro}}.
##' @keywords models
##' @examples
##'
##' ## A simple example
##' data(annualDB)
##' data(EngWea94i)
##' res <- BioGro(EngWea94i)
##' RssBioGro(annualDB,res)
##'
RssBioGro <- function(obs,sim){

    simDat <- data.frame(TT = sim$ThermalT, Stem = sim$Stem, Leaf = sim$Leaf,
                         Root = sim$Root, Rhiz = sim$Rhizome, 
                         Grain = sim$Grain, LAI = sim$LAI)

    indx <- indfun(obs[,1],simDat[,1])
    simDat2 <- simDat[indx,]
    obs2 <- obs[1:length(indx),]

    dobs2 <- dim(obs2)
    dSimDat2 <- dim(simDat2)
    
    if((dobs2[1] != dSimDat2[1]) || (dobs2[2] != dSimDat2[2])){
      cat("dim obs: ",dim(obs),"\n")
      cat("dim simDat2: ",dim(simDat2),"\n")
       stop("dim(obs) should be equal to dim(sim)")
    }

    resid <- obs2[,-1] - simDat2[,-1]
    resid[is.na(resid)] <- 0
    rss <- sum(resid^2)
    rss
  }
