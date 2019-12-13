CC=g++
CC_FLAGS = -std=c++0x -I"../boost_1_71_0"

# File names
excludes = $(addprefix module_library/, big_leaf_multilayer_canopy.cpp collatz_leaf.cpp)
MODULE_SOURCES = $(filter-out $(excludes), $(wildcard module_library/*.cpp))
SOURCES = module_factory.cpp module_wrapper.cpp module_factory_test.cpp system_helper_functions.cpp modules.cpp $(MODULE_SOURCES)
OBJECTS = $(SOURCES:.cpp=.o)

full: $(OBJECTS)
	$(CC) $(OBJECTS) -o module_factory_test

%.o: %.cpp
	$(CC) -MMD -MP -c $(CC_FLAGS) $< -o $@

module_library/%.o: module_library/%.cpp
	$(CC) -MMD -MP -c $(CC_FLAGS) $< -o $@

.PHONY: quick full

clean:
	rm -f $(OBJECTS)

