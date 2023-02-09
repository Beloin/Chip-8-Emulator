//
// Created by beloin on 30/01/23.
//

#include <cstdio>
#include "random"
#include "Chip8.h"
#include "setup/setup.h"
#include "utils/input_utils.h"

void Chip8::initialize() {
    clearGraphics();
    clearStack();
    clearRegisters();
    clearMemory();
    configureRandom();

    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    for (int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    // Reset Timers
    delay_timer = 0;
    sound_timer = 0;
}

void Chip8::configureRandom() {
    rng = std::mt19937(dev());
    dist16 = std::uniform_int_distribution<std::mt19937::result_type>(0, 15);
}

void Chip8::clearMemory() {
    for (unsigned char &i: memory) {
        i = 0;
    }
}

void Chip8::clearRegisters() {
    for (unsigned char &i: V) {
        i = 0;
    }
}

void Chip8::clearGraphics() {
    for (unsigned char &i: gfx) {
        i = 0;
    }
}

void Chip8::clearStack() {
    for (unsigned short &i: stack) {
        i = 0;
    }
}

void Chip8::loadGame(const char *string) {
    std::FILE *fp = std::fopen(string, "rb");
    if (!fp) {
        // Throw...
        std::perror("There's a problem with the file.");
    }

    int buffer_size = 1000;
    auto *buffer = new unsigned char[buffer_size];

    unsigned long i = 0x200; // Starts from byte 512
    unsigned long read_count;
    do {
        read_count = std::fread(buffer, sizeof(*buffer), buffer_size, fp);

        if (i + read_count > 0xFFF - 0x200) {
            // Throw error, cannot read game, too big;
            printf("Game too big :(");
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
                pc += 4; // 2 by default, +2 to skip the next utils
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
                pc += 4; // 2 by default, +2 to skip the next utils
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
            unsigned short x = opcode & 0x0F00 >> 8;
            unsigned short y = opcode & 0x00F0 >> 4;
            unsigned short n = opcode & 0x000F >> 0;

            x = V[x];
            y = V[y];

            V[0xF] = 0;
            for (int y_line = 0; y_line < n; ++y_line) {
                // Linha 0, coluna 1
                // 0 [1] 2
                // 3  4  5
                // 6  7  8
                // Use XOR to check for conflicts > 1
                unsigned short pixel = memory[I + y_line];

                for (int x_line = 0; x_line < 8; ++x_line) { // THIS IS FOR XOR OPERATION
                    unsigned short bit = 0x80 >> x_line; // 0x80 == 0b1000_0000
                    if ((pixel & bit) != 0) { // If pixel & bit == 0, no bit was flipped
                        int index = (x + x_line) + ((y + y_line) * 64);
                        unsigned char current_pixel = gfx[index];

                        if (current_pixel) {
                            V[0xF] = 1;
                        }

                        gfx[index] ^= 1;
                    }


                }

            }

            drawFlag = true;
            pc += 2;
            break;
        }

        case 0xE000:
            switchFor0xE();
            break;

        case 0xF000:
            switchFor0xF();
            break;

        default:
            printf("Unknown utils: 0x%X\n", opcode);
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
            clearGraphics();
            break;

        case 0xEE:
            pc = stack[--sp];
            break;

        default:
            printf("Unknown utils: 0x%X\n", opcode);
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
            printf("Unknown utils: 0x%X\n", opcode);
    }
}

void Chip8::switchFor0xE() {
    unsigned char x = opcode & 0x0F00 >> 8;
    switch (opcode & 0x00FF) {
        case 0x009E: {
            if (key[V[x]] != 0) {
                pc += 4;
            } else {
                pc += 2;
            }

            break;
        }

        case 0x00A1: {
            if (key[V[x]] != 0) {
                pc += 2;
            } else {
                pc += 4;
            }

            break;
        }

        default:
            printf("Unknown utils: 0x%X\n", opcode);
    }

}

void Chip8::switchFor0xF() {
    unsigned short x = opcode & 0x0F00 >> 8;
    switch (opcode & 0x00FF) {
        case 0x0007:
            V[x] = delay_timer;
            pc += 2;
            break;

        case 0x000A:
            V[x] = getKey();
            pc += 2;
            break;

        case 0x0015:
            delay_timer = V[x];
            pc += 2;
            break;

        case 0x0018:
            sound_timer = V[x];
            pc += 2;
            break;

        case 0x001E:
            I += V[x];
            pc += 2;
            break;

        case 0x0029: {
            // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
            // The location are in the memory
            unsigned char hexa = V[x]; // 0 -> 0xF
            char reference = (char) (5 * hexa); // 0 -> 75
            I = memory[reference];
            pc += 2;
            break;
        }

        case 0x0033: {
            // Store BCD representation of VX in M(I)..M(I+2)
            // VX -> 255 -> 1111 1111
            char unit = (char) (V[x] % 10);
            char tens = (char) (V[x] / 10);
            char hundreds = (char) (V[x] / 100);

            memory[I + 0] = unit;
            memory[I + 1] = tens;
            memory[I + 2] = hundreds;

            pc += 2;
            break;
        }

        case 0x0055: {
            for (int i = 0x0; i < 0xF; ++i) {
                memory[I + i] = V[x];
            }

            pc += 2;
            break;
        }

        case 0x0065: {
            for (int i = 0x0; i < 0xF; ++i) {
                V[x] = memory[I + i];
            }

            pc += 2;
            break;
        }

        default:
            printf("Unknown utils: 0x%X\n", opcode);
    }
}

void Chip8::beepPerSoundTier() {
    if (sound_timer == 1) {
        printf("BEEEEEEEP\n");
    }
}

unsigned char Chip8::getKey() {
    return waitForKey();
}

void Chip8::setKeys() {
    char pressed = currentPressedKey();
    for (int i = 0; i < 0xF; ++i) {
        key[i] = pressed == i;
    }
}

unsigned char Chip8::getRandomNumber() {
    unsigned long random_int = dist16(rng);
    return (char) (random_int % 16);
}
