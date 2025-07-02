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


sample_objective_function_graph <- function(n, mu, sd, drivers){
    z <- rnorm(n * 20)
    par <- mu  + sd * matrix(z, 20, n)
    par <- abs(par)
    par[1:10,] <- par[1:10,]/apply(par[1:10,],2, sum)
    par[11:20,] <- par[11:20,]/apply(par[11:20, ],2, sum)
    
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
    
    data.frame(par = t(par), obj = apply(par, 2, objective_function))    
}

optimize <- function(drivers, par0=rep(0.1, 20)){
    print(drivers[['year']][1])
    flow <- partial_run_biocro(
        initial_values = soybean$initial_values,
        parameters = within(params,{chil <- chil * 0.1}),
        drivers = drivers,
        direct_module_names = soybean_direct,
        differential_module_names = soybean$differential_modules,
        arg_names = kronecker(c("jmax_fraction_", "vcmax_fraction_"), 0:9, paste0)
    )    
    
    project_to_simplex <- function(x){ 
        x[1:10] <- x[1:10]/sum(x[1:10])
        x[11:20] <- x[11:20]/sum(x[11:20]) 
        x
    }
    
    objective_function <- function(x){
        #c <- equality_constraint(x)
        # u <- project_to_simplex(x)
        out <- flow_by_jv(x)
        n <- nrow(out)
        -out[n,'Grain']  # +  1e-2 * sum(c^2)
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
        fraction = lapply(result, \(x){project_to_simplex(x$solution)}) |> unlist()
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



growing_season <- lapply(weather, cut_weather_data)
t1 <- Sys.time()
soybean_weather_result <- lapply(soybean_weather, optimize)
# weather_result <- lapply(growing_season, optimize)
t2 <- Sys.time()
t2-t1
save(weather_result,file=".rdata")

# save(soyweather_result, file=".rdata")
# flow_by_jv <- partial_run_biocro(
#     initial_values = soybean$initial_values,
#     parameters = params,
#     drivers = soybean_weather[['2006']],
#     direct_module_names = soybean_direct,
#     differential_module_names = soybean$differential_modules,
#     arg_names = kronecker(c("jmax_fraction_", "vcmax_fraction_"), 0:9, paste0)
# )

# 
opt <- nloptr::nloptr(
    x0 = rep(0.1,20),
    eval_f = objective_function,
    eval_g_ineq = equality_constraint,
    lb = rep(0, 20),
    opts = list(
        algorithm = "NLOPT_LN_BOBYQA",
        # algorithm = "NLOPT_GN_ISRES",
        # algorithm = "NLOPT_LN_AUGLAG_EQ",
        # local_opts = list(algorithm = "NLOPT_LN_COBYLA",
        #                   maxeval = 2000,
        #                   xtol_rel   = 1.0e-7
        # ),
        maxeval = 200,
        xtol_rel   = 1.0e-6,
        print_level = 3)
)

get_ratio <- function(result){
    msgs <- lapply(result, function(x)x$message)
    ys <- names(result)
    nyear <- length(ys)
    years <- rep(ys, each=10)
    
    Jmax <- params$Jmax_at_25
    Vcmax <- params$Vcmax_at_25 
    
    df= data.frame(
        layer = rep(0:9, nyear), 
        year = years,
        param = rep("Jmax/Vcmax", 10 * nyear),
        fraction_ratio = lapply(result, \(x){x$solution[1:10]/x$solution[11:20]}) |> unlist()
    )
    
    df['ratio'] <-  Jmax /Vcmax *  df[['fraction_ratio']]
    list(messages = msgs, results = df )
    
} 
# result <- flow_by_jv(opt$solution)
plt + geom_line(data=result, mapping = aes(x=time,y=Leaf), color = 'red') + theme_bw()

soyres <- rbind(soyres_neldermead$results,soyres_cobyla$results)
soyres$method <- rep(c("neldermead","cobyla"),40)

qlt <- ggplot(soyres_neldermead$results, aes(layer, value, color = year)) + geom_point() + geom_line() + theme_bw() + facet_grid(cols = vars(param)) + labs(y="micromol / m^2 / s")
ggplot(soyres, aes(layer, value, color = method)) + geom_point() + geom_line() + theme_bw() + facet_grid(rows=vars(year), cols = vars(param)) + labs(y="micromol / m^2 / s")




cmat <- rbind(rep(c(1,0),each=10), rep(c(0,1), each=10))
opt <- stats::constrOptim(rep(0.1, 20), objective_function, ui = -cmat, ci = rep(-1,2), method= "Nelder-Mead")



