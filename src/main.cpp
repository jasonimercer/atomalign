/**
 * Software License Agreement CC0
 *
 * \file      main.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <luainterface/luainterface.h>
#include "atomcontainer_interface.h"
#include "atom_interface.h"
#include "matrix_interface.h"
#include "matrix_interface.h"
#include "interactive.h"
#include <stdio.h>

int main(int argc, char** argv)
{
  lua_State* L = luaL_newstate();
  luaL_openlibs(L);

  LuaInterface::luaT_register<Matrix::Type>(L);
  LuaInterface::luaT_register<Atom>(L);
  LuaInterface::luaT_register<AtomContainer>(L);

  register_interactive(L);

  if (argc > 1)
  {
    if(luaL_dofile(L, argv[1]))
    {
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
    }
  }
  else
  {
    fprintf(stderr, "Please supply a script\n");
  }

  lua_close(L);
  return 0;
}
