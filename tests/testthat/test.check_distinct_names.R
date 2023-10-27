# Define some helpful objects
list_with_duplicates <- list(a = 1, b = 2, a = 3, a = 4)
vector_with_duplicates <- unlist(list_with_duplicates)
list_without_names <- list(1, 2, 3)
list_with_duplicates_and_unnamed_elements <- list(1, b = 2, 3, b = 4)

short_df_len <- 4
short_data_frame_with_duplicates <- cbind(
    data.frame(
        a = seq(0, 1, length.out = short_df_len),
        b = seq(0, 2, length.out = short_df_len)
    ),
    a = seq(0, 3, length.out = short_df_len)
)

long_df_len <- 100
long_data_frame_with_duplicates <- cbind(
    data.frame(
        a = seq(0, 1, length.out = long_df_len),
        b = seq(0, 2, length.out = long_df_len)
    ),
    a = seq(0, 3, length.out = long_df_len)
)

# Run the tests
test_that("Duplicated list elements are detected", {
    output <- BioCro:::check_distinct_names(list(arg1 = list_with_duplicates))
    expect_equal(
        output,
        c(
            "`arg1` contains multiple instances of some quantities:\n",
            "  `a` takes the following values:\n",
            "    1\n",
            "    3\n",
            "    4\n"
        )
    )
})

test_that("Duplicated vector elements are detected", {
    output <- BioCro:::check_distinct_names(list(arg1 = vector_with_duplicates))
    expect_equal(
        output,
        c(
            "`arg1` contains multiple instances of some quantities:\n",
            "  `a` takes the following values:\n",
            "    1\n",
            "    3\n",
            "    4\n"
        )
    )
})

test_that("Duplicated elements in multiple lists are detected", {
    output <- BioCro:::check_distinct_names(list(arg1 = list_with_duplicates, arg2 = list_with_duplicates))
    expect_equal(
        output,
        c(
            "`arg1` contains multiple instances of some quantities:\n",
            "  `a` takes the following values:\n",
            "    1\n",
            "    3\n",
            "    4\n",
            "`arg2` contains multiple instances of some quantities:\n",
            "  `a` takes the following values:\n",
            "    1\n",
            "    3\n",
            "    4\n"
        )
    )
})

test_that("Unnammed elements are ignored", {
    output <- BioCro:::check_distinct_names(list(arg1 = list_without_names))
    expect_equal(output, character(0))
})

test_that("Duplicated list elements are detected and unnammed elements are ignored", {
    output <- BioCro:::check_distinct_names(list(arg1 = list_with_duplicates_and_unnamed_elements))
    expect_equal(
        output,
        c(
            "`arg1` contains multiple instances of some quantities:\n",
            "  `b` takes the following values:\n",
            "    2\n",
            "    4\n"
        )
    )
})

test_that("Short duplicated data frame columns are printed in full", {
    output <- BioCro:::check_distinct_names(list(arg1 = short_data_frame_with_duplicates))
    expect_equal(
        output,
        c(
            "`arg1` contains multiple instances of some quantities:\n",
            "  `a` takes the following values:\n",
            "    0, 0.333333333333333, 0.666666666666667, 1\n",
            "    0, 1, 2, 3\n"
        )
    )
})

test_that("Long duplicated data frame columns are truncated for printing", {
    output <- BioCro:::check_distinct_names(list(arg1 = long_data_frame_with_duplicates))
    expect_equal(
        output,
        c(
            "`arg1` contains multiple instances of some quantities:\n",
            "  `a` takes the following values:\n",
            "    0, 0.0101010101010101, 0.0202020202020202, 0.0303030303030303, 0.0404040404040404, ...\n",
            "    0, 0.0303030303030303, 0.0606060606060606, 0.0909090909090909, 0.121212121212121, ...\n"
        )
    )
})
