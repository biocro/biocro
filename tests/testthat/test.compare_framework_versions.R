# Helper function for modifying a version number.
modify_vn <- function(vn, major_increment) {
    vn_split <- strsplit(vn, split = '.', fixed = TRUE)[[1]]
    vn_split[1] <- as.numeric(vn_split[1]) + major_increment
    paste(vn_split, collapse = '.')
}

# Get a few version numbers. Note: this approach will not work if the major
# version number is 0, since the `older_version` below will have a negative
# major version number in that case. The utils::compareVersion function
# (used in compare_framework_versions) does not work with negative negative
# numbers. However, this situation is unlikely to occur since the BioCro
# framework version number is already greater than 1.0.0.
current_version <- BioCro:::framework_version()
older_version <- modify_vn(current_version, -1)
newer_version <- modify_vn(current_version, +1)

test_that('identical framework versions produce no messages', {
    expect_silent(BioCro:::compare_framework_versions('library', current_version))
})

test_that('a warning occurs for older library version', {
    expect_warning(
        BioCro:::compare_framework_versions('library', older_version),
        paste(
            'The `library` module library R package uses an older version of',
            'the BioCro C++ framework than the `BioCro` R package'
        ),
        fixed = TRUE
    )
})

test_that('a warning occurs for newer library version', {
    expect_warning(
        BioCro:::compare_framework_versions('library', newer_version),
        paste(
            'The `library` module library R package uses a newer version of',
            'the BioCro C++ framework than the `BioCro` R package'
        ),
        fixed = TRUE
    )
})
