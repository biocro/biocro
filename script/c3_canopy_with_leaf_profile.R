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
plt <- ggplot(baseline,aes(time, Leaf)) + geom_line()
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

optimize <- function(drivers, par0=rep(0.1, 20)){
    print(drivers[['year']][1])
    flow <- partial_run_biocro(
        initial_values = soybean$initial_values,
        parameters = params,
        drivers = drivers,
        direct_module_names = soybean_direct,
        differential_module_names = soybean$differential_modules,
        arg_names = kronecker(c("jmax_fraction_", "vcmax_fraction_"), 0:9, paste0)
    )    
    
    objective_function <- function(x){
        out <- flow_by_jv(x)
        n <- nrow(out)
        -out[n,'Grain'] # sum(x^2)
    }
    
    equality_constraint <- function(x){
        c(sum(x[1:10]), sum(x[11:20])) - 1
    }
    
    lb <- rep(0, 20)
    
    opts <- list(
        algorithm = "NLOPT_LN_COBYLA",
        maxeval = 3000,
        xtol_rel = 1.0e-4,
        xtol_abs = 1e-7,
        maxtime = 600,
        print_level = 3
    )
    
    sol <- nloptr::nloptr(
        x0 = par0,
        eval_f = objective_function,
        eval_g_eq = equality_constraint,
        lb = lb,
        opts = opts   
    )
    
    return(sol)
}

process_optimizations <- function(result){
    msgs <- lapply(result, function(x)x$message)
    ys <- names(result)
    nyear <- length(ys)
    years <- rep(ys, each=20)
    
    Jmax <- params$Jmax_at_25
    Vcmax <- params$Vcmax_at_25 
    
    df= data.frame(
        layer = rep(0:9, 2 * nyear), 
        year = years,
        param = rep(rep(c("Jmax", "Vcmax"), each = 10), nyear),
        fraction = lapply(result, \(x){x$solution}) |> unlist()
        )
    df['value'] <-  10 * rep(rep( c(Jmax,Vcmax), each=10), nyear) *  df[['fraction']]
    list(messages = msgs, results = df )
}

cut_weather_data <- function(data, start_doy = 150, end_doy = 270, days = NULL){
    if(!is.null(days)){
        end_doy <- start_doy + days
    }
    idx <- (data[['doy']] >= start_doy) & (data[['doy']] <= end_doy)
    data[idx,]
}

t1 <- Sys.time()
short_weather <- lapply(weather, cut_weather_data)
test <- lapply(short_weather, optimize)
# soyweather_result <- lapply(soybean_weather, optimize)
t2 <- Sys.time()
t2-t1
# 
# flow_by_jv <- partial_run_biocro(
#     initial_values = soybean$initial_values,
#     parameters = params,
#     drivers = soybean_weather[['2006']],
#     direct_module_names = soybean_direct,
#     differential_module_names = soybean$differential_modules,
#     arg_names = kronecker(c("jmax_fraction_", "vcmax_fraction_"), 0:9, paste0)
# )

# 
# opt <- nloptr::nloptr(
#     x0 = par,
#     eval_f = objective_function,
#     eval_g_eq = equality_constraint,
#     lb = rep(0, 20),
#     opts = list(
#         algorithm = "NLOPT_LN_COBYLA",
#         # algorithm = "NLOPT_GN_ISRES",
#         # algorithm = "NLOPT_LN_AUGLAG_EQ",
#         # local_opts = list(algorithm = "NLOPT_LN_COBYLA",
#         #                   maxeval = 2000,
#         #                   xtol_rel   = 1.0e-7
#         # ),
#         maxeval = 2000,
#         xtol_rel   = 1.0e-6,
#         print_level = 3)
# )
# 
# result <- flow_by_jv(opt$solution)
plt + geom_line(data=result, mapping = aes(x=time,y=Leaf), color = 'red') + theme_bw()

ggplot(soyres$results, aes(layer, value, color = year)) + geom_point() + geom_line() + theme_bw() + facet_grid(cols = vars(param)) + labs(y="micromol / m^2 / s")
# cmat <- rbind(rep(c(1,0),each=10), rep(c(0,1), each=10))
# opt <- stats::constrOptim(par, objective_function, ui = cmat, ci = rep(1,2))



