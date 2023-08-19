#include <stdio.h>
#include <string.h>
#include "chip8.h"

void initialize(chip8 *chip) {
    memset(chip->memory, 0, sizeof(chip->memory));
    memset(chip->stack, 0, sizeof(chip->stack));
    memset(chip->V, 0, sizeof(chip->V));
    memset(chip->framebuffer, 0, sizeof(chip->framebuffer));
    memset(chip->keyboard, 0, sizeof(chip->keyboard));

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
    // Note: Maybe create an alternate way of loading it if
    // it is too large.
    if(fileSize > MAX_ROM_SIZE - ROM_LOAD_ADDRESS + 1) {
        fprintf(stderr, "Memory exceeded!");
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
        case 0:
            switch(opcode & nnnMask) {
                // Clear screen
                case 0x0E0:
                    memset(chip->framebuffer, 0, sizeof(chip->framebuffer));
                    drawFlag = 1;
                    break;
                // Pop address from the stack
                case 0x0EE:
                    chip->PC = chip->stack[chip->SP--];
                    break;
            }
            break;
        // Set the Program Counter to the nnn address
        case 0x1000:
            chip->PC = opcode & nnnMask;
            break;
        // Push address to the stack
        case 0x2000:
            chip->stack[chip->SP++] = chip->PC;
            chip->PC = opcode & nnnMask;
        // Set the Vx register to nn
        case 0x6000:
            chip->V[(opcode & xMask) >> 8] = opcode & nnMask;
            break;
        // Addition to the Vx register
        case 0x7000:
            chip->V[(opcode & xMask) >> 8] += opcode & nnMask;
            break;
        // Change I address
        case 0xA000:
            chip->I = opcode & nnnMask;
            break;
        case 0xD000: { 
            b8 x = chip->V[(opcode & xMask) >> 8];
            b8 y = chip->V[(opcode & yMask) >> 4];
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
    }
    return drawFlag;
}
