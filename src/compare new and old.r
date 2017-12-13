library(lattice)
library(BioCro)

rm(glycine_max_modules)
glycine_max_modules = within(glycine_max_modules, growth_module_name <- 'partitioning_growth')
glycine_max_modules = within(glycine_max_modules, senescence_module_name <- 'thermal_time_senescence')

glycine_max_modules = within(glycine_max_modules, senescence_module_name <- 'empty_senescence')
glycine_max_modules = within(glycine_max_modules, growth_module_name <- 'utilization_growth_and_senescence')

#glycine_max_initial_state["GI"] = NULL
#glycine_max_initial_state["FKF1"] = NULL
#glycine_max_initial_state["CDF"] = NULL
#glycine_max_initial_state["FT"] = NULL
#glycine_max_initial_state["dawn"] = NULL

myparms = glycine_max_parameters
myparms$rate_constant_grain = 5

system.time({
    for (i in 1) {
    #sresult = Gro(sorghum_initial_state, sorghum_parameters,(weather05), sorghum_modules)
    #mresult = Gro(miscanthus_x_giganteus_initial_state, miscanthus_x_giganteus_parameters,(weather05), miscanthus_x_giganteus_modules)
    gresult = Gro(glycine_max_initial_state, myparms,(weather05), glycine_max_modules)
    #wresult = Gro(willow_initial_state, willow_parameters,(weather05), willow_modules)
    }
})

x11(); print(xyplot(Stem + Leaf + Root + Grain ~ TTc, gresult, type='l', auto=TRUE, ylim=c(-1, 15)))

densityplot(~dGrain, gresult)

#subset(gresult, TTc > 945 & TTc < 955)
badtimes = t(gresult[4617:4620, ])
(badtimes = badtimes[sort(rownames(badtimes)), ])

x11(); xyplot(I(canopy_assimilation_rate * 1e6) ~ TTc, gresult)

load('old_result.rdata')

#for (i in c('s', 'm', 'g', 'w')) {
for (i in c('g', 'w')) {
    result_name = paste(i, 'result', sep='')
    result = get(result_name)
    old_result = get(paste('old_', i, 'result', sep=''))
    
    both_names = intersect(names(result), names(old_result))
    print(are_same <- identical(result[both_names], old_result[both_names]))

    if (!are_same) {
        x11(); print(xyplot(Stem + Leaf + Root ~ TTc, main=result_name, result, type='l', auto=TRUE, ylim=c(-1, 15)))
        x11(); print(xyplot(Stem + Leaf + Root ~ TTc, main=result_name, old_result, type='l', auto=TRUE, ylim=c(-1, 15)))
    }
}

#old_sresult = sresult; old_mresult = mresult; old_gresult = gresult; old_wresult = wresult; save(old_sresult, old_mresult, old_gresult, old_wresult, file='old_result.rdata')

