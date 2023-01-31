# CHIP-8

## Memory:
 - 4Kb -> 4096 address locations
 - 0x000 -> 0xFFF
 - On the real hardware the location of 0x000 to 
0x1FF was usually used to store the interpreteR.
 - And the uppermost 256 bytes (0xEA0-0xFFF) were used
for display refresh and the later 96 bytes were also
used to store the call stack, variables and internal use.

### Registers
 - 16 8-bit register, V0-VF
 - Usually VF is used as a flag for some instructions
 - We have an address register, called _"i"_.
is 12 bits wide, used with opcodes.

### Stack
 - Store return addresses when subroutines are called

### Timers
- Delay Timer -> Used to time things in game
- Sound Timer -> Beeping Sound


### Opcode
 - Opcode are 2 bytes long stored in BigEndian format
 - BigEndian: The most significant byte are in the smallest
address.  

| Little Endian | Big Endian |
|:-------------:|:----------:|
|   0b1000011   | 0b1100001  |

