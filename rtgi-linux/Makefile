TARGET = rtgi

CXXFLAGS = -Wall -g -std=c++11 -MMD -I. -I../header -DGLM_FORCE_RADIANS
LFLAGS = -L./lib -Wl,-rpath,./rtgi-linux/lib/
LIBRARY = -lglfw -lGL
# -lm -lGL

SOURCES := $(wildcard ../src/*.cpp)
OBJECTS := $(addprefix obj/,$(notdir $(SOURCES:.cpp=.o)))
rm = rm -f

$(TARGET): $(OBJECTS)
	@g++ -o $@ $^ $(LFLAGS) $(LIBRARY)
	@echo "Linking complete!"

obj/%.o : ../src/%.cpp
	@g++ $(CXXFLAGS) -c -o $@ $<
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONEY: remove
remove: clean
	@$(rm) $(TARGET)
	@echo "Executable removed!"

.PHONEY: run
run : $(TARGET)
	@(cd .. && exec ./rtgi-linux/$(TARGET))

-include $(OBJECTS:.o:.d)
