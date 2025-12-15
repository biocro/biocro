\name{catm_data}

\docType{data}

\alias{catm_data}

\title{Global annual mean atmopspheric CO2 levels}

\description{
  Multiple years of globally averaged annual mean atmospheric CO2 levels and
  their uncertainties.

  This data is included in the BioCro package so users can reproduce
  calculations in Lochocki \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diac003}] and for exploratory purposes; it is
  likely that most BioCro studies will require different data sets, and no
  attempt is made here to be exhaustive.
}

\usage{catm_data}

\format{
  Data frame with 3 columns and 45 rows:
  \itemize{
    \item \code{year}: the year
    \item \code{Catm}: CO2 concentration (micromol / mol)
    \item \code{unc}: the uncertainty associated with the CO2 concentration
          (micromol / mol)
  }
}

\source{
  Data were obtained from the National Oceanic and Atmospheric Administration's
  Global Monitoring Laboratory (https://gml.noaa.gov/ccgg/trends/data.html) on
  2025-10-21.

  The exact link used was
  https://gml.noaa.gov/webdata/ccgg/trends/co2/co2_annmean_gl.txt.

  Alternatively, the data can be accessed from
  https://gml.noaa.gov/ccgg/trends/gl_data.html by clicking the link to
  \code{Globally averaged marine surface annual mean data (CSV)}.

  These data are provided here as a convenience to BioCro users; please visit
  the NOAA GML webpage for guidelines regarding the use of this data if you are
  intending to include it in a publication.
}

\keyword{datasets}

\concept{weather}
