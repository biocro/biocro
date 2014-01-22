data(forweach.test)
test_that("weachDT produces the same result as weachNEW, only faster",{
    fnew <- weachNEW(as.data.frame(forweach.test),
                    lat = 40,
                    ts = 1,
                    temp.units = "Celsius",
                    rh.units = "fraction",
                    ws.units = "mps",
                    pp.units = "mm")
    forweach.test$doy <- forweach.test$day
    fdt <- weachDT(forweach.test, lat = 40)
    expect_equal(fnew$year, fdt$year)
    expect_equal(fnew$doy, fdt$doy)
    expect_equal(fnew$hour, fdt$hour)
    expect_equal(fnew$SolarR, fdt$SolarR)
    expect_equal(fnew$Temp, fdt$Temp)
    expect_equal(fnew$RH, fdt$RH)
    expect_equal(fnew$WS, fdt$WS)
    expect_equal(fnew$precip, fdt$precip)
})
