#include "../src/BioCro.h"
#include "gtest/gtest.h"
#include <cmath> // trig functions
#include <cfloat> // limits
#include "../src/BioCro.h" // functions we're testing
#include "gtest/gtest.h" // test framework
#include "Random.h" // custom random number generators


///////////////// lightME /////////////////

constexpr auto PI = 3.141592653589793238L;
constexpr auto RADIANS_PER_DEGREE = PI/180;
constexpr double DEGREES_PER_HOUR = 15;
constexpr int DAY_OF_YEAR_OF_VERNAL_EQUINOX = 365/4 - 10;
constexpr int DAY_OF_YEAR_OF_DECEMBER_SOLSTICE = 365 - 10;
constexpr int NOON_HOUR = 12;
constexpr double LATITUDE_OF_ARCTIC_CIRCLE = 66.5;
constexpr double AXIAL_TILT = 23.5;

// At the equator, during the equinox, when the sun is directly over the
// equator, we expect the zenith angle to equal the hour angle (so that their
// cosines are equal as well).  But lightME makes certain adjustments to
// cosine_zenith_angle in certain cases, so we have to adjust our expectations
// as well.
TEST(zenith_angle_calculations, SpringEquinoxAtEquator) {

    // This relatively large tolerance is necessary since specifying the time of
    // the vernal equinox only to the nearest day gives a solar declination
    // insufficiently close to zero.
    constexpr double TOLERANCE = 1E-5;

    for (int hour_of_the_day = 6; hour_of_the_day <= 21; hour_of_the_day++) {

        double hour_angle = (hour_of_the_day - 12) * DEGREES_PER_HOUR;
        double hour_angle_in_radians = hour_angle * RADIANS_PER_DEGREE;
        double expected_cosine_zenith_angle = cos(hour_angle_in_radians);

        struct Light_model result = lightME(0, DAY_OF_YEAR_OF_VERNAL_EQUINOX,
                                            hour_of_the_day);
        EXPECT_NEAR(expected_cosine_zenith_angle,
                    result.cosine_zenith_angle,
                    TOLERANCE);
    }
}


// At solar noon, during the December solstice, we expect the zenith angle to be
// equal to the latitude plus the angle of the earth's tilt so that the cosine
// of the zenith angle is equal to the cosine of the sum of latitude and the
// angle of the earth's tilt.
TEST(zenith_angle_calculations, NoonAtDecemberSolstice) {
    constexpr double TOLERANCE = 1E-15;
    for (int lat = -90; lat <= LATITUDE_OF_ARCTIC_CIRCLE; ++lat) {
        struct Light_model result = lightME(lat,
                                            DAY_OF_YEAR_OF_DECEMBER_SOLSTICE,
                                            NOON_HOUR);
        EXPECT_NEAR(result.cosine_zenith_angle,
                    cos((lat + AXIAL_TILT) * RADIANS_PER_DEGREE),
                    TOLERANCE);
    }
}

// At solar noon, during the equinox, we expect the zenith angle to be
// equal to the latitude so that the cosine of the zenith angle is
// equal to the cosine of the latitude.
TEST(zenith_angle_calculations, NoonAtSpringEquinox) {
    // This relatively large tolerance is necessary since specifying the time of
    // the vernal equinox only to the nearest day gives a solar declination
    // insufficiently close to zero.
    constexpr double TOLERANCE = 0.0018;
    for (int lat = -90; lat <= 90; ++lat) {
        struct Light_model result = lightME(lat, DAY_OF_YEAR_OF_VERNAL_EQUINOX,
                                            NOON_HOUR);
        EXPECT_NEAR(result.cosine_zenith_angle, cos(lat * RADIANS_PER_DEGREE),
                    TOLERANCE);
    }
}

// For any latitude, during the equinox the adjusted value of CosZenithAngle
// will always be at most 1e-1 at hours 6 (sunrise) and 18 (sunset).
TEST(zenith_angle_calculations, SunriseSunset) {
    constexpr double UPPER_LIMIT = 1E-1; // Upper limit when sun is near horizon

    srand(time(NULL));

    for (int i = 0; i < 1e6; ++i) {
        double lat = ((rand() % (int) 180e4) / 1e4) - 90;
        for (int td = 6; td <= 18; td += 12) {
            EXPECT_PRED_FORMAT2(::testing::DoubleLE,

                                lightME(lat,DAY_OF_YEAR_OF_VERNAL_EQUINOX, td)
                                .cosine_zenith_angle,

                                UPPER_LIMIT);
        }
    }
}


