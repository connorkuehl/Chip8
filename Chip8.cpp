/*
 * Connor Kuehl
 * connorkuehl95@gmail.com
 *
 * TODO Document this file
 */

#include "Chip8.h"
#include "error.h"
#include <fstream>
#include <iostream>

Chip8::Chip8() : opcode(0), I(0), pc(START_PROG_MEM), sp(0), stack{0}, V{0}, memory{0}, pixels{0}, delayTimer(0), soundTimer(0), key{0}, updatedPixels(true), running(true), window(nullptr), renderer(nullptr)
{
    // load font set into memory
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8Font[i];

    std::srand(0);
}

Chip8::~Chip8()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

void Chip8::loadROM(const std::string& romFile)
{
    using std::ifstream;

    ifstream fin(romFile, std::ios::binary);

    // Read the ROM into memory, stopping when end of file or end of program space is reached
    if (fin.is_open())
    {
        currentROM = romFile;
        fin.peek();
        for (int i = 0; !fin.eof() && (i + START_PROG_MEM < END_PROG_MEM); ++i)
            memory[i + START_PROG_MEM] = fin.get();

        if (fin.peek() != EOF)
            abortChip8("ROM is too large for program memory space.");
        fin.close();
    }
    else
        abortChip8("Failed to open \"" + romFile + "\"");

    initVideo();
}

void Chip8::initVideo()
{
    // set up SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        abortChip8(std::string("SDL2 failed to initialize. . . ") + SDL_GetError());

    // set up window
    std::string winTitle = std::string(PROG_NAME) + " " +  currentROM;
    window = SDL_CreateWindow(winTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (X_RES * SCALE), (Y_RES * SCALE), SDL_WINDOW_SHOWN);
    if (!window)
        abortChip8(std::string("SDL2 failed to create window. . . ") + SDL_GetError());

    //set up renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
        abortChip8(std::string("SDL2 failed to create renderer. . . ") + SDL_GetError());
    SDL_RenderSetScale(renderer, SCALE, SCALE);
}

void Chip8::play()
{
    std::cout << "PC =    ";
    uint32_t lastUpdate = 0, currentTime;
    while (running)
    {
        currentTime = SDL_GetTicks();

        // Only cycle 60 times per second
        if (((currentTime - lastUpdate) / 1000) <= REFRESH_RATE)
            runCycle();
        if (updatedPixels)
            draw();
        interact();

        lastUpdate = currentTime;

        std::cout << "\b\b\b" << pc;
    }
    std::cout << std::endl;
}


