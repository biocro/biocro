\name{cmi_soybean_weather_data}

\docType{data}

\alias{soybean_weather2002}
\alias{soybean_weather2004}
\alias{soybean_weather2005}
\alias{soybean_weather2006}

\title{Champaign, IL weather data for Soybean-BioCro}

\description{
  Champaign, IL weather data specified at hourly intervals in the CST time zone
  for the years 2002, 2004, 2005, and 2006. The columns correspond to typical
  inputs required for BioCro simulations, with the addition of `day_length`,
  which is specifically required for soybean simulations. Although this quantity
  can be calculated by modules during the course of a simulation, it is included
  in this weather data to speed up the simulations. The time range is restricted
  to the SoyFACE growing season that was used for each year.
}

\format{
  4 data frames corresponding to 4 years of weather data, where each data
  frame has 13 columns and between 2952 - 3384 rows.
}

\source{
  Weather data were obtained from the public SURFRAD and WARM databases and
  processed according to the method described in the Soybean-BioCro paper
  (submitted).
}

\keyword{weather}
