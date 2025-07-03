

create_soybean_model <- function(){
  f <- rep(1, 10)
  names(f) <- paste0("jmax_fraction_", 0:9)
  g <- rep(1, 10)
  names(g) <- paste0("vcmax_fraction_", 0:9)
  
  params <- c(BioCro::soybean$parameters, as.list(f), as.list(g))
  params <- within(params, {
    leaf_width <- leafwidth
  })
  
  soybean_direct <- append(
    Filter(function(x){!(x %in% c(
      "BioCro:ten_layer_c3_canopy",
      "BioCro:ten_layer_canopy_integrator",
      "BioCro:ten_layer_canopy_properties")) }, BioCro::soybean$direct_modules), "BioCro:c3_canopy")
  
  list(
    initial_values = BioCro::soybean$initial_values,
    parameters = params,
    direct_module_names = soybean_direct,
    differential_module_names = BioCro::soybean$differential_modules
  )
}


optimize <- function(weather_data, par0=rep(1, 20), model){
  print(weather_data[['year']][1])
  
  run_args <- c(
    model, 
    list(
      drivers = weather_data, 
      arg_names=  kronecker(c("jmax_fraction_", "vcmax_fraction_"), 0:9, paste0)
      )
    )
  
  flow <- do.call(BioCro::partial_run_biocro, run_args)    
  
  project_to_simplex <- function(x){ 
    x[1:10] <- x[1:10]/sum(x[1:10])
    x[11:20] <- x[11:20]/sum(x[11:20]) 
    x
  }
  
  objective_function <- function(x){
    #c <- equality_constraint(x)
    # u <- project_to_simplex(x)
    out <- flow(x)
    n <- nrow(out)
    -out[n,'Grain']  # +  1e-2 * sum(c^2)
  }
  
  equality_constraint <- function(x){
    
    list(
      constraints = c(
        sum(x[1:10]),
        sum(x[11:20])
      ) - 10,
      jacobian = rbind(rep(c(1,0),each = 10),rep(c(0,1), each = 10))
    )
    # sum(x) - 20
  }
  
  lb <- rep(0, 20)
  
  opts <- list(
    algorithm = "NLOPT_LN_COBYLA",
    maxeval = 3000,
    xtol_rel = 1.0e-6,
    xtol_abs = 1e-7,
    # maxtime = 600,
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

cut_weather_data <- function(data, start_doy = 150, end_doy = 270, days = NULL){
  if(!is.null(days)){
    end_doy <- start_doy + days
  }
  idx <- (data[['doy']] >= start_doy) & (data[['doy']] <= end_doy)
  data[idx,]
}

model <- create_soybean_model()

cl <- parallel::makeCluster(getOption("cl.cores", 4))
t1 <- Sys.time()
#soybean_weather_result <- lapply(soybean_weather, optimize, model=model)
soybean_weather_result <- parallel::parLapply(cl, BioCro::soybean_weather, optimize, model=model)
t2 <- Sys.time()
t2-t1
save(soybean_weather_result,file="soybean_weather_result.rdata")

# 
# growing_season <- lapply(weather, cut_weather_data)
# t1 <- Sys.time()
# weather_result <- lapply(growing_season, optimize)
# t2 <- Sys.time()
# t2-t1
# save(weather_result,file=".rdata")
