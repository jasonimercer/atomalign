function loadMol2(filename)
  local atoms = {}
  local bonds = {}
  local loading_atoms = false
  local loading_bonds = false

  for line in io.lines(filename) do
    if loading_atoms then
      local s, _, idx, name, x, y, z, t = string.find(line, "%s*(%d+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S)")
      if idx then
        atoms[tonumber(idx)] = {t, tonumber(x), tonumber(y), tonumber(z), name}
      else
        loading_atoms = false
      end
    end

    if loading_bonds then
      local s, _, idx, a1, a2 = string.find(line, "%s*(%d+)%s*(%d+)%s*(%d+)")
      if idx then
        bonds[tonumber(idx)] = {tonumber(a1), tonumber(a2)}
      else
        loading_bonds = false
      end
    end

    local a, b = string.find(line, "<TRIPOS>ATOM")
    if b then
      loading_atoms = true
    else
    end

    local a, b = string.find(line, "<TRIPOS>BOND")
    if b then
      loading_bonds = true
    end
  end

  local ac = AtomContainer()

  for k, v in pairs(atoms) do
    local atom = Atom({name = v[5], type = v[1], position = {v[2], v[3], v[4]}})
    ac:add(atom)
  end

  return ac, bonds
end
