#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "types.h"
#include "chip8.h"
#include "input.h"

void initialize(chip8 *chip) {
    memset(chip->memory, 0, sizeof(chip->memory));
    memset(chip->stack, 0, sizeof(chip->stack));
    memset(chip->V, 0, sizeof(chip->V));
    memset(chip->framebuffer, 0, sizeof(chip->framebuffer));

    for(b8 i = 0; i < FONT_SIZE; ++i)
        chip->memory[FONT_START_ADDRESS + i] = font[i];

    chip->I = 0;
    chip->PC = 0x200;
    chip->SP = 0;
    chip->DT = 0;
    chip->ST = 0;
}

void loadROM(chip8 *chip, char ROMname[]) {
    strcat(ROMname, ".ch8");
    FILE *ROMfile = fopen(ROMname, "rb");
    if(ROMfile == NULL) {
        fprintf(stderr, "Couldn't open ROM file!");
        return;
    }

    // Seek to the end of the file
    fseek(ROMfile, 0, SEEK_END);
    // Set fileSize to the size in bytes from
    // the beginning to the ROMfile poiner
    long fileSize = ftell(ROMfile);
    // Seek back to the beginning
    fseek(ROMfile, 0, SEEK_SET);
    // Checks if the filesize is too big to load it at once
    // Note: Maybe create an alternative way of loading it if
    // it is too large.
    if(fileSize > MAX_ROM_SIZE - ROM_LOAD_ADDRESS + 1) {
        fprintf(stderr, "Memory exceeded!\n");
        fclose(ROMfile);
        return;
    }

    // Load the rom in the chip's memory
    fread(chip->memory + ROM_LOAD_ADDRESS, 1, fileSize, ROMfile);
    fclose(ROMfile);
}

u16 fetchInstruction(chip8 *chip) {
    u16 opcode = 0;
    opcode = (((opcode | chip->memory[chip->PC]) << 8) | chip->memory[chip->PC + 1]);
    // Move PC to the next set of instruction in memory
    chip->PC += 2;
    return opcode;
}

