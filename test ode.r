library(BioCro)
library(deSolve)
library(lattice)

myparms = glycine_max_parameters
myparms['Sp_thermal_time_decay'] = 0

#r = Gro_ode(sorghum_initial_state, myparms, weather05, steady_state_modules='c4_canopy', derivative_modules=c('one_layer_soil_profile', 'partitioning_growth', 'thermal_time_senescence'))
state = c(glycine_max_initial_state, myparms, weather05[1, ])
r = Gro_ode(state, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth_and_senescence'))

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

    rm(df, times, n_col, method, col_name)
    return(interpolate)
}

time_reporter = function() {
    counter_ = 0L
    last_update_ = counter_
    iter_size_ = 1e6
    iter_times_ = numeric(iter_size_)
    is.na(iter_times_) = TRUE

    pb = winProgressBar("Progress", label=0)
    gc()  # Force stray progress bar windows to close.

    reset = function() {
        counter_ <<- 0
        last_update_ <<- counter_
        iter_times_ <<- numeric(iter_size_)
        is.na(iter_times_) <<- TRUE
        setWinProgressBar(pb, value=0, label=0)
        invisible()
    }

    update = function(state, t) {
        counter_ <<- counter_ + 1
        iter_times_[counter_] <<- t
        if (counter_ - last_update_ > 51) {
            setWinProgressBar(pb, value=0, label=sprintf("count: %d, time: %0.2f\n", counter_, t))
            last_update_ <<- counter_
        }
        invisible()
    }

    counter = function() { counter_ }

    iter_times = function () { iter_times_[!is.na(iter_times_)] }

    return(list(counter=counter, iter_times=iter_times, update=update, reset=reset))
}

combine_reporters = function(x) {
    stopifnot(is.list(x))
    stopifnot(is.function(x[[1]][[1]]))
    functions = list()

    for (reporter in x) {
        for (func_name in names(reporter)) {
            functions[[func_name]][length(functions[[func_name]]) + 1] = reporter[func_name]
        }
    }

    caller = function(fn) {
        fn = fn  # Force evaluation of fn so that each caller has its own copy.
        function(...) {
            result = list()
            for (func in functions[[fn]]) result[[length(result) + 1]] = do.call(func, list(...))

            if (length(result) == 0)
                invisible()
            else if (length(result) == 1)
                return(result[[1]])
            else
                return(result)
        }
    }

    func_calls = list()
    for (func_name in names(functions)) {
        func_calls[[func_name]] = caller(func_name)
    }

    return (func_calls)
}

state_reporter = function() {
    state_df = list()
    times = numeric()
    state_index = 0L
    highest_index = 0L
    state_ = NULL
    updated = FALSE
    rows = 2^20

    reset = function() {
        state_df <<- list()
        times <<- numeric()
        state_index <<- 0L
        highest_index <<- 0L
        state_ <<- NULL
        updated <<- FALSE
        rows <<- 1e5
        invisible()
    }

    update = function(state, time) {
        updated <<- TRUE
        if (length(state_df) == 0L) {
            state_df <<- vector('list', rows)
            times <<- numeric(rows)
        }
        valid_times = times[seq_len(state_index)]
        if (suppressWarnings(max(valid_times, na.rm=TRUE)) < time) {
            state_index <<- state_index + 1L
        } else {
            state_index <<- as.integer(min(which(valid_times >= time), na.rm=TRUE))
        }
        if (state_index > highest_index) {
            highest_index <<- state_index
            if (state_index > rows) {
                copy = state_df
                rows <<- rows * 2
                state_df <<- vector('list', rows)
                state_df[seq_len(state_index - 1)] <<- copy
            }
        }
        state_df[[state_index]] <<- unlist(state)
        times[state_index] <<- time
    }

    state = function() {
        if (updated) {
            state_df[seq(state_index + 1L, highest_index + 1L)] <<- list(NULL)
            state_ <<- as.data.frame(do.call(rbind, state_df[!is.na(state_df)]))
            updated <<- FALSE
        }
        return (state_)
    }

    return(list(reset=reset, update=update, state=state))
}

