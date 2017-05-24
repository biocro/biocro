##  BioCro/R/c4photo.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2015
## Contributions by Deepak Jaiswal for the temperature response

##' Coupled photosynthesis-stomatal conductance simulation
##'
##' The mathematical model is based on Collatz et al (1992) (see References).
##' Stomatal conductance is based on code provided by Joe Berry.
##'
##'
##' @param Qp quantum flux (direct light), (\eqn{\mu}{micro} mol
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' @param Tl temperature of the leaf (Celsius).
##' @param RH relative humidity (as a fraction, i.e. 0-1).
##' @param vmax maximum carboxylation of Rubisco according to the Collatz
##' model.
##' @param alpha alpha parameter according to the Collatz model. Initial slope
##' of the response to Irradiance.
##' @param kparm k parameter according to the Collatz model. Initial slope of
##' the response to CO2.
##' @param theta theta parameter according to the Collatz model. Curvature for
##' light response.
##' @param beta beta parameter according to the Collatz model. Curvature for
##' response to CO2.
##' @param Rd Rd parameter according to the Collatz model. Dark respiration.
##' @param uppertemp parameter controlling the upper temperature response
##' @param lowertemp parameter controlling the lower temperature response
##' @param Catm Atmospheric CO2 in ppm (or \eqn{\mu}{micro}mol/mol).
##' @param b0 intercept for the Ball-Berry stomatal conductance model.
##' @param b1 slope for the Ball-Berry stomatal conductance model.
##' @param StomWS coefficient which controls the effect of water stress on
##' stomatal conductance and assimilation.
##' @param ws option to control whether the water stress factor is applied to
##' stomatal conductance ('gs') or to Vmax ('vmax').
##' @export
##' @return a \code{\link{list}} structure with components
##' \itemize{
##' \item Gs stomatal conductance (mmol \eqn{m^-2}{m-2} \eqn{s^-
##' }{s-1}\eqn{ 1}{s-1}).
##' \item Assim Net Assimilation (\eqn{\mu}{micro}mol \eqn{m^-2}{m-2}
##' \eqn{s^-1}{s-1}).
##' \item Ci Intercellular CO2 (\eqn{\mu}{micro}mol \eqn{mol^-1}{mol-1}).
##' }
##' @seealso \code{\link{eC4photo}}
##' @references G. Collatz, M. Ribas-Carbo, J. Berry. (1992).  Coupled
##' photosynthesis-stomatal conductance model for leaves of C4 plants.
##' \emph{Australian Journal of Plant Physiology} 519--538.
##' @keywords models
##' @examples
##'
##' \dontrun{
##'      ## First example: looking at the effect of changing alpha
##'       Qps <- seq(0,2000,10)
##'       Tls <- seq(0,55,5)
##'       rhs <- c(0.7)
##'       dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
##'       res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH) ## default alpha = 0.04
##'       res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH,alpha=0.06)
##'
##'      ## Plot comparing alpha 0.04 vs. 0.06 for a range of conditions
##'      lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
##'             type='l',col=c('blue','green'),lwd=2,
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'             key=list(text=list(c('alpha 0.04','alpha 0.06')),
##'               lines=TRUE,col=c('blue','green'),lwd=2))
##'
##'      ## Second example: looking at the effect of changing vmax
##'      ## Plot comparing Vmax 39 vs. 50 for a range of conditions
##'
##'       res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH) ## default Vmax = 39
##'       res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH,vmax=50)
##'
##'      lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
##'             type='l',col=c('blue','green'),lwd=2,
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'             key=list(text=list(c('Vmax 39','Vmax 50')),
##'               lines=TRUE,col=c('blue','green'),lwd=2))
##'
##'      ## Small effect of low RH on  Assim
##'       Qps <- seq(0,2000,10)
##'       Tls <- seq(0,55,5)
##'       rhs <- c(0.2,0.9)
##'       dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
##'       res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH)
##'      # plot for Assimilation and two RH
##'       lattice::xyplot(res1$Assim ~ Qp | factor(Tl) , data = dat1,
##'              groups=RH, type='l',
##'              col=c('blue','green'),lwd=2,
##'              ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              key=list(text=list(c('RH 20%','RH 90%')),
##'                         lines=TRUE,col=c('blue','green'),
##'                         lwd=2))
##'
##'     ## Effect of the previous runs on Stomatal conductance
##'
##'     lattice::xyplot(res1$Gs ~ Qp | factor(Tl) , data = dat1,
##'            type='l', groups=RH,
##'            col=c('blue','green'),lwd=2,
##'            ylab=expression(paste('Stomatal Conductance (',
##'                            mu,mol,' ',m^-2,' ',s^-1,')')),
##'            xlab=expression(paste('Quantum flux (',
##'                            mu,mol,' ',m^-2,' ',s^-1,')')),
##'            key=list(text=list(c('RH 20%','RH 90%')),
##'                      lines=TRUE,col=c('blue','green'),
##'                      lwd=2))
##'
##'
##' ## A Ci curve for the Collatz model
##' ## Assuming constant values of Qp, Temp, and RH
##' ## Notice the effect of the different kparm
##' ## The loop is needed because the length of Ca
##' ## should be the same as Qp
##'
##' Ca <- seq(15,400,5)
##'
##' res1 <- numeric(length(Ca))
##' res2 <- numeric(length(Ca))
##' for(i in 1:length(Ca)){
##'   res1[i] <- c4photo(1500,25,0.7,Catm=Ca[i])$Assim
##'   res2[i] <- c4photo(1500,25,0.7,Catm=Ca[i],kparm=0.8)$Assim
##' }
##'
##' lattice::xyplot(res1 + res2 ~ Ca ,type='l',lwd=2,
##'        col=c('blue','green'),
##'      xlab=expression(paste(CO[2],' (ppm)')),
##'      ylab=expression(paste('Assimilation (',
##'          mu,mol,' ',m^-2,' ',s^-1,')')),
##'      key=list(text=list(c('kparm 0.7','kparm 0.8')),
##'                         lines=TRUE,col=c('blue','green'),
##'                         lwd=2))
##'
##' ## Effect of Reduction in Assimilation due to
##' ## water stress
##'
##' Qps <- seq(0,2000,10)
##' Tls <- seq(0,55,5)
##' rhs <- c(0.7)
##' dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
##' res1 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH) ## default StomWS = 1 No stress
##' res2 <- c4photo(dat1$Qp,dat1$Tl,dat1$RH,StomWS=0.5)
##'
##' ## Plot comparing StomWS = 1 vs. 0.5 for a range of conditions
##' lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
##'        type='l',col=c('blue','green'),lwd=2,
##'        ylab=expression(paste('Assimilation (',
##'            mu,mol,' ',m^-2,' ',s^-1,')')),
##'        xlab=expression(paste('Quantum flux (',
##'             mu,mol,' ',m^-2,' ',s^-1,')')),
##'        key=list(text=list(c('StomWS 1','StomWS 0.5')),
##'            lines=TRUE,col=c('blue','green'),lwd=2))
##'
##'
##' ## Effect on Stomatal Conductance
##' ## Plot comparing StomWS = 1 vs. 0.5 for a range of conditions
##' lattice::xyplot(res1$Gs + res2$Gs ~ Qp | factor(Tl) , data = dat1,
##'         type='l',col=c('blue','green'),lwd=2,
##'         ylab=expression(paste('Stomatal Conductance (mmol ',
##'           m^-2,' ',s^-1,')')),
##'         xlab=expression(paste('Quantum flux (',
##'           mu,mol,' ',m^-2,' ',s^-1,')')),
##'         key=list(text=list(c('StomWS 1','StomWS 0.5')),
##'            lines=TRUE,col=c('blue','green'),lwd=2))
##' }
##'

