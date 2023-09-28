#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} Display;

static const b8 PIXEL_SIZE = 20;
static const u16 SCREEN_HEIGHT = (u16) 32 * PIXEL_SIZE;
static const u16 SCREEN_WIDTH = (u16) 64 * PIXEL_SIZE;

void initGfx(Display *display);
void Draw(chip8 *chip, Display *display);
void exitGfx(Display *display);

#endif
