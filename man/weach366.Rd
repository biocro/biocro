\name{weach366}
\alias{weach366}
\title{if the function CheckLeapYear returns 1 this function will be used}
\usage{
weach366(X, lati, ts = 1, temp.units = c("Farenheit", "Celsius"),
  rh.units = c("percent", "fraction"), ws.units = c("mph", "mps"),
  pp.units = c("in", "mm"), ...)
}
\arguments{
  \item{X}{a matrix (or data frame) containing weather
  information.  The input format is strict but it is meant
  to be used with the data usually obtained from weather
  stations in Illinois. The data frame should have 11
  columns (see details).}

  \item{lati}{latitude at the specific location}

  \item{ts}{timestep for the simulation of sub-daily data
  from daily. For example a value of 3 would return data
  every 3 hours. Only divisors of 24 work (i.e. 1,2,3,4,
  etc.).}

  \item{temp.units}{Option to specify the units in which
  the temperature is entered. Default is Farenheit.}

  \item{rh.units}{Option to specify the units in which the
  relative humidity is entered. Default is percent.}

  \item{ws.units}{Option to specify the units in which the
  wind speed is entered. Default is miles per hour.}

  \item{pp.units}{Option to specify the units in which the
  precipitation is entered. Default is inches.}

  \item{list()}{additional arguments to be passed to
  \code{\link{lightME}}}
}
\description{
if the function CheckLeapYear returns 1 this function will
be used
}

