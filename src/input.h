#ifndef INPUT_H
#define INPUT_H

// This b8 might cause problems
b8 checkKeyDown(b8 key, chip8 *chip);
b8 checkAnyKeyDown(chip8 *chip);
void handleInput(SDL_Event *event, chip8 *chip);
b8 getKey(SDL_Event *event);

#endif
