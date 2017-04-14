/**
 * Software License Agreement BSD2
 *
 * \file      check_if_type_has_l_fromtable.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 * \copyright Copyright (c) 2017, Jason Mercer, All rights reserved.
 *
 * This Software is licensed under BSD 2-clause, see BSD2.txt in the licenses
 * directory.
 */

#ifndef CHECK_IF_TYPE_HAS_L_FROMTABLE_H
#define CHECK_IF_TYPE_HAS_L_FROMTABLE_H

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

namespace LNewCheck
{
// get l_fromtable if flag is true:
template <typename T, bool>
struct get_l_fromtable;

template <typename T>
struct get_l_fromtable<T,true>
{
  static lua_CFunction exec()
  {
    return T::l_fromtable;
  }
};

template <typename T>
struct get_l_fromtable<T,false>
{
  static lua_CFunction exec()
  {
    return 0;
  }
};

// SFINAE test for l_fromtable in T
template<typename T>
struct has_l_fromtable
{
  struct Fallback { int l_fromtable; };
  struct Derived : T, Fallback { };

  template<typename C, C> struct ChT;

  template<typename C>
  static char (&f(ChT<int Fallback::*, &C::l_fromtable>*))[1];

  template<typename C>
  static char (&f(...))[2];

  static bool const value = sizeof(f<Derived>(0)) == 2;
};
}

template <typename T>
inline lua_CFunction get_l_fromtable()
{
  return LNewCheck::get_l_fromtable<T, LNewCheck::has_l_fromtable<T>::value>::exec();
}

}
}

#endif // CHECK_IF_TYPE_HAS_L_FROMTABLE_H

