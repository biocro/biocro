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
if (conditionMessage(result) != "Caught exception in R_Gro_solver: 'NONEXISTANTMODULE' was given as a module name, but ModuleFactory::get_inputs could not find a module with that name.\n") {
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

bad_state = within(sorghum_initial_state, chil <- 10)
result = get_errors(Gro(bad_state, module_missing_parms, weather05, sorghum_modules))
if (is.null(result)) {
    warning('A parameter appearing in more than one state list should produce an error.')
}

crop_list = list(
    list(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules, 'stored_sorghum_results.tsv'),
    list(willow_initial_state, willow_parameters, weather05, willow_modules, 'stored_willow_results.tsv'),
    list(miscanthus_x_giganteus_initial_state, miscanthus_x_giganteus_parameters, weather05, miscanthus_x_giganteus_modules, 'stored_miscanthus_x_giganteus_results.tsv')
)

library(lattice)
for (crop in crop_list) {
    crop_results = Gro(crop[[1]], crop[[2]], crop[[3]], crop[[4]])
    stored_crop_results = read.delim(crop[[5]])
    x11(); print(xyplot(Leaf + Stem + Root + lai ~ TTc, crop_results, type='l'))
    x11(); print(xyplot(Leaf + Stem + Root + lai ~ TTc, stored_crop_results, type='l'))
    x11(); print(xyplot(canopy_assimilation_rate ~ TTc, crop_results, type='l'))
    x11(); print(xyplot(canopy_assimilation_rate ~ TTc, stored_crop_results, type='l'))
    #write.table(crop_results, file=crop[[5]], sep='\t', row.names=FALSE)

    # this test is not good since sometimes the names / types of output columns change
    #if (!isTRUE(all.equal(stored_crop_results[sort(names(stored_crop_results))], crop_results[sort(names(crop_results))], tolerance = .Machine$double.eps ^ 0.4))) {  # The default tolerance is .Machine$double.eps ^ 0.5. That is too small for differences between operating systems, so make it a little larger.
    #    warning('Results are different from previous runs. Verify them and update "stored_crop_results.tsv"')
    #}

    crop_subresults <- crop_results[c("Leaf", "Stem", "Root", "lai", "canopy_assimilation_rate")]
    stored_crop_subresults <- stored_crop_results[c("Leaf", "Stem", "Root", "lai", "canopy_assimilation_rate")]
    if (!isTRUE(all.equal(stored_crop_subresults[sort(names(stored_crop_subresults))], crop_subresults[sort(names(crop_subresults))], tolerance = .Machine$double.eps ^ 0.4))) {  # The default tolerance is .Machine$double.eps ^ 0.5. That is too small for differences between operating systems, so make it a little larger.
        warning('Results are different from previous runs. Verify them and update ', crop[[5]])
    }
}

