# CHIP-8 Emulator

CHIP-8 Emulator written in C

## Get Started

```bash
git clone https://github.com/josemcast/chip8-emulator.git
cd chip8-emulator
cmake -S . -B build
cmake --build build -j
```

## How to run

**GUI mode**

```bash
./build/chip8
```

will run the emulator and show the IBM logo on the screen ([SDL3](https://github.com/libsdl-org/SDL) needed).

**Debug mode**

```bash
./build/chip8 -d
```

will run the emulator with custom debug.ch8 file and print to stdout

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
| 9XY0       | Conditional         |
| ANNN       | Set Index           |
| BNNN       | Jump + offset       |
| CXNN       | Random              |
| FX0A       | Get Key             |