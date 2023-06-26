# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Source files
SRCS = flood_it2.cpp main2.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HDRS = flood_it2.h

# Executable name
TARGET = flood

# Default target
all: $(TARGET)

# Linking object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compiling source files into object files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Clean all generated files, including backups and editor-specific files
purge: clean
	rm -f *~ .*~ #* *.swp

.PHONY: all clean purge
