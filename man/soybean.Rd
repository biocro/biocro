\name{soybean}

\docType{data}

\alias{soybean}

\title{Soybean-BioCro model definition}

\description{
  Initial values, parameters, direct modules, differential modules, and
  a differential equation solver that can be used to run soybean growth
  simulations in Champaign, Illinois and other locations. Along with the soybean
  circadian clock specifications (\code{\link{soybean_clock}}), these values
  define the soybean growth model of Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}], which is commonly referred to as
  \emph{Soybean-BioCro}.

  To represent soybean growth in Champaign, IL, these values must be paired with
  the Champaign weather data (\code{\link{cmi_soybean_weather_data}}). This
  weather data includes the output from the soybean circadian clock model
  (\code{\link{soybean_clock}}), so the clock components do not need to be
  included when running a soybean growth simulation using this weather data.
  The parameters already include the \code{clay_loam} values from the
  \code{\link{soil_parameters}} dataset, which is the appropriate soil type for
  Champaign.

  Some specifications, such as the values of photosynthetic parameters, would
  remain the same in any location; others, such as the latitude or longitude,
  would need to change when simulating crop growth in different locations. Care
  must be taken to understand each input quantity before attempting to run
  simulations in other places or for other cultivars.
}

\usage{soybean}

\format{
  A list of 5 named elements that are suitable for passing to
  \code{\link{run_biocro}}, as described in the help page for
  \code{\link{crop_model_definitions}}.
}

\details{
  As improvements are made to the BioCro modules, their behavior changes, and
  the soybean model parameters must be updated. Following significant module
  updates, reparameterization is performed using the same method and data as
  used in Matthews \emph{et al.} (2022). The following is a summary of
  reparameterizations that have occurred since the original publication of the
  Soybean-BioCro model:
  \itemize{
    \item \emph{2023-06-18}: Several modules have been updated, and the value of
    the atmospheric transmittance has been changed from 0.85 to 0.6 based on
    Campbell and Norman, An Introduction to Environmental Biophysics,
    2nd Edition, Pg 173. Due to these changes, reparameterization of the
    following was required: \code{alphaLeaf}, \code{alphaRoot},
    \code{alphaStem}, \code{alphaShell}, \code{betaLeaf}, \code{betaRoot},
    \code{betaStem}, \code{betaShell}, \code{rateSeneLeaf}, \code{rateSeneStem},
    \code{alphaSeneLeaf}, \code{betaSeneLeaf}, \code{alphaSeneStem}, and
    \code{betaSeneStem}.

    \item \emph{2023-03-15}: Several modules have been updated. The most
    significant changes are that (1) the
    \code{BioCro:no_leaf_resp_neg_assim_partitioning_growth_calculator} now
    reduces the leaf growth rate in response to water stress and (2) the
    partitioning modules now include a new tissue type (\code{shell}). The new
    component allows us to distinguish between components of the soybean pod,
    where \code{shell} represents the pericarp and \code{grain} represents the
    seed. This distinction has been found to be important for accurately
    predicting seed biomass, which is more important in agricultural settings
    than the entire pod mass, since the pericarp is not included in typical
    yield measurements. Due to these changes, reparameterization of the
    following was required: \code{alphaLeaf}, \code{alphaRoot},
    \code{alphaStem}, \code{alphaShell}, \code{betaLeaf}, \code{betaRoot},
    \code{betaStem}, \code{betaShell}, \code{rateSeneLeaf}, \code{rateSeneStem},
    \code{alphaSeneLeaf}, \code{betaSeneLeaf}, \code{alphaSeneStem}, and
    \code{betaSeneStem}. It was also necessary to add a new direct module to the
    model definition: \code{BioCro:leaf_water_stress_exponential}. This module
    calculates the fractional reduction in leaf growth rate due to water stress.

    \item \emph{2024-09-12}: Several changes have been made: (1) The
    \code{mrc1} and \code{mrc2} were renamed to \code{grc_stem} and
    \code{grc_root}, respectively. These two parameters are used to scale the
    assimilate rate, which is commonly called growth respiration coefficient (grc).
    (2) A new module, \code{BioCro:maintenance_respiration}, has been added to
    account for maintenance respiration during the biomass partitioning. This
    module removes a fraction from each organ by a constant parameter called \code{mrc_*}
    (e.g., \code{mrc_leaf}) and also by a temperature-dependent Q10 scaling factor.
    Among these \code{mrc_*} parameters, \code{mrc_leaf} and \code{mrc_stem} are set
    equal to represent maintenance respiration for the shoot, while \code{mrc_grain}
    is assigned a negligible value to prevent grain biomass reduction at the season end.
    No decreasing trends have been seen in the observed data. (3) Parameter optimizations
    against the 2002-2006 biomass datasets were performed to accommodate these changes.
  }

  Whenever a reparameterization is made, this list should be updated, and any
  vignettes using the soybean model should be checked to see if any axis limits,
  etc., need to change.
}

