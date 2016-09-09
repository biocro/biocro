CXXFLAGS=-I/Library/Frameworks/R.framework/Resources/include/
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
#LDFLAGS+=-L/usr/local/lib


OBJ= ../src/AuxBioCro.o Random.o ../src/c4photo.o OldAuxBioCroFunctions.o



test: AuxBioCro-tests.o test-runner.o $(OBJ)
	$(CXX) -v $(LDFLAGS) AuxBioCro-tests.o $(OBJ) test-runner.o -o test -lgtest

AuxBioCro-tests.o: Random.h

Random.o: Random.h

clean:
	$(RM) test *.o