// Now we test irradiance.

TEST(irradiance_calculations, SampleValues) {

    // DBL_DIG = 15, so 16 s.d. should be enough
    double expected_direct_irradiance[13] = {
        0.000000000000000e+00, // hour = 6
        2.892963275968324e-04,
        1.491902732207600e-01,
        4.870550993026899e-01,
        6.527225504827220e-01,
        7.169984283919291e-01,
        7.341560564732688e-01,
        7.169984283919291e-01,
        6.527225504827220e-01,
        4.870550993026899e-01,
        1.491902732207600e-01,
        2.892963275968324e-04,
        0.000000000000000e+00 // hour = 18

    };
    double expected_diffuse_irradiance[13] = {
        1.000000000000000e+00,
        9.997107036724032e-01,
        8.508097267792399e-01,
        5.129449006973101e-01,
        3.472774495172779e-01,
        2.830015716080709e-01,
        2.658439435267311e-01,
        2.830015716080709e-01,
        3.472774495172779e-01,
        5.129449006973101e-01,
        8.508097267792399e-01,
        9.997107036724032e-01,
        1.000000000000000e+00
    };

    for (int i = 0; i <= 12; ++i) {

        int td = i + 6; // ranges from 6 to 18

        struct Light_model lm = lightME(87, DAY_OF_YEAR_OF_VERNAL_EQUINOX, td);

        EXPECT_DOUBLE_EQ(lm.direct_irradiance_fraction, expected_direct_irradiance[i]);
        EXPECT_DOUBLE_EQ(lm.diffuse_irradiance_fraction, expected_diffuse_irradiance[i]);
    }
}

// The sum of the direct and the diffuse irradiance should always be 1.
TEST(irradiance_calculations, DirectDiffuseSum) {
    srand(time(NULL));

    for (int i = 0; i < 1e6; ++i) {
        double lat = ((rand() % (int) 180e4) / 1e4) - 90;
        int DOY = rand() % 365;
        int td = rand() % 25;

        struct Light_model lm = lightME(lat, DOY, td);

        EXPECT_DOUBLE_EQ(lm.direct_irradiance_fraction + lm.diffuse_irradiance_fraction, 1.0);
    }
}

// Miscellaneous sample values
TEST(lightMETest, One) {
    struct Light_model result = lightME(45, 100, 15);
    EXPECT_DOUBLE_EQ (0.94682042635862518, result.direct_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.053179573641374746, result.diffuse_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.58750769610831033, result.cosine_zenith_angle);
}

TEST(lightMETest, Two) {
    struct Light_model result = lightME(45, 100, 12);
    EXPECT_DOUBLE_EQ (0.94866559021836816, result.direct_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.051334409781631909, result.diffuse_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.79286428947577225, result.cosine_zenith_angle);
}


TEST(lightMETest, Three) {
    struct Light_model result = lightME(45, 200, 12);
    EXPECT_DOUBLE_EQ (0.94933880654434322, result.direct_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.050661193455656631, result.diffuse_irradiance_fraction);
    EXPECT_DOUBLE_EQ (0.91294566229056384, result.cosine_zenith_angle);
}



///////////////// sunML //////////////////////

// Convenience class for storing and randomly generating arguments for the sunML
// function.
struct sunMLArgs {

    double Idir;
    double Idiff;
    double LAI;
    int nlayers;
    double cosTheta;
    double kd;
    double chil;
    double heightf;

    static constexpr double SolarConstant = 2650;

    // Used only in writing the tests.
    void print_heading() {
        cout << setw(14) << "Idir" << setw(14) << "Idiff" << setw(14) << "LAI" << setw(14) << "nlayers" << setw(14) << "cosTheta" << setw(14) << "kd" << setw(14) << "chil" << setw(14) << "heightf" << endl;
    }

