local prompt = "> "
local levels_of_bootstrap = 0
local _interactive_readline = interactive_readline

-- implementing set/getfenv for lua 5.2+
debug.setfenv = function(fn, env)
  local i = 1
  while true do
    local name = debug.getupvalue(fn, i)
    if name == "_ENV" then
      debug.upvaluejoin(fn, i, (function() return env end), 1)
      break
    elseif not name then
      break
    end

    i = i + 1
  end

  return fn
end

debug.getfenv = function(fn)
  local i = 1
  while true do
    local name, val = debug.getupvalue(fn, i)
    if name == "_ENV" then
      return val
    elseif not name then
      break
    end
    i = i + 1
  end
end

-- build an environment
local function get_local_env(level)
  local locals = {}
  local vars = {}
  local env = debug.getinfo(level)

  -- let's find the max level:
  local max_level = level
  while debug.getinfo(max_level+1) do
    max_level = max_level + 1
  end

  while env and level <= max_level - levels_of_bootstrap do
    local i,k,v = 1,debug.getlocal(level, 1)

    while k do
      if locals[k] == nil then
        locals[k] = {v, "L", level, i}
      end
      i, k, v = i+1,debug.getlocal(level, i+1)
    end

    if env.func then
      i, k, v = 1,debug.getupvalue(env.func, 1)
      while k do
        if locals[k] == nil then
          locals[k] = {v, "U", level, i, env.func}
        end
        i, k, v = i+1,debug.getupvalue(env.func, i+1)
      end
    end

    level = level + 1
    env = debug.getinfo(level)
  end

  return locals
end


-- build an environment that allows the reading of globals, locals and upvalues
-- globals can be written to as normal
local function add_local_env(cmd, level, extra_cmd)
  local locals = get_local_env(level+1)
  local f = load(cmd)

  if f then
    local caller = debug.getinfo(f).func
    local fenv = debug.getfenv(caller)

    local function index(tab, key, value)
      --print("__index", tab, key, value, fenv)
      if locals[key] then
        return locals[key][1]
      end

      if fenv[key] then
        --print("fenv[key]")
        return fenv[key]
      end

      return _G[key]
    end

    local function newindex(tab,key,value)
      --print("__newindex", tab,key,value)
      if locals[key] then
        local level, i = locals[key][3], locals[key][4]
        --print("locals")
        locals[key][1] = value
        if locals[key][2] == "L" then
          debug.setlocal(level, i, value)
        end
        if locals[key][2] == "U" then
          local env_func = locals[key][5]
          debug.setupvalue(env_func, i, value)
        end
      else
        rawset(_G, key, value)
      end
    end

    local env = {}
    if extra_cmd then
      for k,v in pairs(extra_cmd) do
        env[k] = v
      end
    end

    setmetatable(env, { __index = index, __newindex=newindex })
    debug.setfenv(f, env)
    return f
  end
end


local function next_block(txt)
  local a1, a2, a3, a4 = string.find(txt, "^(%[.-%])(.*)")
  if a1 then
    return a3, a4, "["
  end

  local e1, e2, e3 = string.find(txt, "^%[(.*)")
  if e1 then
    return e3, "", "["
  end

  -- if starts with . and is a variable name
  local b1, b2, b3, b4 = string.find(txt, "^%.([%w_]*)(.*)")
  if b1 then
    return b3, b4, "."
  end

  -- if starts with : and is a variable name
  local c1, c2, c3, c4 = string.find(txt, "^%:([%w_]*)(.*)")
  if c1 then
    return c3, c4, ":"
  end

  local d1, d2, d3, d4 = string.find(txt, "^([%w_]+)(.*)")
  if d1 then
    return d3, d4, "b"
  end

  return txt, nil, nil
end

local function txt2table(txt)
  local t = {}
  while txt do
    local a, b, c = next_block(txt)

    if b then
      table.insert(t, {a,b,c})
    end
    txt = b
  end
  return t
end

local function table2txt(tab)
  local txt = ""
  for i = 1, #tab do
    if tab[i][3] == "b" then
      txt = txt .. tab[i][1]
    end
    if tab[i][3] == "." then
      txt = txt .. "." .. tab[i][1]
    end
    if tab[i][3] == "[" then
      txt = txt .. tab[i][1]
    end
    if tab[i][3] == ":" then
      txt = txt .. ":" .. tab[i][1]
    end
  end
  return txt
end


local function autocomplete_env(txt, level, state)
  local locals = get_local_env(level+1)
