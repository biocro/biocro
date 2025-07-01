library(BioCro)
library(ggplot2)


f <- rep(0.1, 10)
names(f) <- paste0("jmax_fraction_", 0:9)
g <- rep(0.1, 10)
names(g) <- paste0("vcmax_fraction_", 0:9)

params <- c(soybean$parameters, as.list(f), as.list(g))

params <- within(params, {
    leaf_width <- leafwidth
    leaf_n_relative <- 1
})
# direct_module_names = list(
#     "BioCro:ten_layer_c3_canopy",
#     "BioCro:ten_layer_canopy_integrator",
#     "BioCro:ten_layer_canopy_properties",
#     "BioCro:solar_position_michalsky",
#     "BioCro:stefan_boltzmann_longwave",
#     "BioCro:height_from_lai",
#     "BioCro:incident_shortwave_from_ground_par",
#     "BioCro:canopy_gbw_thornley",
#     "BioCro:shortwave_atmospheric_scattering"
# )
baseline <- run_biocro(
    soybean$initial_values,
    soybean$parameters,
    soybean_weather[['2006']],
    soybean$direct_modules,
    soybean$differential_modules
)

# plot baseline 
plt <- ggplot(baseline,aes(time, Grain)) + geom_line()
plt 

soybean_direct <- append(
    Filter(function(x){!(x %in% c(
    "BioCro:ten_layer_c3_canopy",
    "BioCro:ten_layer_canopy_integrator",
    "BioCro:ten_layer_canopy_properties")) }, soybean$direct_modules), "BioCro:c3_canopy")

result <- run_biocro(
    initial_values = soybean$initial_values,
    parameters = params,
    drivers = soybean_weather[['2006']],
    direct_module_names = soybean_direct,
    differential_module_names = soybean$differential_modules
) 

flow_by_jv <- partial_run_biocro(
    initial_values = soybean$initial_values,
    parameters = params,
    drivers = soybean_weather[['2006']],
    direct_module_names = soybean_direct,
    differential_module_names = soybean$differential_modules,
    arg_names = kronecker(c("jmax_fraction_", "vcmax_fraction_"), 0:9, paste0)
)
objective_function <- function(x){
    out <- flow_by_jv(x)
    n <- nrow(out)
    -out[n,'Grain']
}
equality_constraint <- function(x){
    c(sum(x[1:10]), sum(x[11:20])) - 1
}
par <- rep(0.1, 20)
objective_function(par)
equality_constraint((par))
opt <- nloptr::nloptr(
    x0 = par,
    eval_f = objective_function,
    eval_g_eq = equality_constraint,
    lb = rep(0, 20),
    opts = list(
        algorithm = "NLOPT_LN_COBYLA",
        maxeval = 200,
        print_level = 3)
)

result <- flow_by_jv(opt$solution)
plt + geom_line(data=result, mapping = aes(x=time,y=Grain), color = 'red') + theme_bw()
by_height <- data.frame(height = rep(0:9, 2), param = rep(c("jmax", "vcmax") ,each= 10) , value = opt$solution)
lattice::xyplot(value ~ height, group = param , by_height, type=c('l','p'), grid=T, xlab= "Layer", ylab = "Fraction", auto.key=TRUE)