    // Used only in writing the tests.
    void print() {
        cout << setprecision(10) << "{" << Idir << ", " << Idiff << ", " << LAI
             << ", " << nlayers << ", " << cosTheta << ", " << kd << ", "
             << chil << ", " << heightf << "}" << endl;
    }

    // Random generation of reasonable(?) argument values.
    void generate_values() {
        static Rand_double rd {0, 1};

        Idir = rd() * SolarConstant;
        Idiff = SolarConstant - Idir;
        LAI = 10 * rd();

        static Rand_int layer_number {1, 20};
        nlayers = layer_number();

        static Rand_double rd2 { DBL_MIN, 1 };
        cosTheta = rd2();

        kd = rd();
        chil = 10 * rd();
        heightf = 100 * rd() + 1;
    }
};

// Helper for
void compare_light_profiles(Light_profile lp_1, Light_profile lp_2, int nlayers) {
    constexpr auto TOLERANCE = 1E-5;
    for (int layer = 0; layer < nlayers; ++layer) {
        EXPECT_NEAR(lp_1.direct_irradiance[layer],
                    lp_2.direct_irradiance[layer],
                    TOLERANCE);
    }
}

// Check that an out_of_range exception is thrown if cosTheta is not in (0, 1].
TEST(sunMLTest, CheckCosThetaRange) {
    sunMLArgs args = sunMLArgs();
    for (int i = 0; i < 5; ++i) {
        args.generate_values();
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta = 1e-9;
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta = 1.0;
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta += DBL_EPSILON;
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);

        args.cosTheta = 0;
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);

        args.cosTheta = -0.5;
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);
    }
}


// Test that if the other arguments are valid, we get an out_of_range exception
// if and only if nlayers is < 1 or > MAXLAY.
TEST(sunMLTest, layer_size_limit) {
    constexpr auto tries = 5;

    sunMLArgs args = sunMLArgs();

    for (int i = 0; i < tries; ++i) {
        args.generate_values();

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, MAXLAY, args.cosTheta, args.kd, args.chil, args.heightf));

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, 1, args.cosTheta, args.kd, args.chil, args.heightf));

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, MAXLAY + 1, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);

        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, 0, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);

        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, -1, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);
    }
}

// Data for regression tests of sunML:

constexpr auto no_of_test_sets = 5;

sunMLArgs test_sets[no_of_test_sets] = {
    {198.3576848, 2451.642315, 9.891262665, 18, 0.06217077332, 0.2496962988, 9.531538733, 84.92732462},
    {536.5293961, 2113.470604, 4.370307648, 18, 0.2024639231, 0.8390110324, 6.839530777, 76.46642742},
    {24.0156964, 2625.984304, 9.781919347, 17, 0.4370307648, 0.8866557596, 9.643768294, 59.97796567},
    {309.9745335, 2340.025467, 6.79134948, 3, 0.1169715221, 0.4181141771, 3.004108162, 61.38125722},
    {538.7755371, 2111.224463, 1.812213349, 14, 0.679134948, 0.8184337161, 5.241493642, 81.69090093}
};

