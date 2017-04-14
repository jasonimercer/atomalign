/**
 * Software License Agreement BSD2
 *
 * \file      interactive.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 * \copyright Copyright (c) 2017, Jason Mercer, All rights reserved.
 *
 * This Software is licensed under BSD 2-clause, see BSD2.txt in the licenses
 * directory.
 */

#include "luainterface/luainterface.h"

#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

// Let's be honest: the following code is from a different era.
// All examples dealing with readline feel like they come straight
// from 1985. There's bad looking stuff going on here.

static lua_State* _L = 0; // readline is not stateful :(
static int ac_pos = 2; // autocomplete func on stack

static char* history_file = 0;

/**
 * @brief Duplicate a C string
 * @param s Source string
 * @return duplicate, must free manually
 */
static char* dupstr(const char* s)
{
  char *r = (char*) malloc((strlen(s) + 1));
  strcpy(r, s);
  return(r);
}

const char* cmd [] ={ "hello", "world", "hell" ,"word", "quit", " " };


// passed autocomplete responsibility to Lua
char* my_generator(const char* text, int state)
{
  if (_L)
  {
    lua_pushvalue(_L, ac_pos);
    lua_pushstring(_L, text);
    lua_pushinteger(_L, state + 1); // for lua niceness
    if(lua_pcall(_L, 2, 1, 0))
    {
      fprintf(stderr, "%s\n", lua_tostring(_L, -1));
      lua_pop(_L, 1);
      return 0;
    }

    if(lua_isstring(_L, -1))
    {
      char* g = dupstr(lua_tostring(_L, -1));
      lua_pop(_L, 1);
      return g;
    }
    return 0;
  }

  return 0;
}

static char** my_completion(const char * text, int start, int end)
{
  char** matches;

  matches = (char **)NULL;

  if (start == 0)
  {
    matches = rl_completion_matches ((char*)text, &my_generator);
  }
  else
  {
    rl_bind_key('\t',rl_abort);
  }

  return (matches);
}

// Static variable for holding the line.
// personal note: this is disgusting
static char *line_read = (char *)0;
static char *last_line_read = (char *)0;

char* rl_gets (const char* prompt = 0)
{
  rl_bind_key('\t', rl_complete);

  if (line_read)
  {
    if (last_line_read)
    {
      free(last_line_read);
      last_line_read = (char *)NULL;
    }
    last_line_read = line_read;
  }

  /* Get a line from the user. */
  line_read = readline(prompt ? prompt : "");

  // If the line has any text in it, that's different tha the last line, save it on the history.
  if (line_read && *line_read)
  {
    int add_to_history = 1;

    if (last_line_read)
    {
      // don't add if same
      add_to_history = strcmp(last_line_read, line_read);
    }

    if (add_to_history)
    {
      add_history(line_read);
    }

    if (history_file)  // writing to file every line since many interactive scripts end with ctrl+c
    {
      write_history(history_file);
    }
  }

  return (line_read);
}

static void initialize_readline()
{
  rl_readline_name = "Interactive";
  rl_attempted_completion_function = my_completion;
  rl_completion_entry_function = my_generator;  // disable filename auto-complete

  using_history();
}

static int l_interactive_setHistoryFile(lua_State* L)
{
  if (history_file)
  {
    free(history_file);
    history_file = 0;
  }

  if (lua_isstring(L, 1))
  {
    const int len = strlen(lua_tostring(L, 1)) + 1;
    history_file = (char*) malloc(len);
    memcpy(history_file, lua_tostring(L, 1), len);
  }

  return 0;
}

static int l_interactive_readline(lua_State* L)
{
  ac_pos = 2;
  _L = L;
  char* line;

  if (lua_isstring(L, 1))
  {
    line = rl_gets(lua_tostring(L, 1));
  }
  else
  {
    line = rl_gets("");
  }

  if (line)
  {
    lua_pushstring(L, line);
  }
  else
  {
    lua_pushnil(L);
  }

  return 1;
}

#include "interactive_code.h"

void register_interactive(lua_State* L)
{
  _L = L;
  initialize_readline();

  lua_pushcfunction(L, l_interactive_readline);
  lua_setglobal(L, "interactive_readline");

  lua_pushcfunction(L, l_interactive_setHistoryFile);
  lua_setglobal(L, "interactive_setHistoryFile");

  LuaInterface::luaL_dostringn(L, interactive_code, "interactive_code.h");

  // clearing aux functions:
  lua_pushnil(L);
  lua_setglobal(L, "interactive_setHistoryFile");

  lua_pushnil(L);
  lua_setglobal(L, "interactive_readline");

  if (history_file)
  {
    read_history(history_file);
  }
}