c4photo <- function(Qp,Tl,RH,vmax=39,alpha=0.04,kparm=0.7,theta=0.83,
                    beta=0.93,Rd=0.8,uppertemp=37.5,lowertemp=3.0,
                    Catm=380,b0=0.08,b1=3,
                    StomWS=1,ws=c("gs","vmax"))
{
    if((max(RH) > 1) || (min(RH) < 0))
        stop("RH should be between 0 and 1")
    if(any(Catm < 150))
        warning("Stomatal conductance is not reliable for values of Catm lower than 150\n")
    if(any(Catm < 15))
        stop("Assimilation is not reliable for low (<15) Catm values")
    ws <- match.arg(ws)
    if(ws == "gs") ws <- 1
    else ws <- 0

    if(length(Catm) == 1){
      Catm <- rep(Catm,length(Qp))
    }else{
      if(length(Catm) != length(Qp))
        stop("length of Catm should be either 1 or equal to length of Qp")
    }
    
    res <- .Call(c4photo_sym,as.double(Qp),
                 as.double(Tl),as.double(RH),
                 as.double(vmax),as.double(alpha),
                 as.double(kparm),as.double(theta),
                 as.double(beta),
                 as.double(Rd),as.double(Catm),
                 as.double(b0),as.double(b1),as.double(StomWS),as.integer(ws),
                 as.double(uppertemp),as.double(lowertemp))
    res
}
##' Markov chain Monte Carlo for C4 photosynthesis parameters
##'
##'
##' This function implement Markov chain Monte Carlo methods for the C4
##' photosynthesis model of Collatz et al.  The chain is constructed using a
##' Gaussian random walk. This is definitely a beta version of this function
##' and more testing and improvements are needed. The value of this function is
##' in the possibility of examining the empirical posterior distribution (i.e.
##' vectors) of the vmax and alpha parameters.  Notice that you will get
##' different results each time you run it.
##'
##'
##' @aliases MCMCc4photo print.MCMCc4photo
##' @param data observed assimilation data, which should be a data frame or
##' matrix.  The first column should be observed net assimilation rate
##' (\eqn{\mu mol \; m^{-2} \; }{micro mol per meter squared per second}\eqn{
##' s^{-1}}{micro mol per meter squared per second}).  The second column should
##' be the observed quantum flux (\eqn{\mu mol \; m^{-2} \; }{micro mol per
##' meter squared per second}\eqn{ s^{-1}}{micro mol per meter squared per
##' second}).  The third column should be observed temperature of the leaf
##' (Celsius).  The fourth column should be the observed relative humidity in
##' proportion (e.g. 0.7).
##' @param niter number of iterations to run the chain for (default = 20000).
##' @param ivmax initial value for Vcmax (default = 39).
##' @param ialpha initial value for alpha (default = 0.04).
##' @param ikparm initial value for kparm (default = 0.7). Not optimized at the
##' moment.
##' @param itheta initial value for theta (default = 0.83). Not optimized at
##' the moment.
##' @param ibeta initial value for beta (default = 0.93). Not optimized at the
##' moment.
##' @param iRd initial value for dark respiration (default = 0.8).
##' @param Catm see \code{\link{c4photo}} function.
##' @param b0 see \code{\link{c4photo}} function.
##' @param b1 see \code{\link{c4photo}} function.
##' @param StomWS see \code{\link{c4photo}} function.
##' @param ws see \code{\link{c4photo}} function.
##' @param scale This scale parameter controls the size of the standard
##' deviations which generate the moves in the chain.
##' @param sds Finer control for the standard deviations of the prior normals.
##' The first element is for vmax and the second for alpha.
##' @param prior Vector of length 4 with first element prior mean for vmax,
##' second element prior standard deviation for vmax, third element prior mean
##' for alpha and fourth element prior standard deviation for alpha.
##' @param uppertemp parameter controlling the upper temperature response
##' @param lowertemp parameter controlling the lower temperature response
##' @export
##' @return
##' an object of class \code{MCMCc4photo} with components
##' \itemize{
##' \item accept number of accepted moves in the chain.
##' \item resuMC matrix of dimensions \code{niter} by 3 containing the
##' values for Vmax and alpha and the RSS in each iteration of the chain.
##' }
##' @references Brooks, Stephen. (1998). Markov chain Monte Carlo and its
##' application. The Statistician. 47, Part 1, pp. 69-100.
##' @keywords optimize
##' @examples
##'
##' \dontrun{
##' ## Using Beale, Bint and Long (1996)
##' data(obsBea)
##'
##' ## Different starting values
##' resB1 <- MCMCc4photo(obsBea, 100000, scale=1.5)
##' resB2 <- MCMCc4photo(obsBea, 100000, ivmax=25, ialpha=0.1, scale=1.5)
##' resB3 <- MCMCc4photo(obsBea, 100000, ivmax=45, ialpha=0.02, scale=1.5)
##'
##' ## Use the plot function to examine results
##' plot(resB1,resB2,resB3)
##' plot(resB1,resB2,resB3,plot.kind='density',burnin=1e4)
##'
##' }
##'
##'

