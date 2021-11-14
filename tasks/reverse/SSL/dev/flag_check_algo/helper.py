import random
import ctypes
import time

matrixs = []

for i in range(64):
    matrix = []
    for i1 in range(8):
        line = []
        for i2 in range(8):
            line.append(random.randint(0, 10))
        matrix.append(line)
    matrixs.append(matrix)

print(matrixs)