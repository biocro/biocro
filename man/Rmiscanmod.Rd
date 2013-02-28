\name{Rmiscanmod}
\alias{Rmiscanmod}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{ RUE-based model to calculate miscanthus growth and yield. }
\description{
  Simple model to calculate crop growth and yield based on MISCANMOD
  (see references).
}
\usage{
Rmiscanmod(data, RUE = 2.4, LER = 0.01, Tb = 10, k = 0.67, LAIdrd = 0.8, 
    LAIStop = 1.8, RUEdrd = 1.3, RUEStop = 2.5, SMDdrd = -30, 
    SMDStop = -120, FieldC = 45, iWatCont = 45, a = 6682.2, b = -0.33, 
    soildepth = 0.6) 
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{data}{ data.frame or matrix described in details.}
  \item{RUE}{ Radiation use efficiency (g/MJ).}
  \item{LER}{ Leaf expansion rate LAI/GDD.}
  \item{Tb}{ Base Temperature (Celsius).}
  \item{k}{ extinction coefficient of light in the canopy.}
  \item{LAIdrd}{ Leaf Area Index 'down regulation decline'.}
  \item{LAIStop}{ Leaf Area Index 'down regulation decline' threshold .}
  \item{RUEdrd}{ Radiation Use Efficieny 'down regulation decline'.}
  \item{RUEStop}{Radiation Use Efficieny 'down regulation decline' threshold.}
  \item{SMDdrd}{ Soil Moisture Deficit 'down regulation decline'.}
  \item{SMDStop}{ Soil Moisture Deficit 'down regulation decline' threshold.}
  \item{FieldC}{ Soil field capacity.}
  \item{iWatCont}{ Initial water content. }
  \item{a}{ Soil parameter.}
  \item{b}{ Soil parameter.}
  \item{soildepth}{ Soil depth.}
}
\details{
  The data.frame or matrix should contain

 column 1:  year 
 column 2:  month 
 column 3:  day 
 column 4:  JD 
 column 5:  max T (Celsius) 
 column 6:  min T (Celsius) 
 column 7:  PPFD or solar radiation divided by 2 (MJ/m2) 
 column 8:  Potential evaporation 
 column 9:  precip (mm) 

}
\value{
  returns a list
  \item{PotEvp }{ Potential Evaporation.}
  \item{Deficitp }{ Deficitp }
  \item{SMDp}{ Soil Moisture Deficit (potential) }
  \item{AE.PE}{ Actual Evaporation / Potential Evaporation }
  \item{Deficitp2}{ Deficitp2}
  \item{SMDa}{ Soil Moisture Deficit (actual) }
  \item{diffRainPE}{ difference between Rainfall and potential
    evaporation. }
  \item{H2oper}{  H2O percent. }
  \item{SoilMoist}{ Soil Moisture.}
  \item{SoilMatPot}{ Soil Matric Potential. }
  \item{WL.LER}{ Water limited Leaf Expansion Rate. }
  \item{WL.RUE}{  Water limited Radiation Use Efficiency. }
  \item{DDaTb}{ Degree Days above base Temperature. }
  \item{DDcum}{  Degree Days (cumulative). }
  \item{adjSumDD}{ adjusted Sum of Degree Days. }
  \item{LAI}{ Leaf Area Index. }
  \item{pLI}{  proportion of light intercepted. }
  \item{Yield}{ Yield (dry biomass) (g/m2) to convert to Mg/ha
    divide by 100.}
}
\references{
Clifton-Brown, J. C.; Neilson, B.; Lewandowski, I. and Jones, M. B. The
modelled productivity of Miscanthus x giganteus (GREEF et DEU) in
Ireland. Industrial Crops and Products, 2000, 12, 97-109.

Clifton-brown, J. C.; Stampfl, P. F. and  Jones, M. B. Miscanthus biomass
production for energy in Europe and its potential contribution to
decreasing fossil fuel carbon emissions. Global Change Biology, 2004, 10,
509-518.


  }

\examples{

## Need to get an example data set and then run it
\dontrun{
data(WD1979)

res <- Rmiscanmod(WD1979)

## convert to Mg/ha

Yld <- res$Yield / 100

xyplot(Yld ~ 1:365 ,
       xlab="doy",
       ylab="Dry biomass (Mg/ha)")

## although the default value for Field Capacity is 45
## a more reasonable value is closer to 27


}

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ models }

