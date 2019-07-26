c4photo <- function(Qp, Tl, RH, vmax=39, alpha=0.04, kparm=0.7, theta=0.83, 
                    beta=0.93, Rd=0.8, uppertemp=37.5, lowertemp=3.0, 
                    Catm=380, b0=0.08, b1=3, 
                    StomWS=1, ws=c("gs", "vmax"))
{
    if ((max(RH) > 1) || (min(RH) < 0))
        stop("RH should be between 0 and 1")
    if (any(Catm < 150))
        warning("Stomatal conductance is not reliable for values of Catm lower than 150\n")
    if (any(Catm < 15))
        stop("Assimilation is not reliable for low (<15) Catm values")
    if (length(Catm) == 1) {
      Catm <- rep(Catm, length(Qp))
    } else if (length(Catm) != length(Qp)) {
      stop("length of Catm should be either 1 or equal to length of Qp")
    }

    ws <- match.arg(ws)
    if (ws == "gs") ws <- 1
    else ws <- 0

    res <- .Call(c4photo_sym,
                 as.double(Qp),
                 as.double(Tl),
                 as.double(RH),
                 as.double(vmax),
                 as.double(alpha),
                 as.double(kparm),
                 as.double(theta),
                 as.double(beta),
                 as.double(Rd),
                 as.double(Catm),
                 as.double(b0),
                 as.double(b1),
                 as.double(StomWS),
                 as.integer(ws),
                 as.double(uppertemp),
                 as.double(lowertemp))
    return(res)
}

