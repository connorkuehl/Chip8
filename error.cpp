#include "error.h"
#include "Chip8.h"
#include <iostream>

void abortChip8(const std::string& msg)
{
    using std::cerr;

    cerr << PROG_NAME << " ERROR: " << msg << "\nExiting.\n"; 
    exit(-1);
}
