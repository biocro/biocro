## The idea is that I could just simulate leaf water potential from transpiration
data(doy124)

days <- 10
hours <- seq(0,24*days-1)
tmp <- numeric(24*days)
LeafPot <- numeric(24*days)
soilPot <- numeric(24*days)

k <- 1
iSoilPot <- 0 # J/kg
TotRes <- 5e6 # m^-4 s^-1 kg^-1
LeafRes <- 2e6  # m^-4 s^-1 kg^-1
RootRes <- 3e6   # m^-4 s^-1 kg^-1
for(j in 1:days){
  for(i in 1:24){
    lai <- doy124[i,1] 
    doy <- doy124[i,3]
    hr  <- doy124[i,4]
    solar <- doy124[i,5] 
    temp <- doy124[i,6] 
    rh <- doy124[i,7]
    ws <- doy124[i,8]

    m <- hours[k] + 1
    tmp[m] <- CanA(lai,doy,hr,solar,temp,rh,ws)$CanopyTrans
    ## Transpiration is in Mg ha-2 hr-1
    ## Multiply by 1e3 to go from Mg to kg
    ## Multiply by 1e-4 to go from ha to m^2
    f1 <- 1e3 * 1e-4
    ## However, TotRes needs to go from sec to hours
    f2 <- 1/3600
    ## New soil water potential
    iSoilPot <- iSoilPot - tmp[m]*f1
    soilPot[m] <- iSoilPot
    ## Leaf Water Potential
    LeafPot[m] <- soilPot[m]  - (tmp[m]*f1) * (TotRes*f2)
    
    ## Leaf water potential
##    LeafPot[m] <- soilPot 

    k <- k + 1
  }
}

## Soil water potential
plot(hours, soilPot)

plot(hours,tmp*f1 * 1e6 * (1/3600),
     type="l",lwd=2,
     xlab="Hour",
     ylab=expression(paste("Canopy Transpiration (mg  ",
         m^-2," ",s^-1,")")))
## Compared to potato this is lower, but I think I'm not cheating because
## C4 grasses typically transpire less water than C3s.


xyplot(LeafPot + soilPot ~ hours,
     type="l",lwd=2,
     xlab="Hour",
     ylab=expression(paste("Leaf Water Potential (J/kg)")))


## The equations to modify stomatal conductance based on leaf water potential are

gwmod = (psim.leaf - psim.thresh)/1000 * gws
g = g * (1 - gwmod)

## psim.thresh is -1200 by default in wimovac but I had -800
## The maximum level of stomatal conductance closure due to water stress is 0.8
## Stomatal conductance sensitivity factor (from 0 to 5)
## Species dependent stomatal conductance closure factor 10
## Species dependent maximum potential for stomatal closure -1500 (source Campbell?)


## Typical curves for matric potential and water content

showSoilType(6)

swc <- seq(0.2,0.5, 0.05)

swc2mp <- function(x, theta.s, air.entry, b){

  ans <- air.entry * (x/theta.s)^-b

}

st <- SoilType(3)

xyplot(swc2mp(swc, 0.55, st$air.entry, st$b) ~ swc)
