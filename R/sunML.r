sunML <- function(i_dir, i_diff, lai, nlayers, cos_theta, kd, chi_l, height_f)
{
    if (length(c(i_dir, i_diff, lai, nlayers, cos_theta, kd, chi_l, height_f)) != 8)
        stop("All values should be of length 1")

    result <- .Call(R_sunML,
                    as.double(i_dir),
                    as.double(i_diff),
                    as.double(lai),
                    as.integer(nlayers),
                    as.double(cos_theta),
                    as.double(kd),
                    as.double(chi_l),
                    as.double(height_f))


    return (result)
}
