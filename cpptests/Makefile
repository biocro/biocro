CXXFLAGS=-I/usr/local/include
CXXFLAGS+=-std=c++11
CXXFLAGS+=-Wunused-variable

LDFLAGS=
LDFLAGS+=-undefined dynamic_lookup
LDFLAGS+=-L/usr/local/lib


CXXFLAGS+=-DMATHLIB_STANDALONE # needed for using Rmath library outside of R
CXXFLAGS+=-I/usr/local/R/include # adjust as needed to include R libraries; or use a symlink if needed
LDFLAGS+=-L/usr/local/R/lib # adjust as needed or add symlinks; also, add this path to DYLD_FALLBACK_LIBARARY_PATH
LDFLAGS+=-lRmath # needed for, e.g., dpois
# See
# https://cran.r-project.org/doc/manuals/r-release/R-admin.html#The-standalone-Rmath-library
# for more information about using the Rmath library outside of R

TESTED_CODE_OBJ = ../src/AuxBioCro.o ../src/c4photo.o
TEST_UTILITY_OBJ = Random.o relative_error.o AuxBioCroTests/common_generators.o
TEST_CODE_OBJ = AuxBioCroTests/OldAuxBioCroFunctions.o \
    test-runner.o \
    AuxBioCroTests/lightME-test.o \
    AuxBioCroTests/sunML-test.o \
    AuxBioCroTests/EvapoTrans-test.o \
    AuxBioCroTests/WINDprof-test.o \
    AuxBioCroTests/temperature_functions-test.o \
    AuxBioCroTests/RHprof-test.o \
    AuxBioCroTests/watstr-test.o \
    AuxBioCroTests/soilTchoose-test.o \
    AuxBioCroTests/soilML-test.o

OBJ = $(TESTED_CODE_OBJ) $(TEST_UTILITY_OBJ) $(TEST_CODE_OBJ)


test: $(OBJ)
	$(CXX) -v $(LDFLAGS) $(OBJ) -o test -lgtest

AuxBioCroTests/AuxBioCro-tests.o: Random.h

Random.o: Random.h

relative_error.o: relative_error.h

clean:
	$(RM) test *.o */*.o ../src/*.o
