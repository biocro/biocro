collatz_photo <- function(Qp, leaf_temperature, vmax, alpha, kparm,
                          theta, beta, Rd, upperT, lowerT,
                          k_Q10, intercellular_co2_molar_fraction)
{
    result <- .Call(collatz_photoC,
                 as.double(Qp),
                 as.double(leaf_temperature),
                 as.double(vmax),
                 as.double(alpha),
                 as.double(kparm),
                 as.double(theta),
                 as.double(beta),
                 as.double(Rd),
                 as.double(upperT),
                 as.double(lowerT),
                 as.double(k_Q10),
                 as.double(intercellular_co2_molar_fraction))
    return(result)
}

