#include "main_loop.h"
#include "lua536.h"

int setup(void) {
    luaState = luaL_newstate();
    luaL_openlibs(luaState);

    return 0;
}

int cleanup(void) {
    lua_close(luaState);

    return 0;
}

int main(void) {
    setup();
    main_loop();
    cleanup();

    return 0;
}
