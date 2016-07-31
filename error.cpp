#include "error.h"
#include "Chip8.h"
#include <iostream>

void abortChip8(const std::string& msg)
{
    using std::cerr;
    printChip8Error(msg);
    cerr << "\nExiting.\n"; 
    exit(-1);
}

void printChip8Error(const std::string& msg)
{
    using std::cerr;
    cerr << PROG_NAME << " ERROR: " << msg << ".\n";
}
