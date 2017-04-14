#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "luainterface/luainterface.h"

/**
 * @brief Add the 'interactive' function to the lua state
 * @param L lua state
 */
void register_interactive(lua_State* L);

#endif // INTERACTIVE_H
