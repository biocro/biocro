#' Radiation use efficiency based model
#' 
#' Simulates leaf area index, biomass and light interception.  Based on the
#' Monteith (1973) equations, adapted for Miscanthus by Clifton Brown et
#' al.(2001) (see references).
#' 
#' 
#' @param Rad Daily solar radiation (MJ \eqn{ha^{-2}}).
#' @param T.a Daily average temperature (Fahrenheit).
#' @param doy.s first day of the growing season, default 91.
#' @param doy.f last day of the growing season, default 227.
#' @param lai.c linear relationship between growing degree days and leaf area
#' index.
#' @param rue radiation use efficiency, default 2.4.
#' @param T.b base temperature for calculating growing degree days, default 10.
#' @param k light extinction coefficient, default 0.68.
#' @return a \code{\link{list}} structure with components \item{doy}{day of the
#' year.} \item{lai.cum}{cumulative leaf area index.} \item{AG.cum}{cumulative
#' above ground dry biomass (Mg \eqn{ha^{-1}}).} \item{AGDD}{cumulative growing
#' degree days.} \item{Int.e}{Intercepted solar radiation.}
#' @note This empirical model is useful but it has limitations.
#' @seealso \code{\link{RUEmodMY}}
#' @references Monteith (1973) \emph{Principles of Environmental Physics}.
#' Edward Arnold, London. UK.
#' 
#' Clifton-Brown, J.C., Long, S.P. and Jorgensen, U. with contributions from
#' S.A. Humphries, Schwarz, K.-U. and Schwarz, H. (2001) \emph{Miscanthus
#' Productivity}. Ch 4. In: Miscanthus for Energy and Fibre. Edited by: Jones,
#' Michael B. and Walsh, Mary. James & James (Science Publishers), London, UK.
#' @keywords models
#' @examples
#' 
#' 
#' ## See RUEmodMY
#' 
#' 
RUEmod <- function(Rad,T.a,doy.s=91,doy.f=227,lai.c=0.0102,rue=2.4,T.b=10,k=0.68){

T.a <- (T.a - 32)*(5/9)
PAR <- Rad / 2
T.a <- T.a[doy.s:doy.f]
Rad <- Rad[doy.s:doy.f]
deltTemp <- ifelse(T.a-T.b > 0 , T.a-T.b,0)
AGDD <- cumsum(deltTemp)
lai0 <- deltTemp * lai.c
lai.cum <- cumsum(lai0)
e.i <- 1 - exp(-k * lai.cum)
int.Rad <- Rad * e.i
AGBiomass <- rue * int.Rad
AG.cum.g <- cumsum(AGBiomass)  ## This is in grams
AG.cum <- AG.cum.g / 100   ## This is used to convert g m-2 to Mg ha-1.

doy <- doy.s:doy.f

list(doy=doy,lai.cum=lai.cum,AG.cum=AG.cum,AGDD=AGDD,Int.e = e.i)

}
