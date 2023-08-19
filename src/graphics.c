#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include "chip8.h"
#include "graphics.h"

void initGfx(Display *display) {
    // init part
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return;
    }
    u32 rendererFlags = SDL_RENDERER_ACCELERATED |SDL_RENDERER_PRESENTVSYNC;
    u32 windowFlags = 0;

    // window init
    display->window = SDL_CreateWindow("CHIP-8",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          windowFlags);
    if(!display->window) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    // renderer init
    display->renderer = SDL_CreateRenderer(display->window, -1, rendererFlags);
    if(!display->renderer){
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return;
    }
    // Set pixel size
    SDL_RenderSetScale(display->renderer, PIXEL_SIZE, PIXEL_SIZE);

    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);
    SDL_RenderPresent(display->renderer);
}

void Draw(chip8 *chip, Display *display) {
    u32 frameStart = SDL_GetTicks();

    // Set background color
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);

    // Set pixel color
    SDL_SetRenderDrawColor(display->renderer, 0, 255, 0, 255);

    SDL_Point onPixels[FRAMEBUFFER_SIZE];
    u16 onCount = 0;

    for(u16 i = 0; i < FRAMEBUFFER_SIZE; ++i)
        if(chip->framebuffer[i]) {
            // Determine which column we are on
            onPixels[onCount].x = (i % FRAMEBUFFER_WIDTH);
            // Determine which row we are on
            onPixels[onCount].y = (i / FRAMEBUFFER_WIDTH);
            onCount++;
        }

    if(SDL_RenderDrawPoints(display->renderer, onPixels, onCount) != 0) {
            fprintf(stderr, "Error drawing pixels: %s\n", SDL_GetError());
            SDL_DestroyRenderer(display->renderer);
            SDL_DestroyWindow(display->window);
            SDL_Quit();
            return;
    }

    u32 frameTime = SDL_GetTicks() - frameStart;
    
    if(frameTime < FRAME_RATE)
        SDL_Delay(FRAME_RATE - frameTime);
    SDL_RenderPresent(display->renderer);
}

void exitGfx(Display *display) {
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}