Light_profile result_sets[no_of_test_sets] =  {
    {
        {2312.902219, 2019.457561, 1763.418812, 1540.223165, 1345.686009,
         1176.120232, 1028.311028, 899.4608673, 787.134762, 689.2120947,
         603.8452268, 529.4239216, 464.5446264, 407.9838441, 358.6749906,
         315.6882513, 278.2130348, 245.5426822},

        {2289.499494, 1996.054835, 1740.016086, 1516.820439, 1322.283283,
         1152.717506, 1004.908302, 876.0581415, 763.7320362, 665.8093689,
         580.442501, 506.0211958, 441.1419006, 384.5811183, 335.2722649,
         292.2855255, 254.810309, 222.1399564},

        {784.1704519, 682.1345464, 594.0967256, 517.7007476, 451.2370086,
         393.3480897, 342.9020324, 298.9319238, 260.6025457, 227.1887654,
         198.0595887, 172.6653777, 150.5271428, 131.2273769, 114.40213,
         99.73412645, 86.94677391, 75.7989448},

        {0.3686474456, 0.129932918, 0.04579595868, 0.01614117395,
         0.005689093623, 0.002005169286, 0.0007067389172, 0.0002490961239,
         8.779604102e-05, 3.09444591e-05, 1.090663699e-05, 3.844136688e-06,
         1.354898571e-06, 4.775454897e-07, 1.68314957e-07, 5.932403379e-08,
         2.090925873e-08, 7.369645532e-09},

        {0.6313525544, 0.870067082, 0.9542040413, 0.983858826, 0.9943109064,
         0.9979948307, 0.9992932611, 0.9997509039, 0.999912204, 0.9999690555,
         0.9999890934, 0.9999961559, 0.9999986451, 0.9999995225, 0.9999998317,
         0.9999999407, 0.9999999791, 0.9999999926},

        {0.1132321713, 0.1067617615, 0.1002913517, 0.09382094193, 0.08735053214,
         0.08088012236, 0.07440971257, 0.06793930278, 0.06146889299,
         0.0549984832, 0.04852807341, 0.04205766362, 0.03558725384,
         0.02911684405, 0.02264643426, 0.01617602447, 0.009705614683,
         0.003235204894}
    },
    {
        {2036.279857, 1686.323014, 1400.264605, 1166.493809, 975.4948399,
         819.4727725, 692.0451831, 587.9883054, 503.0282517, 433.6694471,
         377.0537485, 330.8448406, 293.1334318, 262.3595513, 237.2488941,
         216.7606982, 200.0450827, 186.4081422},

        {1910.219295, 1560.262452, 1274.204043, 1040.433246, 849.4342776,
         693.4122102, 565.9846209, 461.9277431, 376.9676894, 307.6088849,
         250.9931862, 204.7842783, 167.0728695, 136.298989, 111.1883318,
         90.7001359, 73.98452042, 60.3475799},

        {424.3705493, 345.3661312, 281.0991926, 228.8141143, 186.271957,
         151.6532674, 123.479238, 100.5477375, 81.88139756, 66.6854718,
         54.31362229, 44.24013549, 36.03735544, 29.35735087, 23.91701939,
         19.48598215, 15.87674465, 12.93669817},

        {0.7569536876, 0.5710879447, 0.430860495, 0.3250651111, 0.2452471917,
         0.1850281159, 0.1395954973, 0.1053186039, 0.07945820999, 0.05994769108,
         0.04522787094, 0.03412242027, 0.02574385088, 0.01942259233,
         0.01465348345, 0.01105540258, 0.008340810326, 0.006292771017},

        {0.2430463124, 0.4289120553, 0.569139505, 0.6749348889, 0.7547528083,
         0.8149718841, 0.8604045027, 0.8946813961, 0.92054179, 0.9400523089,
         0.9547721291, 0.9658775797, 0.9742561491, 0.9805774077, 0.9853465165,
         0.9889445974, 0.9916591897, 0.993707229},

        {0.05556569538, 0.05239051279, 0.04921533019, 0.0460401476,
         0.04286496501, 0.03968978242, 0.03651459982, 0.03333941723,
         0.03016423464, 0.02698905204, 0.02381386945, 0.02063868686,
         0.01746350426, 0.01428832167, 0.01111313908, 0.007937956483,
         0.00476277389, 0.001587591297}
    },
    {
        {2045.372241, 1232.440548, 744.250049, 451.0808666, 275.0286881,
         169.3082552, 105.8232395, 67.70098244, 44.80910852, 31.06300143,
         22.80882031, 17.85244255, 14.87631755, 13.08927658, 12.01623975,
         11.37193443, 10.98506365},

        {2034.968543, 1222.03685, 733.8463513, 440.6771689, 264.6249904,
         158.9045575, 95.41954183, 57.29728476, 34.40541084, 20.65930375,
         12.40512263, 7.448744864, 4.472619866, 2.685578893, 1.612542063,
         0.9682367511, 0.5813659633},

        {894.9942886, 537.368863, 322.6438049, 193.7192785, 116.3110864,
         69.83420793, 41.92897148, 25.17440238, 15.11482604, 9.074991302,
         5.448643783, 3.271370998, 1.964130841, 1.179262011, 0.7080265136,
         0.4250970521, 0.2552266748},

        {0.5730110714, 0.3239328813, 0.1831247541, 0.1035235307, 0.05852360846,
         0.03308438887, 0.01870316639, 0.01057321731, 0.005977219146,
         0.003379023402, 0.001910219263, 0.001079879361, 0.000610474126,
         0.0003451113817, 0.0001950973198, 0.0001102918252, 6.234984018e-05},

        {0.4269889286, 0.6760671187, 0.8168752459, 0.8964764693, 0.9414763915,
         0.9669156111, 0.9812968336, 0.9894267827, 0.9940227809, 0.9966209766,
         0.9980897807, 0.9989201206, 0.9993895259, 0.9996548886, 0.9998049027,
         0.9998897082, 0.9999376502},

        {0.1582950627, 0.1487014226, 0.1391077824, 0.1295141422, 0.1199205021,
         0.1103268619, 0.1007332217, 0.09113958157, 0.0815459414, 0.07195230124,
         0.06235866107, 0.05276502091, 0.04317138074, 0.03357774058,
         0.02398410041, 0.01439046025, 0.004796820082}
    },
    {
        {1548.852038, 656.0940125, 309.8997235},
        {1458.510644, 565.7526182, 219.5583293},
        {583.6695905, 226.2577817, 87.80602172},
        {0.0105275321, 3.738528137e-05, 1.327622894e-07},
        {0.9894724679, 0.9999626147, 0.9999998672},
        {0.09220172666, 0.05532103599, 0.01844034533}
    },
    {
        {2348.821032, 2151.260256, 1973.096391, 1812.433742, 1667.560851,
         1536.932714, 1419.154705, 1312.968027, 1217.236571, 1130.935033,
         1053.138171, 983.0110939, 919.8004959, 862.8267234},

        {2004.522652, 1806.961876, 1628.798012, 1468.135362, 1323.262471,
         1192.634335, 1074.856325, 968.669647, 872.9381915, 786.6366538,
         708.839791, 638.7127143, 575.5021163, 518.5283438},

        {281.4646629, 253.1250673, 227.6382171, 204.7170256, 184.1032876,
         165.5647754, 148.8926259, 133.8989906, 120.4149223, 108.2884741,
         97.38298855, 87.57556063, 78.75565362, 70.82385508},

        {0.8858715305, 0.7842834744, 0.6943451133, 0.6147205087, 0.544226922,
         0.4818172461, 0.4265644518, 0.3776478178, 0.3343407395, 0.2959999365,
         0.2620558971, 0.2320044187, 0.2053991186, 0.1818448035},

        {0.1141284695, 0.2157165256, 0.3056548867, 0.3852794913, 0.455773078,
         0.5181827539, 0.5734355482, 0.6223521822, 0.6656592605, 0.7040000635,
         0.7379441029, 0.7679955813, 0.7946008814, 0.8181551965},

        {0.02139150657, 0.01980695053, 0.01822239449, 0.01663783844,
         0.0150532824, 0.01346872636, 0.01188417032, 0.01029961427,
         0.008715058232, 0.00713050219, 0.005545946148, 0.003961390106,
         0.002376834063, 0.0007922780211}
    }
};

