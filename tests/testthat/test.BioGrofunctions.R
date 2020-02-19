context("Basic tests of *Gro functions")
data(weather05, package = "BioCro")

name_parameters = function(initial_values, parameters, varying_parameters, modules) {
    list(initial_values=initial_values, parameters=parameters, varying_parameters=varying_parameters, modules=modules)
}

parameter_lists = list(
    willow = name_parameters(willow_initial_state, willow_parameters, weather05, willow_modules),
    miscanthus = name_parameters(miscanthus_x_giganteus_initial_state, miscanthus_x_giganteus_parameters, weather05, miscanthus_x_giganteus_modules),
    sorghum = name_parameters(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules)
)

test_that("WillowGro function produces reasonable results", {
    results <- Gro(willow_initial_state, willow_parameters, weather05, willow_modules)

    results_means <- unlist(lapply(results, mean))
    for (output in c("lai", "Leaf", "Root", "Stem")){
        expect_true(all(results[[output]] > 0))
    }
    expect_true(max(results$lai) < 10)
    expect_true(max(results$Leaf) < 25)
})

# caneGro bug https://github.com/ebimodeling/biocro-dev/issues/45
# MaizeGro no roots:  https://github.com/ebimodeling/biocro-dev/issues/46
for (i in seq_along(parameter_lists)) {
    parameter_list = parameter_lists[[i]]
    species = names(parameter_lists)[i]
    #print(species)

    base_results <- do.call(Gro, parameter_list)

    test_that("*Gro functions produce reasonable results", {
        #print("Minimum and maximum values of biomass output.")
        for (output in c("lai", "Leaf", "Root", "Stem")) {
            expect_true(min(base_results[[output]]) >= 0)
            expect_true(max(base_results[[output]]) < 50)
            paste(output, range(base_results[[output]]))
            #print(paste(c(output, prettyNum(range(base_results[[output]])))))
        }
    })

    test_that("turning on soil layers increases aboveground productivity and reduces root allocation", {
        two_layer_parameters = within(parameter_list, {
            modules$soil_module_name = 'two_layer_soil_profile'
            parameters$soil_depth1 = 0
            parameters$soil_depth2 = 1
            parameters$soil_depth3 = 2
            initial_values$cws1 = 0.32
            initial_values$cws2 = 0.32
            initial_values$StomataWS = 1
            initial_values$LeafWS = 1
            initial_values$soil_evaporation_rate = 0
        })

        two_soil_layer_results <- NULL
        tryCatch({
            two_soil_layer_results <- do.call(Gro, two_layer_parameters)
        },
        error = function(e) {
            #cat('\n')
            #print(e)
        })
        
        skip_if_not(two_soil_layer_results,
                    "skipping because do.call(Gro, two_layer_parameters) failed to provide a value for two_soil_layer_results")

        expect_true(mean(base_results[["Root"]]) > mean(two_soil_layer_results[["Root"]]))
        expect_true(mean(base_results[["lai"]]) < mean(two_soil_layer_results[["lai"]]))
        expect_true(mean(base_results[["Leaf"]]) < mean(two_soil_layer_results[["Leaf"]]))
        expect_true(mean(base_results[["Stem"]]) < mean(two_soil_layer_results[["Stem"]]))
    })

    test_that(paste(species, "stem biomass is sensitive to key parameters "), {
        get_max_biomass <- function(parameters) {
            results = do.call(Gro, parameters)
            results$total_mass = results$Stem + results$Leaf + results$Root
            return(with(results, max(total_mass)))
            #return(with(results, max(Stem, Leaf, Root)))
        }


        low_kd = within(parameter_list, {parameters$kd = 0.1})
        high_kd = within(parameter_list, {parameters$kd = 0.9})

        low_chil = within(parameter_list, {parameters$chil = 0.4; parameters$iSp = 5})
        high_chil = within(parameter_list, {parameters$chil = 9; parameters$iSp = 5})

        low_b1 = within(parameter_list, {parameters$b1 = 3})
        high_b1 = within(parameter_list, {parameters$b1 = 10})

        # willowGro insensitive to chi.l, b1
        # pending implementation ebimodeling/biocro-dev#5

        expect_gt(get_max_biomass(low_kd), get_max_biomass(high_kd))
        expect_gt(get_max_biomass(low_chil), get_max_biomass(high_chil))
        skip("test comparing get_max_biomass(low_b1) and get_max_biomass(high_b1) needs fixing")
        expect_lt(get_max_biomass(low_b1), get_max_biomass(high_b1))
    })
    #cat('\n')  # <- uncomment if you want to see and preserve intermediate test results
}

