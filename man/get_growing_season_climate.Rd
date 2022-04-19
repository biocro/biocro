\name{get_growing_season_climate}

\alias{get_growing_season_climate}

\title{Truncate weather data to one growing season}

\description{
  Attempt to restrict a year of weather data to a reasonable growing season.
}

\usage{
get_growing_season_climate(climate, threshold_temperature = 0)
}

\arguments{
  \item{climate}{
    A data frame representing one year of weather data, typically intended to be
    passed to \code{\link{run_biocro}} as its \code{drivers} argument. This data
    frame must have columns for the day of year (\code{doy}) and the air
    temperature in degrees C (\code{temp}).
  }

  \item{threshold_temperature}{
    The value of air temperature in degrees C to use when locating the beginning
    and end of the growing season.
  }
}

\details{
  To determine the growing season, this function locates its beginning and end
  based on the air temperature data. The start of the growing season is set by
  the last day in the first half of the year where the air temperature is below
  the threshold temperature, or day 90, whichever is later. The end of the
  growing season is set by the first day of the second half of the year where
  the air temperature is below the threshold temperature, or day 330, whichever
  is earlier. Thus, this function assumes that temperature generally increases
  early in the year and generally decreases later in the year, and is only
  applicable for locations where this is the case.
}

\value{
A copy of the \code{climate} data frame truncated to the growing season.
}
