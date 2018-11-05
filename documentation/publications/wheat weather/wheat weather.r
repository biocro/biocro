library(BioCro)
library(lattice)
library(latticeExtra)
library(integration.helpers)

climate_columns = read.csv(textConnection(
                                 "name, col_ind
                                  date, 1
                                  hour, 2
                                  temperature, 15
                                  relative_humidity, 19
                                  downwelling_shortwave, 20
                                  downwelling_longwave, 22
                                  precipitation, 30
                                  windspeed, 39"), stringsAsFactors=FALSE, strip.white=TRUE)

weather = read.csv('Hour2014.txt', header=FALSE)[climate_columns$col_ind]
names(weather) <- climate_columns$name

head(weather)


