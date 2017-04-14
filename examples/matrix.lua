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
