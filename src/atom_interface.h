/**
 * Software License Agreement CC0
 *
 * \file      atom_interface.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef ATOMINTERFACE_H
#define ATOMINTERFACE_H

#include "atom.h"
#include <luainterface/luainterface.h>

class AtomInterface
{
public:
  static std::string typeName();
  static uint32_t hash();
  static int l_new(lua_State* L);

  static std::vector<luaL_Reg> luaMethods();
};

SpecializeInterface(Atom, AtomInterface)

#endif // ATOMINTERFACE_H
