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

    unsigned char gfx[64*32]; // width x height

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16]; // In order to get back from JUMP or call subroutine
    unsigned short sp;

    unsigned char key[16];

    unsigned short opcode;

    void clear_stack();

    void clear_graphics();

    void clear_registers();

public:

    void initialize();

    void loadGame(const char *string);

    void emulateCycle();

    bool drawFlag;

    void setKeys();

    void clear_memory();

    void beepPerSoundTier();

    void switchFor0x0();

    void switchFor0x8();

    void clearDisplay();

    unsigned char getRandomNumber();

    void switchFor0xE();

    void switchFor0xF();
};


#endif //INC_8BIT_EMULATOR_CHIP8_H
