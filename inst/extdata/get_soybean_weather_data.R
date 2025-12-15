# To use this script, first download a local copy of the
# `cropsinsilico/Soybean-BioCro` repository from
# https://github.com/cropsinsilico/Soybean-BioCro/tree/main/Data/Weather_data.
#
# The contents of this script should be run from an R session with the working
# directory set to the `Data/Weather_data` directory of the
# `cropsinsilico/Soybean-BioCro` repository.
#
# This script will create an rdata file called `soybean_weather.rdata`, which
# should be copied into the `data` directory of the `biocro/biocro` repository.

# Get the raw data, adding a new column for the time zone offset
filenames <- c(
    '2002_Bondville_IL_daylength.csv',
    '2004_Bondville_IL_daylength.csv',
    '2005_Bondville_IL_daylength.csv',
    '2006_Bondville_IL_daylength.csv'
)

soybean_weather_raw <- lapply(filenames, function(x) {
    wd <- read.csv(x)
    wd[['time_zone_offset']] <- -6
    wd
})

# Round all values to 3 significant figures
soybean_weather <- lapply(soybean_weather_raw, function(wd) {
    for (cn in colnames(wd)) {
        if (cn == 'year') next
        wd[[cn]] <- signif(wd[[cn]], digits = 3)
    }
    wd
})

# Round `solar` to the nearest integer
soybean_weather <- lapply(soybean_weather, function(wd) {
    wd$solar <- round(wd$solar)
    wd
})

# Round `rh` to 2 significant figures
soybean_weather <- lapply(soybean_weather, function(wd) {
    wd$rh <- signif(wd$rh, digits = 2)
    wd
})

# Clamp `solar` to positive values
soybean_weather <- lapply(soybean_weather, function(wd) {
    wd$solar <- pmax(wd$solar, 0)
})


# Set the names properly
names(soybean_weather) <- c('2002', '2004', '2005', '2006')

# Truncate the time range
time_info <- list(
    list(year = '2002', sow = 152, harvest = 288),
    list(year = '2004', sow = 149, harvest = 289),
    list(year = '2005', sow = 148, harvest = 270),
    list(year = '2006', sow = 148, harvest = 270)
)

for (x in time_info) {
    wd <- soybean_weather[[x$year]]
    wd <- wd[wd$doy >= x$sow & wd$doy <= x$harvest, ]
    soybean_weather[[x$year]] <- wd
}

# Save the rounded data
save(soybean_weather, file = 'soybean_weather.rdata')
