//
// Created by beloin on 30/01/23.
//

#ifndef INC_8BIT_EMULATOR_CHIP8_H
#define INC_8BIT_EMULATOR_CHIP8_H


class Chip8 {
private:
    unsigned char memory[4096];
    unsigned char V[16]; // Registers
    unsigned short I; // Index register
    unsigned short pc; // program counter

    unsigned char gfx[64*32];

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16]; // In order to get back from JUMP or call subroutine
    unsigned short sp;

    unsigned char key[16];

    unsigned short opcode;
public:
    void initialize();

    void loadGame(const char *string);

    void emulateCycle();

    bool drawFlag;

    void setKeys();
};


#endif //INC_8BIT_EMULATOR_CHIP8_H
