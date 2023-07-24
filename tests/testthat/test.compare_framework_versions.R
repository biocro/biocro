test_that('identical framework versions produce no messages', {
    expect_silent(BioCro:::compare_framework_versions('library', '1.0.0', '1.0.0'))
})

test_that('a warning occurs for smaller library version', {
    expect_warning(
        BioCro:::compare_framework_versions('library', '1.0.0', '1.1.0'),
        paste(
            'The `library` module library R package uses an older version of',
            'the BioCro C++ framework than the `BioCro` R package (1.0.0',
            'vs. 1.1.0).\nTry updating the `library` R package to the latest',
            'version; if that does not solve the problem, contact the',
            '`library` R package maintainer.'
        ),
        fixed = TRUE
    )
})

test_that('a warning occurs for larger library version', {
    expect_warning(
        BioCro:::compare_framework_versions('library', '1.1.0', '1.0.0'),
        paste(
            'The `library` module library R package uses a newer version of',
            'the BioCro C++ framework than the `BioCro` R package (1.1.0',
            'vs. 1.0.0).\nTry updating the `BioCro` R package to the latest',
            'version; if that does not solve the problem, contact the',
            '`BioCro` R package maintainer.'
        ),
        fixed = TRUE
    )
})
