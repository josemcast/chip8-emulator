#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <script.h>
#include <chip8.h>

///////////////// HELPERS ///////////////////////////////////////////////////////////////////////////////
const char *chip8_lua_get_string(lua_State * L, const char *field, uint8_t *buffer) {
    lua_pushstring(L, field);
    lua_gettable(L, -2);
    const char *str = "\0";
    if (lua_isstring(L, -1) & !lua_isnumber(L, -1))
        str = lua_tostring(L, -1);
    
    memcpy(buffer, str, strlen(str));
    lua_pop(L, 1);
    return str;
}

bool chip8_lua_get_boolean(lua_State *L, const char *field) {
    lua_pushstring(L, field);
    lua_gettable(L, -2);
    bool ret = lua_isboolean(L, -1) ? lua_toboolean(L, -1): false;
    lua_pop(L, 1);
    return ret;
}

int chip8_lua_get_integer(lua_State *L, const char *field) {
    lua_pushstring(L, field);
    lua_gettable(L, -2);
    int ret =  lua_isinteger(L, -1) ? lua_tointeger(L, -1): 0;
    lua_pop(L, 1);
    return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

extern chip8_vm_t vm;

// chip8.getPC()
static int chip8_getPC(lua_State *L) {
    uint8_t pc = (uint8_t)(vm.pc - vm.memory);
    lua_pushnumber(L, pc);
    return 1;
}

// chip8.getRegister(VX)
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