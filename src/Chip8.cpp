//
// Created by beloin on 30/01/23.
//

#include "Chip8.h"

void Chip8::initialize() {
    for (unsigned char & i : V) {
        i = 0;
    }
    I = 0x0;
}

void Chip8::loadGame(const char *string) {

}

void Chip8::emulateCycle() {
    // TODO: Fetch opcode. Is this left shift right? Looks like it ignores the first memory
    opcode = memory[pc] << 8 | memory[pc+1];

}

void Chip8::setKeys() {

}
