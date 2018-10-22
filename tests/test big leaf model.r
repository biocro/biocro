library(BioCro)
library(lattice)

subweather =  get_growing_season_climate(weather05)[1:720, ]

system.time({
result = Gro(sorghum_initial_state, sorghum_parameters, subweather, sorghum_modules)
})

sorghum_parameters$k_Q10 = 2
new_sorghum_modules = within(sorghum_modules, canopy_module_name <- 'big_leaf_multilayer_canopy')
system.time({
new_result = Gro(sorghum_initial_state, sorghum_parameters, subweather, new_sorghum_modules)
})

x11(); xyplot(canopy_assimilation_rate + Leaf + new_result$canopy_assimilation_rate + new_result$Leaf ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + new_result$canopy_assimilation_rate + subweather$solar / 1000000 ~ TTc, result, type='l', auto=TRUE)

new_sorghum_modules = within(sorghum_modules, canopy_module_name <- 'canac_with_collatz')
system.time({
new_result = Gro(sorghum_initial_state, sorghum_parameters, subweather, new_sorghum_modules)
})
x11(); xyplot(canopy_assimilation_rate + Leaf + new_result$canopy_assimilation_rate + new_result$Leaf + new_result$lai ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(canopy_assimilation_rate + new_result$canopy_assimilation_rate + subweather$solar / 1000000 ~ TTc, result, type='l', auto=TRUE)
