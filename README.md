# CHIP-8 Emulator

CHIP-8 Emulator written in C

## Dependencies

* [SDL3](https://github.com/libsdl-org/SDL)
* [IMGUI](https://github.com/ocornut/imgui) (Optional)

## Get Started

```bash
git clone https://github.com/josemcast/chip8-emulator.git
cd chip8-emulator
cmake -S . -B build
cd build
make -j
```

The Emulator offers an ImGUI mode, where ImGUI is used for debugging and real-time internal state visualization

```bash
cd build
cmake . -DIMGUI_BUILD=ON
make -j
```

## How to run

```bash
./build/chip8 rom.bin
```

will start the emulator and execute chip-8 rom provided as argument.

The emulator also provides a terminal debug mode, where you can see internal state dump on each step.

```bash
./build/chip8 -d rom.bin
```

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