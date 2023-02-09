//
// Created by beloin on 29/01/23.
//

#include "iostream"
#include "Chip8.h"
#include "setup/setup.h"

void drawGraphics();

void drawGraphics() {

}

Chip8 chip8;

int main(int argc, char **argv) {
    std::cout << "Hello 8Bit" << std::endl;

    setupGraphics();
    setupInput();

    chip8.initialize();
    chip8.loadGame("pong");

    for (;;) {
        chip8.emulateCycle();

        if (chip8.drawFlag) {
            drawGraphics();
        }

        chip8.setKeys();
    }

    return 0;
}