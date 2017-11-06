library(BioCro)
library(deSolve)
library(lattice)

myparms = glycine_max_parameters
myparms['Sp_thermal_time_decay'] = 0

#r = Gro_ode(sorghum_initial_state, myparms, weather05, steady_state_modules='c4_canopy', derivative_modules=c('one_layer_soil_profile', 'partitioning_growth', 'thermal_time_senescence'))
state = c(glycine_max_initial_state, myparms, weather05[1, ])
r = Gro_ode(state, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth'))


previous_times = c()
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

    result = vector('list', n_col)
    names(result) = names(df)

    interpolate = function(t) {
        result[[time_name]] <<- t
        for (col_name in not_time_columns) {
            result[[col_name]] <<- func_list[[col_name]](t)
        }
        return(result)
    }
    return(interpolate)
}

time_reporter = function() {
    counter_ = 0L
    last_update_ = counter_
    iter_size_ = 1e6
    iter_times_ = numeric(iter_size_)
    is.na(iter_times_) = TRUE

    pb = winProgressBar("Progress", label=0)

    reset = function() {
        counter_ <<- 0
        iter_times_ <<- numeric(iter_size_)
        is.na(iter_times_) <<- TRUE
        close(pb)
        pb <<- winProgressBar("Progress", label=0)
    }

    update = function(state, t) {
        counter_ <<- counter_ + 1
        iter_times_[counter_] <<- t
        if (counter_ - last_update_ > 11) {
            setWinProgressBar(pb, value=0, label=sprintf("count: %d, time: %0.2f\n", counter_, t))
            last_update_ <<- counter_
        }
    }

    counter = function() { counter_ }

    iter_times = function () { iter_times_[!is.na(iter_times_)] }

    return(list(counter=counter, iter_times=iter_times, update=update, reset=reset))
}

Gro_desolve = function(parameters, varying_parameters, steady_state_modules, derivative_modules, reporter) {
    ode=function(t, state, parms) {
        vp = varying_parameters(t)
        all_state = c(state, parameters, vp)
        reporter$update(all_state, t)
        result = Gro_ode(all_state, steady_state_modules, derivative_modules, check_parameters=FALSE)
        result[setdiff(names(state), names(result))] = 0
        derivatives = result[names(state)]
        state_of_interest = c(result[setdiff(names(result), names(state))], vp)
        return(list(derivatives, state_of_interest))
    }
    
    counter = function() return(counter_)
    return(list(counter=counter, func=ode, reporter=reporter))
}

weather05$time = seq_len(nrow(weather05))
linear_weather = interpolater(weather05, 'time', approxfun)
step_weather = interpolater(weather05, 'time', function(x, y) approxfun(x, y, method='constant'))
step_weather2 = function(t) weather05[floor(t), ]
spline_weather = interpolater(weather05, 'time', splinefun)
spline2_weather = interpolater(weather05, 'time', function(x, y) splinefun(x, y, method='monoH.FC'))
linear_from_spline_weather = interpolater(as.data.frame(spline2_weather(seq(1, 8760, length=8760*60))), 'time', approxfun)
pchip_weather = interpolater(weather05, 'time', pracma::pchipfun)

times = seq(22, 27, length=1000)

var = 'precip'
x11(); xyplot(linear_weather(times)[[var]] + step_weather(times)[[var]] + step_weather2(times)[[var]] + spline_weather(times)[[var]] + pchip_weather(times)[[var]] + spline2_weather(times)[[var]] + linear_from_spline(times)[[var]] ~ times, type='l', auto=TRUE)
x11(); xyplot( spline_weather(times)[[var]] + pchip_weather(times)[[var]] + spline2_weather(times)[[var]] + linear_from_spline(times)[[var]] ~ times, type='l', auto=TRUE)

my_timer = time_reporter()
empty_reporter = list(update=function(s, t) {}, reset=function() {})

myparms = glycine_max_parameters
myparms['Sp_thermal_time_decay'] = 0
gro_func = Gro_desolve(myparms, linear_from_spline_weather, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth', 'utilization_senescence', 'thermal_time_accumulator'), my_timer)

all_state = c(glycine_max_initial_state, myparms, spline2_weather(8))
system.time({
test_state = state
    for (t in seq(7, 300, length=2604)) {
        linear_from_spline_weather(t)
        Gro_ode(all_state, c('parameter_calculator', 'c3_canopy'), c('utilization_growth', 'utilization_senescence', 'thermal_time_accumulator'), check_parameters=FALSE)
        #for (i in names(test_state)) {
            #if (is.integer(test_state[[i]]))
                #test_state[[i]] = as.numeric(test_state[[i]])
        #}
        #newstate = lapply(state, function(x) if (!is.numeric(x)) as.numeric(x))  # C++ requires that all the variables have type `double`.
        #newstate = lapply(state, as.numeric)  # C++ requires that all the variables have type `double`.
        #result = .Call(BioCro:::R_Gro_ode, newstate, c(), c())
    }
})

(previous_times = rbind(previous_times, system.time({
    gro_func$reporter$reset()
    sim_rows = 300
    result = as.data.frame(lsodes(unlist(glycine_max_initial_state),
        times=seq(7, sim_rows, length=sim_rows),
        gro_func$func,
        atol=1e-7,
        rtol=1e-7,
        maxstep=1000,
        parms=0))
    })
))

my_timer$counter()
x11(); densityplot(my_timer$iter_times(), plot.points=FALSE)
x11(); xyplot(my_timer$iter_times() ~ seq_along(my_timer$iter_times()), type='l')

climate = weather05
climate$TTc = c(0, cumsum((climate$temp - 10) * as.numeric(climate$temp > 10))[-nrow(climate)]) / 24
climate$time = seq_len(nrow(climate))

#x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ time, result, type='l', auto=TRUE)
#x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf + substrate_pool_stem + substrate_pool_grain ~ time, result, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf ~ time, result, type='l', auto=TRUE, ylim=c(-0.00001, 0.00001))
any(result$substrate_pool_leaf < 0)
subset(result, substrate_pool_leaf < 0)

gresult = Gro(glycine_max_initial_state, within(glycine_max_parameters, soil_type_indicator<-6), weather05, within(glycine_max_modules, growth_module_name<-'partitioning_growth'))
gresult$time = seq_len(nrow(gresult))
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ time, gresult, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, gresult, type='l', auto=TRUE)

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

