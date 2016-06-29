data(forweach.test)
test_that("weachDT produces the same result as weachNEW, only faster",{
    cat('Time taken to run weachNEW\n')
    print(system.time({
        fnew <- weachNEW(as.data.frame(forweach.test),
                    lat = 40,
                    ts = 1,
                    temp.units = "Celsius",
                    rh.units = "fraction",
                    ws.units = "mps",
                    pp.units = "mm")
    }))
    # Force loading of [.data.table before the test. so that the time taken to load it will not be included in the test.
	data.table:::`[.data.table`
    cat('Time taken to run weach.data.table\n')
    print(system.time({
        fdt <- weach.data.table(forweach.test, lat = 40)
    }))
    expect_equal(fnew$year, fdt$year)
    expect_equal(fnew$doy, fdt$doy)
    expect_equal(fnew$hour, fdt$hour)
    expect_equal(fnew$SolarR, fdt$SolarR)
    expect_equal(fnew$Temp, fdt$Temp)
    expect_equal(fnew$RH, fdt$RH)
    expect_equal(fnew$WS, fdt$WS)
    expect_equal(fnew$precip, fdt$precip)
})
