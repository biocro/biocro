\name{BioCro-internal}
\alias{indfun}
\alias{objFun}
\alias{objFun2}
\alias{fnpsvp}
\alias{ardBuck}
\alias{alr}
\alias{ialr}
\alias{FmLcFun}
\alias{flow}
\alias{TempToDdryA}
\alias{TempToLHV}
\alias{TempToSFS}
\alias{TempToSWVC}
\title{Internal BioCro functions}
\description{
 Internal BioCro functions
}
\usage{
indfun(obs, pred)
objFun(coefficients,phenStage,iCoefs,ThermalP,data,
                   WetDat,day1,dayn,timestep,lat,iRhizome,irtl,
                   canopyP,seneP,photoP,phenoP,soilP,nitroP,verbose,...)
objFun2(coefficients,phenStage,iCoefs,ThermalP,data,
                   WetDat,day1,dayn,timestep,lat,iRhizome,irtl,
                   canopyP,seneP,photoP,phenoP,soilP,nitroP,verbose,...)

fnpsvp(Tkelvin)
ardBuck(Tcelsius)
alr(props)
ialr(lprops)
flow(SC,CNratio,A,Lc,Tm,resp,kno,Ks,verbose=FALSE)
FmLcFun(Lig,Nit)
TempToDdryA(Temp)
TempToLHV(Temp)
TempToSFS(Temp)
TempToSWVC(Temp)
}
\details{
  These are not intended to be called by the user.
}
\keyword{internal}