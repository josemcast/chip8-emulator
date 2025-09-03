#ifndef __CHIP8_H__
#define __CHIP8_H__

#include <stdint.h>
#include <log.h>
#include <display.h>
#include <keyboard.h>

#ifdef __cplusplus
extern "C" {
#endif
//////////////////// MEMORY LAYOUT AND SIZE //////////////////////////////////////////
#define ROM_INIT    (0x200)
#define FONTS_INIT  (0x050)
#define MEMORY_SIZE (1 << 12)
#define STACK_SIZE  (24)

typedef enum {
    CHIP8_V0,
    CHIP8_V1,
    CHIP8_V2,
    CHIP8_V3,
    CHIP8_V4,
    CHIP8_V5,
    CHIP8_V6,
    CHIP8_V7,
    CHIP8_V8,
    CHIP8_V9,
    CHIP8_VA,
    CHIP8_VB,
    CHIP8_VC,
    CHIP8_VD,
    CHIP8_VE,
    CHIP8_VF,
    VX_COUNT
}registers_t;


//////////////////// OPCODE UTILS ////////////////////////////////////////////////////
#define OPCODE_SIZE         (4)
#define OPCODE_SHIFT        (12)
#define OPCODE_MASK         ((1 << OPCODE_SIZE) -1)
#define OPCODE_GET(opcode)  ((opcode >> OPCODE_SHIFT) & OPCODE_MASK)

#define VX_SIZE             (4)
#define VX_SHIFT            (8)
#define VX_MASK             ((1 << VX_SIZE) -1)
#define VX_GET(mi)          ((mi >> VX_SHIFT) & VX_MASK)
#define VY_SIZE             VX_SIZE
#define VY_SHIFT            (4)
#define VY_MASK             ((1 << VY_SIZE) -1)
#define VY_GET(mi)          ((mi >> VY_SHIFT) & VY_MASK)

#define ADDR_GET(mi)        ((mi & 0xFFF))
#define IMME_GET(mi)        ((mi & 0xFF))
#define NIBBLE_GET(mi)      ((mi & 0xF))

//////////////////// CHIP-8 VM definition ////////////////////////////////////////////
typedef struct{
    uint8_t *rom;
    size_t rom_size;
    chip8_display_handler display_handler;
    chip8_keyboard_poll keyboard_poll;
    bool log_enable;
    chip8_logtype_t log_type;
    const char* log_filename;
}chip8_config_t;

typedef struct{
    uint8_t memory[MEMORY_SIZE];
    uint16_t stack[STACK_SIZE];
    int sp;                         // stack pointer index, -1 means empty
    uint8_t *pc;
    uint8_t registers[VX_COUNT];
    uint16_t index;
    uint8_t delay_timer;
    uint8_t sound_timer;
} chip8_vm_t;


//////////////////// API /////////////////////////////////////////////////////////////
void chip8_init(chip8_config_t *);
void chip8_load_fonts(void);
void chip8_load_memory(uint8_t *,size_t);
void chip8_dump_memory(void);
void chip8_clock_60hz();
void chip8_step();

#ifdef __cplusplus
}
#endif
#endif // __CHIP8_H__