// Regression test on miscellaneous random input; check that the output doesn't
// change.
TEST(sunMLTest, miscellaneous_test_data) {
    for (int setNo = 0; setNo < no_of_test_sets; ++setNo) {
        sunMLArgs args = test_sets[setNo];

        Light_profile lp = sunML(args.Idir, args.Idiff, args.LAI, args.nlayers,
                                 args.cosTheta, args.kd, args.chil,
                                 args.heightf);

        compare_light_profiles(lp, result_sets[setNo], args.nlayers);
    }
}


/*
// Doesn't test.  Just surveys some sample results.
TEST(sunMLTest, survey) {

    double Idir = 1;
    double Idiff = 1;
    double LAI = 000.001;
    int nlayers = 4;

    double kd = 1;
    double chil = 1;
    double heightf = 1;


    for (double theta = 0; theta <= PI/2; theta += 0.3) {

        struct Light_profile lp = sunML(Idir, Idiff, LAI, nlayers, cos(theta), kd, chil, heightf);

        printf("theta: %f\n\n", theta);
        printf("%15s%15s%15s%15s%15s%15s%15s\n\n", "layer", "direct_irrad", "diffuse_irrad", "total_irrad", "sunlit_frac", "shaded_frac", "height");

        for (int i = 0; i < nlayers; ++i) {
            printf("%15i%15f%15f%15f%15f%15f%15f\n", i, lp.direct_irradiance[i], lp.diffuse_irradiance[i], lp.total_irradiance[i], lp.sunlit_fraction[i], lp.shaded_fraction[i], lp.height[i]);
        }
        printf("\n");
    }
}



// helper function for generating test sets
void print_Light_profile(Light_profile lp, int nlayers) {
    cout << "{{";
    for (int i = 0; i < nlayers; ++i) {
        cout << lp.direct_irradiance[i];
        if (i < nlayers - 1) {
            cout << ", ";
        }
    }
    cout << "}," << endl;

    cout << "{";
    for (int i = 0; i < nlayers; ++i) {
        cout << lp.diffuse_irradiance[i];
        if (i < nlayers - 1) {
            cout << ", ";
        }
    }
    cout << "}," << endl;

    cout << "{";
    for (int i = 0; i < nlayers; ++i) {
        cout << lp.total_irradiance[i];
        if (i < nlayers - 1) {
            cout << ", ";
        }
    }
    cout << "}," << endl;

    cout << "{";
    for (int i = 0; i < nlayers; ++i) {
        cout << lp.sunlit_fraction[i];
        if (i < nlayers - 1) {
            cout << ", ";
        }
    }
    cout << "}," << endl;

    cout << "{";
    for (int i = 0; i < nlayers; ++i) {
        cout << lp.shaded_fraction[i];
        if (i < nlayers - 1) {
            cout << ", ";
        }
    }
    cout << "}," << endl;

    cout << "{";
    for (int i = 0; i < nlayers; ++i) {
        cout << lp.height[i];
        if (i < nlayers - 1) {
            cout << ", ";
        }
    }
    cout << "}}" << endl;

}

// for generating test sets
TEST(sunMLTest, Survey2) {
    sunMLArgs args = sunMLArgs();
    for (int i = 0; i < 2; ++i) {
        args.generate_values();
        Light_profile lp = sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf);
        args.print_heading();
        args.print();
        print_Light_profile(lp, args.nlayers);
    }
}
*/

