#include <cmath> // trig functions
#include <cfloat> // limits
#include <gtest/gtest.h> // test framework

#include "../Random.h" // custom random number generators
#include "../relative_error.h" // custom assert/expect functions

#include "OldAuxBioCroFunctions.h" // copies of old function definitions from AuxBioCro.cpp

#include "../../src/BioCro.h" // functions we're testing


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

    double verySmallCosTheta;

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

        static Rand_double rd2 {-1, 1};
        cosTheta = rd2();

        static Rand_double rd3 {-1E-10, 1E-10};
        verySmallCosTheta = rd3();

        kd = rd();
        chil = 10 * rd();
        heightf = 100 * rd() + 1;
    }
};

// Helper for
void compare_light_profiles(Light_profile lp_1, Light_profile lp_2, int nlayers,
                            double absolute_tolerance = 0,
                            double relative_tolerance = 1E-9) {
    for (int layer = 0; layer < nlayers; ++layer) {
        expect_near_rel(lp_1.direct_irradiance[layer],
                        lp_2.direct_irradiance[layer],
                        absolute_tolerance,
                        1E-5, // Allow greater relative difference here
                        "\nTESTING direct_irradiance");
        expect_near_rel(lp_1.diffuse_irradiance[layer],
                        lp_2.diffuse_irradiance[layer],
                        absolute_tolerance,
                        relative_tolerance,
                        "\nTESTING diffuse_irradiance");
        expect_near_rel(lp_1.total_irradiance[layer],
                        lp_2.total_irradiance[layer],
                        1E-5, // Values near zero; allow positive absolute difference.
                        relative_tolerance,
                        "\nTESTING total_irradiance");
        expect_near_rel(lp_1.sunlit_fraction[layer],
                        lp_2.sunlit_fraction[layer],
                        DBL_MIN,
                        1E-7,
                        "\nTESTING sunlit_fraction");
        expect_near_rel(lp_1.shaded_fraction[layer],
                        lp_2.shaded_fraction[layer],
                        absolute_tolerance,
                        relative_tolerance,
                        "\nTESTING shaded_fraction");
        expect_near_rel(lp_1.height[layer],
                        lp_2.height[layer],
                        absolute_tolerance,
                        relative_tolerance,
                        "\nTESTING height[layer]");
    }
}

// Check that an out_of_range exception is thrown if cosTheta is not in [-1, 1].
TEST(sunMLTest, CheckCosThetaRange) {
    sunMLArgs args = sunMLArgs();
    for (int i = 0; i < 5; ++i) {
        args.generate_values();
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta = 1;
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta = 0;
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta = -1;
        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf));

        args.cosTheta = 1 + DBL_EPSILON;
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);

        args.cosTheta = -1 - DBL_EPSILON;
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, args.heightf), std::out_of_range);
    }
}


// Test that if the other arguments are valid, we get an out_of_range exception
// if and only if nlayers is < 1 or > MAXLAY.
TEST(sunMLTest, LayerSizeLimit) {
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

// Test that if the other arguments are valid, we get an out_of_range exception
// if and only if kd_range is < 0 or > 1.
TEST(sunMLTest, kdRange) {
    constexpr auto tries = 5;

    sunMLArgs args = sunMLArgs();

    for (int i = 0; i < tries; ++i) {
        args.generate_values();

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, 0, args.chil, args.heightf));
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, 0 - DBL_MIN, args.chil, args.heightf), std::out_of_range);

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, 1, args.chil, args.heightf));
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, 1 + DBL_EPSILON, args.chil, args.heightf), std::out_of_range);
    }
}

// Test that if the other arguments are valid, we get an out_of_range exception
// if and only if chil is < 0.
TEST(sunMLTest, chilRange) {
    constexpr auto tries = 5;

    sunMLArgs args = sunMLArgs();

    for (int i = 0; i < tries; ++i) {
        args.generate_values();

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, 0, args.heightf));
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, 0 - DBL_MIN, args.heightf), std::out_of_range);
    }
}

// Test that if the other arguments are valid, we get an out_of_range exception
// if and only if heightf is <= 0.
TEST(sunMLTest, heightfRange) {
    constexpr auto tries = 5;

    sunMLArgs args = sunMLArgs();

    for (int i = 0; i < tries; ++i) {
        args.generate_values();

        ASSERT_NO_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, DBL_MIN));
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, 0), std::out_of_range);
        ASSERT_THROW(sunML(args.Idir, args.Idiff, args.LAI, args.nlayers, args.cosTheta, args.kd, args.chil, 0 - DBL_MIN), std::out_of_range);
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
TEST(sunMLTest, MiscellaneousTestData) {
    for (int setNo = 0; setNo < no_of_test_sets; ++setNo) {
        sunMLArgs args = test_sets[setNo];

        Light_profile lp = sunML(args.Idir, args.Idiff, args.LAI, args.nlayers,
                                 args.cosTheta, args.kd, args.chil,
                                 args.heightf);

        compare_light_profiles(lp, result_sets[setNo], args.nlayers, 1E-5);
    }
}

// Regression test on miscellaneous random input generated on the fly; check
// that the output doesn't change.
TEST(sunMLTest, MiscellaneousTestData2) {
    sunMLArgs args = sunMLArgs();
    for (int i = 0; i < 1E4; ++i) {
        args.generate_values();

        Light_profile Oldlp = OldsunML(args.Idir,

                                       args.Idiff, args.LAI,
                                       args.nlayers,

                                       // If cosTheta <= 0, we want the revised
                                       // function's results to match the old
                                       // functions's results when a small
                                       // positive number is substituted for
                                       // values of cosTheta <= 0.  (NOTE: If we
                                       // use DBL_MIN in place of 1E-10, we get
                                       // results for the direct_irradiance that
                                       // are further from the mathematical
                                       // limit as we approach 0 from the
                                       // right!)
                                       fmax(1E-10, args.cosTheta),

                                       args.kd,
                                       args.chil, args.heightf);
        Light_profile lp = sunML(args.Idir, args.Idiff, args.LAI, args.nlayers,
                                 args.cosTheta, args.kd, args.chil,
                                 args.heightf);

        compare_light_profiles(lp, Oldlp, args.nlayers);

        // extra testing for values of cosTheta near zero:

        Light_profile Oldlp2 = OldsunML(args.Idir, args.Idiff, args.LAI, args.nlayers,

                                        fmax(1E-10, args.verySmallCosTheta),

                                        args.kd, args.chil,
                                        args.heightf);

        Light_profile lp2 = sunML(args.Idir, args.Idiff, args.LAI, args.nlayers,
                                  args.verySmallCosTheta, args.kd, args.chil,
                                  args.heightf);

        compare_light_profiles(lp2, Oldlp2, args.nlayers);
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
