CC = clang++
CXXFLAGS= -std=c++11 -c
LINKER = $(CC) -std=c++11
OBJECTS = main.o Chip8.o error.o
EXECUTABLE = chip8

all: $(EXECUTABLE)

debug: CXXFLAGS += -g -Wall
debug: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LINKER) $(OBJECTS) -o	$(EXECUTABLE)

main.o: main.cpp
	$(CC) $(CXXFLAGS)	main.cpp

Chip8.o: Chip8.cpp Chip8.h
	$(CC) $(CXXFLAGS)	Chip8.cpp

error.o: error.cpp error.h
	$(CC) $(CXXFLAGS)	error.cpp

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)
