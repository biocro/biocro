library(BioCro)
library(deSolve)
library(lattice)

myparms = glycine_max_parameters
myparms['Sp_thermal_time_decay'] = 0

#r = Gro_ode(sorghum_initial_state, myparms, weather05, steady_state_modules='c4_canopy', derivative_modules=c('one_layer_soil_profile', 'partitioning_growth', 'thermal_time_senescence'))
state = c(glycine_max_initial_state, myparms, weather05[1, ])
r = Gro_ode(state, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth'))

# Given a data frame with r rows and c columns, one column being time, and a desired time, t, return a vector of c elements, one for each column, of values interpolated between values in the two rows whose times are an interval that contains t.
#
# v = interpolate(df, t)

# Given a data frame, and an interpolation method, return an interpolation function.
# interpolate(df, t) = interpolator(df, method)

interpolater = function(df, time_name, method) {
    n_col = ncol(df)
    times = df[[time_name]]
    func_list = vector('list', n_col - 1)
    not_time_columns = setdiff(names(df), time_name)
    for (col_name in not_time_columns) {
        func_list[[col_name]] = method(times, df[[col_name]])
    }
    interpolate = function(t) {
        result = vector('list', n_col)
        names(result) = names(df)
        result[[time_name]] = t
        for (col_name in not_time_columns) {
            result[[col_name]] = func_list[[col_name]](t)
        }
        return(as.data.frame(result))
    }
    return(interpolate)
}

Gro_desolve = function(parameters, varying_parameters, steady_state_modules, derivative_modules) {
    counter_ = 0
    ode=function(t, state, parms) {
        counter_ <<- counter_ + 1
        vp = varying_parameters(t)
        all_state = c(state, parameters, vp)
        result = Gro_ode(all_state, steady_state_modules, derivative_modules)
        result[setdiff(names(state), names(result))] = 0
        derivatives = result[names(state)]
        state_of_interest = c(result[setdiff(names(result), names(state))], vp)
        return(list(derivatives, state_of_interest))
    }
    
    counter = function() return(counter_)
    return(list(counter=counter, func=ode))
}

weather05$time = seq_len(nrow(weather05))
linear_weather = interpolater(weather05, 'time', approxfun)
step_weather = interpolater(weather05, 'time', function(x, y) approxfun(x, y, method='constant'))
step_weather2 = function(t) weather05[floor(t), ]

times = seq(1, 8, length=100)
all.equal(step_weather(times), step_weather2(times))

x11(); xyplot(linear_weather(times)[['temp']] + step_weather(times)[['temp']] + step_weather2(times)[['temp']] ~ times, type='l', auto=TRUE)

linear_gro = Gro_desolve(myparms, linear_weather, steady_state_modules=c(), derivative_modules=c('thermal_time_accumulator'))
step_gro = Gro_desolve(myparms, step_weather, steady_state_modules=c(), derivative_modules=c('thermal_time_accumulator'))

result = as.data.frame(lsodes(unlist(glycine_max_initial_state),
    times=seq_len(nrow(weather05) - 8000) + 7,
    my_gro$func,
    parms=0))

my_gro$counter()

climate = weather05
climate$TTc = c(0, cumsum((climate$temp - 10) * as.numeric(climate$temp > 10))[-nrow(climate)]) / 24
climate$time = seq_len(nrow(climate))

xyplot(Stem + Leaf + Root + Rhizome + Grain + substrate_pool_leaf ~ time, result, type='l', auto=TRUE)
xyplot(Stem + Leaf + Root + Rhizome + Grain + substrate_pool_leaf ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(TTc ~ time, result, type='l', auto=TRUE, subset=time < 761)
x11(); xyplot(TTc ~ time, climate, type='l', auto=TRUE, subset=time < 761)
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
r = Gro(glycine_max_initial_state, glycine_max_parameters, weather05, glycine_max_modules)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, r, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, nr, type='l', auto=TRUE)

same_names=intersect(names(nr), names(r))
all.equal(nr[same_names], r[same_names])

r = Gro(within(sorghum_initial_state, {cws1=soil_water_content; cws2=soil_water_content}), within(sorghum_parameters, {soil_depth1=0; soil_depth2=0.5; soil_depth3=1}), weather05, within(sorghum_modules, {soil_module_name = "one_layer_soil_profile"}))
xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, r, type='l', auto=TRUE)
xyplot(soil_water_content ~ TTc, r, type='l', auto=TRUE)

