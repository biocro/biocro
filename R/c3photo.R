c3photo <- function(Qp, Tl, RH, vcmax=100, jmax=180, Rd=1.1, Catm=380, O2=210, b0=0.08, b1=5, theta=0.7, StomWS=1.0,
                    ws=c("gs", "vmax"), electrons_per_carboxylation=4.5, electrons_per_oxygenation=5.25)
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
                 as.double(StomWS),as.integer(ws),
                 as.double(electrons_per_carboxylation), as.double(electrons_per_oxygenation))
    res
}

