Gro_deriv = compiler::cmpfun(function (parameters, varying_parameters, steady_state_modules, derivative_modules) {
    state_names = character(0)
    result_names = character(0)
    result_name_length = 0
    state_diff = character(0)
    result_diff = character(0)

    function(t, state, parms) {
        vp = varying_parameters(t)
        all_state = c(state, parameters, vp)
        result = .Call(BioCro:::R_Gro_ode, all_state, steady_state_modules, derivative_modules)
        temp = names(result)
        if (length(temp) != result_name_length) {
            state_names <<- names(state)
            result_names <<- temp
            result_name_length <<- length(result_names)
            state_diff <<- setdiff(state_names, result_names) 
            result_diff <<- setdiff(result_names, state_names)
        }
        result[state_diff] = 0
        derivatives = result[state_names]
        state_of_interest = c(result[result_diff], vp)
        return(list(derivatives, c(state_of_interest, derivatives)))
    }
})

(previous_times = rbind(previous_times, system.time({
for (i in seq_len(11000)) {
    vp  = linear_from_spline_weather(30)
    all_state = c(glycine_max_initial_state, myparms, vp)
    steady_state_modules=c('soil_type_selector', 'leaf_water_stress_exponential', 'stomata_water_stress_linear', 'parameter_calculator', 'c3_canopy', 'soil_evaporation')
    derivative_modules=c('utilization_growth', 'utilization_senescence', 'thermal_time_accumulator', 'bucket_soil_drainage')
    result = .Call(BioCro:::R_Gro_ode, all_state, steady_state_modules, derivative_modules)
}})
))

system.time({
for (i in seq_len(11000)) {
    result = .Call(BioCro:::R_Gro_ode, all_state, steady_state_modules, derivative_modules)
    state_names = names(glycine_max_initial_state)
    result_names = names(result)
    result[setdiff(state_names, result_names)] = 0
    #result[fast_set_diff(state_names, result_names)] = 0
    #derivatives = result[state_names]
    #state_of_interest = c(result[setdiff(result_names, state_names)], vp)
    #state_of_interest = c(result[fast_set_diff(result_names, state_names)], vp)
}
})
