-- This script loads small fragments of the two systems, finds a transform match
-- on carbons, loads the large fragments and applies the small fragment transform.
-- Once the large fragments are in similar frames they are initersected against
-- each other to remove unmatchable points.
-- After the large fragments are alighned and trimmed, a refined transformation is
-- found that matches multiple unit cells.

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

-- load in large data sets
ac1_big = loadXYZ("n17003_crystal1_big.xyz")
ac2_big = loadXYZ("n17003_crystal2_big.xyz")

-- apply transform to ac1_big
ac1_big_t = ac1_big:transformed(t)

-- the two large data sets do not have equivalent expansions of the unit cell
-- we can intersect them against each other to trim them down to comparable sets
tol = 2.0  -- Angstrom
ac1_big_t_isect = ac1_big_t:intersected(ac2_big, tol)
ac2_big_isect = ac2_big:intersected(ac1_big_t, tol)

-- remove transformation from intersected big data set
ac1_big_isect = ac1_big_t_isect:untransformed(t)


t2 = AtomContainer.align(ac1_big_isect, ac2_big_isect, {steps = 1000, rho_begin = 1, rho_end = 1e-5}, t)
print("Alignment residual (Big): " .. t2.diff^(1/2))

ac1_big_t_isect2 = ac1_big_isect:transformed(t2)

-- save aligned atom containers
saveXYZ(ac1_big_t, "n17003_crystal1_big_trans.xyz")
saveXYZ(ac1_big_t_isect2, "n17003_crystal1_big_same_space2.xyz")
saveXYZ(ac1_big_t_isect, "n17003_crystal1_big_same_space.xyz")
saveXYZ(ac2_big_isect, "n17003_crystal2_big_same_space.xyz")