///////////////// WINDprof /////////////////

/*
// For generating sample data only.
TEST(WINDprof, survey) {
  Rand_double wind_speed_generator { 0, 100 };
  Rand_double LAI_generator { 0, 1 };
  Rand_int nlayers_generator { 1, 20 };

  
 
 for (int i = 0; i < 10; ++i) {
    auto WindSpeed = wind_speed_generator();
    auto LAI = LAI_generator();
    auto nlayers = nlayers_generator();
    cout << setprecision(10) << "{" << WindSpeed << ", " << LAI << ", " << nlayers << "}" << endl;
    double wind_speed_profile[nlayers];
    WINDprof(WindSpeed, LAI, nlayers, wind_speed_profile);
    cout << "{";
    for (int layer = 0; layer < nlayers; ++ layer) {
      cout << wind_speed_profile[layer];
      if (layer < nlayers - 1) {
        cout << ", ";
      }
    }
    cout << "}" << endl;
  }
}
*/

// sample data
constexpr auto num_trials = 5;

// input
double WINDprof_args[][3] = {
  {8.196468057, 0.08196468057, 18},
  {64.72380617, 0.6472380617, 20},
  {40.70143633, 0.4070143633, 15},
  {95.59181556, 0.9559181556, 17},
  {71.55012775, 0.7155012775, 14}
};

