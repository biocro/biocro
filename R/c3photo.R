##  /R/c3photo.R by Fernando Ezequiel Miguez  Copyright (C) 2009-2010

##' Simulates C3 photosynthesis
##'
##' Simulates coupled assimilation and stomatal conductance based on Farquhar
##' and Ball-Berry.
##' @param Qp Quantum flux
##' @param Tl Leaf temperature
##' @param RH Relative humidity (fraction -- 0-1)
##' @param vcmax Maximum rate of carboxylation
##' @param jmax Maximum rate of electron transport
##' @param Rd Leaf dark respiration
##' @param Catm Atmospheric carbon dioxide.
##' @param O2 Atmospheric Oxygen concentration (mmol/mol)
##' @param b0 Intercept for the Ball-Berry model
##' @param b1 Slope for the Ball-Berry model
##' @param theta Curvature parameter
##' @export
##' @return A list
##' \itemize{
##' \item Gs Stomatal Conductance
##' \item Assim CO2 Assimilation
##' \item Ci Intercellular CO2
##' }
##' @note ~~further notes~~ ## Additional notes about assumptions
##' @author Fernando E. Miguez
##' @seealso See Also \code{\link{Opc3photo}}
##' @references
##'
##' Farquhar (1980) Ball-Berry (1987)
##' @keywords models
##' @examples
##'
##'
##' ## Testing the c3photo function
##' ## First example: looking at the effect of changing Vcmax
##' Qps <- seq(0,2000,10)
##' Tls <- seq(0,55,5)
##' rhs <- c(0.7)
##' dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
##'
##' res1 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH) ## default Vcmax = 100
##' res2 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH,vcmax=120)
##'
##' ## Plot comparing alpha 0.04 vs. 0.06 for a range of conditions
##' lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
##'             type='l',col=c('blue','green'),lwd=2,
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'             key=list(text=list(c('Vcmax 100','Vcmax 120')),
##'               lines=TRUE,col=c('blue','green'),lwd=2))
##'
##' ## Second example: looking at the effect of changing Jmax
##' ## Plot comparing Jmax 300 vs. 100 for a range of conditions
##'
##' res1 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH) ## default Jmax = 300
##' res2 <- c3photo(dat1$Qp,dat1$Tl,dat1$RH,jmax=100)
##'
##' lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
##'            type='l',col=c('blue','green'),lwd=2,
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'             key=list(text=list(c('Jmax 300','Jmax 100')),
##'               lines=TRUE,col=c('blue','green'),lwd=2))
##'
##' ## A/Ci curve
##'
##' Ca <- seq(20,1000,length.out=50)
##' dat2 <- data.frame(Qp=rep(700,50), Tl=rep(25,50), rh=rep(0.7,50))
##' res1 <- c3photo(dat2$Qp, dat2$Tl, dat2$rh, Catm = Ca)
##' res2 <- c3photo(dat2$Qp, dat2$Tl, dat2$rh, Catm = Ca, vcmax = 70)
##'
##' lattice::xyplot(res1$Assim ~ res1$Ci,
##'            lwd=2,
##'            panel = function(x,y,...){
##'                    lattice::panel.xyplot(x,y,type='l',col='blue',...)
##'                    lattice::panel.xyplot(res2$Ci,res2$Assim, type='l', col =
##'            'green',...)
##'            },
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')))
##'
##'
##'
c3photo <- function(Qp,Tl,RH,vcmax=100,jmax=180,Rd=1.1,Catm=380,O2=210,b0=0.08,b1=5,theta=0.7,StomWS=1.0,ws=c("gs","vmax"))
{ 
  ws <- match.arg(ws)
  if(ws == "gs") ws <- 1
  else ws <- 0

  if((length(Qp) != length(Tl)) || (length(Qp) != length(RH)))
    stop("the lengths of the Qp, Tl and RH vectors should be equal")
  
    if(max(RH) > 1)
        stop("RH should be between 0 and 1")
##     if(Catm < 20)
##         warning("Stomatal conductance is not reliable for values of Catm lower than 20\n")
##     if(Catm < 15)
##         stop("Assimilation is not reliable for low (<15) Catm values")
    
    if(length(Catm) == 1){
      Catm <- rep(Catm,length(Qp))
    }else{
      if(length(Catm) != length(Qp))
        stop("length of Catm should be either 1 or equal to length of Qp")
    }

    res <- .Call(c3photo_sym,as.double(Qp),
                 as.double(Tl),as.double(RH),
                 as.double(vcmax),as.double(jmax),
                 as.double(Rd),as.double(Catm),
                 as.double(b0),as.double(b1),
                 as.double(O2),as.double(theta),
                 as.double(StomWS),as.integer(ws))
    res
}

