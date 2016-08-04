/*
 * Connor Kuehl
 * connorkuehl95@gmail.com
 *
 * This program is a Chip8 Emulator (though I think 'interpreter' is
 * the more technically correct term based on what I've read. Using the
 * Chip8 object, this program will load Chip8 programs and execute their
 * instructions.
 *
 * Chip8.h contains the class definition for the Chip8 class. It also
 * includes useful global constants and C pre processor macros to
 * improve legibility of the dissection of Chip8 instructions. This
 * header file also contains the HEX representation of the fontset
 * used by the Chip8.
 *
 * I tried to keep this implementation of the Chip8 interpreter as close
 * to the technical specifications in terms of stack size, register and 
 * other variable sizes.
 */

#ifndef CHIP8_H_
#define CHIP8_H_

#include <SDL2/SDL.h>
#include <string>
#include <cstdint>
#include <cstdlib>

#define VX V[(opcode & 0x0F00) >> 8]
#define VY V[(opcode & 0x00F0) >> 4]
#define NNN (opcode & 0x0FFF)
#define KK (opcode & 0x00FF)

static const std::string&   PROG_NAME = "Chip8";
static const int   START_PROG_MEM = 0x200;
static const int   END_PROG_MEM   = 0xFFF;
static const int   X_RES          = 64;
static const int   Y_RES          = 32;
static const int   SCALE          = 10;
static const float REFRESH_RATE   = 1.f/10.f;

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
        ~Chip8();

        void loadROM(const std::string&); // Load a Chip8 ROM file into Program data memory space
        void play();                      // The 'run' loop. 
    private:
        void initVideo();                 // Set up SDL2 systems
        void runCycle();                  // Fetch, decode, and execute opcode
        void draw();                      // Draw to the screen
        void interact();                  // Keyboard state and user input

        uint16_t    opcode;
        uint16_t    I;                    // Address Register
        uint16_t    pc;                   // Program Counter, program space: 0x200 - 0xFFF
        uint8_t     sp;                   // Stack Pointer
        uint16_t    stack[16];            // Program Stack
        uint8_t     V[16];                // Chip8 has 16 8-bit registers
        uint8_t     memory[4096];         // RAM
        uint8_t     pixels[X_RES*Y_RES];  // Makes up the drawable screen
        uint8_t     delayTimer;           // Refresh rate of the screen
        uint8_t     soundTimer;           // Play a sound after counting down from 60
        uint8_t     key[16];              // Key press, Chip8 keyboard is 0x0 - 0xF
        bool        updatedPixels;        // Flag, if true we need to redraw the pixels
        bool        running;              // Used to determine if the machine is on and running
        std::string currentROM;
        /* GRAPHICS */
        SDL_Window* window;               // To display a window
        SDL_Renderer* renderer;           // To render color and the texture that holds pixels
};

#endif

