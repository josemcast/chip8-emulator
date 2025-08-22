#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <chip8.h>
#include <display.h>
#include <keyboard.h>
#include <log.h>
#include <font.h>
#include <utilities.h>


static chip8_vm_t vm;   
static bool running = 0;

////////////////////////// Initialization functions /////////////////////////////////////////////////
void chip8_init(chip8_config_t *config){
    vm = (chip8_vm_t){0};
    vm.sp = -1;
    
    init_log(config->log_enable, config->log_type, config->log_filename);

    chip8_load_fonts();
    chip8_load_memory(config->rom, config->rom_size);
    
    vm.pc = (vm.memory + ROM_INIT);
    memset(vm.registers,0, VX_COUNT);
    vm.index = 0x0000;
    
    running = 1;

    init_display(config->display_handler);
    init_keyboard(config->keyboard_handler, config->keyboard_poll);       
}

void chip8_load_memory(uint8_t *bin, size_t size){
    CHIP8_TRACELOG(CHIP8_LOG_INFO, "Loading binary into memory...Start addr 0x%x with size 0x%lx\n", ROM_INIT, size);
    uint8_t *dst = (vm.memory + ROM_INIT);
    uint8_t *src = (uint8_t*)bin;
    uint16_t i = 0;
    while(i < ( size)){
        *dst++ = *src++;
        ++i;
    }
}

void chip8_load_fonts(){
    CHIP8_TRACELOG(CHIP8_LOG_INFO, "Loading fonts at 0x%x\n", FONTS_INIT);
    uint8_t *dst = (vm.memory + FONTS_INIT);
    uint8_t *src = chip8_fonts;
    uint16_t i = 0;
    while(i < (ARRAY_LEN(chip8_fonts, uint8_t))){
        *dst++ = *src++;
        ++i;
    }
}

////////////////////////// Debug functions //////////////////////////////////////////////////////////
void chip8_dump_memory(){
    FILE *fp = fopen("chip8_memdump.bin", "wb");
    if(fp == NULL){
        CHIP8_TRACELOG(CHIP8_LOG_ERROR,"Could not open file to dump memory\n");
        return;
    }

    fwrite(vm.memory, 1, MEMORY_SIZE, fp);
    fclose(fp);
}

void chip8_dump_state(){
    
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "==========================================================\n");
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "PC:%3lX\n", (vm.pc - vm.memory));
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "I:%3X\n", vm.index);
    for (int i = 0; i < VX_COUNT; i++){
        if(i % 4 == 0) CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "\n");
        CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "V%1X:%3X ", i, vm.registers[i]);
    }
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "\nstack:");
    for(int i = 0; i<STACK_SIZE; i++)
    {
        if (i % 8 == 0) CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "\n");
        CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "%3X ", vm.stack[i]);
    }
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "\n==========================================================\n");
}

////////////////////////// CPU functions ////////////////////////////////////////////////////////////
static uint16_t chip8_fetch(){
    uint16_t op = (uint16_t)*(vm.pc++); 
    return (op << 8) | *(vm.pc++);
}

static void chip8_stack_push(uint16_t nnn){
    if((vm.sp + 1) == STACK_SIZE){
        CHIP8_TRACELOG(CHIP8_LOG_ERROR, "Stack is full\n");
        exit(1);
    }
    vm.stack[++vm.sp] = nnn;
}

static uint16_t chip8_stack_pop(){
    if(vm.sp == -1){
        CHIP8_TRACELOG(CHIP8_LOG_ERROR, "Stack is empty\n");
        exit(1);
    }
    return vm.stack[vm.sp--];
}

static void opcode0_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 0 - %X\n", mi);
    uint8_t mode = IMME_GET(mi);
    switch (mode)
    {
        case 0xE0:
            clear_display();
            break;
        case 0xEE:
            vm.pc = (vm.memory + chip8_stack_pop());
            break;
        default:
            CHIP8_TRACELOG(CHIP8_LOG_ERROR, "not implemented yet\n");
            break;
    }

}

