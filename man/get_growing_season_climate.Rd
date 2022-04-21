\name{get_growing_season_climate}

\alias{get_growing_season_climate}

\title{Truncate weather data to one growing season}

\description{
  Attempt to restrict a year of weather data to a growing season.
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
  (or equal to) the threshold temperature, or day 90, whichever is later. The
  end of the growing season is set by the first day of the second half of the
  year where the air temperature is below (or equal to) the threshold
  temperature, or day 330, whichever is earlier.

  This is not a sophisticated function and no attempt is made to ensure that the
  output is reasonable. For example, if the air temperature never exceeds the
  threshold value, a growing season beginning on day 183 (the last day of the
  first half of the year) and ending on day 184 (the first day of the second
  half of the year) will be returned. If the air temperature always exceeds the
  threshold value, the growing season will go from day 90 to day 330.

  This function also assumes that the air temperature generally increases early
  in the year and generally decreases later in the year, and is only applicable
  for locations where this is the case. It is therefore unlikely to work
  properly in the Southern Hemisphere or the tropics.

  In general, the appropriate threshold temperature will depend on the species
  that is being modeled. For a perennial grass, the growth season might be said
  to begin after the last freeze, requiring a threshold temperature of 0 degrees
  C. Of course, this is an oversimplification of a complicated biological
  process, and a plant has no way of knowing when it has experienced the last
  freezing day of the year.

  On the other hand, annual crops like soybean are not typically sown until
  conditions are warmer and might require a higher threshold, such as 15 degrees
  C. Again, this is an oversimplification of a complicated process. Farmers
  typically take trends in temperature, historical data, soil conditions, and
  weather predictions into account when deciding to sow, and they may also be
  constrained by external factors like the availability of machinery, seeds, or
  labor.

  Thus, the logic encoded here is an oversimplification in many cases. It is
  likely not appropriate in every situation, and more tailored approaches might
  be required. Nevertheless, it can be a simple way to make a decent estimation
  of a growing season that is sufficient for some purposes.
}

\value{
A copy of the \code{climate} data frame truncated to the growing season.
}
