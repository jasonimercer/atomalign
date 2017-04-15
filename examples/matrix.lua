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
