#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "chip8.h"
#include "input.h"
#include "graphics.h"

// Declar instantele de emulare si afisare
chip8 chip;
Display display;

int main() {
    // Folosesc ca seed pentru randomizare data actuala
    srand(time(0));
    // Procesul de initializare a consolei
    initialize(&chip);

    // char ROMname[] = "IBM Logo";
    // char ROMname[] = "Pong [Paul Vervalin, 1990]";
    // char ROMname[] = "test_opcode";
    char ROMname[] = "Keypad Test [Hap, 2006]";

    // Incarc ROMul
    loadROM(&chip, ROMname);

    // Initializez display-ul
    initGfx(&display);

    // semaforul de inchidere
    b8 close_requested = 0;
    b8 decrementCounter = 0;
    while(!close_requested) {
        u32 frameStart = SDL_GetTicks();

        u16 opcode = fetchInstruction(&chip);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                close_requested = 1;
            handleInput(&event, &chip);
        }

        if(executeInstruction(&chip, opcode) == 1)
             Draw(&chip, &display);
        u32 frameTime = SDL_GetTicks() - frameStart;

        // printf("frameTime: %d, TIME_PER_FRAME: %f, delay: %f\n", frameTime, TIME_PER_FRAME, TIME_PER_FRAME - (float)frameTime);

        if(frameTime < TIME_PER_FRAME)
            SDL_Delay(TIME_PER_FRAME - (float)frameTime);
        decrementCounter++;
        if(decrementCounter == 10) {
            decrementCounter = 0;
            if(chip.DT > 0)
                chip.DT--;
            if(chip.ST > 0)
                chip.ST--;
        }
    }

    exitGfx(&display);

    return 0;
}
