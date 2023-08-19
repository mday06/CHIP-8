#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "graphics.h"

chip8 chip;
Display display;

int main() {
    initialize(&chip);

    char ROMname[] = "IBM Logo";
    loadROM(&chip, ROMname);

    initGfx(&display);

    b8 close_requested = 0;
    while(!close_requested) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                close_requested = 1;
            }
        }
        u16 opcode = fetchInstruction(&chip);
        if(executeInstruction(&chip, opcode) == 1)
             Draw(&chip, &display);
    }

    exitGfx(&display);

    return 0;
}
