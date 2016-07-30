/*
 * Connor Kuehl
 * connorkuehl95@gmail.com
 *
 * TODO Document this file
 */

#include "Chip8.h"
#include "error.h"
#include <fstream>

Chip8::Chip8() : opcode(0), I(0), pc(START_PROG_MEM), sp(0), stack{0}, V{0}, memory{0}, pixels{0}, delayTimer(0), soundTimer(0), key{0}
{}

void Chip8::loadROM(const std::string& romFile)
{
    using std::ifstream;

    ifstream fin(romFile, std::ios::binary);

    // Read the ROM into memory, stopping when end of file or end of program space is reached
    if (fin.is_open())
    {
        fin.peek();
        for (int i = 0; !fin.eof() && (i + START_PROG_MEM < END_PROG_MEM); ++i)
            memory[i + START_PROG_MEM] = fin.get();
        fin.close();
    }
    else
        abortChip8("Failed to open " + romFile);
}

void Chip8::runCycle()
{}

void Chip8::draw()
{}

void Chip8::interact()
{}