MCMCc4photo <- function(data, niter = 20000, ivmax = 39,
                        ialpha = 0.04, ikparm = 0.7, itheta=0.83,
                        ibeta=0.93, iRd = 0.8, Catm = 380,
                        b0 = 0.08, b1 = 3, StomWS=1, ws=c("gs","vmax"), scale = 1,
                        sds=c(1,0.005),prior=c(39,10,0.04,0.02),
                        uppertemp=37.5,lowertemp=3.0){

    if(ncol(data) != 4)
        stop("ncol data should be 4")

    if(length(prior) != 4)
        stop("length of prior should be 4")

    if(niter < 2)
        stop("niter should be at least 2")
    
    assim <- data[,1]
    qp <- data[,2]
    temp <- data[,3]
    rh <- data[,4]

    ws <- match.arg(ws)
    if(ws == "gs") ws <- 1
    else ws <- 0
    
    res <- .Call(McMCc4photo, as.double(assim), as.double(qp),
                 as.double(temp), as.double(rh), as.integer(niter),
                 as.double(ivmax), as.double(ialpha), as.double(ikparm),
                 as.double(itheta), as.double(ibeta),
                 as.double(iRd), as.double(Catm), as.double(b0), as.double(b1),
                 as.double(StomWS), as.double(scale), as.double(sds[1]),
                 as.double(sds[2]), as.integer(ws), as.double(prior),
                 as.double(uppertemp),as.double(lowertemp))
    res$resuMC <- t(res$resuMC)
    res$niter <- niter
    colnames(res$resuMC) <- c("Vcmax","Alpha","RSS")
    res$prior <- prior
    structure(res, class = "MCMCc4photo")
}




