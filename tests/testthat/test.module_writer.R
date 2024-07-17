test_that("Duplicate input variables raise error",
    {
        expect_error(BioCro:::module.write(inputs = c('x', 'x')))
    }
)

test_that("Duplicate output variables raise error",
    {
        expect_error(BioCro:::module.write(output = c('y', 'y')))
    }
)