b8 executeInstruction(chip8 *chip, u16 opcode) {
    // This function also returns 1 if the framebuffer
    // changed or 0 if it didn't change
    b8 drawFlag = 0;
    switch(opcode & firstNibbleMask) {
        case 0x0000:
            switch(opcode & nnMask) {
                // Clear screen
                case 0xE0:
                    memset(chip->framebuffer, 0, sizeof(chip->framebuffer));
                    drawFlag = 1;
                    break;
                // Pop address from the stack
                case 0xEE:
                    chip->PC = chip->stack[chip->SP];
                    chip->SP--;
                    break;
            }
            break;
        // Set the Program Counter to the nnn address
        case 0x1000:
            chip->PC = opcode & nnnMask;
            break;
        // Push address to the stack
        case 0x2000:
            chip->stack[chip->SP] = chip->PC;
            chip->SP++;
            chip->PC = opcode & nnnMask;
            break;
        // Skip one instruction if Vx == nn
        case 0x3000:
            if(chip->V[(opcode & xMask) >> 8] == (opcode & nnMask))
                chip->PC += 2;
            break;
        // Skip one instruction if Vx != nn
        case 0x4000:
            if(chip->V[(opcode & xMask) >> 8] != (opcode & nnMask))
                chip->PC += 2;
            break;
        // Skip if Vx == Vy
        case 0x5000:
            if(chip->V[(opcode & xMask) >> 8] == chip->V[(opcode & yMask) >> 4])
                chip->PC += 2;
            break;
        // Set the Vx register to nn
            break;
        case 0x6000:
            chip->V[(opcode & xMask) >> 8] = opcode & nnMask;
            break;
        // Addition to the Vx register
        case 0x7000:
            chip->V[(opcode & xMask) >> 8] += opcode & nnMask;
            break;
        // Logical and arithmetic instructions
        case 0x8000:
            switch(opcode & nMask) {
                // Set Vx to the value of Vy
                case 0x0:
                    chip->V[(opcode & xMask) >> 8] = chip->V[(opcode & yMask) >> 4];
                    break;
                // Binary OR
                case 0x1:
                    chip->V[(opcode & xMask) >> 8] |= chip->V[(opcode & yMask) >> 4];
                    break;
                // Binary AND
                case 0x2:
                    chip->V[(opcode & xMask) >> 8] &= chip->V[(opcode & yMask) >> 4];
                    break;
                // Binary XOR
                case 0x3:
                    chip->V[(opcode & xMask) >> 8] ^= chip->V[(opcode & yMask) >> 4];
                    break;
                // Addition to Vx register
                case 0x4:
                    chip->V[0xF] = 0;
                    if((u16) (chip->V[(opcode & xMask) >> 8] + chip->V[(opcode & yMask) >> 4]) > 255)
                        chip->V[0xF] = 1;
                    chip->V[(opcode & xMask) >> 8] += chip->V[(opcode & yMask) >> 4];
                    break;
                // Set Vx to the difference between Vx and Vy
                case 0x5:
                    chip->V[0xF] = 0;
                    if(chip->V[(opcode & xMask) >> 8] > chip->V[(opcode & yMask) >> 4])
                        chip->V[0xF] = 1;
                    chip->V[(opcode & xMask) >> 8] -= chip->V[(opcode & yMask) >> 4];
                    break;
                // Set Vx to Vy, Shift Vx one bit to the right, Set VF to the value that the shifted bit stored
                case 0x6: {
                    b8 x = (opcode & xMask) >> 8;
                    chip->V[x] = chip->V[(opcode & yMask) >> 4];
                    chip->V[0xF] = chip->V[x] & 1;
                    chip->V[x] = chip->V[x] >> 1;
                    break;
                }
                // Set Vx to the difference between Vy and Vx
                case 0x7:
                    chip->V[0xF] = 0;
                    if(chip->V[(opcode & yMask) >> 4] > chip->V[(opcode & xMask) >> 8])
                        chip->V[0xF] = 1;
                    chip->V[(opcode & xMask) >> 8] = chip->V[(opcode & yMask) >> 4] - chip->V[(opcode & xMask) >> 9];
                    break;
                // Same as 0x6, but shift the Vx register to the left
                case 0xE: {
                    b8 x = (opcode & xMask) >> 8;
                    chip->V[x] = chip->V[(opcode & yMask) >> 4];
                    chip->V[0xF] = chip->V[x] & 0b10000000;
                    chip->V[x] = chip->V[x] << 1;
                    break;
                }
            }
            break;
        // Skip if Vx != Vy
        case 0x9000:
            if(chip->V[(opcode & xMask) >> 8] != chip->V[(opcode & yMask) >> 4])
                chip->PC += 2;
            break;
        // Change I address
        case 0xA000:
            chip->I = opcode & nnnMask;
            break;
        // Jump with offset
        case 0xB000:
            chip->PC = (opcode & nnnMask) + chip->V[0];
            break;
        // Generate a random number and store it in Vx
        case 0xC000:
            chip->V[(opcode & xMask) >> 8] = rand() & (opcode & nnMask);
            break;
        case 0xD000: { 
            b8 x = chip->V[(opcode & xMask) >> 8] % 64;
            b8 y = chip->V[(opcode & yMask) >> 4] % 32;
            b8 n = opcode & nMask;

            chip->V[0xF] = 0;

            for(b8 row = 0; row < n; ++row)
                for(b8 col = 0; col < 8; ++col) {
                    b8 spriteBit = (chip->memory[chip->I + row] >> (7 - col)) & 1;
                    u16 pos = FRAMEBUFFER_WIDTH * (y + row) + x + col;
                    if(!chip->V[0xf] && chip->framebuffer[pos] == 1 && spriteBit == 1)
                        chip->V[0xF] = 1;
                    chip->framebuffer[pos] ^= spriteBit;
                }

            drawFlag = 1;
            break;
        }
        case 0xE000:
            switch(opcode & nnMask) {
                case 0x9E:
                     if(checkKeyDown(chip->V[(opcode & xMask) >> 8], chip))
                         chip->PC += 2;
                    break;
                case 0xA1:
                     if(!checkKeyDown(chip->V[(opcode & xMask) >> 8], chip))
                         chip->PC += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & nnMask) {
                // Set Vx to DT
                case 0x07:
                    chip->V[(opcode & xMask) >> 8] = chip->DT;
                    break;
                // Set DT to Vx
                case 0x15:
                    chip->DT = chip->V[(opcode & xMask) >> 8];
                    break;
                // Set ST to Vx
                case 0x18:
                    chip->ST = chip->V[(opcode & xMask) >> 8];
                    break;
                // Add Vx to the I register
                case 0x1E:
                    if(chip->I + chip->V[(opcode & xMask) >> 8] > 0xFFF)
                        chip->V[0xF] = 1;
                    chip->I += chip->V[(opcode & xMask) >> 8];
                    break;
                // Get key
                case 0x0A:
                    // chip->V[(opcode & xMask) >> 8] = getKey();
                    // if(getKey() == 0x10)
                    //     chip->PC -= 2;
                    break;
                // Load font character from memory
                case 0x29:
                    chip->I = FONT_START_ADDRESS + (chip->V[(opcode & xMask) >> 8] * 5);
                    break;
                // Store the decimal digits of Vx in memory, beginning at I
                case 0x33: {
                    b8 x = chip->V[(opcode & xMask) >> 8];
                    chip->memory[chip->I] = x/100;
                    chip->memory[chip->I + 1] = (x/10) % 10;
                    chip->memory[chip->I + 2] = x%10;
                    break;
                }
                // Store register to memory
                case 0x55: {
                    b8 x = (opcode & xMask) >> 8;
                    for(b8 i = 0; i <= x; ++i) {
                        chip->memory[chip->I + i] = chip->V[x];
                    }
                    break;
                }
                // Load register from memory
                case 0x65: {
                    b8 x = (opcode & xMask) >> 8;
                    for(b8 i = 0; i <= x; ++i) {
                        chip->V[i] = chip->memory[chip->I + i];
                    }
                    break;
                }
            }
            break;
    }
    return drawFlag;
}