## Function for printing the MCMCc4photo objects
##' @export
##' @S3method print MCMCc4photo
print.MCMCc4photo <- function(x,burnin=1,level=0.95,digits=1,...){

    ul <- 1 - (1-level)/2
    ll <- (1 - level)/2
    xMat <- x$resuMC[burnin:x$niter,1:2]
    colnames(xMat) <- c("Vmax","alpha")
    cat("\n Markov chain Monte Carlo for the Collatz C4 photosynthesis model")
    
    cat("\n Summary of the chain")
    cat("\n Moves:",x$accept,"Prop:",x$accept/x$niter,"\n")
    cat("\n Summaries for vmax and alpha:\n")
    sum1 <- summary(x$resuMC[burnin:x$niter,1])
    sum2 <- summary(x$resuMC[burnin:x$niter,2])
    nm <- names(sum1)
    mat <- matrix(rbind(sum1,sum2),nrow=2,ncol=6)
    colnames(mat) <- nm
    rownames(mat) <- c("vmax","alpha")
    print(mat,...)
    cat("\n",level*100,"% Quantile Intervals for vmax and alpha:\n")
    qua1 <- quantile(x$resuMC[burnin:x$niter,1],c(ll,ul))
    qua2 <- quantile(x$resuMC[burnin:x$niter,2],c(ll,ul))
    mat2 <- rbind(qua1,qua2)
    rownames(mat2) <- c("vmax","alpha")
    colnames(mat2) <- c(ll,ul)
    print(mat2,...)
    cat("\n correlation matrix:\n")
    print(cor(xMat),...)
    cat("\n RSS range:",range(x$resuMC[burnin:x$niter,3]),"\n")
    invisible(x)
    
}
##' Plotting function for MCMCc4photo objects
##'
##' By default it prints the trace of the four parameters being estimated by
##' the \code{\link{MCMCc4photo}} function. As an option the density can be
##' plotted.
##'
##' This function uses internally \code{\link[lattice]{xyplot}},
##' \code{\link[lattice]{densityplot}} and
##' \code{\link[lattice]{panel.mathdensity}} both in the 'lattice' package.
##'
##' @param x \code{\link{MCMCc4photo}} object.
##' @param x2 optional additional \code{\link{MCMCc4photo}} object.
##' @param x3 optional additional \code{\link{MCMCc4photo}} object.
##' @param plot.kind 'trace' plots the iteration history and 'density' plots
##' the kernel density.
##' @param type only the options for line and point are offered.
##' @param burnin this will remove part of the chain that can be considered
##' burn-in period.  The plots will no include this part.
##' @param cols Argument to pass colors to the line or points being plotted.
##' @param prior Whether to plot the prior density. It only works when x2 =
##' NULL and x3 = NULL. Default is FALSE.
##' @param pcol Color used for plotting the prior density.
##' @param \dots Optional arguments.
##' @seealso \code{\link{MCMCc4photo}}
##' @keywords hplot
##' @export
##' @S3method plot MCMCc4photo

