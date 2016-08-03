# Recommended settings per the GNU Make manual
SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .cpp .o

# USER PREFERENCES -- feel free to change the compiler (i.e. g++)
CC = clang++

# REQUIRED BY PROJECT -- only change if you know what you're doing
EXECUTABLE = chip8
CFLAGS = -std=c++11 -g
ALL_FLAGS = -I. $(CFLAGS)
LDFLAGS = -lSDL2
SOURCES = main.cpp Chip8.cpp error.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# BUILD
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(ALL_FLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS) 

%.o: %.cpp
	$(CC) $(ALL_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
