system_derivatives <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list()
)
{
    # system_derivatives is used to create a function that can be called by a
    # differential equation solver such as LSODES.
    #
    # initial_values: a list of named quantities representing the initial values
    # of quantities that follow differential evolution rules, i.e., the
    # differential quantities. Note: the values of these quantities are not
    # important and won't be used in this function, but their names are
    # crucial.
    #
    # parameters: a list of named quantities that don't change with time. This
    # list should include a `timestep` element that specifies the time spacing
    # used in the drivers (in units of hours).
    #
    # drivers: a dataframe of quantities defined at equally spaced time
    # intervals. The drivers should include a "time" column with units of days;
    # if "time" is not included, then "doy" and "hour" must be included.
    #
    # direct_module_names: a character vector or list specifying the names
    # of direct modules to use in the system.
    #
    # differential_module_names: a character vector or list of differential
    # module names.
    #
    # The return value of system_derivatives is a function with three inputs
    # (`t`, `differential_quantities`, and `parms`) that returns derivatives for
    # each of the differential quantities. This function signature and the
    # requirements for its inputs are set by the LSODES function from the
    # deSolve library. The `t` input should be a single time value and the
    # `differential_quantities` input should be a vector with the same names as
    # `initial_values`, i.e., the names of the differential quantities. `parms`
    # is required by LSODES, but we don't use it for anything.
    #
    # --------------------------------------------------------------------------
    #
    # Example 1: a simple oscillator with derivatives only.
    #
    #     oscillator_initial_values <- list(
    #         position = 0,
    #         velocity = 1
    #     )
    #
    #     oscillator_parameters <- list(
    #         timestep = 1,
    #         mass = 1,
    #         spring_constant = 1
    #     )
    #
    #     oscillator_direct_modules <- c()
    #
    #     oscillator_differential_modules <- c("harmonic_oscillator")
    #
    #     oscillator_system_derivatives <- system_derivatives(
    #         oscillator_initial_values,
    #         oscillator_parameters,
    #         get_growing_season_climate(weather05),
    #         oscillator_direct_modules,
    #         oscillator_differential_modules
    #     )
    #
    #     iv <- as.numeric(oscillator_initial_values)
    #     names(iv) <- names(oscillator_initial_values)
    #     times = seq(0, 5, length=100)
    #
    #     library(deSolve)
    #
    #     result = as.data.frame(lsodes(iv, times, oscillator_system_derivatives))
    #
    #     lattice::xyplot(
    #         position + velocity ~ time,
    #         type='l',
    #         auto=TRUE,
    #         data=result
    #     )
    #
    # --------------------------------------------------------------------------
    #
    # Example 2: solving 100 hours of a soybean simulation. This will run very
    # slow compared to a regular call to run_biocro.
    #
    #     soybean_system <- system_derivatives(
    #         soybean_initial_values,
    #         soybean_parameters,
    #         soybean_weather2002,
    #         soybean_direct_modules,
    #         soybean_differential_modules
    #     )
    #
    #     iv <- as.numeric(soybean_initial_values)
    #     names(iv) <- names(soybean_initial_values)
    #     times = seq(from=0, to=100, by=1)
    #
    #     library(deSolve)
    #
    #     result = as.data.frame(lsodes(iv, times, soybean_system))
    #
    #     lattice::xyplot(Leaf + Stem ~ time, type='l', auto=TRUE, data=result)
    #
    # --------------------------------------------------------------------------
    # Example 3: calculating a single derivative (here we use the same
    # `soybean_system` created in example 2):
    #
    #     derivs <- soybean_system(0, iv, NULL)
    #     print(derivs)

    # Check to make sure the initial values are properly defined
    if (!is.list(initial_values)) {
        stop('"initial_values" must be a list')
    }

    if (length(initial_values) != length(unlist(initial_values))) {
        item_lengths = unlist(lapply(initial_values, length))
        error_message = sprintf("The following initial_values members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the parameters are properly defined
    if (!is.list(parameters)) {
        stop('"parameters" must be a list')
    }

    if (length(parameters) != length(unlist(parameters))) {
        item_lengths = unlist(lapply(parameters, length))
        error_message = sprintf("The following parameters members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the drivers are properly defined
    if (!is.list(drivers)) {
        stop('"drivers" must be a list')
    }

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Check to make sure the module names are vectors or lists of strings
    direct_module_names <- unlist(direct_module_names)
    if (length(direct_module_names) > 0 & !is.character(direct_module_names)) {
        stop('"direct_module_names" must be a vector or list of strings')
    }

    differential_module_names <- unlist(differential_module_names)
    if (length(differential_module_names) > 0 & !is.character(differential_module_names)) {
        stop('"differential_module_names" must be a vector or list of strings')
    }

    # C++ requires that all the variables have type `double`
    initial_values = lapply(initial_values, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)

    # Create a function that returns a derivative
    function(t, differential_quantities, parms)
    {
        # Note: parms is required by LSODES but we aren't using it here. We
        # don't need to do any format checking here because LSODES will have
        # already done it.

        # Call the C++ code that calculates a derivative
        derivs <- .Call(
            R_system_derivatives,
            as.list(differential_quantities),
            t,
            parameters,
            drivers,
            direct_module_names,
            differential_module_names
        )

        # LSODES requires the output from this function to be a list whose first
        # element is a named vector of the derivatives in the same order as in
        # the `differential_quantities` input. Right now `derivs` is a list, so
        # we need to convert it to a properly ordered vector and wrap that
        # vector in a list.
        result <- numeric(length(differential_quantities))
        for (i in seq_along(result)) {
            result[i] <- derivs[[names(differential_quantities)[i]]]
        }
        names(result) <- names(differential_quantities)
        return(list(result))
    }
}
