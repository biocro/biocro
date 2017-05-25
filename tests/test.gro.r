library(BioCro)

get_errors = function(expr) {
    errors = NULL
    tryCatch(expr, error = function(e) {
        errors <<- e
    })
    errors
}

wrong_modules = within(sorghum_modules, canopy_module_name<-'NONEXISTANTMODULE')
result = get_errors({Gro(sorghum_initial_state, sorghum_parameters, weather05, wrong_modules)})
if (conditionMessage(result) != 'NONEXISTANTMODULE was passed as a module, but that module could not be found.\n') {
    warning('Nonexistant module names should produce an error.')
}

missing_modules = within(sorghum_modules, canopy_module_name <- NULL)
result = get_errors(Gro(sorghum_initial_state, sorghum_parameters, weather05, missing_modules))
if (conditionMessage(result) != 'The following modules names are NULL, but they must be defined: canopy_module_name.') {
    warning('Missing module names should produce an error.')
} 

gro_missing_parms = within(sorghum_parameters, alpha1 <- NULL)
result = get_errors(Gro(sorghum_initial_state, gro_missing_parms, weather05, sorghum_modules))
if (is.null(result)) {
    warning('Missing parameters should produce an error.')
}

module_missing_parms = within(sorghum_parameters, leafwidth <- NULL)
result = get_errors(Gro(sorghum_initial_state, module_missing_parms, weather05, sorghum_modules))
if (is.null(result)) {
    warning('Missing parameters should produce an error.')
}

bad_state= within(sorghum_initial_state, chil<- 10)
result = get_errors(Gro(bad_state, module_missing_parms, weather05, sorghum_modules))
if (is.null(result)) {
    warning('A parameter appearing in more than one state list should produce an error.')
}

system.time({
    for (i in 1:5) {
        test = Gro(sorghum_initial_state, sorghum_parameters, weather06, sorghum_modules)
    }
})

