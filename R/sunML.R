##
##  BioCro/R/sunML.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2009
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 or 3 of the License
##  (at your option).
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  A copy of the GNU General Public License is available at
##  http://www.r-project.org/Licenses/
##
##

sunML <- function(I.dir,I.diff,LAI=8,nlayers=8,kd=0.1,chi.l=1,cos.theta=0.5){

k0 = cos.theta * sqrt(chi.l^2 + tan(acos(cos.theta))^2);
k1 = chi.l + 1.744*(chi.l + 1.183)^-0.733;
k = abs(k0/k1);
LAI.sun <- numeric(nlayers)
LAI.shade <- numeric(nlayers)
F.sun <- numeric(nlayers)
F.shade <- numeric(nlayers)
LAI.i = LAI/nlayers;
I.solar <- numeric(nlayers)
I.diffuse <- numeric(nlayers)
I.total <- numeric(nlayers)

for( i in 0:nlayers){
CumLAI = LAI.i * i;
    if(i == 0){
      Isolar = I.dir;
      Idiffuse = I.diff;
    }
      else{
        Isolar = I.dir * exp(-k * CumLAI);
        Idiffuse = I.diff * exp(-kd * CumLAI);
      }
    Itotal = Isolar + Idiffuse;

    Ls = (1-exp(-k*CumLAI))/k;
    Ld = CumLAI - Ls;
    Ls2 = (cos.theta * (1-exp(-k*Ls/cos.theta)))/k;
    Ld2 = CumLAI - Ls2;

I.solar[i] <- Isolar
I.diffuse[i] <- Idiffuse
I.total[i] <- Itotal
F.sun[i] <- Ls2/CumLAI
F.shade[i] <- Ld2/CumLAI
LAI.sun[i] <- LAI*F.sun[i]
LAI.shade[i] <- LAI*F.shade[i]

}

list(I.solar=I.solar , I.diffuse=I.diffuse, I.total=I.total,
     LAI.sun=LAI.sun,LAI.shade=LAI.shade,Fsun=F.sun,Fshade=F.shade)
}
