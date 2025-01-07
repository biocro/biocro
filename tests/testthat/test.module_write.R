test_that('Results have not changed', {
    # Get stored results
    stored_fname <- file.path('..', 'test_data', 'testmod.h')
    stored_file <- paste(readLines(stored_fname), collapse='\n')

    # Re-run the same commands used to generate the stored results
    xs <- c('x1','x2','x3')
    xs_units <- c('Mg / ha', 'Mg / ha / hr', 'dimensionless')
    ys <- c('y1','y2')

    new_file <- module_write(
        'testmod',
        'standardBML',
        'direct',
        inputs = xs,
        input_units = xs_units,
        outputs = ys
    )

    expect_equal(
        new_file,
        stored_file
    )
})

test_that('Duplicates are detected', {
    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A', 'A', 'B'),
            outputs = c('C', 'D')
        ),
        'Duplicate inputs detected.',
        fixed = TRUE
    )

    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A', 'B'),
            outputs = c('C', 'C', 'D')
        ),
        'Duplicate outputs detected.',
        fixed = TRUE
    )
})

test_that('Length mismatches are detected', {
    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A', 'B'),
            outputs = c('C', 'D'),
            input_units = 'm / s'
        ),
        'The arguments `inputs` and `input_units` do not have the same length.',
        fixed = TRUE
    )

    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A', 'B'),
            outputs = c('C', 'D'),
            output_units = 'm / s'
        ),
        'The arguments `outputs` and `output_units` do not have the same length.',
        fixed = TRUE
    )
})

test_that('Spaces are detected', {
    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A B', 'B'),
            outputs = c('C', 'D')
        ),
        'The values of `inputs` cannot have any spaces in them.',
        fixed = TRUE
    )

    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A', 'B'),
            outputs = c('C D', 'D')
        ),
        'The values of `outputs` cannot have any spaces in them.',
        fixed = TRUE
    )
})

test_that('Numbers are detected', {
    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('1B', 'B'),
            outputs = c('C', 'D')
        ),
        'The values of `inputs` cannot start with a number.',
        fixed = TRUE
    )

    expect_error(
        module_write(
            'testmod',
            'standardBML',
            'direct',
            inputs = c('A', 'B'),
            outputs = c('1D', 'D')
        ),
        'The values of `outputs` cannot start with a number.',
        fixed = TRUE
    )
})
