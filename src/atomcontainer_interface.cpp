/**
 * Software License Agreement CC0
 *
 * \file      atomcontainer_interface.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include "atomcontainer_interface.h"
#include "atom_interface.h"
#include "matrix_interface.h"

using namespace LuaInterface;

std::string AtomContainerInterface::typeName()
{
  return "AtomContainer";
}

uint32_t AtomContainerInterface::hash()
{
  return COMPILE_TIME_CRC32_STR("AtomContainer");
}

int AtomContainerInterface::l_new(lua_State* L)
{
  AtomContainer::Ptr ac(new AtomContainer());

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  if (lua_istable(L, 1))
  {
    lua_pushnil(L);

    while (lua_next(L, 1))
    {
      ac->atoms_.push_back(luaT_to<Atom>(L, -1));
      lua_pop(L, 1);
    }
  }

  for (int i = 1; i <= lua_gettop(L); i++)
  {
    if (luaT_is<Atom>(L, i))
    {
      ac->atoms_.push_back(luaT_to<Atom>(L, i));
    }
  }

  return luaT_push(L, ac);
}

static int l_at(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  // shifting index by -1 to convert from base 1 to base zero
  size_t idx = lua_tointeger(L, 2) - 1;

  if (idx >= ac->atoms_.size())
  {
    return luaL_error(L, "Invalid index");
  }

  luaT_push<Atom>(L, ac->atoms_.at(idx));
  return 1;
}

static int l_clear(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  ac->atoms_.clear();
  return 0;
}

static int l_near(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);
  Matrix::Ptr p = MatrixInterface::as(L, 2);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  if (!p)
  {
    return luaL_error(L, "Test point expected");
  }

  size_t idx;
  double dist2;

  if (ac->near(p, idx, dist2))
  {
    lua_pushinteger(L, idx + 1);
    lua_pushnumber(L, sqrt(fabs(dist2)));
    return 2;
  }

  return 0;
}

static int l_intersect(lua_State* L)
{
  AtomContainer::Ptr acSrc = luaT_to<AtomContainer>(L, 1);
  AtomContainer::Ptr acPattern = luaT_to<AtomContainer>(L, 2);
  double tol = lua_tonumber(L, 3);

  acSrc->intersect(*acPattern, tol);
  return 0;
}

static int l_intersected(lua_State* L)
{
  // replacing the atom container with a copy of it
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);
  luaT_push(L, ac->copy());
  lua_replace(L, 1);

  l_intersect(L);
  lua_settop(L, 1);  // remove all but the intersected copy
  return 1;
}

static int l_size(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  lua_pushinteger(L, ac->atoms_.size());
  return 1;
}

static int l_add(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);
  Atom::Ptr a = luaT_to<Atom>(L, 2);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  if (!a)
  {
    return luaL_argerror(L, 2, "Atom expected");
  }

  ac->atoms_.push_back(a);
  return 0;
}

static int l_copy(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  luaT_push(L, ac->copy());
  return 1;
}

static int l_filter(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  if (!ac)
  {
    return luaL_error(L, "AtomContainer expected");
  }

  if (!lua_isfunction(L, 2))
  {
    return 0;
  }

  std::vector<Atom::Ptr> atoms;

  for (size_t i = 0; i < ac->atoms_.size(); i++)
  {
    lua_pushvalue(L, 2);
    luaT_push(L, ac->atoms_[i]->copy());
    lua_call(L, 1, 1);

    if (lua_toboolean(L, -1))
    {
      atoms.push_back(ac->atoms_[i]->copy());
    }

    lua_pop(L, 1);
  }

  ac->atoms_ = atoms;
  return 0;
}

static int l_filtered(lua_State* L)
{
  // replacing the atom container with a copy of it
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);
  luaT_push(L, ac->copy());
  lua_replace(L, 1);

  // we now have a copy of the container and hopefully a function
  l_filter(L);  // filter the copy
  lua_settop(L, 1);  // remove all but the filtered copy
  return 1;
}

static void get_number(lua_State* L, int tab_idx, const char* key, double& dest)
{
  if (lua_getfield(L, tab_idx, key) != LUA_TNIL)
  {
    dest = lua_tonumber(L, -1);
  }
  lua_pop(L, 1);
}

static void get_number(lua_State* L, int tab_idx, const char* key, int& dest)
{
  if (lua_getfield(L, tab_idx, key) != LUA_TNIL)
  {
    dest = lua_tointeger(L, -1);
  }
  lua_pop(L, 1);
}

template <int forward>
int l_transform(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  double dx = lua_tonumber(L, 2);
  double dy = lua_tonumber(L, 3);
  double dz = lua_tonumber(L, 4);

  double rx = lua_tonumber(L, 5);
  double ry = lua_tonumber(L, 6);
  double rz = lua_tonumber(L, 7);

  if (lua_istable(L, 2))
  {
    get_number(L, 2, "dx", dx);
    get_number(L, 2, "dy", dy);
    get_number(L, 2, "dz", dz);

    get_number(L, 2, "rx", rx);
    get_number(L, 2, "ry", ry);
    get_number(L, 2, "rz", rz);
  }

  if (forward == 1)
  {
    ac->transform(dx, dy, dz, rx, ry, rz);
  }
  else
  {
    ac->untransform(dx, dy, dz, rx, ry, rz);
  }
  return 0;
}

template <int forward>
int l_transformed(lua_State* L)
{
  // replacing the atom container with a copy of it
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);
  luaT_push(L, ac->copy());
  lua_replace(L, 1);

  l_transform<forward>(L);
  lua_settop(L, 1);  // remove all but the transformed copy
  return 1;
}

static int l_closest_dist_squared(lua_State* L)
{
  AtomContainer::Ptr ac1 = luaT_to<AtomContainer>(L, 1);
  AtomContainer::Ptr ac2 = luaT_to<AtomContainer>(L, 2);

  if (!ac1 || !ac2)
  {
    return luaL_error(L, "Atom containers expected");
  }

  const double d2 = AtomContainer::closestDistanceSquared(*ac1, *ac2);

  lua_pushnumber(L, d2);

  return 1;
}

static int l_align(lua_State* L)
{
  AtomContainer::Ptr ac1 = luaT_to<AtomContainer>(L, 1);
  AtomContainer::Ptr ac2 = luaT_to<AtomContainer>(L, 2);

  if (!ac1 || !ac2)
  {
    return luaL_error(L, "Atom containers expected");
  }

  std::vector<double> X;
  X.resize(6, 0);

  int steps = 200;
  double rho_begin = 1e1;
  double rho_end = 1e-3;

  // letting the user supply multiple tables with params
  for (int i = 3; i <= lua_gettop(L); i++)
  {
    if (lua_istable(L, i))
    {
      get_number(L, i, "dx", X[0]);
      get_number(L, i, "dy", X[1]);
      get_number(L, i, "dz", X[2]);

      get_number(L, i, "rx", X[3]);
      get_number(L, i, "ry", X[3]);
      get_number(L, i, "rz", X[5]);

      get_number(L, i, "rho_begin", rho_begin);
      get_number(L, i, "rho_end", rho_end);
      get_number(L, i, "steps", steps);
    }
  }

  double diff = AtomContainer::align(ac1, ac2,
                                     X[0], X[1], X[2], X[3], X[4], X[5],
                                     rho_begin, rho_end, steps);

  lua_newtable(L);
  const int tab_pos = lua_gettop(L);

  lua_pushnumber(L, X[0]);
  lua_setfield(L, tab_pos, "dx");

  lua_pushnumber(L, X[1]);
  lua_setfield(L, tab_pos, "dy");

  lua_pushnumber(L, X[2]);
  lua_setfield(L, tab_pos, "dz");

  lua_pushnumber(L, X[3]);
  lua_setfield(L, tab_pos, "rx");

  lua_pushnumber(L, X[4]);
  lua_setfield(L, tab_pos, "ry");

  lua_pushnumber(L, X[5]);
  lua_setfield(L, tab_pos, "rz");

  lua_pushnumber(L, diff);
  lua_setfield(L, tab_pos, "diff");

  return 1;
}

static int l_tostring(lua_State* L)
{
  AtomContainer::Ptr ac = luaT_to<AtomContainer>(L, 1);

  std::string s = "AtomContainer({";

  lua_getglobal(L, "tostring");
  for (size_t i = 0; i < ac->atoms_.size(); i++)
  {
    lua_pushvalue(L, -1);
    luaT_push(L, ac->atoms_[i]);
    lua_call(L, 1, 1);

    if (i)
    {
      s += ", ";
    }

    s += lua_tostring(L, -1);
    lua_pop(L, 1);
  }

  s += "})";

  lua_pushstring(L, s.c_str());
  return 1;
}

std::vector<luaL_Reg> AtomContainerInterface::luaMethods()
{
  std::vector<luaL_Reg> methods;

  methods.push_back(luaL_toreg("at", l_at));
  methods.push_back(luaL_toreg("clear", l_clear));
  methods.push_back(luaL_toreg("near", l_near));
  methods.push_back(luaL_toreg("size", l_size));
  methods.push_back(luaL_toreg("add", l_add));
  methods.push_back(luaL_toreg("copy", l_copy));
  methods.push_back(luaL_toreg("closestDistanceSquared", l_closest_dist_squared));
  methods.push_back(luaL_toreg("align", l_align));

  methods.push_back(luaL_toreg("filter", l_filter));
  methods.push_back(luaL_toreg("filtered", l_filtered));

  methods.push_back(luaL_toreg("transform", l_transform<1>));
  methods.push_back(luaL_toreg("transformed", l_transformed<1>));

  methods.push_back(luaL_toreg("untransform", l_transform<0>));
  methods.push_back(luaL_toreg("untransformed", l_transformed<0>));

  methods.push_back(luaL_toreg("intersect", l_intersect));
  methods.push_back(luaL_toreg("intersected", l_intersected));

  methods.push_back(luaL_toreg("__tostring", l_tostring));

  return methods;
}

// leaving this here in case we want to add functions to the
// AtomContainer namespace
std::vector<luaL_Reg> AtomContainerInterface::luaFunctions()
{
  std::vector<luaL_Reg> functions;

  functions.push_back(luaL_toreg("closestDistanceSquared", l_closest_dist_squared));
  functions.push_back(luaL_toreg("align", l_align));

  return functions;
}
