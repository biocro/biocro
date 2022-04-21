soil_parameters <- local({
    soil_row <- function(...) {setNames(list(...), c(
        "soil_silt_content",           # dimensionless
        "soil_clay_content",           # dimensionless
        "soil_sand_content",           # dimensionless
        "soil_air_entry",              # J / kg
        "soil_b_coefficient",          # dimensionless
        "soil_saturated_conductivity", # J * s / m^3
        "soil_saturation_capacity",    # dimensionless
        "soil_field_capacity",         # dimensionless
        "soil_wilting_point",          # dimensionless
        "soil_bulk_density"            # Mg / m^3
    ))}

    list(
        sand =            soil_row(0.05, 0.03, 0.92, -0.7, 1.7, 5.8e-3, 0.87, 0.09, 0.03, 1.60),
        loamy_sand =      soil_row(0.12, 0.07, 0.81, -0.9, 2.1, 1.7e-3, 0.72, 0.13, 0.06, 1.55),
        sandy_loam =      soil_row(0.25, 0.10, 0.65, -1.5, 3.1, 7.2e-4, 0.57, 0.21, 0.10, 1.50),
        loam =            soil_row(0.40, 0.18, 0.42, -1.1, 4.5, 3.7e-4, 0.57, 0.27, 0.12, 1.43),
        silt_loam =       soil_row(0.65, 0.15, 0.20, -2.1, 4.7, 1.9e-4, 0.59, 0.33, 0.13, 1.36),
        sandy_clay_loam = soil_row(0.13, 0.27, 0.60, -2.8, 4.0, 1.2e-4, 0.48, 0.26, 0.15, 1.39),
        clay_loam =       soil_row(0.34, 0.34, 0.32, -2.6, 5.2, 6.4e-5, 0.52, 0.32, 0.20, 1.35),
        silty_clay_loam = soil_row(0.58, 0.33, 0.09, -3.3, 6.6, 4.2e-5, 0.52, 0.37, 0.21, 1.24),
        sandy_clay =      soil_row(0.07, 0.40, 0.53, -2.9, 6.0, 3.3e-5, 0.51, 0.34, 0.24, 1.30),
        silty_clay =      soil_row(0.45, 0.45, 0.10, -3.4, 7.9, 2.5e-5, 0.52, 0.39, 0.25, 1.28),
        clay =            soil_row(0.20, 0.60, 0.20, -3.7, 7.6, 1.7e-5, 0.53, 0.40, 0.27, 1.19)
    )
})
