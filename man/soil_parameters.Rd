\name{soil_parameters}

\docType{data}

\alias{soil_parameters}

\title{Soil properties}

\description{
  A collection of soil property data.
}

\usage{soil_parameters}

\format{
  A list of named elements, where each element represents the hydraulic
  properties of one type of soil. The soil types are defined following the
  \href{https://en.wikipedia.org/wiki/Soil_texture}{USDA soil texture classification}
  scheme, and 11 of the 12 possible types are included ("silt" is not
  available). The following names are used to indicate the various soil types:
  \itemize{
    \item \code{sand}
    \item \code{loamy_sand}
    \item \code{sandy_loam}
    \item \code{loam}
    \item \code{silt_loam}
    \item \code{sandy_clay_loam}
    \item \code{clay_loam}
    \item \code{silty_clay_loam}
    \item \code{sandy_clay}
    \item \code{silty_clay}
    \item \code{clay}
  }
  For each soil type, the following parameter values are provided:
  \itemize{
    \item \code{soil_silt_content} (dimensionless)
    \item \code{soil_clay_content} (dimensionless)
    \item \code{soil_sand_content} (dimensionless)
    \item \code{soil_air_entry} (J / kg)
    \item \code{soil_b_coefficient} (dimensionless)
    \item \code{soil_saturated_conductivity} (J * s / m^3)
    \item \code{soil_saturation_capacity} (dimensionless)
    \item \code{soil_field_capacity} (dimensionless)
    \item \code{soil_wilting_point} (dimensionless)
    \item \code{soil_bulk_density} (Mg / m^3)
  }
}

% This description is based on some old comments in the code; who is Melanie and what is getsoilprop.c?
\source{
  These soil property values are based on Table 9.1 from Campbell and Norman's
  textbook \emph{An Introduction to Environmental Biophysics} (1998). Bulk
  density values are taken from function \code{getsoilprop.c} from Melanie
  (Colorado). The bulk density of sand in \code{getsoilprop.c} is 0, which isn't
  sensible, and here a value of \code{1.60 Mg / m^3} is used instead.

  The wilting point value of 0.21 (corrected from 0.32) for silty clay loam is
  based on the list of book corrections available from
  \href{https://web.archive.org/web/20150806180927/http://www.public.iastate.edu/~bkh/teaching/505/norman_book_corrections.pdf}{Brian Hornbuckle's teaching website}
  using the Wayback Machine, since it does not seem to be available on his
  \href{https://faculty.sites.iastate.edu/bkh/teaching}{current site}.
}

\keyword{datasets}
