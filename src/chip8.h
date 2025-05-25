#ifndef CHIP8_H
#define CHIP8_H

typedef struct {
    b8 memory[4096];
    u16 stack[16];

    // Registers
    b8 V[16];
    u16 I, PC;
    b8 SP;
    b8 DT;
    b8 ST;
    
    b8 framebuffer[64 * 32];
    b8 keyboard[16];
} chip8;

// Framebuffer related
static const b8 FRAMEBUFFER_WIDTH = 64;
static const b8 FRAMEBUFFER_HEIGHT = 32;
static const u16 FRAMEBUFFER_SIZE = 64 * 32;

// Rom related
static const u16 MAX_ROM_SIZE = 4096;
static const u16 ROM_LOAD_ADDRESS = 0x200;

// Frequency related
static const u16 INSTRUCTIONS_PER_SECOND = 600;
static const float TIME_PER_FRAME = 1000.0f/(float)INSTRUCTIONS_PER_SECOND;

// Font related
static const b8 FONT_SIZE = 80;
static const b8 FONT_START_ADDRESS = 0x50;
static const b8 font[16 * 5] = {
    // 0
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    // 1
    0x20, 0x60, 0x20, 0x20, 0x70,
    // 2
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    // 3
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    // 4
    0x90, 0x90, 0xf0, 0x10, 0x10,
    // 5
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    // 6
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    // 7
    0xf0, 0x10, 0x20, 0x40, 0x40,
    // 8
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    // 9
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    // A
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    // B
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    // C
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    // D
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    // E
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    // F
    0xf0, 0x80, 0xf0, 0x80, 0x80,
};

// Opcode masks
static const u16 firstNibbleMask = 0xF000;
static const u16 xMask = 0xF00;
static const u16 yMask = 0xF0;
static const u16 nnnMask = 0xFFF;
static const u16 nnMask = 0xFF;
static const u16 nMask = 0xF;

void initialize(chip8 *chip);
void loadROM(chip8 *chip, char ROMname[]);
u16 fetchInstruction(chip8 *chip);
b8 executeInstruction(chip8 *chip, u16 opcode);

#endif
