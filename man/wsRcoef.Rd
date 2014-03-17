\name{wsRcoef}
\alias{wsRcoef}
\title{R coefficient for water stress}
\usage{
wsRcoef(aw, fieldc, wiltp, phi1, phi2, wsFun = c("linear", "logistic", "exp",
  "none"))
}
\arguments{
  \item{aw}{available water}

  \item{fieldc}{Field capacity of the soil (fraction).}

  \item{wiltp}{Wilting point of the soil (fraction).}

  \item{phi1}{coefficient which controls the spread of the
  logistic function.}

  \item{phi2}{coefficient which controls the effect on leaf
  area expansion.}

  \item{wsFun}{option to control which method is used for
  the water stress function.}
}
\description{
determines whether the argument wsFun is linear, logistic,
exponential, or something else and calculates a value for
wsPhoto based on that.
}

