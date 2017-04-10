library(BioCro)
data(sorghum_initial_state)
data(sorghum_parameters)
data(weather05)

get_errors = function(expr) {
    errors = NULL
    tryCatch(expr, error = function(e) {
        errors <<- e
    })
    errors
}

sorghum_modules = list(canopy_module_name='c4_canopy', soil_module_name='one_layer_soil_profile', growth_module_name='partitioning_growth', senescence_module_name='thermal_time_senescence')

wrong_modules = within(sorghum_modules, canopy_module_name<-'NONEXISTANTMODULE')
result = get_errors({Gro(sorghum_initial_state, sorghum_parameters, weather05, wrong_modules)})
if (conditionMessage(result) != 'NONEXISTANTMODULE was passed as a module, but that module could not be found.\n') {
    warning('Nonexistant module names should produce an error.')
}

missing_modules = within(sorghum_modules, canopy_module_name<-NULL)
result = get_errors(Gro(sorghum_initial_state, sorghum_parameters, weather05, missing_modules))
if (conditionMessage(result) != 'The following modules names are NULL, but they must be defined: canopy_module_name.') {
    warning('Missing module names should produce an error.')
} 

missing_parms = within(sorghum_parameters, alpha1<-NULL)
result = get_errors(Gro(sorghum_initial_state, missing_parms, weather05, sorghum_modules))
if (conditionMessage(result) != 'This function cannot continue unless all state variables are set.') {
    warning('Missing parameters should produce an error.')

test = Gro(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules)

