pp <- willowphotoParms()
cc <- willowcanopyParms()
day1 <- 100
dayn <- 200

data(weather05)
WetDat <- weather05


soil <- willowsoilParms(wsFun = 'linear',
                        FieldC = 0.182,
                        WiltP = 0.09,
                        soilDepth = 200, # > 200
                        soilLayers = 3, 
                        iWatCont = 0.2, # just more than field capacity
                        soilDepths = c(0, 0.2, 0.5, 0.8))


i <- list(iRhizome = 1, iStem = 1, iLeaf = 0, iRoot = 1, ifrRhizome = 0.01, ifrStem = 0.01)

iplant <- list(iStem = i$iStem,
               iRhizome = i$iStem / 2,
               iRoot = i$iStem / 2)

test_that("willowGro is sensitive to b0 stomatal slope",{
  
  cc <- canopyParms()
  pp <- list(vmax = "100", jmax = "180", Rd = "1.1", b0 = "0.08", 
             b1 = "5.0", Catm = "400", O2 = "210")
  pp$b0 <- 0.01#sa.samples$salix$cuticular_cond[1]/1000000
  b <- willowGro(WetDat = WetDat, photoControl = pp, soilControl = soil,
                 iPlantControl = iplant,
                 canopyControl=cc, 
                 day1 = day1, dayn = dayn)
  pp$b0 <- 4#sa.samples$salix$cuticular_cond[3]/1000000
  c <- willowGro(WetDat = WetDat, photoControl = pp, soilControl = soil,
                 iPlantControl = iplant,
                 canopyControl=cc, 
                 day1 = day1, dayn = dayn)
  
  expect_more_than(max(b$Stem), max(c$Stem))
  expect_less_than(mean(b$CanopyTrans), mean(c$CanopyTrans))
  expect_more_than(mean(b$CanopyAssim), mean(c$CanopyAssim))
  
  
})


test_that("willowGro is sensitive to stomatal Slope", {

  pp <- list(vmax = "100", jmax = "180", Rd = "1.1", b0 = "0.08", 
             b1 = "5.0", Catm = "400", O2 = "210")
  pp$b1 <- 0.2#sa.samples$salix$stomatal_slope.BB[1]
  b <- willowGro(WetDat = WetDat, photoControl = pp, soilControl = soil,
                 iPlantControl = iplant,
                 #canopyControl=cc, 
                 day1 = day1, dayn = dayn)
  pp$b1 <- 10#sa.samples$salix$stomatal_slope.BB[3]
  c <- willowGro(WetDat = WetDat, photoControl = pp, soilControl = soil,
                 iPlantControl = iplant,
                 #canopyControl=cc, 
                 day1 = day1, dayn = dayn)
  
  expect_more_than(max(b$Stem), max(c$Stem))
  expect_less_than(mean(b$CanopyTrans), mean(c$CanopyTrans))
  expect_more_than(mean(b$CanopyAssim), mean(c$CanopyAssim))
  
})


test_that("BioGro sensitive to Soil parameter phi1", {
  soil$wsFun = 1
  soil$phi1 <- 0.1
  b <- willowGro(WetDat = WetDat, photoControl = pp, soilControl = soil,
                 iPlantControl = iplant,
                 #canopyControl=cc, 
                 day1 = day1, dayn = dayn)
  soil$phi1 <- 0.01
  c <- willowGro(WetDat = WetDat, photoControl = pp, soilControl = soil,
                 iPlantControl = iplant,
                 #canopyControl=cc, 
                 day1 = day1, dayn = dayn)
  expect_less_than(max(b$Stem), max(c$Stem))
  expect_less_than(mean(b$CanopyTrans), mean(c$CanopyTrans))
  expect_less_than(mean(b$CanopyAssim), mean(c$CanopyAssim))
  
})