// output
double WINDprof_results[][MAXLAY] = {
  {8.196468057, 8.170383284, 8.144381525, 8.118462514, 8.092625989, 8.066871687,
   8.041199347, 8.015608708, 7.990099509, 7.964671492, 7.939324398, 7.914057969,
   7.88887195, 7.863766083, 7.838740114, 7.81379379, 7.788926855, 7.764139058},
  
  {64.72380617, 63.27407886, 61.85682352, 60.47131282, 59.11683572, 57.7926971,
   56.49821743, 55.23273239, 53.99559253, 52.78616297, 51.60382302, 50.44796591,
   49.31799847, 48.21334079, 47.13342599, 46.07769984, 45.04562055, 44.03665847,
   43.0502958, 42.08602634},
  
  {40.70143633, 39.93564881, 39.18426939, 38.447027, 37.72365563, 37.01389432,
   36.317487, 35.63418241, 34.96373402, 34.30589996, 33.66044289, 33.02712993,
   32.40573259, 31.7960267, 31.19779227},
  
  {95.59181556, 91.90228281, 88.35515402, 84.94493284, 81.66633509, 78.51428054,
   75.48388503, 72.57045291, 69.76946979, 67.07659549, 64.48765737, 61.99864383,
   59.6056981, 57.30511229, 55.09332159, 52.96689882, 50.92254903},
  
  {71.55012775, 69.0356631, 66.60956353, 64.26872364, 62.01014719, 59.83094321,
   57.72832234, 55.69959325, 53.74215917, 51.85351461, 50.03124212, 48.27300921,
   46.57656534, 44.93973909}
};

TEST(WINDprofTest, miscellaneous_test_data) {
  for (int i = 0; i < num_trials; ++i) {
    double arg_set[3];
    for (int j = 0; j < 3; ++j) {
      arg_set[j] = WINDprof_args[i][j];
    }
    int nlayers = arg_set[2];
    double wind_speed_profile[nlayers];

    WINDprof(arg_set[0], arg_set[1], arg_set[2], wind_speed_profile);

    for (int layer = 0; layer < nlayers; ++layer) {
      EXPECT_NEAR(wind_speed_profile[layer], WINDprof_results[i][layer], 1E-5);
    }
  }
}

///////////////// RHprof /////////////////


// For generating sample data only.
TEST(RHprof, survey) {
  Rand_double RH_generator { 0, 1 };
  Rand_int nlayers_generator { 1, 20 };
 
 for (int i = 0; i < 5; ++i) {
    auto RH = RH_generator();
    auto nlayers = nlayers_generator();
    cout << setprecision(10) << "{" << RH << ", " << nlayers << "}" << endl;
    double relative_humidity_profile[nlayers];
    RHprof(RH, nlayers, relative_humidity_profile);
    cout << "{";
    for (int layer = 0; layer < nlayers; ++ layer) {
      cout << relative_humidity_profile[layer];
      if (layer < nlayers - 1) {
        cout << ", ";
      }
    }
    cout << "}" << endl;
  }
}


// sample data

// input
double RHprof_args[][2] = {
    {0.6578684265, 19},
    {0.3867285004, 10},
    {0.8407413091, 1},
    {0.2415659511, 4},
    {0.3329553391, 3}
};

// output
double RHprof_results[][MAXLAY] = {
    {0.6698219137, 0.681992596, 0.69438442, 0.7070014037, 0.7198476384,
     0.7329272895, 0.7462445982, 0.7598038828, 0.7736095399, 0.7876660463,
     0.8019779597, 0.8165499209, 0.8313866551, 0.8464929732, 0.8618737735,
     0.8775340433, 0.8934788607, 0.9097133958, 0.9262429128},

    {0.4111878012, 0.437194072, 0.4648451537, 0.4942450749, 0.5255044441,
     0.5587408652, 0.5940793802, 0.6316529396, 0.6716029026, 0.7140795688},

    {0.9858879473},

    {0.2919991927, 0.352961699, 0.4266517309, 0.5157264938},

    {0.4158632934, 0.5194158449, 0.6487536269}
};

TEST(RHprofTest, miscellaneous_test_data) {
  for (int i = 0; i < num_trials; ++i) {
    double arg_set[2];
    for (int j = 0; j < 2; ++j) {
      arg_set[j] = RHprof_args[i][j];
    }
    int nlayers = arg_set[1];
    double relative_humidity_profile[nlayers];

    RHprof(arg_set[0], arg_set[1], relative_humidity_profile);

    for (int layer = 0; layer < nlayers; ++layer) {
      EXPECT_NEAR(relative_humidity_profile[layer], RHprof_results[i][layer], 1E-5);
    }
  }
}
