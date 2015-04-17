
get_c4photoparms <- function(vmax=39, alpha=0.04, kparm=0.7, theta=0.83, beta=0.93, Rd=0.8, Catm=380, b0=0.01, b1=3, ws=c("gs","vmax"),UPPERTEMP=37.5,LOWERTEMP=3.0){ 
  ws <- match.arg(ws)
  if(ws == "gs") ws <- 1
  else ws <- 0  
  list(vmax=vmax,alpha=alpha,kparm=kparm,theta=theta,beta=beta,Rd=Rd,Catm=Catm,b0=b0,b1=b1,ws=ws,UPPERTEMP=UPPERTEMP,LOWERTEMP=LOWERTEMP)  
}

get_c3photoparms <- function(vmax=100, jmax=180, Rd=1.1, Catm=380, O2 = 210, b0=0.08, b1=5, theta=0.7, StomWS=1.0, ws=c("vmax")){  
  ws <- match.arg(ws)
  if(ws == "gs") ws <- 1
  else ws <- 0  
  list(vmax=vmax,jmax=jmax,Rd=Rd,Catm=Catm,O2=O2,b0=b0,b1=b1,theta=theta,StomWS=StomWS,ws=ws)  
}

get_camphotoparms <- function (cam=100){
  list(cam=cam)
}


New_photoParms <- function (c4photoControl = list(), c3photoControl = list(), camphotoControl = list(), wsflag = c("none","gs","vmax")) {
  
  wsflag <- match.arg(wsflag)
      if(wsflag=="none") {wsflag = 0}
      if(wsflag =="gs") {wsflag =1}
      if(wsflag=="vmax'"){ wsflag =1}
 
  c4parms <- get_c4photoparms()
  c4parms[names(c4photoControl)] <- c4photoControl
  
  c3parms <- get_c3photoparms()
  c3parms[names(c3photoControl)] <- c3photoControl
  
  camparms <- get_camphotoparms()
  camparms[names(camphotoControl)] <- camphotoControl
  
  result <-list(c4parms=c4parms, c3parms=c3parms,camparms=camparms,wsflag=wsflag)
  
}
