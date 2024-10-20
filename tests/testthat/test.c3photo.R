test_that("c3photo is sensitive to changes in vcmax", {
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
        jmax = 180,
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
        Rd = 1.1,
        Rd_c = 18.72,
        Rd_Ea = 46.39e3,
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
        tpu_rate_max = 23,
        Vcmax_c = 26.35,
        Vcmax_Ea = 65.33e3
    )

    # Get net assimilation for vmax = 100 micromol / m^2 / s
    inputs$vmax1 = 100
    a_100 <- evaluate_module("BioCro:c3_assimilation", inputs)$Assim

    # Get net assimilation for vmax = 10 micromol / m^2 / s
    inputs$vmax1 = 10
    a_10 <- evaluate_module("BioCro:c3_assimilation", inputs)$Assim

    # The two values should be different
    expect_false(a_100 == a_10)
})
