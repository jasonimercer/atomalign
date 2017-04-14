/**
 * Software License Agreement BSD2
 *
 * \file      luainterface.h
 * \author    Jason Mercer <jason.mercer@gmail.com>
 * \copyright Copyright (c) 2017, Jason Mercer, All rights reserved.
 *
 * This Software is licensed under BSD 2-clause, see BSD2.txt in the licenses
 * directory.
 */

#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include <boost/tuple/tuple.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <luainterface/check/check_if_type_has_luamethods.h>
#include <luainterface/check/check_if_type_has_luafunctions.h>
#include <luainterface/check/check_if_type_has_l_new.h>
#include <luainterface/check/check_if_type_has_l_totable.h>
#include <luainterface/check/check_if_type_has_l_fromtable.h>
#include <luainterface/compile_time_hash.h>

#include <map>
#include <vector>
#include <string>

#include <stdio.h>
#include <string.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace LuaInterface
{
// Interfaces are expected to specialize the following structs
// to establish the relationship between base and interface types
// as well as lineage
template <typename Base>
struct BaseToInterface
{
  typedef void type;
};

template <typename Interface>
struct InterfaceToBase
{
  typedef void type;
};

template <typename Base>
struct BaseToParent
{
  typedef void type;
};

// This macro links base and interface types together
// Expected to be called from the base namespace
#define SpecializeInterface(Base, Interface) \
namespace LuaInterface \
{ \
template <> \
struct BaseToInterface<Base> \
{ \
  typedef Interface type; \
}; \
template <> \
struct InterfaceToBase<Interface> \
{ \
  typedef Base type; \
}; \
}
// end of macro SpecializeInterface

// This macro links base and parent types together
// Expected to be called from the base namespace
#define SpecializeInheritance(Base, Parent) \
namespace LuaInterface \
{ \
template <> \
struct BaseToParent<Base> \
{ \
  typedef Parent type; \
}; \
}
// end of macro SpecializeInheritance

namespace Private
{
typedef boost::tuple<void*, std::vector<boost::uint32_t> > tuple;
}

template <typename T>
inline boost::shared_ptr<T> luaT_to(lua_State* L, int idx)
{
  typedef typename LuaInterface::BaseToInterface<T>::type interface;

  Private::tuple** tuple_ptr_ptr = (Private::tuple**)lua_touserdata(L, idx);

  if (tuple_ptr_ptr && *tuple_ptr_ptr)
  {
    size_t n = (*tuple_ptr_ptr)->get<1>().size();
    const boost::uint32_t hash = interface::hash();
    for (size_t i = 0; i < n; i++)
    {
      if ((*tuple_ptr_ptr)->get<1>().at(i) == hash)
      {
        return boost::shared_ptr<T>(*(boost::shared_ptr<T>*)
                                    (*tuple_ptr_ptr)->get<0>());
      }
    }
  }

  return boost::shared_ptr<T>();
}


namespace Private
{

// add methods to the metamethod table of an object type
template<class T>
inline void luaT_addMethods(lua_State* L, std::vector<luaL_Reg> methods);

template <typename T>
struct fetch_lineage
{
  static void exec(std::vector<boost::uint32_t>& lineage)
  {
    typedef typename LuaInterface::BaseToInterface<T>::type interface;
    typedef typename LuaInterface::BaseToParent<T>::type parent;

    lineage.push_back(interface::hash());

    fetch_lineage<parent>::exec(lineage);
  }
};

template <>
struct fetch_lineage<void>
{
  static void exec(std::vector<boost::uint32_t>& )
  {
  }
};

// push metatable on the stack
template<class T>
int luaT_mt(lua_State* L)
{
  typedef typename LuaInterface::BaseToInterface<T>::type interface;
  luaL_getmetatable(L, interface::typeName().c_str());
  return 1;
}

// add name-function pairs to table at top of stack
inline void luaT_addToTable(lua_State* L,
                            const std::vector<luaL_Reg>& vals)
{
  for (size_t i = 0; i < vals.size(); i++)
  {
    lua_pushcfunction(L, vals[i].func);
    lua_setfield(L, -2, vals[i].name);
  }
}

inline void luaT_addToTable(lua_State* L,
                            const std::map<std::string, lua_CFunction>& vals)
{
  std::map<std::string, lua_CFunction>::const_iterator it;
  for (it = vals.begin(); it != vals.end(); it++)
  {
    lua_pushcfunction(L, it->second);
    lua_setfield(L, -2, it->first.c_str());
  }
}

// add methods to the metamethod table of an object type
template<class T>
inline void luaT_addMethods(lua_State* L,
                            std::vector<luaL_Reg>& methods)
{
  luaT_mt<T>(L);
  luaT_addToTable(L, methods);
  lua_pop(L,1);
}

template<typename Base, typename T>
struct get_lineage_methods
{
  static void exec(std::map<std::string, lua_CFunction>& methods)
  {
    typedef typename LuaInterface::BaseToInterface<T>::type interface;
    typedef typename LuaInterface::BaseToParent<T>::type parent;

    // get the methods from the parent generations
    get_lineage_methods<Base, parent>::exec(methods);

    // get the methods from this generation
    std::vector<luaL_Reg> gen_methods = get_luaMethods<interface>();

    // add to vtable overriding parent methods with child methods
    for (size_t i = 0; i < gen_methods.size(); i++)
    {
      methods[gen_methods[i].name] = gen_methods[i].func;
    }
  }
};

// deal with void case (do nothing)
template<typename Base>
struct get_lineage_methods<Base, void>
{
  static void exec(std::map<std::string, lua_CFunction>&)
  {
  }
};

// basic tostring metamethod acting like type()
template<class T>
int luaT_tostring(lua_State* L)
{
  typedef typename LuaInterface::BaseToInterface<T>::type interface;
  boost::shared_ptr<T> p = luaT_to<T>(L, 1);
  lua_pushfstring(L, "%s: %p", interface::typeName().c_str(), p.get());
  return 1;
}

// garbage collection metamethod.
// decrement refcount, delete if needed
template<class T>
int luaT_gc(lua_State* L)
{
  tuple** tuple_ptr_ptr = (Private::tuple**)lua_touserdata(L, 1);

  if (tuple_ptr_ptr && *tuple_ptr_ptr)
  {
    boost::shared_ptr<T>* sp = (boost::shared_ptr<T>*)
        (*tuple_ptr_ptr)->get<0>();
    delete sp;
    delete *tuple_ptr_ptr;
  }

  return 0;
}

// method to get a pointer as a string
template<class T>
int luaT_stringpointer(lua_State* L)
{
  boost::shared_ptr<T> p = luaT_to<T>(L, 1);
  lua_pushfstring(L, "%p", p.get());
  return 1;
}

inline luaL_Reg luaL_toreg(const char* name, lua_CFunction func)
{
  luaL_Reg r;
  r.name = name;
  r.func = func;
  return r;
}

template<typename T>
inline std::vector<luaL_Reg> _lua_base_methods()
{
  std::vector<luaL_Reg> base_methods;
  base_methods.push_back(luaL_toreg("__gc", luaT_gc<T>));
  base_methods.push_back(luaL_toreg("__tostring", luaT_tostring<T>));
  base_methods.push_back(luaL_toreg("topointer", luaT_stringpointer<T>));
  base_methods.push_back(luaL_toreg("metatable", luaT_mt<T>));
  return base_methods;
}

static int lua_pushoptionalcfunction(lua_State* L, lua_CFunction func)
{
  if (func)
  {
    lua_pushcfunction(L, func);
  }
  else
  {
    lua_pushnil(L);
  }

  return 1;
}
}  // namespace Private

template <typename T>
inline int luaT_push(lua_State* L, const boost::shared_ptr<T>& ptr)
{
  typedef typename LuaInterface::BaseToInterface<T>::type interface;

  if (!ptr)
  {
    lua_pushnil(L);
  }
  else
  {
    Private::tuple** tuple_ptr_ptr =
        (Private::tuple**)lua_newuserdata(L, sizeof(Private::tuple*));

    // initialize the data
    (*tuple_ptr_ptr) = new Private::tuple();

    boost::shared_ptr<T>* sp = new boost::shared_ptr<T>(ptr);
    (*tuple_ptr_ptr)->get<0>() = sp;

    std::vector<boost::uint32_t> lineage;
    Private::fetch_lineage<T>::exec(lineage);

    (*tuple_ptr_ptr)->get<1>() = lineage;

    luaL_getmetatable(L, interface::typeName().c_str());

    lua_setmetatable(L, -2);
  }

  return 1;
}

// test type
template<typename T>
inline int luaT_is(lua_State* L, int idx)
{
  typedef typename LuaInterface::BaseToInterface<T>::type interface;

  Private::tuple** tuple_ptr_ptr = (Private::tuple**)lua_touserdata(L, idx);

  if (tuple_ptr_ptr && *tuple_ptr_ptr)
  {
    size_t n = (*tuple_ptr_ptr)->get<1>().size();
    const boost::uint32_t hash = interface::hash();
    for (size_t i = 0; i < n; i++)
    {
      if ((*tuple_ptr_ptr)->get<1>().at(i) == hash)
      {
        return 1;
      }
    }
  }

  return 0;
}

inline int luaL_dostringn(lua_State* L, const char* code, const char* name)
{
  return luaL_loadbuffer(L, code, strlen(code), name) ||
         lua_pcall(L, 0, LUA_MULTRET, 0);
}

inline luaL_Reg luaL_toreg(const char* name, lua_CFunction func)
{
  luaL_Reg r;
  r.name = name;
  r.func = func;
  return r;
}

template<typename T>
inline void luaT_register(lua_State* L)
{
  typedef typename LuaInterface::BaseToInterface<T>::type interface;
  const std::string name = interface::typeName();

  // create the function that will register the data
  // argument of the function is a table with the following key/values
  // name: name of object (can use x.y.z to create namespaces)
  // constructor: constructor function
  // functions: table of auxilary functions to be added to named table
  // methods: table of methods to be added to object metatable
  // metatable: object metatable
  // totable: function to translate object to a table
  // fromtable: function to translate table to an object
  const char* make_object =
      "return function(a)\n"\
      "  local name = a.name\n"\
      "  local ctor = a.constructor\n"\
      "  local functions = a.functions\n"\
      "  local methods = a.methods\n"\
      "  local mt = a.metatable\n"\
      "  local totable = a.totable\n"\
      "  local fromtable = a.fromtable\n"\
      "  local t = _G\n"\
      "  for i in string.gmatch(name,\n \"[^%.]+\") do\n"\
      "    t[i] = t[i] or {}\n"\
      "    t = t[i]\n"\
      "  end\n"\
      "  if t == _G then\n"\
      "    error(\"failed to register \" .. name)\n"\
      "  end\n"\
      "  t.new = ctor\n"\
      "  for k, v in pairs(functions) do\n"\
      "    t[k] = v\n"\
      "  end\n"\
      "  for k, v in pairs(methods) do\n"\
      "    mt[k] = v\n"\
      "  end\n"\
      "  if totable then\n"\
      "    mt.toTable = totable\n"\
      "  end\n"\
      "  if fromtable then\n"\
      "    mt.fromTable = fromtable\n"\
      "    t.fromTable = fromtable\n"\
      "  end\n"\
      "  -- allow __index metamethod to work as expected\n"\
      "  mt[\"__index\"] = function(t, key)\n"\
      "    local mt_val = rawget(mt, key)\n"\
      "    if mt_val ~= nil then\n"\
      "      return mt_val\n"\
      "    else\n"\
      "      local index = methods[\"__index\"]\n"\
      "      if index then\n"\
      "        return index(t, key)\n"\
      "      end\n"\
      "    end\n"\
      "    return nil\n"\
      "  end\n"\
      "  -- make function table callable for c'tor syntactic sugar\n"\
      "  local table_mt = getmetatable(t) or {}\n"\
      "  if ctor then\n"\
      "    table_mt.__call = function(obj, ...) return ctor(...) end\n"\
      "  end\n"\
      "  setmetatable(t, table_mt)"\
      "end";

  // get the function that will register the class
  if (luaL_dostringn(L, make_object, "@luaT_register"))
  {
    fprintf(stderr, "luaT_register: %s\n", lua_tostring(L, -1));
    return;
  }

  lua_newtable(L);

  // function argument: name
  lua_pushstring(L, name.c_str());
  lua_setfield(L, -2, "name");

  // function argument: optional constructor
  Private::lua_pushoptionalcfunction(L, Private::get_l_new<interface>());
  lua_setfield(L, -2, "constructor");

  // function argument: optional totable
  Private::lua_pushoptionalcfunction(L, Private::get_l_totable<interface>());
  lua_setfield(L, -2, "totable");

  // function argument: optional from
  Private::lua_pushoptionalcfunction(L, Private::get_l_fromtable<interface>());
  lua_setfield(L, -2, "fromtable");

  // function argument: functions
  lua_newtable(L);
  std::vector<luaL_Reg> functions = Private::get_luaFunctions<interface>();
  Private::luaT_addToTable(L, functions);
  lua_setfield(L, -2, "functions");

  // function argument: methods
  lua_newtable(L);
  std::map<std::string, lua_CFunction> methods;
  Private::get_lineage_methods<T,T>::exec(methods);
  Private::luaT_addToTable(L, Private::_lua_base_methods<T>());
  Private::luaT_addToTable(L, methods);
  lua_setfield(L, -2, "methods");

  // function argument: metatable
  luaL_newmetatable(L, name.c_str());
  lua_setfield(L, -2, "metatable");

  // call the function
  if (lua_pcall(L, 1, 0, 0))
  {
    fprintf(stderr, "luaT_register: %s\n", lua_tostring(L, -1));
  }
}
}  // namespace LuaInterface

#endif  // LUAINTERFACE_H

