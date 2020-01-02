library(BioCro)
library(lattice)

subweather = get_growing_season_climate(weather05)[121:312, ]

partial_parms = c(sorghum_initial_state, sorghum_parameters)
partial_parms = within(partial_parms, {
           lai = 1
           k_Q10 = 2
           StomataWS = 0.1
})

days = expand.grid(hour=0:23, doy=do.call(seq, as.list(range(subweather$doy))))
for (i in seq_len(nrow(days))) {
    canopy_parms = c(partial_parms, subweather[i, ])
	canopy_parms = within(canopy_parms, {doy_dbl = doy + hour / 24.0})
    cresult = CanA(lai=1, doy=days$doy[i], hr=days$hour[i], StomataWS=canopy_parms$StomataWS, solar=10, temp=canopy_parms$temp, rh=canopy_parms$rh, windspeed=canopy_parms$windspeed, nlayers=10)
    result = test_module('big_leaf_multilayer_canopy', canopy_parms)
    days[i, c('canopy_assimilation_rate')] = result$canopy_assimilation_rate
    days[i, 'CanopyAssim'] = cresult$CanopyAssim
}

x11(); xyplot(canopy_assimilation_rate + CanopyAssim ~ I(doy * 24 + hour), days, type='l', auto=TRUE)


angles = seq(0, pi/2, length=90)
result = numeric()
#result = list()
for (i in seq_along(angles)) {
    r = sunML(i_dir=0, i_diff=100, lai=2, nlayers=10, cos(angles[i]), kd=0.1, chi_l=0.7, height=3)
    #result[i] = sum(r$direct_irradiance)
    #result[i] = sum(r$direct_irradiance * r$sunlit_fraction)
    result[i] = sum(r$diffuse_irradiance * r$shaded_fraction)
    #result[[i]] = r
}

x11(); xyplot(result ~ angles)
