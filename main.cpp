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
