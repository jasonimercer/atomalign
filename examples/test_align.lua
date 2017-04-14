-- load XYZ files into atom containers, filter them, align them and write out to new XYZ files

function loadXYZ(filename)
  local ac = AtomContainer()
  local getline = io.lines(filename)

  local count = tonumber(getline())
  local name = getline()

  local names = {}  -- for keeping track of H3, O4, C1 etc
  for i = 1, count do
    local line = getline()

    local a, b, t, x, y, z = string.find(line, "(%S+)%s+(%S+)%s+(%S+)%s+(%S+)")

    if a == nil then
      error("pattern match on '" .. line .. "' failed")
    end

    x = tonumber(x)
    y = tonumber(y)
    z = tonumber(z)

    names[t] = (names[t] or 0) + 1

    local atom = Atom({name = t .. names[t], type = t, position = {x, y, z}})
    ac:add(atom)
  end

  return ac
end

function saveXYZ(ac, filename)
  local f = io.open(filename, "w")

  f:write(string.format("%i\n%s\n", ac:size(), filename))

  for i = 1, ac:size() do
    local atom = ac:at(i)
    local p = atom:position()
    local x, y, z = p:get(1), p:get(2), p:get(3)
    f:write(string.format("%s %f %f %f\n", atom:type(), x, y, z))
  end
  f:close()
end

-- load files into atom containers
ac1 = loadXYZ("n17003_crystal1.xyz")
ac2 = loadXYZ("n17003_crystal2.xyz")

-- custom filter function
function onlyCarbon(atom)
  return atom:type() == "C"
end

ac1C = ac1:filtered(onlyCarbon)
ac2C = ac2:filtered(onlyCarbon)

-- starting point for search for best alignment transformation
start = {rz = math.pi}

-- find best alignment transform
t = AtomContainer.align(ac1C, ac2C, {steps = 1000, rho_begin = 1, rho_end = 1e-5}, start)

print("Alignment residual (Carbons): " .. t.diff^(1/2))

-- transform container 1 via best found transform on carbons
ac1:transform(t)

print("Alignment residual (All): " .. AtomContainer.closestDistanceSquared(ac1, ac2)^(1/2))

-- save aligned atom containers
saveXYZ(ac1, "n17003_crystal1_C.xyz")
saveXYZ(ac2, "n17003_crystal2_C.xyz")
