library(BioCro)
library(deSolve)
library(lattice)
library(integration.helpers)

previous_times = c()

test_weather = within(weather05, {
    time = seq_len(nrow(weather05))
    year = NULL
    precipitation_rate = weather05$precip / 3600
})
spline_weather = interpolater(test_weather, 'time', function(x, y) splinefun(x, y, method='monoH.FC'))
linear_from_spline_weather = interpolater(as.data.frame(spline_weather(seq(1, 8760, length=8760*60))), 'time', approxfun)

myparms = glycine_max_parameters
myparms$rate_constant_root = 2
myparms$rate_constant_grain = 5
myparms$rate_constant_leaf = 1
gro_func = Gro_deriv(myparms, linear_from_spline_weather, steady_state_modules=c('soil_type_selector', 'leaf_water_stress_exponential', 'stomata_water_stress_linear', 'parameter_calculator', 'c3_canopy', 'soil_evaporation'), derivative_modules=c('utilization_growth', 'utilization_senescence', 'thermal_time_accumulator', 'bucket_soil_drainage'))
gro_reporter = combine_reporters(list(time_reporter(), state_reporter()),
    gro_func
)
#gro_reporter = time_reporter(gro_func)

(previous_times = rbind(previous_times, system.time({
#    gro_reporter$reset()
    start_time = 2953
    end_time = 5000
    duration = end_time - start_time
    result = as.data.frame(lsodes(unlist(glycine_max_initial_state),
        times=seq(start_time, end_time, length=duration),
        gro_reporter$ode,
        atol=1e-4,
        rtol=1e-4,
        parms=0))
    })
))

#old_reporter = gro_reporter; save(old_reporter, file='old_reporter.rdata')
load('old_reporter.rdata')
both_names = intersect(names(old_reporter$state()), names(gro_reporter$state()))
identical(old_reporter$state()[both_names], gro_reporter$state()[both_names])

ns = gro_reporter$state()
gro_reporter$counter()
subns = subset(ns, time > 4617.4 & time < 4617.6)
subns[grepl("(substrate_|transport_|mass_|utilization_|\\<(Leaf|Stem|Root|Rhizome|Grain)\\>|start_grain|d_grain|time)", names(subns))]

cbind(
subns$substrate_pool_stem.1[-nrow(subns)] * diff(subns$time),
diff(subns$substrate_pool_stem),
subns$substrate_pool_stem[-nrow(subns)])


all_state = c(glycine_max_initial_state, myparms, linear_from_spline_weather(8))
system.time({
test_state = state
    for (t in seq(7, 300, length=15e3)) {
        linear_from_spline_weather(t)
        Gro_ode(all_state, c('parameter_calculator', 'soil_type_selector', 'leaf_water_stress_exponential', 'stomata_water_stress_linear', 'c3_canopy'), c('utilization_growth', 'utilization_senescence', 'thermal_time_accumulator'), check_parameters=FALSE)
        #for (i in names(test_state)) {
            #if (is.integer(test_state[[i]]))
                #test_state[[i]] = as.numeric(test_state[[i]])
        #}
        #newstate = lapply(state, function(x) if (!is.numeric(x)) as.numeric(x))  # C++ requires that all the variables have type `double`.
        #newstate = lapply(state, as.numeric)  # C++ requires that all the variables have type `double`.
        #result = .Call(BioCro:::R_Gro_ode, newstate, c(), c())
    }
})

x11(); densityplot(gro_reporter$iter_times(), plot.points=FALSE)
x11(); xyplot(gro_reporter$iter_times() ~ seq_along(gro_reporter$iter_times()), type='l')

climate = weather05
climate$TTc = c(0, cumsum((climate$temp - 10) * as.numeric(climate$temp > 10))[-nrow(climate)]) / 24
climate$time = seq_len(nrow(climate))

x11(); xyplot(Leaf + Stem + Root + Rhizome + Grain ~ time, result, type='l', auto=TRUE, ylim=c(-0.1, 10))
x11(); xyplot(Leaf + Stem + Root + Rhizome + Grain ~ TTc , result, type='l', auto=TRUE, ylim=c(-0.1, 10))
x11(); xyplot(soil_water_content + I(precip / 1000 + .3) ~ time, result, type='l', auto=TRUE, subset = TTc > 10)
x11(); xyplot(soil_water_content ~ time, ns,     type='l', auto=TRUE, subset = TTc > 10)
x11(); xyplot(soil_water_content ~ time, result,     type='l', auto=TRUE, subset = TTc > 10)
x11(); xyplot(soil_water_content + I(precip / 1000 + .30) ~ time, ns, type='l', auto=TRUE, subset = TTc > 10)
x11(); xyplot(precipitation_rate ~ TTc, ns, type='l', auto=TRUE)


