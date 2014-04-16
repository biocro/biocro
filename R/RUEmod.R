
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