--[[
    ABC.XYZ["a"][2].fo:

1       ABC,.XYZ["a"][2].fo:,b
2       XYZ,["a"][2].fo:,.
3       ["a"],[2].fo:,[
4       [2],.fo:,[
5       fo,:,.
6       ,,:
7
--]]
  local function f(txt, last, state)
    local function _match(needle, haystack)
      return needle == string.sub(haystack, 1, string.len(needle))
    end
    local g
    if txt == "" then
      g = load("return _G")()
    else
      g = load("return " .. txt)()
    end

    if type(g) == type({}) then
      for k, v in pairs(g) do
        if string.sub(k, 1, 1) ~= "_" then
          if _match(last[1], k) then
            state = state - 1
            if state == 0 then
              if last[3] == "." then
                return txt .. last[3] .. k
              end
              if last[3] == "[" then
                return txt .. "[" .. k .. "]"
              end
              if last[3] == "" then
                return txt ..  k
              end
              if last[3] == "b" then
                return txt ..  k
              end
            end
          end
        end
      end
    end

    local mt = getmetatable(g)
    if mt then
      for k,v in pairs(mt) do
        if _match(last[1], k) then
          state = state - 1
          if state == 0 then
            return txt .. last[3] .. k
          end
        end
      end
    end

    return nil
  end

  local tab = txt2table(txt)

  --for k,v in pairs(tab) do
  --  print(k, "|"..table.concat(v, "|").."|")
  --end

  local last = tab[#tab]
  tab[#tab] = nil

  local txt1 = table2txt(tab)

  local caller = debug.getinfo(f).func
  local fenv = debug.getfenv(caller)
  setmetatable(locals, { __index = fenv })
  debug.setfenv(f, locals)

  if txt == "" then
    txt1, last = "", {"","",""}
  end

  --print("1"..txt, "2"..txt1, "3"..table.concat(last,"|"))

  return f(txt1, last, state)
end



local function find_table(t, text, state)
  local len = string.len(text)
  for k, v in pairs(t) do
    if string.sub(k, 1, 1) ~= "_" then -- not showing keys starting with _
      local m = string.sub(k, 1, 0 + len)
      if text == m then
        if state == 1 then
          return k, state
        end
        state = state - 1
      end
    end
  end
  return nil, state
end

local function autocomplete(text, state)
  return autocomplete_env(text, 5, state)
end

local function loadline(extra_cmd)
  local line = {}
  local chunk, err
  local keep_going = true

  while keep_going do
    --io.write(prompt)
    --local x = io.read()
    local x = _interactive_readline(prompt, autocomplete);
    if x == nil then
      return false -- done
    end

    table.insert(line, x)

    chunk, err = load(table.concat(line, "\n"), "=stdin")
    if err then
      keep_going = string.sub(err, -6) == "<eof>'"
    else
      return add_local_env(table.concat(line, "\n"), 4, extra_cmd)
    end
  end

  print(err)
  return function() end
end

local function bt(level, exclude_last, msg) -- backtrace
  local result = {}
  local function where(env)
    if env.what == "C" then
      return "in C"
    end
    if env.what == "Lua" then
      if env.name then
        return "in function '"..env.name .. "'"
      else
        return ""
      end
    end
    if env.what == "main" then
      return "in main chunk"
    end
    return "in " .. (env.what or "unknown")
  end

  level = level or 1
  local trace = {}
  local env = debug.getinfo(level)

  if msg then
    table.insert(result, msg)
    -- print(msg)
  end

  while env do
    table.insert(trace, (env.short_src or "") .. ":" ..
                        (env.currentline or 0) .. ": " ..
                        where(env))
    level = level + 1
    env = debug.getinfo(level)
  end

  -- peel off last X trace levels as it's botstrap and startup
  for i=1, exclude_last do
    table.remove(trace)
  end

  table.insert(result, "stack traceback:")
  for k,v in pairs(trace) do
    table.insert(result, "\t" .. v)
    -- print("\t" .. v)
  end

  return table.concat(result, "\n")
end


function interactive(msg, json)
  json = json or {}
  local level_offset = json.level_offset or 0
  local caller = debug.getinfo(2 + level_offset)
  local src = caller.short_src or ""
  local line = caller.currentline or "0"

  local header = json.header
  if json.header == nil then
    header = true
  end
  local printmsg = json.message
  if json.message == nil then
    printmsg = true
  end

  local bt_txt = bt(3 + level_offset, levels_of_bootstrap)

  local function bt_()
    print(bt_txt)
  end

  if header then
    print("Interactive environment initiated from (" .. src .. ":" .. line .. ")")
    print(bt_txt)
    print("Continue script with ctrl + d")
  end

  if printmsg then
    if msg then
      print(msg)
    end
  end

  while true do
    local interactive_statement = loadline({bt=bt_})
    if interactive_statement == false then
      print() -- newline for ctrl+d
      return
    end

    local stat, err = pcall(interactive_statement)

    if err then
      print(err)
    end
  end

end

-- the history file needs to be writable so if it's in a nested dir structure
-- then all the parent directories need to be created
local dirname = os.getenv("HOME") .. "/.atom_align/"
local filename = dirname .. "history"

os.execute("mkdir -p " .. dirname)

interactive_setHistoryFile(filename)


