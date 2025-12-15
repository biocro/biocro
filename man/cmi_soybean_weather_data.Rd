\name{cmi_soybean_weather_data}

\docType{data}

\alias{soybean_weather}
\alias{cmi_soybean_weather_data}

\title{Champaign, IL weather data for Soybean-BioCro}

\description{
  Champaign, IL weather data specified at hourly intervals in the CST time zone
  for the years 2002, 2004, 2005, and 2006. The data includes typical inputs
  required for BioCro simulations, with the addition of \code{day_length}, which
  is specifically required for soybean simulations. Although this quantity can
  be calculated by modules during the course of a simulation, it is included in
  this weather data to speed up the simulations. The time range is restricted to
  the SoyFACE growing season that was used for each year.

  This weather data is included in the BioCro package so users can reproduce
  the calculations of Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}] and for exploratory purposes; it is
  likely that most BioCro studies will require different data sets, and no
  attempt is made here to be exhaustive.
}

\usage{soybean_weather}

\format{
  A list of 4 named elements, where each element is a data frame corresponding
  to one year of weather data and the name of each element is a year, for
  example \code{'2004'}. Each data frame has 2952 - 3384 observations
  (representing hourly time points) of 14 variables:
  \itemize{
    \item \code{year}: the year
    \item \code{doy}: the day of year
    \item \code{hour}: the hour
    \item \code{time_zone_offset}: the time zone offset relative to UTC (hr)
    \item \code{precip}: preciptation rate (mm / hr)
    \item \code{rh}: the ambient relative humidity (dimensionless)
    \item \code{dw_solar}: downwelling global solar radiation (J / m^2 / s)
    \item \code{up_solar}: upwelling global solar radiation (J / m^2 / s)
    \item \code{netsolar}: net global solar radiation (downwelling - upwelling)
          (J / m^2 /s)
    \item \code{solar}: the incoming photosynthetically active photon flux
          density (PPFD) measured on a ground area basis including direct and
          diffuse sunlight light just outside the crop canopy
          (micromol / m^2 / s)
    \item \code{temp}: the ambient air temperature (degrees Celsius)
    \item \code{windspeed}: the wind speed in the ambient air just outside the
          canopy (m / s)
    \item \code{zen}: the solar zenith angle (degrees)
    \item \code{day_length}: the length of the daily photoperiod (hours)
  }
}

\source{
  Weather data were obtained from the public SURFRAD and WARM databases and
  processed according to the method described in Matthews \emph{et al.} (2022)
  [\doi{10.1093/insilicoplants/diab032}]. See that paper for a full description
  of the data processing.

  In brief, the columns in the data frames were determined from SURFRAD and WARM
  variables as follows:
  \itemize{
    \item \code{precip}: from the \code{precip} variable in the WARM data set
    \item \code{rh}: from the \code{rh} variable in the SURFRAD data set
    \item \code{dw_solar}: from the \code{dw_solar} variable in the SURFRAD data
          set
    \item \code{up_solar}: from the \code{uw_solar} variable in the SURFRAD data
          set
    \item \code{netsolar}: from the \code{netsolar} variable in the SURFRAD data
          set
    \item \code{solar}: from the \code{par} variable in the SURFRAD data set;
          when these values are not available, the \code{netsolar} and
          \code{up_solar} variables are used to make an estimate; when these
          values are also not available, the \code{dw_solar} variable is used to
          make an estimate
    \item \code{temp}: from the \code{temp} variable in the SURFRAD data set
    \item \code{windspeed}: from the \code{windspd} variable in the SURFRAD data
          set
    \item \code{zen}: from the \code{zen} variable in the SURFRAD data set
    \item \code{day_length}: calculated from \code{solar} using an
          oscillator-based circadian clock
  }

  The WARM data set includes daily values. Hourly values for precipitation are
  derived from daily totals by assuming a constant rate of precipitation
  throughout the day.

  The SURFRAD data set includes values at 1 or 3 minute intervals. Hourly values
  are determined by averaging over hourly intervals, where the value at hour
  \code{h} is the average over that hour. Some values are missing; any missing
  entries are filled by interpolating between neighboring hours.

  To create this data frame, hourly values for all columns except
  \code{day_length} are extracted from the WARM and SURFRAD data. Then, BioCro
  is used to run the circadian clock model that determines photoperiod length.
  (See this page for additional information about the clock model:
  \code{\link{soybean_clock}}.) The result from this calculation is then
  appended to the weather data frame as a new column.

  The \code{time_zone_offset} is set to a constant value of -6 since this data
  is specified in the CST time zone (i.e., UTC-6). Since the value of this
  quantity does not change, it could in principle be considered a parameter
  rather than a driver; however, it is included with the weather data for
  convenience.

  To reduce size the in the BioCro repository, the raw data values are rounded.
  This was done using the commands in a script that is included with the BioCro
  package. This script can be located by typing
  \code{system.file('extdata', 'get_soybean_weather_data.R', package = 'BioCro')}.
}

\keyword{datasets}

\concept{weather}