x11(); xyplot(Grain ~ time, result, type='l', auto=TRUE)
x11(); xyplot(ns[[27]] + ns[[201]] ~ time, ns, type='l', auto=TRUE)
x11(); xyplot(Grain ~ time, ns, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_grain ~ time, ns, type='l', auto=TRUE, subset=time > 4617.51)
x11(); xyplot(substrate_pool_grain ~ TTc, ns, type='l', auto=TRUE, subset=time > 4617.51)
x11(); xyplot(Grain ~ time, ns, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ time, ns, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf + substrate_pool_stem + substrate_pool_grain ~ time, ns, type='l', auto=TRUE)

plot(substrate_pool_grain ~ time, result, type='l', subset=time>4614)

x11(); xyplot(substrate_pool_leaf + substrate_pool_stem + substrate_pool_grain ~ time, ns, type='l', auto=TRUE, subset=time > 4617 & time < 4618)
x11(); xyplot(mass_fraction_leaf + mass_fraction_stem + mass_fraction_grain ~ time, ns, type='l', auto=TRUE, subset=time > 4617 & time < 4618)
x11(); xyplot(substrate_pool_leaf ~ time, result, type='l', auto=TRUE, ylim=c(-0.00001, 0.00001))
any(result$substrate_pool_leaf < 0)
subset(result, substrate_pool_leaf < 0)
subset(result, substrate_pool_grain < 0)

test_func = Gro_desolve(myparms, linear_from_spline_weather, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth_and_senescence', 'thermal_time_accumulator'), my_timer)
(previous_times = rbind(previous_times, system.time({
    test_func$reporter$reset()
    test_result = as.data.frame(lsodes(unlist(ns[52048,]),
        times=seq(1, 2, length=2),
        test_func$func,
        atol=1e-7,
        rtol=1e-7,
        parms=0))
    })
))

my_timer$state()[grepl("(substrate_|mass_|utilization_|\\<(Leaf|Stem|Root|Rhizome|Grain)\\>|start_grain|d_grain)", names(my_timer$state()))]
xyplot(Grain ~ time, my_timer$state(), type='l')

x11(); xyplot(Grain ~ time, test_result, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ time, test_result, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, test_result, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf +  substrate_pool_grain ~ time, test_result, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf ~ time, test_result, type='l', auto=TRUE, ylim=c(-0.00001, 0.00001))
any(test_result$substrate_pool_leaf < 0)
subset(test_result, substrate_pool_leaf < 0)


glycine_max_parameters$rate_constant_root = 2
glycine_max_parameters$rate_constant_grain = 9
glycine_max_parameters$rate_constant_leaf = 1
glycine_max_parameters$soil_type_indicator = 6
gresult = Gro(glycine_max_initial_state, glycine_max_parameters, weather05, within(glycine_max_modules, {growth_module_name<-'utilization_growth_and_senescence'; senescence_module_name<-'empty_senescence'}))

gresult$time = seq_len(nrow(gresult))
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ time, gresult, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, gresult, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf + substrate_pool_stem + substrate_pool_grain ~ time, gresult, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf ~ time, gresult, type='l', auto=TRUE, ylim=c(-0.00001, 0.00001))

easy_parms = sorghum_parameters
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
r = Gro(glycine_max_initial_state, glycine_max_parameters, weather05, glycine_max_modules)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, r, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, nr, type='l', auto=TRUE)

same_names=intersect(names(nr), names(r))
all.equal(nr[same_names], r[same_names])

r = Gro(within(sorghum_initial_state, {cws1=soil_water_content; cws2=soil_water_content}), within(sorghum_parameters, {soil_depth1=0; soil_depth2=0.5; soil_depth3=1}), weather05, within(sorghum_modules, {soil_module_name = "one_layer_soil_profile"}))
xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, r, type='l', auto=TRUE)
xyplot(soil_water_content ~ TTc, r, type='l', auto=TRUE)