void Chip8::runCycle()
{
    if (pc > END_PROG_MEM || pc < START_PROG_MEM)
        abortChip8("Seg fault!");
    // Load the two byte quantity for decoding
    opcode = memory[pc] << 8 | memory[pc + 1];

    // Isolate highest 4 bits which contain opcode
    switch (opcode & 0xF000)
    {
        /*
         * Special case: three opcodes start with 0x0 as highest 4 bits, but are distinguishible
         *               by the last byte (i.e, 0NNN (syscall), 00E0(clear screen), 00EE(ret)
         *                                        ^^              ^^                  ^^
         */
        case 0x0000:
            switch (opcode & 0x00FF)
            {
                // 0x0NNN - unused, this is a chip8 system call
                case 0x0000:
                    printChip8Error("RCA 1802 system call is not supported. :(");
                    pc += 2;
                    break;
                // 0x00E0 - clears the screen
                case 0x00E0:
                    for (int i = 0; i < (X_RES * Y_RES); ++i)
                        pixels[i] = 0;
                    updatedPixels = true;
                    pc += 2;
                    break;
                    // 0x00EE - RET from a function call
                case 0x00EE: 
                    pc = stack[sp--];
                    pc += 2;
                    break;
                default:
                    printChip8Error("Encountered unknown (mangled?) opcode for 0x0. Skipping.");
                    pc += 2;
                    break;
            }
            break;
            // 0x1NNN - JMP to address at address `NNN`
        case 0x1000:
            pc = NNN;
            break;
            // 0x2NNN - CALL subroutine at address `NNN`
        case 0x2000:
            stack[++sp] = pc;
            pc = NNN;
            break;
            // 0x3XKK - SKIP next instruction if VX == `KK`
        case 0x3000:
            if (VX == KK)
                pc += 2;
            pc += 2;
            break;
            // 0x4XKK - SKIP next instruction if VX != `KK`
        case 0x4000:
            if (VX != KK)
                pc += 2;
            pc += 2;
            break;
            // 0x5XY0 - SKIP next instruction if VX == VY
        case 0x5000:
            if (VX == VY)
                pc += 2;
            pc += 2;
            break;
            // 0x6XKK - SET VX = `KK`
        case 0x6000:
            VX = KK;
            pc += 2;
            break;
            // 0x7XKK - SET VX += `KK`
        case 0x7000:
            VX += KK;
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
                    VX = VY;
                    pc += 2;
                    break;
                    // 0x8XY1 - SET VX = VX | VY
                case 0x0001:
                    VX |= VY;
                    pc += 2;
                    break;
                    // 0x8XY2 - SET VX = VX & VY
                case 0x0002:
                    VX &= VY;
                    pc += 2;
                    break;
                    // 0x8XY3 - SET VX = VX ^ VY
                case 0x0003:
                    VX ^= VY;
                    pc += 2;
                    break;
                    // 0x8XY4 - SET VX += VY (VF is set to 1 if there's a carry, 0 if not)
                case 0x0004:
                    {
                        if (VY > 0xFF - VX)
                            V[0xF] = 1;
                        else
                            V[0xF] = 0;

                        uint16_t temp = VX + VY;
                        VX = temp & 0x00FF;
                        pc += 2;
                        break;
                    }
                    // 0x8XY5 - SET VX -= VY (VF is set to 0 if there's a borrow, 1 if not)
                case 0x0005:
                    if (VX > VY)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    VX -= VY;
                    pc += 2;
                    break;
                    // 0x8XY6 - SET VX = VX >> 1 (VF is LSB of VX prior to shift)
                case 0x0006:
                    V[0xF] = VX & 0x1;
                    VX >>= 1;
                    pc += 2;
                    break;
                    // 0x8XY7 - SET VX = VY - VX (VF is set to 0 if there's a borrow, 1 if not)
                case 0x0007:
                    if (VY > VX)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    VX = VY - VX;
                    pc += 2;
                    break;
                    // 0x8XYE - SET VX = VX << 1 (VF is MSB of VX prior to shift)
                case 0x000E:
                    V[0xF] = VX >> 7;
                    VX <<= 1;
                    pc += 2;
                    break;
                default:
                    printChip8Error("Encountered unknown (mangled?) opcode for 0x8. Skipping.");
                    pc += 2;
                    break;
            }
            break;
            // 0x9XY0 - SKIP next instruction if VX != VY
        case 0x9000:
            if (VX != VY)
                pc += 2;
            pc += 2;
            break;
            // 0xANNN - SET I = `NNN`
        case 0xA000:
            I = NNN;
            pc += 2;
            break;
            // 0xBNNN - JMP to address `NNN` + V0
        case 0xB000:
            pc = NNN + V[0];
            break;
            // 0xCXKK - SET VX = randomNum & KK
        case 0xC000:
            VX = (std::rand() % 0xFF) & KK;
            pc += 2;
            break;
            // 0xDXYN - DRW
        case 0xD000:
            {
                uint16_t height = opcode & 0x000F;
                uint16_t pixel;

                V[0xF] = 0;
                for (uint8_t row = 0; row < height; ++row)
                {
                    // load pixel
                    pixel = memory[I + row];
                    for (uint8_t col = 0; col < 8; ++col)
                    {
                        if ((pixel & (0x80 >> col)) != 0)
                        {
                            if (pixels[(VX + col + ((VY + row) * X_RES))] == 1)
                                V[0xF] = 1;
                            pixels[VX + col + ((VY + row) * 64)] ^= 1;
                        }
                    }
                }
                updatedPixels = true;
                pc += 2;
                break;
            }
            /*
             * Special case: multiple opcodes start with 0xE as highest 4 bits
             */
        case 0xE000:
            switch (opcode & 0x00FF)
            {
                // 0xEX9E - SKIP next instruction if key stored in VX is pressed
                case 0x009E: 
                    if (key[VX] != 0)
                        pc += 2;
                    pc += 2;
                    break;
                    // 0xEXA1 - SKIP next instruction if key stored in VX ISN'T pressed
                case 0x00A1:
                    if (key[VX] == 0)
                        pc += 2;
                    pc += 2;
                    break;
                default:
                    printChip8Error("Encountered unknown (mangled?) opcode for 0xE. Skipping.");
                    pc += 2;
                    break;
            }
            break;
            /*
             * Special case: multiple opcodes start with 0xF as highest 4 bits
             */
        case 0xF000:
            switch (opcode & 0x00FF)
            {
                // 0xFX07 - SET VX = delayTimer
                case 0x0007:
                    VX = delayTimer;
                    pc += 2;
                    break;
                    // 0xFX0A - Wait for keypress, then store it in VX
                case 0x000A:  
                    {
                        bool keyPressed = false;
                        for (int i = 0; i < 16; ++i)
                        {
                            if (key[i] != 0)
                            {
                                VX = i;
                                keyPressed = true;
                            }
                        }
                        if (!keyPressed)
                            return;
                        pc += 2;
                        break;
                    }
                // 0xFX15 - SET delay timer to VX
                case 0x0015:
                    delayTimer = VX;
                    pc += 2;
                    break;
                // 0xFX18 - SET sound timer to VX
                case 0x0018:
                    soundTimer = VX;
                    pc += 2;
                    break;
                // 0xFX1E - SET I += VX
                case 0x001E:
                    if (I + VX > 0x0FFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    I += VX;
                    pc += 2;
                    break;
                // 0xFX29 - SET I to location of sprite in VX, 0-F in hex are a 4x5 font
                case 0x0029:
                    I = VX * 0x5;
                    pc += 2;
                    break;
                // 0xFX33 - Store decimal parts of VX - I = hundreds, I+1 = tens, I+2 = ones
                case 0x0033:    
                    {
                        // The value in register X is at MOST 255 (0xFF)
                        // Find out how many full 100s there are
                        int hundreds = VX / 100;
                        // subtract the hundreds, find out how many full 10s there are
                        int tens = (VX - (100 * hundreds)) / 10;
                        // the remainder is the ones
                        int ones = ((VX - (100 * hundreds)) - (10 * tens));

                        memory[I] = hundreds;
                        memory[I + 1] = tens;
                        memory[I + 2] = ones;
                        pc += 2;
                        break;
                    }
                    // 0xFX55 - Stores V0 through VX in memory starting at address I
                    case 0x0055:
                        for (int i = 0; i <= VX; ++i)
                            memory[I + i] = V[i];   // maybe I should change `i` to `k`
                        pc += 2;
                        break; 
                    // 0xFX65 - Fills V0 through VX with values in memory starting at addr I
                    case 0x0065:
                        for (int i = 0; i <= VX; ++i)
                            V[i] = memory[I + i];
                        pc += 2;
                        break;
            }
            break;
        default:
            printChip8Error("Unknown Opcode. Skipping.");
            pc += 2;
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
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int y = 0; y < Y_RES; ++y)
    {
        for (int x = 0; x < X_RES; ++x)
        {
            if (pixels[(y * X_RES) + x] != 0)
                SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    SDL_RenderPresent(renderer);
    updatedPixels = false;
    SDL_Delay(1000 / 60);
}

void Chip8::interact()
{
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_1: key[0x1] = 1;
                                     break;
                        case SDLK_2: key[0x2] = 1;
                                     break;
                        case SDLK_3: key[0x3] = 1;
                                     break;
                        case SDLK_4: key[0xC] = 1;
                                     break;
                        case SDLK_q: key[0x4] = 1;
                                     break;
                        case SDLK_w: key[0x5] = 1;
                                     break;
                        case SDLK_e: key[0x6] = 1;
                                     break;
                        case SDLK_r: key[0xD] = 1;
                                     break;
                        case SDLK_a: key[0x7] = 1;
                                     break;
                        case SDLK_s: key[0x8] = 1;
                                     break;
                        case SDLK_d: key[0x9] = 1;
                                     break;
                        case SDLK_f: key[0xE] = 1;
                                     break;
                        case SDLK_z: key[0xA] = 1;
                                     break;
                        case SDLK_x: key[0x0] = 1;
                                     break;
                        case SDLK_c: key[0xB] = 1;
                                     break;
                        case SDLK_v: key[0xF] = 1;
                                     break;
                    }
                    break;
                    case SDL_KEYUP:
                    {
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_1: key[0x1] = 0;
                                         break;
                            case SDLK_2: key[0x2] = 0;
                                         break;
                            case SDLK_3: key[0x3] = 0;
                                         break;
                            case SDLK_4: key[0xC] = 0;
                                         break;
                            case SDLK_q: key[0x4] = 0;
                                         break;
                            case SDLK_w: key[0x5] = 0;
                                         break;
                            case SDLK_e: key[0x6] = 0;
                                         break;
                            case SDLK_r: key[0xD] = 0;
                                         break;
                            case SDLK_a: key[0x7] = 0;
                                         break;
                            case SDLK_s: key[0x8] = 0;
                                         break;
                            case SDLK_d: key[0x9] = 0;
                                         break;
                            case SDLK_f: key[0xE] = 0;
                                         break;
                            case SDLK_z: key[0xA] = 0;
                                         break;
                            case SDLK_x: key[0x0] = 0;
                                         break;
                            case SDLK_c: key[0xB] = 0;
                                         break;
                            case SDLK_v: key[0xF] = 0;
                                         break;
                        }
                        break;
                    }
                }
        }
    }
}
