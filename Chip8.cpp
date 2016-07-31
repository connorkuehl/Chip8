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
{
    std::srand(0);
}

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
        abortChip8("Failed to open \"" + romFile + "\"");
}

void Chip8::runCycle()
{
    // Load the two byte quantity for decoding
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Isolate highest 4 bits which contain opcode
    switch (opcode & 0xF000)
    {
        /*
         * Special case: three opcodes start with 0x0 as highest 4 bits, but are distinguishible
         *               by the last 4 bits (i.e, 0NNN (syscall), 00E0(clear screen), 00EE(ret)
         *                                           ^               ^                   ^
         */
        case 0x0000:
            switch (opcode & 0x000F)
            {
                // 0x0NNN - unused, this is a chip8 system call
                // 0x00E0 - clears the screen
                case 0x0000: 
                    break;
                // 0x00EE - RET from a function call
                case 0x000E: 
                    pc = stack[sp--];
                    break;
                default: // TODO error handle bad opcode
                    break;
            }
            break;
        // 0x1NNN - JMP to address at address `NNN`
        case 0x1000:
            pc = opcode & 0x0FFF;
            break;
        // 0x2NNN - CALL subroutine at address `NNN`
        case 0x2000:
            if (sp == 15)
                abortChip8("Stack overflow.");
            stack[++sp] = pc;
            pc = opcode & 0x0FFF;
            break;
        // 0x3XNN - SKIP next instruction if VX == `NN`
        case 0x3000:
            if (REG_X == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        // 0x4XNN - SKIP next instruction if VX != `NN`
        case 0x4000:
            if (REG_X != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        // 0x5XY0 - SKIP next instruction if VX == VY
        case 0x5000:
            if (REG_X != REG_Y)
                pc += 4;
            else
                pc += 2;
            break;
        // 0x6XNN - SET VX = `NN`
        case 0x6000:
            REG_X = opcode & 0x00FF;
            pc += 2;
            break;
        // 0x7XNN - SET VX += `NN`
        case 0x7000:
            REG_X += opcode & 0x00FF;
            pc += 2;
            break;
        /*
         * Special case: multiple opcodes start with 0x8 as highest 4 bits
         */
        case 0x8000:
            switch (opcode & 0x000F)
            {
                // 0x8XY0 - SET VX = VY
                case 0x0000:
                    REG_X = REG_Y;
                    pc += 2;
                    break;
                // 0x8XY1 - SET VX = VX | VY
                case 0x0001:
                    REG_X = REG_X | REG_Y;
                    pc += 2;
                    break;
                // 0x8XY2 - SET VX = VX & VY
                case 0x0002:
                    REG_X = REG_X & REG_Y;
                    pc += 2;
                    break;
                // 0x8XY3 - SET VX = VX ^ VY
                case 0x0003:
                    REG_X = REG_X ^ REG_Y;
                    pc += 2;
                    break;
                // 0x8XY4 - SET VX += VY (VF is set to 1 if there's a carry, 0 if not)
                case 0x0004:
                    if (REG_Y > 0xFF - REG_X)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    REG_X += REG_Y;
                    pc += 2;
                    break;
                // 0x8XY5 - SET VX -= VY (VF is set to 0 if there's a borrow, 1 if not)
                case 0x0005:
                    if (REG_X > REG_Y)
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    REG_X -= REG_Y;
                    pc += 2;
                    break;
                // 0x8XY6 - SET VX = VX >> 1 (VF is LSB of VX prior to shift)
                case 0x0006:
                    V[0xF] = opcode & 0x0001;
                    REG_X = REG_X >> 1;
                    pc += 2;
                    break;
                // 0x8XY7 - SET VX = VY - VX (VF is set to 0 if there's a borrow, 1 if not)
                case 0x0007:
                    if (REG_Y > REG_X)
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    REG_X = REG_Y - REG_X;
                    pc += 2;
                    break;
                // 0x8XYE - SET VX = VX << 1 (VF is MSB of VX prior to shift)
                case 0x000E:
                    V[0xF] = opcode & 0x0001;
                    REG_X = REG_X << 1;
                    pc += 2;
                    break;
                default: //TODO error handle bad opcode
                    break;
            }
            break;
        // 0x9XY0 - SKIP next instruction if VX != VY
        case 0x9000:
            if (REG_X != REG_Y)
                pc += 4;
            else
                pc += 2;
            break;
        // 0xANNN - SET I = `NNN`
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        // 0xBNNN - JMP to address `NNN` + V0
        case 0xB000:
            pc = (opcode & 0x0FFF) + V[0];
            break;
        // 0xCXNN - SET VX = randomNum & NN
        case 0xC000:
            REG_X = (std::rand() % 0xFF) & (opcode & 0x00FF);
            pc += 2;
            break;
        // 0xDXYN - Draw sprite at coord (VX, VY) with a width of 8 pixels and height of N pixels
        //          (VF is set to 1 if any screen pixels are flipped from set to unset when the
        //          sprite is drawn and 0 if that does not happen.
        case 0xD000:
            break;
        /*
         * Special case: multiple opcodes start with 0xE as highest 4 bits
         */
        case 0xE000:
            switch (opcode & 0x000F)
            {
                // 0xEX9E - SKIP next instruction if key stored in VX is pressed
                case 0x000E:
                    break;
                // 0xEXA1 - SKIP next instruction if key stored in VX ISN'T pressed
                case 0x0001:
                    break;
                default: // TODO error handling for bad opcode
                    break;
            }
            break;
        /*
         * Special case: multiple opcodes start with 0xF as highest 4 bits
         */
        case 0xF000:
            switch (opcode & 0x000F)
            {
                // 0xFX07 - SET VX = delayTimer
                case 0x0007:
                    REG_X = delayTimer;
                    pc += 2;
                    break;
                // 0xFX0A - Wait for keypress, then store it in VX
                case 0x000A:
                    break;
                // Sub special case: multiple opcodes ending with 0x
                case 0x0005:
                    switch (opcode & 0x00F0)
                    {
                        // 0xFX15 - SET delay timer to VX
                        case 0x0010:
                            delayTimer = REG_X;
                            pc += 2;
                            break;
                        // 0xFX55 - Stores V0 through VX in memory starting at address I
                        case 0x0050:
                            break;
                        // 0xFX65 - Fills V0 through VX with vaues in memory starting at addr I
                        case 0x0060:
                            break;
                        default: // TODO error handling for bad opcode
                            break;
                    }
                    break;
                // 0xFX18 - SET sound timer to VX
                case 0x0008:
                    soundTimer = REG_X;
                    pc += 2;
                    break;
                // 0xFX1E - SET I += VX
                case 0x000E:
                    I += REG_X;
                    pc += 2;
                    break;
                // 0xFX29 - SET I to location of sprite in VX, 0-F in hex are a 4x5 font
                case 0x0009:
                    break;
                // 0xFX33 - Store decimal parts of VX - I = hundreds, I+1 = tens, I+2 = ones
                case 0x0003:
                    int hundreds = REG_X / 100;
                    int tens = (REG_X - (100 * hundreds)) / 10;
                    int ones = ((REG_X - (100 * hundreds)) - (10 * tens));

                    memory[I] = hundreds;
                    memory[I + 1] = tens;
                    memory[I + 2] = ones;
                    pc += 2;
                    break;
            }
            break;
        default: // TODO error handling for bad opcode
            break;
    }

    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0)
    {
        if (soundTimer == 1)
            // TODO PLAY SOUND
        --soundTimer;
    }
}

void Chip8::draw()
{}

void Chip8::interact()
{}
