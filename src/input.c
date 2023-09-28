#include <stdio.h>
#include <SDL2/SDL.h>
#include "types.h"
#include "chip8.h"
#include "input.h"

b8 checkKeyDown(b8 key, chip8 *chip) {
    if(chip->keyboard[key])
        return 1;
    return 0;
}

b8 checkAnyKeyDown(chip8 *chip) {
    for(int key = 0; key < 0xF; ++key)
        if(chip->keyboard[key])
            return 1;
    return 0;
}

void getKey(chip8 *chip) {
    SDL_Event event;
    if(SDL_PollEvent(&event))
        if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.scancode == SDL_SCANCODE_X)
                chip->keyboard[0x0] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_1)
                chip->keyboard[0x1] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_2)
                chip->keyboard[0x2] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_3)
                chip->keyboard[0x3] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_Q)
                chip->keyboard[0x4] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_W)
                chip->keyboard[0x5] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_E)
                chip->keyboard[0x6] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_A)
                chip->keyboard[0x7] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_S)
                chip->keyboard[0x8] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_D)
                chip->keyboard[0x9] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_Z)
                chip->keyboard[0xA] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_C)
                chip->keyboard[0xB] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_4)
                chip->keyboard[0xC] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_R)
                chip->keyboard[0xD] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_F)
                chip->keyboard[0xE] = 1;
            if(event.key.keysym.scancode == SDL_SCANCODE_V)
                chip->keyboard[0xF] = 1;
        }
}
