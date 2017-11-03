library(BioCro)
library(deSolve)
library(lattice)

myparms = glycine_max_parameters
myparms['Sp_thermal_time_decay'] = 0

#r = Gro_ode(sorghum_initial_state, myparms, weather05, steady_state_modules='c4_canopy', derivative_modules=c('one_layer_soil_profile', 'partitioning_growth', 'thermal_time_senescence'))
state = c(glycine_max_initial_state, myparms, weather05[1, ])
r = Gro_ode(state, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth'))

Gro_desolve = function(parameters, varying_parameters, steady_state_modules, derivative_modules) {
    function(t, state, parms) {
        vp = varying_parameters[floor(t + 1L), ]
        all_state = c(state, parameters, vp)
        result = Gro_ode(all_state, steady_state_modules, derivative_modules)
        result[setdiff(names(state), names(result))] = 0
        derivatives = result[names(state)]
        state_of_interest = c(result[setdiff(names(result), names(state))], vp)
        return(list(derivatives, state_of_interest))
    }
}

myfunc = Gro_desolve(myparms, weather05, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth'))

result = as.data.frame(lsodes(unlist(glycine_max_initial_state),
    times=seq_len(nrow(weather05) - 5000) + 7,
    myfunc,
    parms=0))

xyplot(Stem + Leaf + Root + Rhizome + Grain + substrate_pool_leaf ~ time, result, type='l', auto=TRUE)
xyplot(substrate_pool_leaf + substrate_pool_stem + substrate_pool_grain ~ time, result, type='l', auto=TRUE)
xyplot(substrate_pool_leaf ~ time, result, type='l', auto=TRUE)



easy_parms = sorghum_parameters
easy_parms['Sp_thermal_time_decay'] = 0
easy_parms['LeafN'] = 0
easy_parms['LeafArea'] = 2
easy_parms['parameter'] = 5
easy_parms['PAR'] = 7
r = Gro_ode(sorghum_initial_state, easy_parms, weather05, steady_state_modules='test_calc_state', derivative_modules=c('test_derivs'))

library(BioCro)
soil_parms = sorghum_parameters
soil_parms['soil_type_identifier'] = 6
r = Gro_ode(sorghum_initial_state, soil_parms, weather05, steady_state_modules=c('soil_type_selector'), derivative_modules=('one_layer_soil_profile_derivatives'))

library(BioCro)
library(lattice)
r = Gro(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, r, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, nr, type='l', auto=TRUE)

same_names=intersect(names(nr), names(r))
all.equal(nr[same_names], r[same_names])

r = Gro(within(sorghum_initial_state, {cws1=soil_water_content; cws2=soil_water_content}), within(sorghum_parameters, {soil_depth1=0; soil_depth2=0.5; soil_depth3=1}), weather05, within(sorghum_modules, {soil_module_name = "one_layer_soil_profile"}))
xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, r, type='l', auto=TRUE)
xyplot(soil_water_content ~ TTc, r, type='l', auto=TRUE)

