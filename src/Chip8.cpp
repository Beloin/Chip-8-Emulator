//
// Created by beloin on 30/01/23.
//

#include <cstdio>
#include "Chip8.h"
#include "setup/setup.h"

void Chip8::initialize() {
    clear_graphics();
    clear_stack();
    clear_registers();
    clear_memory();

    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    // Reset Timers
}

void Chip8::clear_memory() {
    for (unsigned char &i: memory) {
        i = 0;
    }
}

void Chip8::clear_registers() {
    for (unsigned char &i: V) {
        i = 0;
    }
}

void Chip8::clear_graphics() {
    for (unsigned char &i: gfx) {
        i = 0;
    }
}

void Chip8::clear_stack() {
    for (unsigned short &i: stack) {
        i = 0;
    }
}

void Chip8::loadGame(const char *string) {
    std::FILE *fp = std::fopen(string, "rb");
    if (!fp) {
        // Throw...
        std::perror("");
    }

    int buffer_size = 1000;
    auto *buffer = new unsigned char[buffer_size];

    unsigned long i = 0x200; // Starts from byte 512
    unsigned long read_count;
    do {
        read_count = std::fread(buffer, sizeof(*buffer), buffer_size, fp);

        if (i + read_count > 0xFFF - 0x200) {
            // Throw error, cannot read game, too big;
        }

        for (; i < i + read_count; ++i) {
            memory[i] = buffer[i];
        }
        i += read_count;

    } while (read_count != 0);


    std::fclose(fp);
}

void Chip8::emulateCycle() {
    // This left shift converts 1 Byte to two bytes and then put an OR mask on it
    opcode = memory[pc] << 8 | memory[pc + 1];

    switch (opcode & 0xF000) { // First 4 Bits
        case 0x0000:
            switchFor0x0();
            break;

        case 0x1000: {
            // 0x1NNN -> goto NNN
            unsigned short addr = opcode & 0x0FFF;
            pc = addr;
            break;
        }

        case 0x2000:
            // 0x2NNN call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000: {
            // 0x3XNN skips next instruction if VX equals  NN
            unsigned char x = (opcode & 0x0F00) >> 8;
            unsigned char value = opcode & 0x00FF;

            if (V[x] == value) {
                pc += 2;
            }

            pc += 2;
            break;
        }

        case 0x4000: {
            // 0x4XNN skips next instruction if VX not equals NN
            unsigned char x = (opcode & 0x0F00) >> 8;
            unsigned char value = opcode & 0x00FF;

            if (V[x] != value) {
                pc += 2;
            }

            pc += 2;

            break;
        }

        case 0x5000: {
            // 0x5XY0 skips the next instruction if Vx equals Vy
            int x = opcode & 0x0F00 >> 8;
            int y = opcode & 0x00F0 >> 4;

            if (V[x] == V[y]) {
                pc += 4; // 2 by default, +2 to skip the next opcode
            } else {
                pc += 2;
            }

            break;
        }

        case 0x6000: {
            // 0x6XNN -> Sets Vx = NN
            int x = opcode & 0x0F00 >> 8;
            V[x] = opcode & 0x00FF;
            pc += 2;
            break;
        }

        case 0x7000: {
            unsigned char x = opcode & 0x0F00 >> 8;
            unsigned char value = opcode & 0x00FF;
            V[x] += value;
            pc += 2;
            break;
        }

        case 0x8000:
            switchFor0x8();
            break;

        case 0x9000: {
            // 0x5XY0 skips the next instruction if Vx equals Vy
            int x = opcode & 0x0F00 >> 8;
            int y = opcode & 0x00F0 >> 4;

            if (V[x] != V[y]) {
                pc += 4; // 2 by default, +2 to skip the next opcode
            } else {
                pc += 2;
            }
            break;
        }

        case 0xA000: // 0xANNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000:
            pc = V[0] + (opcode & 0x0FFF);
            break;

        case 0xC000: {
            int x = (opcode & 0x0F00) >> 8;
            V[x] = (opcode & 0x00FF) & getRandomNumber();
            pc += 2;
            break;
        }

        case 0xD000: {
            unsigned char x = opcode & 0x0F00 >> 8;
            unsigned char y = opcode & 0x00F0 >> 4;
            unsigned char n = opcode & 0x000F >> 0;

            x = V[x];
            y = V[y];

            V[0xF] = 0;
            for (int i = 0; i < n; ++i) {
                // Linha 0, coluna 1
                // 0 [1] 2
                // 3  4  5
                // 6  7  8
                // Use XOR to check for conflicts > 1
                unsigned short pixel = memory[I + i];
                unsigned short index = (x * 32) + (y);

                for (int j = 0; j < 8; ++j) {

                }

            }

            drawFlag = true;
            break;
        }

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }

    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        beepPerSoundTier();
        --sound_timer;
    }

}

void Chip8::switchFor0x0() {
    switch (opcode & 0x00FF) {
        case 0xE0:
            clearDisplay();
            break;

        case 0xEE:
            pc = stack[--sp];
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }
}

void Chip8::switchFor0x8() {
    unsigned char x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0x000F) {
        case 0x0000:
            V[x] = V[y];
            pc += 2;
            break;

        case 0x0001:
            V[x] |= V[y];
            pc += 2;
            break;

        case 0x0002:
            V[x] &= V[y];
            pc += 2;
            break;

        case 0x0003:
            V[x] ^= V[y];
            pc += 2;
            break;

        case 0x0004:
            // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
            if (V[y] > (0xFF - V[x])) {
                V[0xF] = 1;
            } else {
                V[0xF] = 0;
            }

            V[x] += V[y];
            pc += 2;
            break;

        case 0x0005:
            // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
            if (V[y] > V[x]) {
                V[0xF] = 0;
            } else {
                V[0xF] = 1;
            }

            V[x] -= V[y];
            pc += 2;
            break;

        case 0x0006: {
            unsigned char least_b = V[x] & 0x01; // XXXX XXXX & 0000 0001 -> 0000 000X
            V[15] = least_b;
            V[x] >>= 1;
            pc += 2;
            break;
        }

        case 0x0007:
            // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
            if (V[y] < V[x]) {
                V[0xF] = 0;
            } else {
                V[0xF] = 1;
            }

            V[x] = V[y] - V[x];
            pc += 2;
            break;

        case 0x000E: {
            unsigned char most_b = (V[x] & 0x80) >> 7; // XXXX XXXX & 1000 0000 -> X000 0000 >> 7 -> 0000 000X
            V[15] = most_b;
            V[x] <<= 1;
            pc += 2;
            break;
        }

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }
}

void Chip8::beepPerSoundTier() {
    if (sound_timer == 1) {
        printf("BEEEEEEEP\n");
    }
}
