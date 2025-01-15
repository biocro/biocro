\name{add_time_to_weather_data}

\alias{add_time_to_weather_data}

\title{Add a time component to input}

\description{
  Ensure, if possible, that input data that varies over time has a "time"
  component.
}

\usage{
  add_time_to_weather_data(drivers)
}

\arguments{
  \item{drivers}{
    A list or dataframe representing known system parameters that vary over
    time, such as weather data.
  }
}

\value{
  If drivers has no `time` component, then one is added, provided it \emph{does}
  have components for doy and hour. See \link{time} for its definition. Otherwise
  drivers is returned as is.
}

\note{
  Preconditions: If drivers is a list, the values should be vectors of equal
  length. Moreover, if it already contains a time component, then it shouldn't
  contain either a doy or an hour component unless it contains both of them and
  the values are mutually consistent. Furthermore, the time represented by
  (doy, hour), if given, or by time, if given, should be spaced by the timestep.
}

\examples{
  # Add a time column to the buit-in 2002 weather data
  new_weather <- add_time_to_weather_data(weather[['2002']])
}
