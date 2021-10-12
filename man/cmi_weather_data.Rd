\name{cmi_weather_data}

\docType{data}

\alias{weather1995}
\alias{weather1996}
\alias{weather1997}
\alias{weather1998}
\alias{weather1999}
\alias{weather2000}
\alias{weather2001}
\alias{weather2002}
\alias{weather2003}
\alias{weather2004}
\alias{weather2005}
\alias{weather2006}
\alias{weather2007}
\alias{weather2008}
\alias{weather2009}
\alias{weather2010}
\alias{weather2011}
\alias{weather2012}
\alias{weather2013}
\alias{weather2014}
\alias{weather2015}
\alias{weather2016}
\alias{weather2017}
\alias{weather2018}
\alias{weather2019}
\alias{weather2020}

\title{Champaign, IL weather data}

\description{
  Multiple years of Champaign, IL weather data specified at hourly intervals in
  the CDT time zone. The columns correspond to typical inputs required for
  BioCro simulations. Note: some values are missing near the start of 1995 since
  the data is not available from SURFRAD.
}

\format{
  26 data frames corresponding to 26 years of weather data (1995 - 2020), where
  each data frame has 8760 rows (representing hourly time points) and 8 columns:
  \itemize{
    \item \code{year}: the year
    \item \code{doy}: the day of year
    \item \code{hour}: the hour
    \item \code{precip}: preciptation rate (mm / hr)
    \item \code{rh}: the ambient relative humidity (dimensionless)
    \item \code{solar}: the incoming photosynthetically active photon flux
          density (PPFD) measured on a ground area basis including direct and
          diffuse sunlight light just outside the crop canopy
          (micromol / m^2 / s)
    \item \code{temp}: the ambient air temperature (degrees Celsius)
    \item \code{windspeed}: the wind speed in the ambient air just outside the
          canopy (m / s)
  }
}

\source{
  Weather data were obtained from the public SURFRAD and WARM databases and
  processed according to the method described in the BioCro II paper
  (submitted). See that paper for a full description of the data processing.

  In brief, the columns in the data frames were determined from SURFRAD and WARM
  variables as follows:
  \itemize{
    \item \code{precip}: from the \code{precip} variable in the WARM data set
    \item \code{rh}: from the \code{rh} variable in the SURFRAD data set
    \item \code{solar}: from the \code{par} variable in the SURFRAD data set;
          when these values are not available, the \code{direct_n},
          \code{diffuse}, and \code{zen} variables are used to make an estimate
    \item \code{temp}: from the \code{temp} variable in the SURFRAD data set
    \item \code{windspeed}: from the \code{windspd} variable in the SURFRAD data
          set
  }

  The WARM data set includes daily values. Hourly values for precipitation are
  derived from daily totals by assuming a constant rate of precipitation
  throughout the day.

  The SURFRAD data set includes values at 1 or 3 minute intervals. Hourly values
  are determined by averaging over hourly intervals, where the value at hour
  \code{h} is the average over the preceeding hour. Some values are missing; any
  missing entries are filled via an interpolation procedure based on the
  assumption that values at the same hour of sequential days should be similar.
}

\keyword{weather}
