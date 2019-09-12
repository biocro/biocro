library(BioCro)

modules = c('water_vapor_properties_from_air_temperature', 'collatz_leaf')

parms = c(sorghum_parameters, weather05[13, ])
parms[c('vmax', 'alpha', 'k_Q10', 'wind_speed', 'StomataWS', 'layer_wind_speed')] =
       c(39, 0.04, 1, 3, 1, 1.2)

parms$incident_par = parms$solar
fraction_of_irradiance_in_PAR = 0.5;  # dimensionless.
joules_per_micromole_PAR = 0.235;   # J / micromole. For the wavelengths that make up PAR in sunlight, one mole of photons has, on average, approximately 2.35 x 10^5 joules:
parms$incident_irradiance = parms$incident_par * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR

#Gro_ode(parms, modules, list())
#result = run_modules(parms, modules)
#result[c('leaf_assimilation_rate', 'leaf_temperature', 'temp', 'leaf_stomatal_conductance')]


Qp = 1900
Tl = 36
RH = .9
vmax = 39
alpha = 0.04
kparm = 0.7
theta = 0.83
beta = 0.93
Rd = 0.8
uppertemp = 37.5
lowertemp = 3
Catm = 380
b0 = 0.08
b1 = 3
StomWS = 1
ws = "gs"

(c4result = c4photo(
    Qp = Qp,
    Tl = Tl,
    RH = RH,
    vmax = vmax,
    alpha = alpha,
    kparm = kparm,
    theta = theta,
    beta = beta,
    Rd = Rd,
    uppertemp = uppertemp,
    lowertemp = lowertemp,
    Catm = Catm,
    b0 = b0,
    b1 = b1,
    StomWS = StomWS,
    ws = ws
))

k_Q10 = 2
intercellular_co2_molar_fraction = c4result$Ci
(collatz_result = collatz_photo(
    Qp = Qp,
    leaf_temperature = Tl,
    vmax = vmax,
    alpha = alpha,
    kparm = kparm,
    theta = theta,
    beta = beta,
    Rd = Rd,
    upperT = uppertemp,
    lowerT = lowertemp,
    k_Q10 = k_Q10,
    intercellular_co2_molar_fraction = intercellular_co2_molar_fraction
))

c4result$assimilation_rate / collatz_result$assimilation_rate
c4result$GrossAssim / collatz_result$gross_assimilation_rate

k_Q10 = 2
intercellular_co2_molar_fraction = 40
(collatz_result = collatz_photo(
    Qp = Qp,
    leaf_temperature = Tl,
    vmax = vmax,
    alpha = alpha,
    kparm = kparm,
    theta = theta,
    beta = beta,
    Rd = Rd,
    upperT = uppertemp,
    lowerT = lowertemp,
    k_Q10 = k_Q10,
    intercellular_co2_molar_fraction = intercellular_co2_molar_fraction
))

cis = seq(1, 400, length=50)
values = numeric(length(cis))
for (i in seq_along(cis)) {
    values[i] = (collatz_photo(
        Qp = Qp,
        leaf_temperature = Tl,
        vmax = vmax,
        alpha = alpha,
        kparm = kparm,
        theta = theta,
        beta = beta,
        Rd = Rd,
        upperT = uppertemp,
        lowerT = lowertemp,
        k_Q10 = k_Q10,
        intercellular_co2_molar_fraction = cis[i]
    ))$assimilation_rate
}

x11(); plot(values ~ cis, type='l')
