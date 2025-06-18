library(BioCro)
library(ggplot2)


f <- exp(-seq(0, 3, length.out = 10))
f <- f/sum(f)
names(f) <- paste0("leaf_n_profile_", 0:9)
u <- 0:10/10
names(u) <- paste0("vcmax_n_fraction_", 0:9)

params <- c(soybean$parameters, as.list(u), as.list(f))

params <- within(params, {
    StomataWS<-0
    leaf_n_relative <- 2
    lai <- 3
    leaf_width <- 0
    
})

baseline <- run_biocro(
    soybean$initial_values,
    params,
    soybean_weather[['2006']],
    direct_module_names = list(
        "BioCro:ten_layer_c3_canopy",
        "BioCro:ten_layer_canopy_integrator",
        "BioCro:ten_layer_canopy_properties",
        "BioCro:solar_position_michalsky",
        "BioCro:stefan_boltzmann_longwave",
        "BioCro:height_from_lai",
        "BioCro:incident_shortwave_from_ground_par",
        "BioCro:canopy_gbw_thornley",
        "BioCro:shortwave_atmospheric_scattering"
    )
)

# plot baseline 
ggplot(baseline,aes(solar, canopy_assimilation_molar_flux)) + geom_point()



result <- run_biocro(
    initial_values = list(),
    parameters = params,
    drivers = soybean_weather[['2006']],
    direct_module_names = list(
        "BioCro:c3_canopy",
        "BioCro:solar_position_michalsky",
        "BioCro:stefan_boltzmann_longwave",
        "BioCro:height_from_lai",
        "BioCro:canopy_gbw_thornley"
        ),
    differential_module_names = list()
) 

ggplot(baseline, aes(solar, canopy_assimilation_molar_flux)) + geom_point() + geom_point(data=result, mapping = aes(x=solar,y=canopy_assimilation_molar_flux), color = 'red') + theme_bw()
