library(BioCro)
library(lattice)

subweather = get_growing_season_climate(weather05)[1:700, ]
subweather = get_growing_season_climate(weather05)

canopy_parms = c(sorghum_initial_state, sorghum_parameters, subweather[612, ])
canopy_parms = within(canopy_parms, {
           lai = 1
           k_Q10 = 2
           StomataWS = 0.1
           doy_dbl = doy + hour / 24.0
})

cresult = CanA(lai=1, doy=canopy_parms$doy, hr=canopy_parms$hour, StomataWS=canopy_parms$StomataWS, solar=canopy_parms$solar, temp=canopy_parms$temp, rh=canopy_parms$rh, windspeed=canopy_parms$windspeed, nlayers=10)
result = test_module('big_leaf_multilayer_canopy', canopy_parms)
cresult[c('canopy_conductance', 'CanopyAssim', 'CanopyTrans')]
result[c('canopy_conductance', 'canopy_assimilation_rate', 'canopy_transpiration_rate')]

system.time({
result = Gro(sorghum_initial_state, sorghum_parameters, subweather, sorghum_modules)
})
#save(result, file='before unit change.rdata')
nresult = result
rm(result)
load('before unit change.rdata')
identical(result, nresult)
all.equal(result, nresult)
x11(); xyplot(canopy_assimilation_rate + Leaf ~ TTc, result, type='l', ylim=c(-0.01, 0.10), auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + Leaf ~ TTc, nresult, type='l', ylim=c(-0.01, 0.10), auto=TRUE)

sorghum_parameters$k_Q10 = 2
new_sorghum_modules = within(sorghum_modules, canopy_module_name <- 'big_leaf_multilayer_canopy')
system.time({
new_result = Gro(sorghum_initial_state, sorghum_parameters, subweather, new_sorghum_modules)
})
#save(new_result, file='before speed up.rdata')
#newer_result = new_result
#rm(new_result)
#load('before speed up.rdata')
#identical(newer_result, new_result)
#all.equal(newer_result, new_result)
range(result$canopy_conductance)
range(new_result$canopy_conductance)


x11(); xyplot(canopy_conductance ~ TTc, result, type='l', ylim=c(-20, 700), auto=TRUE)
x11(); xyplot(canopy_conductance ~ TTc, new_result, type='l', ylim=c(-20, 700), auto=TRUE)

x11(); xyplot(canopy_assimilation_rate + Leaf ~ TTc, result, type='l', ylim=c(-0.02, 0.10), auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + Leaf ~ TTc, new_result, type='l', ylim=c(-0.02, 0.10), auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + Leaf ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + Leaf ~ TTc, new_result, type='l', auto=TRUE)
x11(); xyplot(canopy_assimilation_rate ~ TTc, result, subset = TTc < 500, type='l', ylim=c(-0.005, 0.005), auto=TRUE)
x11(); xyplot(canopy_assimilation_rate ~ TTc, new_result, subset = TTc < 500, type='l', ylim=c(-0.005, 0.005), auto=TRUE)

x11(); xyplot(canopy_assimilation_rate ~ TTc, result, type='l', ylim=c(-0.05, 0.21), auto=TRUE)
x11(); xyplot(canopy_assimilation_rate ~ TTc, new_result, type='l', ylim=c(-0.05, 0.21), auto=TRUE)
x11(); xyplot(Leaf ~ TTc, result, type='l', ylim=c(-0.5, 3), auto=TRUE)
x11(); xyplot(Leaf ~ TTc, new_result, type='l', ylim=c(-0.5, 3), auto=TRUE)

x11(); xyplot(lai ~ TTc, result, subset = TTc < 500, type='l', ylim=c(0, 0.15), auto=TRUE)
x11(); xyplot(lai ~ TTc, new_result, subset = TTc < 500, type='l', ylim=c(0, 0.15), auto=TRUE)
x11(); xyplot(Leaf + Stem + Root + lai ~ TTc, result, type='l', ylim=c(-0.5, 15), auto=TRUE)
x11(); xyplot(Leaf + Stem + Root + lai ~ TTc, new_result, type='l', ylim=c(-0.5, 15), auto=TRUE)

x11(); xyplot(stomatal_conductance_coefs ~ TTc, result, ylim=c(-0.1, 1.2), type='l', auto=TRUE)
x11(); xyplot(stomatal_conductance_coefs ~ TTc, new_result, ylim=c(-0.1, 1.2), type='l', auto=TRUE)

x11(); xyplot(soil_water_content ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(soil_water_content ~ TTc, new_result, type='l', auto=TRUE)
x11(); xyplot(canopy_transpiration_rate ~ TTc, result, ylim=c(-1, 10), type='l', auto=TRUE)
x11(); xyplot(canopy_transpiration_rate ~ TTc, new_result, ylim=c(-1, 10), type='l', auto=TRUE)
x11(); xyplot(canopy_transpiration_rate ~ TTc, newer_result, ylim=c(-1, 10), type='l', auto=TRUE)
#x11(); xyplot(Leaf ~ TTc, new_result, type='l', auto=TRUE)



x11(); xyplot(canopy_assimilation_rate + Leaf + new_result$canopy_assimilation_rate + new_result$Leaf ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + new_result$canopy_assimilation_rate + subweather$solar / 100000 ~ TTc, result, type='l', auto=TRUE)

new_sorghum_modules = within(sorghum_modules, canopy_module_name <- 'canac_with_collatz')
system.time({
new_result = Gro(sorghum_initial_state, sorghum_parameters, subweather, new_sorghum_modules)
})
x11(); xyplot(canopy_assimilation_rate + Leaf + new_result$canopy_assimilation_rate + new_result$Leaf + new_result$lai ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + new_result$canopy_assimilation_rate + subweather$solar / 1000000 ~ TTc, result, type='l', auto=TRUE)
