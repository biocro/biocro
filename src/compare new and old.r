library(lattice)
library(BioCro)

sorghum_parameters['Sp_thermal_time_decay'] = 0

system.time({
    result = Gro(sorghum_initial_state, sorghum_parameters,(weather05), sorghum_modules)
})

#old_result = result; save(old_result, file='old_result.rdata')
load('old_result.rdata')

both_names = intersect(names(result), names(old_result))

identical(result[both_names], old_result[both_names])

x11(); xyplot(Stem + Leaf + Root ~ TTc, head(result, 8000))
x11(); xyplot(Stem + Leaf + Root ~ TTc, old_result)

