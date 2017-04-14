/**
 * Software License Agreement BSD2
 *
 * \file      check_if_type_has_l_luafunctions.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 * \copyright Copyright (c) 2017, Jason Mercer, All rights reserved.
 *
 * This Software is licensed under BSD 2-clause, see BSD2.txt in the licenses
 * directory.
 */

#ifndef CHECK_IF_TYPE_HAS_LUAFUNCTIONS_H
#define CHECK_IF_TYPE_HAS_LUAFUNCTIONS_H

#include <vector>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace LuaInterface
{
namespace Private
{

// call luaFunctions if flag is true:
template <typename T, bool>
struct call_luaFunctions;

template <typename T>
struct call_luaFunctions<T,true>
{
  static std::vector<luaL_Reg> exec()
  {
    return T::luaFunctions();
  }
};

template <typename T>
struct call_luaFunctions<T,false>
{
  static std::vector<luaL_Reg> exec()
  {
    return std::vector<luaL_Reg>();
  }
};

// SFINAE test for luaFunctions in T
template<typename T>
struct has_luaFunctions
{
  struct Fallback { int luaFunctions; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C>
  static char (&f(ChT<int Fallback::*, &C::luaFunctions>*))[1];

  template<typename C>
  static char (&f(...))[2];

  static bool const value = sizeof(f<Derived>(0)) == 2;
};

template <typename T>
inline std::vector<luaL_Reg> get_luaFunctions()
{
  return call_luaFunctions<T, has_luaFunctions<T>::value>::exec();
}

}
}
#endif // CHECK_IF_TYPE_HAS_LUAFUNCTIONS_H

