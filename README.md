#Chip8++
*A Chip8 Emulator/Interpreter written in C++ using the SDL2 library.*

![Space Invaders](screenshots/space_invaders.gif)

####Dependencies (Look here before you download!)
- A C++ compiler (clang++, g++, minGW), the Makefile will default to clang.
- SDL2 development headers. (libsdl2-dev)

####Getting Started
1. Clone this git repo or download the .zip (don't forget to unzip it)
2. Run `make` in the top-level directory which contains the Makefile
3. Play some games `./chip8 rom/BRIX`

####About This Project
I really enjoy learning about hardware so I thought an emulation/interpretation project would help me learn a lot about the basics of CPUs. This program emulates the Chip8 virtual machine and runs Chip8 programs. I tried to keep my implementation as true to the specifications detailed in various technical documents and the Wikipedia page.

The interpreter is bundled with a bunch of ROMs that are free and a part of the public domain.

I've remapped the keybindings to a more modern control-set suggested by various articles about the Chip8.

    CHIP8 Keypad          Modern Keyboard
     |1|2|3|C|              |1|2|3|4|
     |4|5|6|D|              |Q|W|E|R|
     |7|8|9|E|              |A|S|D|F|
     |A|0|B|F|              |Z|X|C|V|

####Special Thanks to...
- [Cowgod's Chip8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Laurence Muller's blog post on getting started](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
- [Wikipedia's Chip8 Technical Specifications](https://en.wikipedia.org/wiki/CHIP-8)
- [The amazing people behind SDL2](https://libsdl.org/)
- [Lazyfoo's SDL Tutorials](http://lazyfoo.net/)

