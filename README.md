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

print("A")
print(A)

B = A:reshaped(1, 9):tr()
print("B = A:reshaped(1, 9):tr()")
print(B)

print("b")
print(b)

x = b / A

print("x = b / A")
print(x)

print("A * x - b")
print(A * x - b)

print("I = A:inv() * A")
I = A:inv() * A
print(I)

A = Matrix({{3, 2, 2}, {2, 3, -2}}):tr()
U, S, VT = A:svd()

print("A")
print(A)

print("U, S, VT = A:svd()")

print("U")
print(U)

print("S")
print(S)

print("VT")
print(VT)

print("U * S * VT:tr()")
print(U * S * VT:tr())
```

This can be run from the build directory:

```
./atom_align  ../examples/matrix.lua
A
Matrix({{1.0, 0.0, 2.0}, {2.0, 3.0, 0.0}, {1.0, 1.0, 1.0}})
B = A:reshaped(1, 9):tr()
Matrix({{1.0}, {0.0}, {2.0}, {2.0}, {3.0}, {0.0}, {1.0}, {1.0}, {1.0}})
b
Matrix({{4.0}, {10.0}, {1.0}})
x = b / A
Matrix({{26.0}, {-14.0}, {-11.0}})
A * x - b
Matrix({{0.0}, {0.0}, {0.0}})
I = A:inv() * A
Matrix({{1.0, 8.8817841970013e-16, 8.8817841970013e-16}, {-4.4408920985006e-16, 1.0, -4.4408920985006e-16}, {-4.4408920985006e-16, -4.4408920985006e-16, 1.0}})
A
Matrix({{3.0, 2.0}, {2.0, 3.0}, {2.0, -2.0}})
U, S, VT = A:svd()
U
Matrix({{-0.70710678118655, 0.23570226039552}, {-0.70710678118655, -0.23570226039552}, {-1.6653345369377e-16, 0.94280904158206}})
S
Matrix({{5.0, 0.0}, {0.0, 3.0}})
VT
Matrix({{-0.70710678118655, 0.70710678118655}, {-0.70710678118655, -0.70710678118655}})
U * S * VT:tr()
Matrix({{3.0, 2.0}, {2.0, 3.0}, {2.0, -2.0}})
```
