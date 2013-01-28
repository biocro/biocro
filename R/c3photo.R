##
##  /R/c3photo.R by Fernando Ezequiel Miguez  Copyright (C) 2009-2010
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



c3photo <- function(Qp,Tl,RH,vcmax=100,jmax=180,Rd=1.1,Catm=380,O2=210,b0=0.08,b1=5,theta=0.7)
{

  if((length(Qp) != length(Tl)) || (length(Qp) != length(RH)))
    stop("the lengths of the Qp, Tl and RH vectors should be equal")
  
    if(max(RH) > 1)
        stop("RH should be between 0 and 1")
##     if(Catm < 20)
##         warning("Stomatal conductance is not reliable for values of Catm lower than 20\n")
##     if(Catm < 15)
##         stop("Assimilation is not reliable for low (<15) Catm values")
    
    if(length(Catm) == 1){
      Catm <- rep(Catm,length(Qp))
    }else{
      if(length(Catm) != length(Qp))
        stop("length of Catm should be either 1 or equal to length of Qp")
    }

    res <- .Call(c3photo_sym,as.double(Qp),
                 as.double(Tl),as.double(RH),
                 as.double(vcmax),as.double(jmax),
                 as.double(Rd),as.double(Catm),
                 as.double(b0),as.double(b1),
                 as.double(O2),as.double(theta))
    res
}

