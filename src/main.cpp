/*
 * Connor Kuehl
 * connorkuehl95@gmail.com
 *
 * This program is a Chip8 Emulator (though I think 'interpreter' is
 * the more technically correct term based on what I've read. Using the
 * Chip8 object, this program will load Chip8 programs and execute their
 * instructions.
 *
 * main.cpp is the entry point for the Chip8 interpreter. It will perform
 * a cursory error check to make sure the appropriate number of command line
 * arguments are present and then hands control over to a Chip8 object.
 */

#include "Chip8.h"
#include "error.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
        abortChip8("Usage is chip8 <path_to_ROM>");

    Chip8 chip8;
    chip8.loadROM(argv[1]);
    chip8.play();

    return 0;
}
