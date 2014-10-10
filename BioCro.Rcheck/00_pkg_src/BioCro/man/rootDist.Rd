\name{rootDist}
\alias{rootDist}
\title{Finds rootDist for the soilML function}
\usage{
rootDist(layers, rootDepth, depthsp, rfl)
}
\arguments{
  \item{layers}{Integer used to specify number of soil
  layers, should be greater than 2}

  \item{rootDepth}{depth of the root in soil}

  \item{depthsp}{represents vaule of the sequence depths in
  soilML}

  \item{rfl}{Root factor lambda. A Poisson distribution is
  used to simulate the distribution of roots in the soil
  profile and this parameter can be used to change the
  lambda parameter of the Poisson.}
}
\description{
This function returns a value for rootDist based on the
arguments layers, rootDepth, depthsp, and rfl. This value
then factors into several equations in the primary function
soilML.
}

