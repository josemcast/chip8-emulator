#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <script.h>
#include <chip8.h>

extern chip8_vm_t vm;

static int chip8_getPC(lua_State *L) {
    uint8_t pc = (uint8_t)(vm.pc - vm.memory);
    lua_pushnumber(L, pc);
    return 1;
}

static int chip8_getRegister(lua_State *L) {
    uint8_t reg = luaL_checkinteger(L, 1);
    if(reg > 0xF)
        lua_pushnil(L);
    else
        lua_pushnumber(L, vm.registers[reg]);

    return 1;
}

const luaL_Reg chip8_clbck[] = {
    {.name = "getPC", chip8_getPC},
    {.name = "getRegister", chip8_getRegister},
    {NULL, NULL},
};

static int chip8_lib(lua_State *L) {
    luaL_newlib(L, chip8_clbck);
    return 1;
}

void chip8_script_run(const char* script){
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaL_requiref(L, "chip8", chip8_lib, 1);

    if(luaL_dofile(L, script) != LUA_OK) {
        fprintf(stderr,"Could not load config file: %s\n", lua_tostring(L, -1));
        lua_close(L);
        exit(1);
    }

    lua_close(L);
}