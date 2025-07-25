# There is a problematic test case that is a challenge for this module, but the
# normal testing setup does not store the input values with sufficiently many
# digits to trigger the convergence error. So here we test this module using a
# dedicated script in addition to its normal test cases.

# Define constants
celsius_to_kelvin <- 273.15             # K or degrees C
ideal_gas_constant <- 8.31446261815324  # J / K / mol

# A copy of the C++ function with the same name
saturation_vapor_pressure <- function(
    air_temperature  # degrees C
)
{
    a <- (18.678 - air_temperature / 234.5) * air_temperature;
    b <- 257.14 + air_temperature;
    611.21 * exp(a / b)  # Pa
}

# A copy of the C++ function with the same name
molar_volume <- function(
  temperature,  # degrees C
  pressure      # Pa
)
{
    TK = temperature + celsius_to_kelvin  # K
    ideal_gas_constant * TK / pressure    # m^3 / mol
}

# Helping function that runs the C++ Nikolov function via its module
solve_equations <- function(
  air_temperature,  # degrees C
  delta_t,          # degrees C
  ea,               # Pa
  gsv,              # m / s
  lw,               # m
  windspeed,        # m / s
  p                 # Pa
)
{
  mv  <- molar_volume(air_temperature, p)            # m^3 / mol
  svp <- saturation_vapor_pressure(air_temperature)  # Pa

  inputs <- list(
    windspeed = 0,                                 # m / s (this ensures gbw_leaf = gbw_free)
    leafwidth = lw,                                # m
    temp = air_temperature,                        # degrees C
    leaf_temperature = air_temperature + delta_t,  # degrees C
    Gs = gsv / mv,                                 # mol / m^2 / s
    rh = ea / svp,                                 # dimensionless
    air_pressure = p                               # mol / m^2 / s
  )

  evaluate_module('BioCro:leaf_gbw_nikolov', inputs)
}

# Investigate a tricky case where the Dekker method gets confused; here it will
# report a "bracket zero width" error, but this should be ignored.
case_dekker <- list(
  air_temperature = 16.39999999999999857891452847979963,
  delta_t = -0.00000051257671529469917004462332,
  ea = 1865.12043054997297986119519919157028,
  gsv = 0.00019007768245344941384494807934,
  lw = 0.10000000000000000555111512312578,
  windspeed = 0.18352098710326952835103497818636,
  p = 101325.00000000000000000000000000000000
)

test_that('Zero width bracket is not a convergence failure in Nikolov model', {
    expect_silent(
        do.call(solve_equations, case_dekker)
    )
})
