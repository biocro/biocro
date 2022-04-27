\name{get_growing_season_climate}

\alias{get_growing_season_climate}

\title{Truncate weather data to one growing season}

\description{
  Attempt to restrict a year of weather data to a growing season; not intended
  to be a general-use function (see below for a detailed discussion of its
  shortcomings).
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
  \strong{DISCLAIMER}: This function is included here primarily to reproduce
  the output of older BioCro calculations, where it used to be hard-coded into
  every simulation. It has several severe limitations which are discussed below,
  and is not intended to be a general-use function for subsetting weather data.

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

  In general, an appropriate threshold temperature would depend on the species
  that is being modeled. For a perennial grass, the growth season might be said
  to begin after the last freeze, requiring a threshold temperature of 0 degrees
  C. Of course, this is an oversimplification of a complicated biological
  process, and a plant has no way of knowing when it has experienced the last
  freezing day of the year.

  On the other hand, annual crops like maize or soybean are not typically sown
  until conditions are warmer and might require a higher threshold. Again, this
  is an oversimplification of a complicated process. Farmers typically take
  trends in temperature, historical data, soil conditions, and weather
  predictions into account when deciding to sow, and they may also be
  constrained by external factors like the availability of machinery, seeds, or
  labor.

  It should also be noted that as the threshold temperature increases, the
  likelihood of that air temperature occurring at night, even in the middle of
  summer, also increases. Consequently, if the threshold is set too high, an
  unrealistically short growing season may be predicted. For example, calling
  \code{get_growing_season_climate(weather$'2005', 15)} returns a two-day
  growing season (days 183--184) because the temperatures in the late night of
  day 183 and the early morning of day 184 both dip below 15 degrees C.

  Thus, the logic encoded here is an oversimplification in several ways. It is
  likely not appropriate in many situations, and more tailored approaches would
  be required.
}

\value{
A copy of the \code{climate} data frame truncated to the growing season.
}
