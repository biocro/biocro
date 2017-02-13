CXXFLAGS=-I/Library/Frameworks/R.framework/Resources/include -I/usr/local/include
CXXFLAGS+=-std=c++11
CXXFLAGS+=-Wunused-variable

LDFLAGS=
# commented out what seems not to be needed:
#LDFLAGS+=-std=c++11
#LDFLAGS+=-Wl,-headerpad_max_install_names
LDFLAGS+=-undefined dynamic_lookup
#LDFLAGS+=-single_module
#LDFLAGS+=-multiply_defined suppress
#LDFLAGS+=-L/Library/Frameworks/R.framework/Resources/lib
LDFLAGS+=-L/usr/local/lib

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
	$(RM) test *.o */*.o
