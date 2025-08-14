#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <chip8.h>
#include <display.h>
#include <log.h>
#include <font.h>
#include <utilities.h>


static chip8_vm_t vm;   
static bool running = 0;

void chip8_init(bool log_enable, log_type_t lt, uint8_t *bin, size_t size){
    vm = (chip8_vm_t){0};
    vm.sp = -1;
    
    printf("\n\n");
    chip8_load_fonts();
    chip8_load_memory(bin, size);
    
    vm.pc = (vm.memory + ROM_INIT);
    memset(vm.registers,0, VX_COUNT);
    vm.index = 0x0000;
    
    running = 1;

    init_screen();
    if (log_enable == true)
        init_log(lt);       
}

chip8_vm_t *chip8_get_state(){
    return &vm;
}

void chip8_load_memory(uint8_t *bin, size_t size){
    printf("Loading binary into memory...Start addr 0x%x with size 0x%lx\n", ROM_INIT, size);
    uint8_t *dst = (vm.memory + ROM_INIT);
    uint8_t *src = (uint8_t*)bin;
    uint16_t i = 0;
    while(i < ( size)){
        *dst++ = *src++;
        ++i;
    }
}

void chip8_load_fonts(){
    printf("Loading fonts at 0x%x\n", FONTS_INIT);
    uint8_t *dst = (vm.memory + FONTS_INIT);
    uint8_t *src = chip8_fonts;
    uint16_t i = 0;
    while(i < (ARRAY_LEN(chip8_fonts, uint8_t))){
        *dst++ = *src++;
        ++i;
    }
}

void chip8_dump_memory(){
    FILE *fp = fopen("chip8_memdump.bin", "wb");
    if(fp == NULL){
        printf("Could not open file to dump memory\n");
        return;
    }

    fwrite(vm.memory, 1, MEMORY_SIZE, fp);
    fclose(fp);
}

static uint16_t chip8_fetch(){
    uint16_t op = (uint16_t)*(vm.pc++); 
    return (op << 8) | *(vm.pc++);
}

static void opcode0_handler(uint16_t mi){
    log_debug("opcode 0");
    uint8_t mode = IMME_GET(mi);
    switch (mode)
    {
        case 0xE0:
            clear_screen();
            break;
        default:
            printf("not implemented yet\n");
            break;
    }

}

//jump to NNN
static void opcode1_handler(uint16_t mi){
    log_debug("opcode 1");
    vm.pc = (vm.memory + ADDR_GET(mi));
}

static void opcode2_handler(uint16_t mi){
    log_debug("opcode 2");
}

static void opcode3_handler(uint16_t mi){
    log_debug("opcode 3");
}

static void opcode4_handler(uint16_t mi){
    log_debug("opcode 4");
}

static void opcode5_handler(uint16_t mi){
    log_debug("opcode 5");
}

//set VX to NN
static void opcode6_handler(uint16_t mi){
    log_debug("opcode 6");
    vm.registers[VX_GET(mi)] = IMME_GET(mi);
}

//Add NN to VX
static void opcode7_handler(uint16_t mi){
    log_debug("opcode 7");
    vm.registers[VX_GET(mi)] += IMME_GET(mi);
}

//Set index to NNN
static void opcodeA_handler(uint16_t mi){
    log_debug("opcode A");
    vm.index = ADDR_GET(mi);
}

//Display
static void opcodeD_handler(uint16_t mi){
    log_debug("opcode D");
    uint8_t vx = vm.registers[VX_GET(mi)];
    uint8_t vy = vm.registers[VY_GET(mi)];
    uint8_t n = NIBBLE_GET(mi);

    for(int i = 0; i < n; i++){
        set_screen(vx, vy+i, (vm.memory[vm.index+i]));
    }
}

//placeholder for future opcode handlers
static void opcodeN_handler(uint16_t mi)
{
    //TO DO
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
    opcodeN_handler,
    opcodeN_handler,
    opcodeA_handler,
    opcodeN_handler,
    opcodeN_handler,
    opcodeD_handler,
    opcodeN_handler,
    opcodeN_handler
};

int chip8_run() {

    if (!running)
        return 1;

    printf("\nRunning...\n");

    while(1){
        uint16_t mi = chip8_fetch();
        uint8_t opcode = OPCODE_GET(mi);

        if ((opcode >= 0x0F)){
            deinit_log();
            return 1;
        }

        process_opcodes[opcode](mi);
    }
}

void chip8_step() {

    if (!running)
        return;

    uint16_t mi = chip8_fetch();
    uint8_t opcode = OPCODE_GET(mi);

    if ((opcode >= 0x0F)){
        deinit_log();
        return;
    }

    process_opcodes[opcode](mi);
}
