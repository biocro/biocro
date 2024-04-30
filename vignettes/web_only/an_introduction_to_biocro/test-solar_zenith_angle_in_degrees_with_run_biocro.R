context('Test basic functioning of the direct module "solar_zenith_angle_in_degrees".')

direct_modules <- c("solar_zenith_angle_in_degrees")
differential_modules <- c()

initial_values <- list()
parameters <- list(timestep = 1)


# Run the system using a cza as the the value of the cosine of the
# zenith angle and return the value of zenith_angle_in_degrees from
# the result.
angle_from_cosine <- function(cza) {
    # For these tests, the value of time is immaterial, but either
    # time or both doy and hour must be in the list of keys for
    # drivers.
    drivers <- list(time = 1, cosine_zenith_angle = cza)

    result = run_biocro(initial_values, parameters, drivers, direct_modules, differential_modules)

    result$zenith_angle_in_degrees
}


test_that("When the cosine is 1/2, the angle is 60 degrees.", {
    angle <- angle_from_cosine(0.5)
    expect_equal(angle, 60)
})

test_that("When the cosine is 1, the angle is 0 degrees", {
    angle <- angle_from_cosine(1)
    expect_equal(angle, 0)
})

test_that("When the cosine is -1, the angle is 180 degrees", {
    angle <- angle_from_cosine(-1)
    expect_equal(angle, 180)
})

test_that("When the cosine is more than 1, the angle is not a number", {
    angle <- angle_from_cosine(1.000000001)
    expect_true(is.nan(angle))
})

test_that("When the cosine is less than -1, the angle is not a number", {
    angle <- angle_from_cosine(-1.000000001)
    expect_true(is.nan(angle))
})
