library(BioCro)
library(ggplot2)


f <- rep(1, 10)
names(f) <- paste0("jmax_fraction_", 0:9)
g <- rep(1, 10)
names(g) <- paste0("vcmax_fraction_", 0:9)

params <- c(soybean$parameters, as.list(f), as.list(g))

params <- within(params, {
    leaf_width <- leafwidth
})

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

process_optimizations <- function(result){
   
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
    
    df['value'] <-rep(rep( c(Jmax,Vcmax), each=10), nyear) *  df[['fraction']]
    df
}


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



load(file="weather_result.rdata")
load(file="biocro/script/soybean_weather_result.rdata")

load(file="biocro/script/soybean_weather_result_exchange.rdata")

load(file="biocro/script/soybean_weather_result_high_co2.rdata")

res <- process_optimizations(soybean_weather_result)

plt + geom_line(data=result, mapping = aes(x=time,y=Leaf), color = 'red') + theme_bw()

qlt <- ggplot(soyres_neldermead$results, aes(layer, value, color = year)) + geom_point() + geom_line() + theme_bw() + facet_grid(cols = vars(param)) + labs(y="micromol / m^2 / s")
ggplot(res, aes(layer, value, color = year)) + geom_point() + geom_line() + theme_bw() + facet_grid(cols = vars(param)) + labs(y="micromol / m^2 / s")
ggplot(resex, aes(layer, value, color = year)) + geom_point() + geom_line() + theme_bw() + facet_grid(cols = vars(param)) + labs(y="micromol / m^2 / s")
ggplot(resd, aes(layer, value, color = exchange)) + geom_point() + geom_line() + theme_bw() +facet_grid(cols = vars(param),rows=vars(year))



cmat <- rbind(rep(c(1,0),each=10), rep(c(0,1), each=10))
opt <- stats::constrOptim(rep(0.1, 20), objective_function, ui = -cmat, ci = rep(-1,2), method= "Nelder-Mead")



