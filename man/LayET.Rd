\docType{data}
\name{LayET}
\alias{LayET}
\title{Weather data}
\format{Data frame of dimensions 384 by 9.}
\source{
    Simulated.
}
\description{
    Layer data for evapo/transpiration. Simulated data to show the result of
    the EvapoTrans function.
}
\details{
    lfClass: Leaf class, `sun' or `shade'.

    layer: Layer in the canopy, 1 to 8.

    hour: Hour of the day ([0, 23]).

    Rad: Direct light.

    Itot: Total radiation.

    Temp: Air temperature (degrees Celsius).

    RH: Relative humidity ([0, 1]).

    WindSpeed: Wind speed (m s^1).

    LAI: Leaf area index.
}
\keyword{datasets}