\source{
  This model is described in detail in Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}]. Here we make a few notes about some of
  its components:
  \itemize{
    \item For this model, the ODE solver type should not be
    \code{boost_rosenbrock} or \code{auto} (which defaults to
    \code{boost_rosenbrock} when a fixed step size Euler ODE solver is not
    required, as in this case) since the integration will fail unless the
    tolerances are stringent (e.g., \code{output_step_size = 0.01},
    \code{adaptive_rel_error_tol = 1e-9}, \code{adaptive_abs_error_tol = 1e-9}).

    \item For the initial total seed mass per land area, we use the following
    equation: \code{Number of seeds per meter * weight per seed / row spacing}.
    The number of seeds per meter is 20 and the row spacing is 0.38 m, as
    reported in Morgan \emph{et al.} (2004) [\doi{10.1104/pp.104.043968}]. The
    weight per seed is based on the average of .12 to .18 grams, as reported by
    \href{https://www.feedipedia.org/node/42}{Feedipedia}. Thus, we have an
    initial biomass of
    \code{(20 seeds / m) * (0.15 g / seed) / (0.38 m) = 7.89 g / m^2},
    equivalent to \code{0.0789 Mg / ha} in the typical BioCro units. Since this
    model does not have a seed component, this value is used to determine the
    initial \code{Leaf}, \code{Stem}, and \code{Root} biomass, assuming 80\%
    leaf, 10\% stem, and 10\% root.

    \item For historical reasons, the seed tissue in this model is called
    \code{Grain}. The entire pod biomass can be calculated by adding the
    \code{Grain} and \code{Shell} biomass.

    \item For historical reasons, this model includes a \code{Rhizome} tissue.
    Soybean does not have a rhizome, so the rhizome in the model does not grow
    or senesce. To achieve this, the \code{kRhizome_emr} and
    \code{rateSeneRhizome} parameters must be set to 0. It is also necessary to
    specify values for several other quantities such as \code{alphaSeneRhizome},
    \code{betaSeneRhizome}, and the initial rhizome mass, although the actual
    values of these quantities will have no effect on the simulation output.

    \item For historical reasons, some of the modules that define Soybean-BioCro
    require input quantities that are not actually used for any calculations;
    these "extraneous" parameters are identified in \code{data/soybean.R}.

    \item The \code{sowing_time} input to the
    \code{soybean_development_rate_calculator} module is set to 0 because
    Soybean-BioCro uses the weather data to set the sowing time. In other words,
    the weather data is truncated so it begins at the beggining of the simulation.

    \item Leaf reflectance and transmittance in the PAR band are estimated from
    [\doi{10.2134/agronmonogr31.c7}],
    [\doi{10.2134/agronj1971.00021962006300010038x}],
    and [\doi{10.2134/agronj1991.00021962008300030026x}]. Reflectance and
    transmittance in the NIR band are from [\doi{10.2134/agronmonogr31.c7}].
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
