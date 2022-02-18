# When the BioCro package is loaded, automatically create lists of modules from
# the module name lists specified in the `data` directory
.onLoad <- function(libname, pkgname) {
    assign('glycine_max_direct_modules', std_lib(BioCro::glycine_max_direct_module_names), envir = topenv())
    assign('glycine_max_differential_modules', std_lib(BioCro::glycine_max_differential_module_names), envir = topenv())

    assign('manihot_esculenta_direct_modules', std_lib(BioCro::manihot_esculenta_direct_module_names), envir = topenv())
    assign('manihot_esculenta_differential_modules', std_lib(BioCro::manihot_esculenta_differential_module_names), envir = topenv())

    assign('miscanthus_x_giganteus_direct_modules', std_lib(BioCro::miscanthus_x_giganteus_direct_module_names), envir = topenv())
    assign('miscanthus_x_giganteus_differential_modules', std_lib(BioCro::miscanthus_x_giganteus_differential_module_names), envir = topenv())

    assign('sorghum_direct_modules', std_lib(BioCro::sorghum_direct_module_names), envir = topenv())
    assign('sorghum_differential_modules', std_lib(BioCro::sorghum_differential_module_names), envir = topenv())

    assign('soybean_direct_modules', std_lib(BioCro::soybean_direct_module_names), envir = topenv())
    assign('soybean_differential_modules', std_lib(BioCro::soybean_differential_module_names), envir = topenv())

    assign('soybean_clock_direct_modules', std_lib(BioCro::soybean_clock_direct_module_names), envir = topenv())
    assign('soybean_clock_differential_modules', std_lib(BioCro::soybean_clock_differential_module_names), envir = topenv())

    assign('willow_direct_modules', std_lib(BioCro::willow_direct_module_names), envir = topenv())
    assign('willow_differential_modules', std_lib(BioCro::willow_differential_module_names), envir = topenv())

    assign('zea_mays_direct_modules', std_lib(BioCro::zea_mays_direct_module_names), envir = topenv())
    assign('zea_mays_differential_modules', std_lib(BioCro::zea_mays_differential_module_names), envir = topenv())
}
