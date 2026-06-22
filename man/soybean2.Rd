\name{soybean2}

\docType{data}

\alias{soybean2}

\title{Soybean-BioCro model definition}

\description{
  Alternate version of the \code{\link{soybean}} model, where the main
  difference is that \code{soybean2} uses a new set of soil water modules.
}

\usage{soybean2}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\details{
  As improvements are made to the BioCro modules, their behavior changes, and
  the soybean model parameters must be updated. Following significant module
  updates, reparameterization is performed using a script that is included with
  the BioCro package; its location can be found by typing
  \code{system.file('extdata', 'parameterize_soybean.R', package = 'BioCro')} in
  an R session. The parameterization script generally uses the same method and
  data as used in Matthews \emph{et al.} (2022), with a few differences, such as
  the separation of pod mass into separate seed and shell components.

  The following is a summary of reparameterizations that have occurred since the
  this version of the soybean model was created:

  \itemize{
    \item \emph{2026-06-22}: The model was changed to use the new
          \code{stomata_water_stress_bilinear} module instead of the
          \code{stomata_water_stress_linear}, and the behavior of the
          \code{partitioning_growth_calculator} module when canopy assimilation
          is negative was altered. These changes produce more reasonable
          simulation results when there is severe water stress, especially late
          in the season during grain filling. A reparameterization was required
          after making these changes.

    \item \emph{2026-05-13}: The soil water modules were updated by migrating
          the  soil water modules from DSSAT into BioCro. This process required
          many changes  to make the modules work with an hourly time step. Also,
          the StomataWS function was changed based on a calibration on stomatal
          conductance data from Gray et al. (2016). These changes caused large
          differences in the calculated biomass, therefore  the partitioning
          coefficients were reparameterized.
  }
}

\source{
  \strong{parameters}

  \itemize{
    \item Basal crop coefficient: Values of \code{kcbmax}, \code{kcbmin}, and
          \code{skc} for maize and cotton are reported in Table 3 of DeJonge &
          Thorp (2017) [\doi{10.13031/trans.12321}]. The two crops have the same
          values of \code{kcbmax} and \code{kcbmin} (1.15 and 0) and nearly
          indentical values of \code{skc} (0.5 and 0.6, respectively),
          indicating that these parameters do not change much for different
          crops. Here we use 1.15, 0, and 0.55 for soybean, where the \code{skc}
          value is the mean of the maize and cotton values.
    \item Stomatal water stress parameters: Values of \code{StomataWS_gradient}
          and \code{StomataWS_intercept} were determined by matching predicted
          and values of stomatal conductance with observed ones as reported in
          Gray \emph{et al}. 2016. A script for determining the values is
          included with the BioCro package; its location can be found by typing
          \code{system.file('extdata', 'optimize_StomataWS_linear.R', package = 'BioCro')}
          in an R session.
    \item Regarding soil types in Champaign County: according to
          \href{https://websoilsurvey.nrcs.usda.gov/app/}{the Web Soil Survey},
          soils in this region are predominantly silty clay loam, loam, and silt loam,
          with variation across soil depth. In BioCro, soil types 4--7 correspond to
          loamy soils. Although this classification does not provide the most accurate
          representation of soil hydraulic properties, it is reasonable to constrain
          our parameterization within this range for consistency. In our current setup,
          we assign type 7 (silty clay loam) to layers 3--6 and type 6 (clay loam) to
          layers 1--2. This configuration produces reasonably good agreement with
          observed soil water dynamics at SoyFACE.
  }
}

\seealso{
  \itemize{
    \item \code{\link{run_biocro}}
    \item \code{\link{modules}}
    \item \code{\link{crop_model_definitions}}
    \item \code{\link{soybean_clock}}
  }
}

\keyword{datasets}

\concept{crop_models}
