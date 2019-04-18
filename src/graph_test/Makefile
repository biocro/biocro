CXX=g++
BIOCRO_INCLUDE = ".."

LDFLAGS = -L$(BIOCRO_INCLUDE) -lbiocro

CXXFLAGS = -std=c++11 -g -I$(BIOCRO_INCLUDE)

SOURCES = main.cpp ../dependency_graph.cpp
OBJECTS = $(SOURCES:.cpp=.o)
graph_test: $(OBJECTS)
	$(CXX)  $^ $(LDFLAGS) -o $@

# To obtain object files
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
