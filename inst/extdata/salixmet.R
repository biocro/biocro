require(data.table)
require(lubridate)
require(ggplot2)
require(PEcAn.all)

load(system.file("/extdata/salix.RData", package = "BioCro"))

settings.xml <- "~/pecan.xml"

settings <- read.settings(settings.xml)
settings$database <- list(userid = "dlebauer", passwd = "g01llin1", name = "ebi_production", host = "localhost")

testsites <- unique(salix.testyield[,list(site, lat, lon, city, country)])

betysites <- query.base('select * from sites;')

for(sitei in testsites$site){
  site <- testsites[site == sitei,]
  lat <- site$lat
  lon <- site$lon
  
  outdir <- "/home/dlebauer/inputs"
  dir.create(path = outdir, showWarnings = FALSE)
  start.date <- salix.testyield[site == sitei, min(ymd(planting_date))]
  end.date <- start.date + salix.testyield[site == sitei, max(stand_age)]
  
  ### 
  
  nearest.site <- betysites[which.min((lat - betysites$lat)^2 + (lon - betysites$lon)^2),]
  dxdy <- sqrt((nearest.site$lat - lat)^2 + (nearest.site$lon - lon)^2)
  site.exists <- dxdy < 0.25
  
  if(site.exists){
    site.id <- nearest.site$id
  } else if(!site.exists) {
    query.base(paste0("insert into sites (city, country, lat, lon) values(",
                      vecpaste(c(site$city, site$country, lat, lon)), ");"))
    site.id <- query.base("select max(id) from sites;")
  }
  
  metfiles <- query.base(paste0("select start_date, end_date, file_name, file_path ",
                               "from inputs join dbfiles on dbfiles.file_id = inputs.file_id ",
                               "where start_date <= '", start.date, 
                               "' and end_date >= '", end.date, 
                               "' and site_id =", site.id, ";"))
  if(nrow(metfiles) > 1){
    logger.error("more than one set of metfiles; need to choose one")
  } else if(nrow(metfiles) == 1){
 #   weather <- read.csv(file.path(metfiles$file_path, metfiles$file_name), row.names = NULL)
    logger.info("using existing weather data located at", metfiles[,c])
  } else if(nrow(metfiles) == 0) {
    logger.info("downloading weather data from RNCEP")
    weather <- InputForWeach(lat, lon, year(start.date), year(end.date))
    metout <- paste0(outdir, "/", lat,  "-", lon, "/", year(start.date), "-", year(end.date), "/")
    dir.create(metout, showWarnings = FALSE, recursive = TRUE)
    write.csv(weather, file = file.path(metout, "weather.csv"))
    file.id <- 1+ max(query.base(paste0("select max(file_id) from inputs;")))
    queries <- list(
      input.insert = paste0("insert into inputs ",
                             "(created_at, site_id, format_id, start_date, end_date, access_level, notes) ",
                             "values( NOW(),", 
                             vecpaste(c(site.id, 28, start.date, end.date, 4, "downloaded from NCEP by InputForWeach()")),
                             ");"),
      set.file_id = "SET @FILE_ID=LAST_INSERT_ID();\n",
      insert.file_id = "UPDATE inputs SET file_id=@FILE_ID WHERE id=@FILE_ID;",
      dbfiles.insert = paste0("insert into dbfiles ",
                              "(file_id, file_name, file_path, machine_id, created_at, created_user_id) ",
                              "values(",
                              "@FILE_ID, 'weather.csv', '", metout, "', 2, NOW(),2);")
    )
    con <- query.base.con(settings)
    lapply(queries, function(x) query.base(x, con = con))
 
  }
}
