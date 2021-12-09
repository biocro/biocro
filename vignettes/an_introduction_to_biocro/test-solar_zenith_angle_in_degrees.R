context('Test basic functioning of the direct module "solar_zenith_angle_in_degrees".')

# Run the module using a cza as the the value of the cosine of the
# zenith angle and return the value of zenith_angle_in_degrees from
# the result.
angle_from_cosine <- function(cza) {
    input_quantities <- list(cosine_zenith_angle = cza)

    result <- evaluate_module("solar_zenith_angle_in_degrees", input_quantities)

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
