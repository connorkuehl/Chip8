/*
 * Connor Kuehl
 * connorkuehl95@gmail.com
 *
 * TODO Document this file
 */

#ifndef CHIP8_H_
#define CHIP8_H_

#include <SDL2/SDL.h>
#include <string>
#include <cstdint>
#include <cstdlib>

#define PROG_NAME  "Chip8"
#define REG_X V[(opcode & 0x0F00) >> 8]
#define REG_Y V[(opcode & 0x00F0) >> 4]
#define START_PROG_MEM   0x200
#define END_PROG_MEM     0xFFF

static uint8_t chip8Font[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8
{
    public:
        Chip8();

        void loadROM(const std::string&);  // Load a Chip8 ROM file into Program data memory space
        void runCycle();            // Fetch, decode, and execute opcode
        void draw();                // Draw to the screen
        void interact();            // Keyboard state and user input
    private:
        uint16_t    opcode;
        uint16_t    I;              // Address Register
        uint16_t    pc;             // Program Counter, program space: 0x200 - 0xFFF
        uint16_t    sp;             // Stack Pointer
        uint16_t    stack[16];      // Program Stack
        uint8_t     V[16];          // Chip8 has 16 8-bit registers
        uint8_t     memory[4096];   // RAM
        uint8_t     pixels[64*32];  // Makes up the drawable screen
        uint8_t     delayTimer;     // Refresh rate of the screen
        uint8_t     soundTimer;     // Play a sound after counting down from 60
        uint8_t     key[16];        // Key press, Chip8 keyboard is 0x0 - 0xF
};

#endif

