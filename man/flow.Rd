\name{flow}
\alias{flow}
\title{Returns values based on if kno is less than, equal to, or greater than three.}
\usage{
flow(SC, CNratio, A, Lc, Tm, resp, kno, Ks, verbose = FALSE)
}
\arguments{
  \item{SC}{Soil Carbon}

  \item{CNratio}{ratio of carbon to nitrogen}

  \item{A}{effects of teperature and moisture}

  \item{Lc}{See \code{\link{FmLcFun}}}

  \item{TM}{effect of soil texture on active SOM turnover}

  \item{resp}{respiration}

  \item{kno}{an integer value which determines}

  \item{Ks}{flow constant}

  \item{verbose}{Only used in the R version for debugging}
}
\description{
Returns Values for SC, fC, Resp, Kf, and MinN to be used in
the Century function
}

