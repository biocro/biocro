\name{crop_specifications}

\docType{data}

\alias{glycine_max_differential_modules}
\alias{glycine_max_direct_modules}
\alias{glycine_max_initial_values}
\alias{glycine_max_ode_solver}
\alias{glycine_max_parameters}
\alias{manihot_esculenta_differential_modules}
\alias{manihot_esculenta_direct_modules}
\alias{manihot_esculenta_initial_values}
\alias{manihot_esculenta_ode_solver}
\alias{manihot_esculenta_parameters}
\alias{miscanthus_x_giganteus_differential_modules}
\alias{miscanthus_x_giganteus_direct_modules}
\alias{miscanthus_x_giganteus_initial_values}
\alias{miscanthus_x_giganteus_ode_solver}
\alias{miscanthus_x_giganteus_parameters}
\alias{sorghum_differential_modules}
\alias{sorghum_direct_modules}
\alias{sorghum_initial_values}
\alias{sorghum_ode_solver}
\alias{sorghum_parameters}
\alias{soybean_clock_differential_modules}
\alias{soybean_clock_direct_modules}
\alias{soybean_clock_initial_values}
\alias{soybean_clock_parameters}
\alias{soybean_differential_modules}
\alias{soybean_direct_modules}
\alias{soybean_initial_values}
\alias{soybean_ode_solver}
\alias{soybean_parameters}
\alias{willow_differential_modules}
\alias{willow_direct_modules}
\alias{willow_initial_values}
\alias{willow_ode_solver}
\alias{willow_parameters}
\alias{zea_mays_differential_modules}
\alias{zea_mays_direct_modules}
\alias{zea_mays_initial_values}
\alias{zea_mays_ode_solver}
\alias{zea_mays_parameters}

\title{Crop specifications}

\description{
  Sets of initial values, parameters, direct modules, differential modules, and
  differential equation solvers that can be used (along with the included
  weather data) to run growth simulations for several crops in Champaign, IL:
  \itemize{
    \item \code{glycine_max} (unpublished)
    \item \code{manihot_esculenta} (unpublished)
    \item \code{miscanthus_x_giganteus}
    \item \code{sorghum} (unpublished)
    \item \code{soybean}
    \item \code{willow}
    \item \code{zea_mays} (unpublished)
  }

  The \code{soybean} specifications are intended to be used along with the
  special soybean weather data (\code{\link{cmi_soybean_weather_data}}). The
  other crops should be used with the other weather data
  (\code{\link{cmi_weather_data}}).

  \strong{Important note}: some specifications, such as the values of
  photosynthetic parameters, would remain the same in any location; others, such
  as the latitude or longitude, would need to change when simulating crop growth
  in different locations. Care must be taken to understand each input quantity
  before attempting to run simulations in other places.
}

\format{
  Each crop has associated initial values, parameters, direct modules,
  differential modules, and differential equation solvers. These objects are
  prefixed with the name of the crop; for example, the soybean parameters are
  called \code{soybean_parameters}. In general, the specifications have the
  following format:
  \itemize{
    \item \code{CROP_initial_values}: a list of named elements specifying the
          initial values of differential quantities for crop \code{CROP}
    \item \code{CROP_parameters}: a list of named elements specifying the values
          of the parameters for crop \code{CROP}
    \item \code{CROP_direct_modules}: a list of direct modules required for crop
          \code{CROP}; some elements are named to facilitate swapping out
          different modules that can fulfill the same role
    \item \code{CROP_differential_modules}: a list of differential modules
          required for crop \code{CROP}; some elements are named to facilitate
          swapping out different modules that can fulfill the same role
    \item \code{CROP_ode_solver}: a list of named elements that specify the type
          of solver to use for crop \code{CROP}, as well as any additional
          parameters that determine its behavior
  }

  The soybean model has additional lists of initial values, parameters, direct
  modules, and differential modules that can be used for its circadian clock
  component. These lists are prefixed by \code{soybean_clock}.
}

\source{
  For the published crop models, see the following papers:
  \itemize{
    \item Miguez, F. E., Zhu, X., Humphries, S., Bollero, G. A. & Long, S. P.
          \emph{A semimechanistic model predicting the growth and production of
          the bioenergy crop Miscanthus x giganteus: description,
          parameterization and validation}. GCB Bioenergy 1, 282-296 (2009).
    \item Miguez, F. E., Maughan, M., Bollero, G. A. & Long, S. P.
          \emph{Modeling spatial and dynamic variation in growth, yield, and
          yield stability of the bioenergy crops Miscanthus x giganteus and
          Panicum virgatum across the conterminous United States}.
          GCB Bioenergy 4, 509-520 (2012).
    \item Wang, D. et al. \emph{A physiological and biophysical model of coppice
          willow (Salix spp.) production yields for the contiguous USA in
          current and future climate scenarios}.
          Plant, Cell & Environment 38, 1850-1865 (2015).
    \item Matthews, M. et al. \emph{Soybean-BioCro: A semi-mechanistic model of
          soybean growth}. \emph{in silico} Plants (2021).
  }
}

\keyword{specifications}
