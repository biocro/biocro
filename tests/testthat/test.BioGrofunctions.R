context("Basic tests of biocro simulations")
data(weather05, package = "BioCro")

name_parameters = function(
    initial_values,
    parameters,
    drivers,
    steady_state_module_names,
    derivative_module_names,
    numerical_integrator
)
{
    list(
        initial_values = initial_values,
        parameters = parameters,
        drivers = drivers,
        steady_state_module_names = steady_state_module_names,
        derivative_module_names = derivative_module_names,
        numerical_integrator = numerical_integrator
    )
}

parameter_lists = list(
    willow =     name_parameters(willow_initial_values,                 willow_parameters,                 weather05, willow_steady_state_modules,                 willow_derivative_modules,                 willow_numerical_integrator),
    miscanthus = name_parameters(miscanthus_x_giganteus_initial_values, miscanthus_x_giganteus_parameters, weather05, miscanthus_x_giganteus_steady_state_modules, miscanthus_x_giganteus_derivative_modules, miscanthus_x_giganteus_numerical_integrator),
    sorghum =    name_parameters(sorghum_initial_values,                sorghum_parameters,                weather05, sorghum_steady_state_modules,                sorghum_derivative_modules,                sorghum_numerical_integrator)
)

test_that("Willow simulation produces reasonable results", {
    results <- biocro_simulation(
        willow_initial_values,
        willow_parameters,
        weather05,
        willow_steady_state_modules,
        willow_derivative_modules,
        willow_numerical_integrator
    )

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

    base_results <- do.call(biocro_simulation, parameter_list)

    test_that("*biocro_simulation functions produce reasonable results", {
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
            derivative_module_names$soil_profile = 'two_layer_soil_profile'
            parameters$soil_depth1 = 0
            parameters$soil_depth2 = 1
            parameters$soil_depth3 = 2
            parameters$wsFun = 2
            parameters$hydrDist = 0
            parameters$rfl = 0.2
            parameters$rsdf = 0.44
            initial_values$cws1 = 0.32
            initial_values$cws2 = 0.32
        })

        two_soil_layer_results <- do.call(biocro_simulation, two_layer_parameters)

        expect_true(mean(base_results[["Stem"]]) < mean(two_soil_layer_results[["Stem"]]))

        # these tests must be skipped because they always fail
        skip("tests comparing biomass for different soil profiles needs fixing")
        expect_true(mean(base_results[["Root"]]) > mean(two_soil_layer_results[["Root"]]))
        expect_true(mean(base_results[["lai"]]) < mean(two_soil_layer_results[["lai"]]))
        expect_true(mean(base_results[["Leaf"]]) < mean(two_soil_layer_results[["Leaf"]]))

        # change FALSE to TRUE to compare the biomass values
        if (FALSE) {
            temp1 <- base_results[,c("TTc", "Root", "Stem", "Leaf")]
            temp1$layers <- 1
            temp2 <- two_soil_layer_results[,c("TTc", "Root", "Stem", "Leaf")]
            temp2$layers <- 2
            temp <- rbind(temp1, temp2)
            x11()
            print(xyplot(Root + Leaf + Stem ~ TTc, group=layers, type='l', temp, auto=TRUE, main=paste(species, "two layer")))
        }
    })

    test_that(paste(species, "stem biomass is sensitive to key parameters "), {
        get_max_biomass <- function(parameters) {
            results = do.call(biocro_simulation, parameters)
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

        # willowbiocro_simulation insensitive to chi.l, b1
        # pending implementation ebimodeling/biocro-dev#5

        expect_gt(get_max_biomass(low_kd), get_max_biomass(high_kd))
        expect_gt(get_max_biomass(low_chil), get_max_biomass(high_chil))
        skip("test comparing get_max_biomass(low_b1) and get_max_biomass(high_b1) needs fixing")
        expect_lt(get_max_biomass(low_b1), get_max_biomass(high_b1))
    })
    #cat('\n')  # <- uncomment if you want to see and preserve intermediate test results
}

