//
// Created by beloin on 30/01/23.
//

#ifndef INC_8BIT_EMULATOR_CHIP8_H
#define INC_8BIT_EMULATOR_CHIP8_H
#include "random"


class Chip8 {
private:
    unsigned char memory[4096];
    unsigned char V[16]; // Registers
    unsigned short I; // Index register
    unsigned short pc; // program counter

    unsigned char gfx[64 * 32]; // width x height

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16]; // In order to get back from JUMP or call subroutine
    unsigned short sp;

    // This is used as a "Boolean" array which see if key[X], where 0 < X < 16, is pressed or not
    unsigned char key[16];

    unsigned short opcode;

    // Random device and engine
    std::mt19937 rng;
    std::random_device dev;
    std::uniform_int_distribution<std::mt19937::result_type> dist16;

    void clearStack();

    void clearGraphics();

    void clearRegisters();

    void clearMemory();

    void beepPerSoundTier();

    void switchFor0x0();

    void switchFor0x8();

    unsigned char getRandomNumber();

    void switchFor0xE();

    void switchFor0xF();

    unsigned char getKey();

    void configureRandom();

public:

    void initialize();

    void loadGame(const char *string);

    void emulateCycle();

    bool drawFlag;

    void setKeys();
};


#endif //INC_8BIT_EMULATOR_CHIP8_H
