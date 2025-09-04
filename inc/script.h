#ifndef __CHIP8_SCRIPT_H__
#define __CHIP8_SCRIPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <chip8.h>

const char *chip8_lua_get_string(lua_State *, const char*, uint8_t *);
bool chip8_lua_get_boolean(lua_State *, const char*);
int chip8_lua_get_integer(lua_State *, const char*);
void chip8_script_load_conf(const char*, global_conf_t *);
void chip8_script_run(const char*);

#ifdef __cplusplus
}
#endif
#endif //__CHIP8_SCRIPT_H__