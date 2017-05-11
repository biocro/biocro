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
    print(species)

    base_results <- do.call(Gro, parameter_list)

    test_that("*Gro functions produce reasonable results", {
    print("Minimum and maximum values of biomass output.")
        for (output in c("lai", "Leaf", "Root", "Stem")) {
            expect_true(min(base_results[[output]]) >= 0)
            expect_true(max(base_results[[output]]) < 50)
            paste(output, range(base_results[[output]]))
            print(paste(c(output, prettyNum(range(base_results[[output]])))))
        }
    })

    test_that("turning on soil layers increases aboveground productivity and reduces root allocation", {
        two_layer_parameters = within(parameter_list, {
                    modules$soil_module_name = 'two_layer_soil_profile'
                    parameters$cws1 = 0.32
                    parameters$cws2 = 0.32
                    parameters$soilDepth1 = 0
                    parameters$soilDepth2 = 1
                    parameters$soilDepth3 = 2
                    }) 

        two_soil_layer_results <- do.call(Gro, two_layer_parameters)
        for (output in c("lai", "Leaf", "Root", "Stem")){
                expect_true(mean(base_results[[output]]) < mean(two_soil_layer_results[[output]]))
            }          
    })

    test_that(paste(species, "stem biomass is sensitive to key parameters "), {
            get_max_biomass <- function(parameters) {
                results = do.call(Gro, parameters)
                return(with(results, max(Stem, Leaf, Root)))
            } 


            low_kd = within(parameter_list, {parameters$kd = 0.1})
            high_kd = within(parameter_list, {parameters$kd = 0.9})
            expect_gt(get_max_biomass(low_kd), get_max_biomass(high_kd))

            low_chil = within(parameter_list, {parameters$chil = 0.4; parameters$iSp = 5})
            high_chil = within(parameter_list, {parameters$chil = 9; parameters$iSp = 5})

            low_b1 = within(parameter_list, {parameters$b1 = 3})
            high_b1 = within(parameter_list, {parameters$b1 = 10})

            # willowGro insensitive to chi.l, b1 
            # pending implementation ebimodeling/biocro-dev#5

            if (species != "willow") {
                expect_gt(get_max_biomass(low_chil), get_max_biomass(high_chil))
                expect_lt(get_max_biomass(low_b1), get_max_biomass(high_b1))
            }
    })
    cat('\n')
}

