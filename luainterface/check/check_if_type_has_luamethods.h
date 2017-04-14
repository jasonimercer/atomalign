/**
 * Software License Agreement BSD2
 *
 * \file      check_if_type_has_l_luamethods.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 * \copyright Copyright (c) 2017, Jason Mercer, All rights reserved.
 *
 * This Software is licensed under BSD 2-clause, see BSD2.txt in the licenses
 * directory.
 */

#ifndef CHECK_IF_TYPE_HAS_LUAMETHODS_H
#define CHECK_IF_TYPE_HAS_LUAMETHODS_H

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

// call luaMethods if flag is true:
template <typename T, bool>
struct call_luaMethods;

template <typename T>
struct call_luaMethods<T,true>
{
  static std::vector<luaL_Reg> exec()
  {
    return T::luaMethods();
  }
};

template <typename T>
struct call_luaMethods<T,false>
{
  static std::vector<luaL_Reg> exec()
  {
    return std::vector<luaL_Reg>();
  }
};

// SFINAE test for luaMethods in T
template<typename T>
struct has_luaMethods
{
  struct Fallback { int luaMethods; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C>
  static char (&f(ChT<int Fallback::*, &C::luaMethods>*))[1];

  template<typename C>
  static char (&f(...))[2];

  static bool const value = sizeof(f<Derived>(0)) == 2;
};

template <typename T>
inline std::vector<luaL_Reg> get_luaMethods()
{
  return call_luaMethods<T, has_luaMethods<T>::value>::exec();
}

}
}
#endif // CHECK_IF_TYPE_HAS_LUAMETHODS_H

