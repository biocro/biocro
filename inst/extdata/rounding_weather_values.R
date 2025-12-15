# Here, the original `weather` list should be created by loading
# `cmi_weather_data.RData` from
# https://github.com/eloch216/oscillator-based-circadian-clock-analysis/tree/main/output_archive/RData
# using load('cmi_weather_data.RData')

# Make a copy of the weather data with a different name
raw_weather <- weather

# Round all values to 3 significant figures
weather <- lapply(raw_weather, function(wd) {
    for (cn in colnames(wd)) {
        if(cn=="year") next
        wd[[cn]] <- signif(wd[[cn]], digits = 3)
    }
    wd
})

# Round `solar` to the nearest integer
# Clamp to positive values
weather <- lapply(weather, function(wd) {
    wd$solar <- pmax(0, round(wd$solar))
})

# Round `rh` to 2 significant figures
weather <- lapply(weather, function(wd) {
    wd$rh <- signif(wd$rh, digits = 2)
    wd
})

# Save the rounded data
save(weather, file = 'cmi_weather_data_2.rdata')

# When storing this in the BioCro repository, renamed to remove the `_2`
