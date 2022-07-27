CXX		:= g++
LD 		:= g++
CXXFLAGS:= -I./ -O3
#VPATH 	= .:device:profiler:scheduler:scheduler/strategies:simulation:tasks:workers
SOURCES = $(wildcard *.cpp) $(wildcard **/*.cpp) $(wildcard scheduler/**/*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

simulator := engine_simulator

$(simulator): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(simulator) $(OBJECTS)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(OBJECTS:.o=.d)

.PHONY : clean
clean :
	-rm $(simulator) $(OBJECTS) $(OBJECTS:.o=.d)
