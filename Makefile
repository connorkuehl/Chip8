CC = clang++
CXXFLAGS= -std=c++11 -c
LINKER = $(CC) -std=c++11
LDFLAGS = -lSDL2
OBJECTS = main.o Chip8.o error.o
EXECUTABLE = chip8

all: $(EXECUTABLE)

debug: CXXFLAGS += -g -Wall
debug: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINKER) $(OBJECTS) $(LDFLAGS) -o	$(EXECUTABLE)

main.o: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS)	main.cpp

Chip8.o: Chip8.cpp Chip8.h
	$(CC) $(CXXFLAGS) $(LDFLAGS)	Chip8.cpp

error.o: error.cpp error.h
	$(CC) $(CXXFLAGS) $(LDFLAGS)	error.cpp

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