Gro_desolve = function(parameters, varying_parameters, steady_state_modules, derivative_modules, reporter) {
    ode=function(t, state, parms) {
        vp = varying_parameters(t)
        all_state = c(state, parameters, vp)
        #result = Gro_ode(all_state, steady_state_modules, derivative_modules, check_parameters=FALSE)
        result = .Call(BioCro:::R_Gro_ode, all_state, steady_state_modules, derivative_modules)
        result[setdiff(names(state), names(result))] = 0
        derivatives = result[names(state)]
        state_of_interest = c(result[setdiff(names(result), names(state))], vp)
        reporter$update(c(state_of_interest[setdiff(names(state_of_interest), names(all_state))], all_state, derivatives), t)
        return(list(derivatives, c(state_of_interest, derivatives)))
    }
    
    counter = function() return(counter_)
    return(list(counter=counter, func=ode, reporter=reporter))
}

weather05$time = seq_len(nrow(weather05))
weather05$year = NULL
linear_weather = interpolater(weather05, 'time', approxfun)
spline_weather = interpolater(weather05, 'time', function(x, y) splinefun(x, y, method='monoH.FC'))
linear_from_spline_weather = interpolater(as.data.frame(spline_weather(seq(1, 8760, length=8760*60))), 'time', approxfun)

times = seq(22, 27, length=1000)

var = 'precip'
#x11(); xyplot(linear_weather(times)[[var]] + spline_weather(times)[[var]] + linear_from_spline_weather(times)[[var]] ~ times, type='l', auto=TRUE)

my_timer = combine_reporters(list(time_reporter(), state_reporter()))
empty_reporter = list(update=function(s, t) {}, reset=function() {})

myparms = glycine_max_parameters
myparms$Sp_thermal_time_decay = 0
myparms$rate_constant_root = 2
myparms$rate_constant_grain = 5
myparms$rate_constant_leaf = 1
myparms$soil_type_indicator = 6
gro_func = Gro_desolve(myparms, linear_from_spline_weather, steady_state_modules=c('parameter_calculator', 'c3_canopy'), derivative_modules=c('utilization_growth_and_senescence', 'thermal_time_accumulator'), my_timer)


(previous_times = rbind(previous_times, system.time({
    gro_func$reporter$reset()
    start_row = 7
    sim_rows = 4650 #nrow(weather05)
    result = as.data.frame(lsodes(unlist(glycine_max_initial_state),
        times=seq(start_row, sim_rows + start_row, length=sim_rows),
        gro_func$func,
        atol=1e-7,
        rtol=1e-7,
        parms=0))
    })
))


ns = my_timer$state()
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

my_timer$counter()
x11(); densityplot(my_timer$iter_times(), plot.points=FALSE)
x11(); xyplot(my_timer$iter_times() ~ seq_along(my_timer$iter_times()), type='l')

climate = weather05
climate$TTc = c(0, cumsum((climate$temp - 10) * as.numeric(climate$temp > 10))[-nrow(climate)]) / 24
climate$time = seq_len(nrow(climate))

x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ time, result, type='l', auto=TRUE)
x11(); xyplot(Grain ~ time, result, type='l', auto=TRUE)
x11(); xyplot(Grain ~ time, ns, type='l', auto=TRUE, subset=time > 4617.51 & time < 4617.515)
x11(); xyplot(substrate_pool_grain ~ time, ns, type='l', auto=TRUE, subset=time > 4617.51)
x11(); xyplot(substrate_pool_grain ~ TTc, ns, type='l', auto=TRUE, subset=time > 4617.51)
x11(); xyplot(Grain ~ time, ns, type='l', auto=TRUE)
x11(); xyplot(Stem + Leaf + Root + Rhizome + Grain ~ TTc, result, type='l', auto=TRUE)
x11(); xyplot(substrate_pool_leaf + substrate_pool_stem + substrate_pool_grain ~ time, result, type='l', auto=TRUE)

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