//jump to NNN
static void opcode1_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 1 - %X\n", mi);
    vm.pc = (vm.memory + ADDR_GET(mi));
}

static void opcode2_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 2 - %X\n", mi);
    chip8_stack_push((vm.pc - vm.memory));
    vm.pc = (vm.memory + ADDR_GET(mi));
}

//Skip next instruction if VX == NN
static void opcode3_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 3 - %X\n", mi);
    if(vm.registers[VX_GET(mi)] == IMME_GET(mi))
        vm.pc += 2;
}

//Skip next instruction if VX != NN
static void opcode4_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 4 - %X\n", mi);
    if(vm.registers[VX_GET(mi)] != IMME_GET(mi))
        vm.pc += 2;
}

//Skip next instruction if VX == VY
static void opcode5_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 5 - %X\n", mi);
    if(vm.registers[VX_GET(mi)] == vm.registers[VY_GET(mi)])
        vm.pc += 2;
}

//set VX to NN
static void opcode6_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 6 - %X\n", mi);
    vm.registers[VX_GET(mi)] = IMME_GET(mi);
}

//Add NN to VX
static void opcode7_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 7 - %X\n", mi);
    vm.registers[VX_GET(mi)] += IMME_GET(mi);
}

//Add NN to VX
static void opcode8_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 8 - %X\n", mi);
    uint8_t mode = NIBBLE_GET(mi);
    switch (mode)
    {
        case 0x0: // Set VX to value of VY
            vm.registers[VX_GET(mi)] = vm.registers[VY_GET(mi)];
            break;
        case 0x1: // Bitwise OR -> VX = VX | VY
            vm.registers[VX_GET(mi)] = (vm.registers[VX_GET(mi)] | vm.registers[VY_GET(mi)]);
            break;
        case 0x2: // Bitwise AND -> VX = VX & VY
            vm.registers[VX_GET(mi)] = (vm.registers[VX_GET(mi)] & vm.registers[VY_GET(mi)]);
            break;
        case 0x3: // Bitwise XOR -> VX = VX ^ VY
            vm.registers[VX_GET(mi)] = (vm.registers[VX_GET(mi)] ^ vm.registers[VY_GET(mi)]);
            break;
        case 0x4: // VX = VX + VY with carry flag set on VF
            if (vm.registers[VX_GET(mi)] > (UINT8_MAX - vm.registers[VY_GET(mi)]))
                vm.registers[CHIP8_VF] = 1;
            else
                vm.registers[CHIP8_VF] = 0;

            vm.registers[VX_GET(mi)] = (vm.registers[VX_GET(mi)] + vm.registers[VY_GET(mi)]);
            break;
        case 0x5: // VX = VX - VY with VF set to 1 or zero depending on which value is large
            if (vm.registers[VX_GET(mi)] > vm.registers[VY_GET(mi)])
                vm.registers[CHIP8_VF] = 1;
            else
                vm.registers[CHIP8_VF] = 0;

            vm.registers[VX_GET(mi)] = (vm.registers[VX_GET(mi)] - vm.registers[VY_GET(mi)]);
            break;
        case 0x6: // VX = (VY >> 1) with VF set to bit shifted out
            vm.registers[CHIP8_VF] = vm.registers[VY_GET(mi)] & 0x1;
            vm.registers[VX_GET(mi)] = (vm.registers[VY_GET(mi)] >> 1);
            break;  
        case 0x7: // VX = VX - VY with VF set to 1 or zero depending on which value is large
            if (vm.registers[VY_GET(mi)] > vm.registers[VX_GET(mi)])
                vm.registers[CHIP8_VF] = 1;
            else
                vm.registers[CHIP8_VF] = 0;

            vm.registers[VX_GET(mi)] = (vm.registers[VY_GET(mi)] - vm.registers[VX_GET(mi)]);
            break;
        case 0xE: // VX = (VY << 1) with VF set to bit shifted out
            vm.registers[CHIP8_VF] = (vm.registers[VY_GET(mi)] >> 7) & 0x1;
            vm.registers[VX_GET(mi)] = (vm.registers[VY_GET(mi)] << 1);
            break;  
        default:
            break;
    }

}

