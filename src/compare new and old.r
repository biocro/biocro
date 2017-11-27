library(lattice)
library(BioCro)

sorghum_parameters['Sp_thermal_time_decay'] = 0
miscanthus_x_giganteus_parameters['Sp_thermal_time_decay'] = 0
glycine_max_parameters['Sp_thermal_time_decay'] = 0
willow_parameters['Sp_thermal_time_decay'] = 0

glycine_max_modules = within(glycine_max_modules, growth_module_name <- 'partitioning_growth')
glycine_max_modules = within(glycine_max_modules, senescence_module_name <- 'empty_senescence')

glycine_max_initial_state["GI"] = NULL
glycine_max_initial_state["FKF1"] = NULL
glycine_max_initial_state["CDF"] = NULL
glycine_max_initial_state["FT"] = NULL
glycine_max_initial_state["dawn"] = NULL

system.time({
    sresult = Gro(sorghum_initial_state, sorghum_parameters,(weather05), sorghum_modules)
    mresult = Gro(miscanthus_x_giganteus_initial_state, miscanthus_x_giganteus_parameters,(weather05), miscanthus_x_giganteus_modules)
    gresult = Gro(glycine_max_initial_state, glycine_max_parameters,(weather05), glycine_max_modules)
    wresult = Gro(willow_initial_state, willow_parameters,(weather05), willow_modules)
})

#old_sresult = sresult; old_mresult = mresult; old_gresult = gresult; old_wresult = wresult; save(old_sresult, old_mresult, old_gresult, old_wresult, file='old_result.rdata')
load('old_result.rdata')

for (i in c('s', 'm', 'g', 'w')) {
    result_name = paste(i, 'result', sep='')
    result = get(result_name)
    old_result = get(paste('old_', i, 'result', sep=''))
    
    both_names = intersect(names(result), names(old_result))
    print(identical(result[both_names], old_result[both_names]))

    x11(); print(xyplot(Stem + Leaf + Root ~ TTc, main=result_name, result, type='l'))
    x11(); print(xyplot(Stem + Leaf + Root ~ TTc, main=result_name, old_result, type='l'))
}


    x11(); print(xyplot(Stem + Leaf + Root ~ TTc, main=result_name, gresult, type='l'))
