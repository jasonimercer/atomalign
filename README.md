# Overview

`atomalign` is a Lua environment for aligning sets of atoms.

# Downloading and Compiling

Clone the repository with:

```
git clone https://github.com/jasonimercer/atomalign.git
```

Fetch dlib and lua dependencies and compile with:

```
cd atomalign
./fetch-deps.sh
mkdir build
cd build
cmake ..
make
```

You should now have `atomalign` built in your local directory. Running `./atomalign` should prompt with a request for a source script.

# Examples

Consider the following script (available in examples/matrix.lua)

```lua
-- This script tests some matrix operations
A = Matrix({{1, 0, 2}, {2, 3, 0}, {1, 1, 1}})
b = Matrix({{4, 10, 1}}):tr()

B = A:reshaped(1, 9)

print("A Matrix")
print(A)

print("B Matrix (A reshaped to a single row)")
print(B)

print("b Matrix")
print(b)

print("Solving for A x = b")
x = A / b

print("x:")
print(x)

print("A x - b:")
print(A * x - b)
```

This can be run from the build directory:

```
./atom_align  ../examples/matrix.lua
A Matrix
Matrix({{1.000000, 0.000000, 2.000000}, {2.000000, 3.000000, 0.000000}, {1.000000, 1.000000, 1.000000}})
B Matrix (A reshaped to a single row)
Matrix({{1.000000, 0.000000, 2.000000, 2.000000, 3.000000, 0.000000, 1.000000, 1.000000, 1.000000}})
b Matrix
Matrix({{4.000000}, {10.000000}, {1.000000}})
Solving for A x = b
x:
Matrix({{26.000000}, {-14.000000}, {-11.000000}})
A x - b:
Matrix({{0.000000}, {0.000000}, {0.000000}})
```