//Skip next instruction if VX != VY
static void opcode9_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode 9 - %X\n", mi);
    if(vm.registers[VX_GET(mi)] != vm.registers[VY_GET(mi)])
        vm.pc += 2;
}

//Set index to NNN
static void opcodeA_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode A - %X\n", mi);
    vm.index = ADDR_GET(mi);
}

//jump to NNN + offset
static void opcodeB_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode B - %X\n", mi);
    vm.pc = (vm.memory + ADDR_GET(mi) + vm.memory[CHIP8_V0]);
}

//Set VX to RAND AND NN
static void opcodeC_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode C - %X\n", mi);
    vm.registers[VX_GET(mi)] = RANDI() & IMME_GET(mi);
}

//Display
static void opcodeD_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode D - %X\n", mi);
    uint8_t vx = vm.registers[VX_GET(mi)];
    uint8_t vy = vm.registers[VY_GET(mi)];
    uint8_t n = NIBBLE_GET(mi);
    uint8_t collision = 0;
    for(int i = 0; i < n; i++){
        collision = set_display(vx, vy+i, (vm.memory[vm.index+i]));
        vm.registers[CHIP8_VF] = collision;
    }
}

//Skip if key
static void opcodeE_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode E - %X\n", mi);
    uint8_t mode = IMME_GET(mi);
    switch (mode)
    {
        case 0x9E: //skip if key in VX is pressed
            poll_keyboard();
            printf("aqui\n");
            if(is_keycode_pressed(vm.registers[VX_GET(mi)]))
                vm.pc += 2;
            break;
        case 0xA1: //skip if key in VX is NOT pressed
            poll_keyboard();
            if(!is_keycode_pressed(vm.registers[VX_GET(mi)]))
                vm.pc += 2;
            break;
        default:
            break;
    }
}

//xF opcodes
static void opcodeF_handler(uint16_t mi){
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "Opcode F - %X\n", mi);
    uint8_t mode = IMME_GET(mi);
    switch (mode)
    {
        case 0x0A: // wait for key
            while(1){
                poll_keyboard();
                if(is_keycode_pressed(CHIP8_KEYCODE_0)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_0;
                    break;        
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_1)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_1;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_2)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_2;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_3)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_3;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_4)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_4;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_5)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_5;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_6)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_6;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_7)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_7;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_8)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_8;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_9)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_9;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_A)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_A;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_B)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_B;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_C)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_C;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_D)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_D;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_E)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_E;
                    break;
                }
                if(is_keycode_pressed(CHIP8_KEYCODE_F)){
                    vm.registers[VX_GET(mi)] = CHIP8_KEYCODE_F;
                    break;
                }
            }
            break;
        case 0x29:
            uint8_t key = vm.registers[VX_GET(mi)] & 0xF;
            vm.index = FONTS_INIT + key * FONT_HEIGHT;
            break;
        case 0xFF:
            exit(0);
            break;
        default:
            break;
    }
}

void (*process_opcodes[])(uint16_t) = {
    opcode0_handler,
    opcode1_handler,
    opcode2_handler,
    opcode3_handler,
    opcode4_handler,
    opcode5_handler,
    opcode6_handler,
    opcode7_handler,
    opcode8_handler,
    opcode9_handler,
    opcodeA_handler,
    opcodeB_handler,
    opcodeC_handler,
    opcodeD_handler,
    opcodeE_handler,
    opcodeF_handler
};

void chip8_step() {

    if (!running) {return;}

    uint16_t mi = chip8_fetch();
    uint8_t opcode = OPCODE_GET(mi);

    if ((opcode >= 0xFE)){
        deinit_log();
        return;
    }

    process_opcodes[opcode](mi);
    chip8_dump_state();
    show_display();
}