plot.MCMCc4photo <- function(x,x2=NULL,x3=NULL,
                             plot.kind=c("trace","density"),type=c("l","p"),
                             burnin=1,cols=c("blue","green","purple"),prior=FALSE,pcol="black",...){

    plot.kind <- match.arg(plot.kind)
    type <- match.arg(type)
    ## This first code is to plot the first object only
    ## Ploting the trace
    if(missing(x2) && missing(x3)){
        if(plot.kind == "trace"){
            plot1 <-  lattice::xyplot(x$resuMC[burnin:x$niter,1] ~ burnin:x$niter ,
                             xlab = "Iterations", type = type, col=cols[1],
                             ylab = expression(paste("Vmax (",mu,mol," ",m^-2," ",s^-1,")")),
                             ...)
            plot2 <-  lattice::xyplot(x$resuMC[burnin:x$niter,2] ~ burnin:x$niter ,
                             xlab = "Iterations", type = type, col=cols[1],
                             ylab = expression(paste("alpha (",mol," ",m^-1,")")),
                             ...)
            print(plot1,position=c(0,0,0.5,1),more=TRUE)
            print(plot2,position=c(0.5,0,1,1))
        } else
        ## Ploting the density
        if(plot.kind == "density"){
            if(prior == FALSE){
                plot1 <-  densityplot(~x$resuMC[burnin:x$niter,1],xlab="Vmax",
                                      col=cols[1],
                                      plot.points=FALSE,...)
                plot2 <-  densityplot(~x$resuMC[burnin:x$niter,2],xlab="alpha",
                                      col=cols[1],
                                      plot.points=FALSE,...)
            }else{
                plot1 <-  densityplot(~x$resuMC[burnin:x$niter,1],xlab="Vmax",
                                      col=cols[1],
                                      plot.points=FALSE,
                                      panel = function(xi,...){
                                          lattice::panel.densityplot(xi,...)
                                          lattice::panel.mathdensity(dmath=dnorm, args=list(mean = x$prior[1], sd = x$prior[2]), col=pcol)
                                      },...)
                plot2 <-  densityplot(~x$resuMC[burnin:x$niter,2],xlab="alpha",
                                      col=cols[1],
                                      plot.points=FALSE,
                                      panel = function(xi,...){
                                          lattice::panel.densityplot(xi,...)
                                          lattice::panel.mathdensity(dmath=dnorm, args=list(mean = x$prior[3], sd = x$prior[4]), col=pcol)
                                      },...)
            }
            print(plot1,position=c(0,0,0.5,1),more=TRUE)
            print(plot2,position=c(0.5,0,1,1))
        }
    } else
    ## This part of the code is to plot objects x and x2
    ## Ploting the trace
    if(missing(x3)){
        n1 <- x$niter
        n2 <- x2$niter
        maxchainLength <- max(n1,n2)
        tmpvec11 <- x$resuMC[burnin:n1,1]
        tmpvec12 <- x2$resuMC[burnin:n2,1]
        tmpvec21 <- x$resuMC[burnin:n1,2]
        tmpvec22 <- x2$resuMC[burnin:n2,2]
        ymin1 <- min(c(tmpvec11,tmpvec12))*0.95
        ymax1 <- max(c(tmpvec11,tmpvec12))*1.05
        ymin2 <- min(c(tmpvec21,tmpvec22))*0.95
        ymax2 <- max(c(tmpvec21,tmpvec22))*1.05
        if(plot.kind == "trace"){
            plot1 <-  lattice::xyplot(tmpvec11 ~ burnin:n1 ,
                             xlim=c(I(burnin-0.05*maxchainLength),I(maxchainLength*1.05)),
                             ylim=c(ymin1,ymax1),
                             xlab = "Iterations", type = "l",
                             ylab = expression(paste("Vmax (",mu,mol," ",m^-2," ",s^-1,")")),
                             panel = function(x,y,...){
                                 lattice::panel.xyplot(x,y,col=cols[1],...)
                                 lattice::panel.xyplot(burnin:n2,tmpvec12,col=cols[2],...)
                             },...)
            plot2 <-  lattice::xyplot(tmpvec21 ~ burnin:n2 ,
                             xlim=c(I(burnin-0.05*maxchainLength),I(maxchainLength*1.05)),
                             ylim=c(ymin2,ymax2),
                             xlab = "Iterations", type = "l",
                             ylab = expression(paste("alpha (",mol," ",m^-1,")")),
                             panel = function(x,y,...){
                                 lattice::panel.xyplot(x,y,col=cols[1],...)
                                 lattice::panel.xyplot(burnin:n2,tmpvec22,col=cols[2],...)
                             },...)                       
            print(plot1,position=c(0,0,0.5,1),more=TRUE)
            print(plot2,position=c(0.5,0,1,1))
        } else
        ## ploting the density
        if(plot.kind == "density"){
            plot1 <-  densityplot(~ tmpvec11 + tmpvec12 ,xlab="Vmax",
                                  plot.points=FALSE,col=cols[1:2],...)
            plot2 <-  densityplot(~ tmpvec21 + tmpvec22 ,xlab="alpha",
                                  plot.points=FALSE,col=cols[1:2],...)
            print(plot1,position=c(0,0,0.5,1),more=TRUE)
            print(plot2,position=c(0.5,0,1,1))
        }
    }else
{
    n1 <- x$niter
    n2 <- x2$niter
    n3 <- x3$niter
    maxchainLength <- max(n1,n2,n3)
    tmpvec11 <- x$resuMC[burnin:n1,1]
    tmpvec12 <- x2$resuMC[burnin:n2,1]
    tmpvec13 <- x3$resuMC[burnin:n3,1]
    tmpvec21 <- x$resuMC[burnin:n1,2]
    tmpvec22 <- x2$resuMC[burnin:n2,2]
    tmpvec23 <- x3$resuMC[burnin:n3,2]
    ymin1 <- min(c(tmpvec11,tmpvec12,tmpvec13))*0.95
    ymax1 <- max(c(tmpvec11,tmpvec12,tmpvec13))*1.05
    ymin2 <- min(c(tmpvec21,tmpvec22,tmpvec23))*0.95
    ymax2 <- max(c(tmpvec21,tmpvec22,tmpvec23))*1.05
    if(plot.kind == "trace"){
        plot1 <-  lattice::xyplot(tmpvec11 ~ burnin:n1 ,
                         xlim=c(I(burnin-0.05*maxchainLength),I(maxchainLength*1.05)),
                         ylim=c(ymin1,ymax1),
                         xlab = "Iterations", type = "l",
                         ylab = expression(paste("Vmax (",mu,mol," ",m^-2," ",s^-1,")")),
                         panel = function(x,y,...){
                             lattice::panel.xyplot(x,y,col=cols[1],...)
                             lattice::panel.xyplot(burnin:n2,tmpvec12,col=cols[2],...)
                             lattice::panel.xyplot(burnin:n3,tmpvec13,col=cols[3],...)
                         },...)                       
        
        plot2 <-  lattice::xyplot(tmpvec21 ~ burnin:n1 ,
                         xlim=c(I(burnin-0.05*maxchainLength),I(maxchainLength*1.05)),
                         ylim=c(ymin2,ymax2),
                         xlab = "Iterations", type = "l",
                         ylab = expression(paste("alpha (",mol," ",m^-1,")")),
                         panel = function(x,y,...){
                             lattice::panel.xyplot(x,y,col=cols[1],...)
                             lattice::panel.xyplot(burnin:n2,tmpvec22,col=cols[2],...)
                             lattice::panel.xyplot(burnin:n3,tmpvec23,col=cols[3],...)
                         },...)                       
        
        print(plot1,position=c(0,0,0.5,1),more=TRUE)
        print(plot2,position=c(0.5,0,1,1))
    } else
    if(plot.kind == "density"){
        plot1 <-  densityplot(~ tmpvec11 + tmpvec12 + tmpvec13
                              ,xlab="Vmax", plot.points=FALSE,col=cols,...)
        plot2 <-  densityplot(~ tmpvec21 + tmpvec22 + tmpvec23
                              ,xlab="alpha", plot.points=FALSE,col=cols,...)
        print(plot1,position=c(0,0,0.5,1),more=TRUE)
        print(plot2,position=c(0.5,0,1,1))
    }
}
}
