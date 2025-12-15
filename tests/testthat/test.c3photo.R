test_that("c3photoC is sensitive to changes in vcmax", {
    # redmine issue #1478

    # Set up basic inputs for the "c3_assimilation" module, which is
    # just a wrapper for the `c3photoC` function
    inputs <- list(
        atmospheric_pressure = 101325,
        b0 = 0.08,
        b1 = 5,
        beta_PSII = 0.5,
        Catm = 380,
        electrons_per_carboxylation = 4.5,
        electrons_per_oxygenation = 5.25,
        gbw = 1.2,
        Gs_min = 1e-3,
        Gstar_c = 19.02,
        Gstar_Ea = 37.83e3,
        Jmax_at_25 = 180,
        Jmax_c = 17.57,
        Jmax_Ea = 43.54e3,
        Kc_c = 38.05,
        Kc_Ea = 79.43e3,
        Ko_c = 20.30,
        Ko_Ea = 36.38e3,
        O2 = 210,
        phi_PSII_0 = 0.352,
        phi_PSII_1 = 0.022,
        phi_PSII_2 = -3.4e-4,
        Qabs = 1500,
        RL_at_25 = 1.1,
        RL_c = 18.72,
        RL_Ea = 46.39e3,
        rh = 0.7,
        StomataWS = 1,
        temp = 10,
        theta = 0.7,
        theta_0 = 0.76,
        theta_1 = 0.018,
        theta_2 = -3.7e-4,
        Tleaf = 10,
        Tp_c = 19.77399,
        Tp_Ha = 62.99e3,
        Tp_Hd = 182.14e3,
        Tp_S = 0.588e3,
        Tp_at_25 = 23,
        Vcmax_c = 26.35,
        Vcmax_Ea = 65.33e3
    )

    # Get net assimilation for Vcmax_at_25 = 100 micromol / m^2 / s
    inputs$Vcmax_at_25 = 100
    a_100 <- evaluate_module("BioCro:c3_assimilation", inputs)$Assim

    # Get net assimilation for Vcmax_at_25 = 10 micromol / m^2 / s
    inputs$Vcmax_at_25 = 10
    a_10 <- evaluate_module("BioCro:c3_assimilation", inputs)$Assim

    # The two values should be different
    expect_false(a_100 == a_10)
})

test_that('c3photoC produces self-consistent outputs', {
    # Run c3photoC with a range of Qabs and Catm values
    c3photo_res <- module_response_curve(
        'BioCro:c3_assimilation',
        within(soybean$parameters, {
            StomataWS = 1
            Tleaf = 32
            gbw = 1.2
            rh = 0.7
            temp = 30
        }),
        expand.grid(
            Qabs = seq(0, 900, by = 150),
            Catm = seq(20, 620, by = 100)
        )
    )

    # Now we have values of Ci; check to see if the FvCB module reproduces the
    # same assimilation rates. First, we will need to calculate values of key
    # parameters at leaf temperature.
    c3_parameters_inputs <-
        module_info('BioCro:c3_parameters', verbose = FALSE)$inputs

    c3_parameters_res <- module_response_curve(
        'BioCro:c3_parameters',
        list(),
        c3photo_res[, c3_parameters_inputs]
    )

    # We will also need to calculate J from Jmax, and calculate the solubulity
    # of O2 in water. Here we need to reproduce some code from c3photoC; it
    # would be better to make these functions available to the user via modules
    # instead.
    solo <- function(LeafT) {
        (0.047 - 0.0013087 * LeafT + 2.5603e-05 * LeafT^2 - 2.1441e-07 * LeafT^3) / 0.026934;
    }

    Oi <- c3photo_res$O2 * solo(c3photo_res$Tleaf)

    j_from_jmax <- function(absorbed_ppfd, dark_adapted_phi_PSII, beta_PSII, Jmax, theta) {
        I2 <- absorbed_ppfd * dark_adapted_phi_PSII * beta_PSII
        (Jmax + I2 - sqrt((Jmax + I2)^2 - 4.0 * theta * I2 * Jmax)) / (2.0 * theta)
    }

    J <- sapply(seq_len(nrow(c3photo_res)), function(i) {
        j_from_jmax(
            c3photo_res$Qabs[i],
            c3_parameters_res$phi_PSII[i],
            soybean$parameters$beta_PSII,
            c3_parameters_res$Jmax_norm[i] * soybean$parameters$Jmax_at_25,
            c3_parameters_res$theta[i]
        )
    })

    # Now we can run the FvCB model
    fvcb_res <- module_response_curve(
        'BioCro:FvCB',
        list(
            alpha_TPU = 0 # hard-coded to 0 in c3photoC
        ),
        data.frame(
            Ci = c3photo_res$Ci,
            Gstar = c3_parameters_res$Gstar,
            J = J,
            Kc = c3_parameters_res$Kc,
            Ko = c3_parameters_res$Ko,
            Oi = Oi,
            RL = c3_parameters_res$RL_norm * soybean$parameters$RL_at_25,
            TPU = c3_parameters_res$Tp_norm * soybean$parameters$Tp_at_25,
            Vcmax = c3_parameters_res$Vcmax_norm * soybean$parameters$Vcmax_at_25,
            electrons_per_carboxylation = c3photo_res$electrons_per_carboxylation,
            electrons_per_oxygenation = c3photo_res$electrons_per_oxygenation
        )
    )

    # Assimilation and photorespiration rates calculated using the FvCB module
    # should agree with those from c3photoC
    expect_equal(
        fvcb_res$An,
        c3photo_res$Assim
    )

    expect_equal(
        fvcb_res$Vc,
        c3photo_res$GrossAssim
    )

    expect_equal(
        fvcb_res$Vc - fvcb_res$RL - fvcb_res$An,
        c3photo_res$Rp
    )

    # We can also check the conductance-limited assimilation; again this
    # requires some repeated code, a situation that could be fixed in the
    # future.
    conductance_limited_assim <- with(c3photo_res, {
        Catm / (1.37 / gbw + 1.6 / Gs)
    })

    expect_equal(
        conductance_limited_assim,
        c3photo_res$Assim_conductance
    )

    # We can also check to see if the Ball-Berry model agrees with the output
    # from c3photoC
    bb_res <- module_response_curve(
        'BioCro:ball_berry',
        list(),
        data.frame(
            net_assimilation_rate = c3photo_res$Assim,
            Catm = c3photo_res$Catm,
            rh = c3photo_res$rh,
            b0 = c3photo_res$b0,
            b1 = c3photo_res$b1,
            gbw = c3photo_res$gbw,
            leaf_temperature = c3photo_res$Tleaf,
            temp = c3photo_res$temp
        )
    )

    expect_equal(
        bb_res$leaf_stomatal_conductance,
        c3photo_res$Gs
    )

    expect_equal(
        bb_res$hs,
        c3photo_res$RHs
    )

    expect_equal(
        bb_res$cs,
        c3photo_res$Cs
    )
})
