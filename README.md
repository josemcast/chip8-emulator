# CHIP-8 Emulator

CHIP-8 Emulator written in C

## Dependencies

* [SDL3](https://github.com/libsdl-org/SDL)

## Get Started

```bash
git clone https://github.com/josemcast/chip8-emulator.git
cd chip8-emulator
git clone https://github.com/libsdl-org/SDL.git SDL
cmake -S . -B build
cd build
make -j
```

## How to run

The emulator has two running modes: default and script.

To run the emulator on default just run the command

```bash
./build/chip8 rom.bin
```

and the emulator will start up and execute chip-8 rom provided as argument.

In script mode, the startup configuration and runtime behavior can be modified using the Lua programming language. The project already provides both scripts with default values in [scripts](scripts/) folder. Use `-s` option to run the emulator in script mode.

```bash
./build/chip8 -s scripts/config.lua scripts/run.lua
```

## Keyboard Layout

Back in the days CHIP-8 were used with computers with heaxadecimal keypads 

| 1   | 2   | 3   | C   |
| --- | --- | --- | --- |
| 4   | 5   | 6   | D   |
| 7   | 8   | 9   | E   |
| A   | 0   | B   | F   |

The keypad-keyboard mapping used in this emulator is



| 1   | 2   | 3   | 4   |
| --- | --- | --- | --- |
| Q   | W   | E   | R   |
| A   | S   | D   | F   |
| Z   | X   | C   | V   |

which is based on QWERTY keyboard layout.

## Supported CHIP8 Opcodes

| **Opcode** | **Action**          |
|:----------:|:-------------------:|
| 00E0       | Clear Screen        |
| 00EE       | Return              |
| 1NNN       | goto                |
| 2NNN       | Call                |
| 3XNN       | Conditional         |
| 4XNN       | Conditional         |
| 5XY0       | Conditional         |
| 6XNN       | Set Immediate       |
| 7XNN       | Add Immediate       |
| 8XY0       | Set                 |
| 8XY1       | Bitwise OR          |
| 8XY2       | Bitwise AND         |
| 8XY3       | Bitwise XOR         |
| 8YX4       | Add with Carry Flag |
| 8XY5       | Subtract with Flag  |
| 8XY6       | Shift right         |
| 8XY7       | Subtract with Flag  |
| 8XYE       | Shift left          |
| 9XY0       | Conditional         |
| ANNN       | Set Index           |
| BNNN       | Jump + offset       |
| CXNN       | Random              |
| EX9E       | Skip if Key         |
| EXA1       | Skip if not Key     |
| FX07       | Get delay timer     |
| FX0A       | Get Key             |
| FX15       | Set delay timer     |
| FX18       | Set sound timer     |
| FX1E       | Add to index        |
| FX33       | BCD conversion      |
| FX55       | Store               |
| FX65       | Load                |