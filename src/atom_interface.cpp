/**
 * Software License Agreement CC0
 *
 * \file      atom_interface.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "atom_interface.h"
#include "matrix_interface.h"

using namespace LuaInterface;

std::string AtomInterface::typeName()
{
  return "Atom";
}

uint32_t AtomInterface::hash()
{
  return COMPILE_TIME_CRC32_STR("Atom");
}

static void kwargs(lua_State* L, int idx, Atom::Ptr p)
{
  lua_getfield(L, idx, "name");
  if (!lua_isnil(L, -1))
  {
    p->name_ = lua_tostring(L, -1);
  }
  lua_pop(L, 1);

  lua_getfield(L, idx, "type");
  if (!lua_isnil(L, -1))
  {
    p->type_ = lua_tostring(L, -1);
  }
  lua_pop(L, 1);

  lua_getfield(L, idx, "position");
  p->pos_ = MatrixInterface::as(L, -1);
  lua_pop(L, 1);
}

int AtomInterface::l_new(lua_State* L)
{
  Atom::Ptr p(new Atom());

  if (luaT_is<Atom>(L, 1))
  {
    p = luaT_to<Atom>(L, 1)->copy();
  }

  if (lua_istable(L, 1))
  {
    kwargs(L, 1, p);
  }

  return luaT_push(L, p);
}

static int l_setname(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  a->name_ = lua_tostring(L, 2);
  return 0;
}

static int l_getname(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  lua_pushstring(L, a->name_.c_str());
  return 1;
}

static int l_settype(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  a->type_ = lua_tostring(L, 2);
  return 0;
}

static int l_gettype(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  lua_pushstring(L, a->type_.c_str());
  return 1;
}

static int l_setpos(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  Matrix::Ptr p = MatrixInterface::as(L, 2);

  a->pos_ = p;
  return 0;
}

static int l_getpos(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  luaT_push(L, a->pos_);
  return 1;
}

static int l_tostring(lua_State* L)
{
  Atom::Ptr a = luaT_to<Atom>(L, 1);

  if (!a)
  {
    return luaL_argerror(L, 1, "Atom Expected");
  }

  lua_getglobal(L, "tostring");
  luaT_push(L, a->pos_);
  lua_call(L, 1, 1);

  std::string pos_string = lua_tostring(L, -1);
  lua_pop(L, 1);

  lua_pushfstring(L,
                  "Atom({name = \"%s\", "
                        "type = \"%s\", "
                        "pos = %s})",
                  a->name_.c_str(), a->type_.c_str(),
                  pos_string.c_str());
  return 1;
}

std::vector<luaL_Reg> AtomInterface::luaMethods()
{
  std::vector<luaL_Reg> methods;

  methods.push_back(luaL_toreg("setName", l_setname));
  methods.push_back(luaL_toreg("name", l_getname));
  methods.push_back(luaL_toreg("getType", l_settype));
  methods.push_back(luaL_toreg("type", l_gettype));
  methods.push_back(luaL_toreg("getPosition", l_setpos));
  methods.push_back(luaL_toreg("position", l_getpos));

  methods.push_back(luaL_toreg("__tostring", l_tostring));

  return methods;
}
