/**
 * Software License Agreement CC0
 *
 * \file      matrix_interface.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#ifndef MATRIX_INTERFACE_H
#define MATRIX_INTERFACE_H

#include <luainterface/luainterface.h>
#include "matrix.h"

class MatrixInterface
{
public:
  static std::string typeName();
  static uint32_t hash();
  static int l_new(lua_State* L);

  static std::vector<luaL_Reg> luaMethods();
  static std::vector<luaL_Reg> luaFunctions();

  static Matrix::Ptr as(lua_State* L, int idx);
};

SpecializeInterface(Matrix::Type, MatrixInterface)

#endif // MATRIX_INTERFACE_H
