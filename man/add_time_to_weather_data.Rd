\name{add_time_to_weather_data}

\alias{add_time_to_weather_data}

\title{Add a time component to input}

\description{
  Ensure, if possible, that input data that varies over time has a "time"
  component. See the documentation for \code{\link{time}} for more information
  about this quantity.

  It is rare for users to call this function directly because it is called
  internally by \code{\link{run_biocro}}.
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
  If \code{drivers} has \code{doy} and \code{hour} columns, then it is assumed
  to represent weather data, and will be modified as follows:
  \itemize{
    \item A new \code{time} column will be computed from \code{doy} and
          \code{hour}.
    \item The original \code{doy} and \code{hour} columns will be removed.
  }

  In this case, it is expected that the \code{BioCro:format_time} direct module
  will be used to re-compute \code{doy} and \code{hour} from \code{time}.

  If \code{drivers} does not have \code{doy} and \code{hour} columns, then
  \code{drivers} will be returned as-is.
}

\note{
  \strong{Preconditions:}

  \itemize{
    \item If \code{drivers} is a list, the values should be vectors of equal
          length.
    \item If \code{drivers} already contains a \code{time} component, then it
          shouldn't contain either a \code{doy} or an \code{hour} component
          unless it contains both of them and the values are mutually
          consistent.
  }

  \strong{Why is the 'BioCro:format_time' module necessary?}

  If values of \code{doy} and \code{hour} are supplied to \code{run_biocro} in
  the drivers, undesired results may happen during interpolation. For example,
  if two sequential rows have \code{(time = 3599, doy = 150, hour = 23)} and
  \code{(time = 3600, doy = 151, hour = 0)}, and the results are to be returned
  at half-hour time intervals, then linear interpolation between these rows
  would produce \code{(time = 3599.5, doy = 150.5, hour = 11.5)}. Typically it
  is expected that \code{doy} takes only integer values, so this may cause
  issues. Using the \code{BioCro:format_time} module to calculate \code{doy} and
  \code{hour} from \code{time} will ensure that the result includes
  \code{(time = 3599.5, doy = 150, hour = 23.5)} instead.
}

\examples{
  # Add a time column to the buit-in 2002 weather data
  new_weather <- add_time_to_weather_data(weather[['2002']])

  # Compare column names
  colnames(weather[['2002']])
  colnames(new_weather)
}
