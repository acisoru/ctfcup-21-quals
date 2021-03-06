#!/usr/bin/env python3
import binascii

# data from 0x1590A6F49A0
data = binascii.unhexlify(b"781F48364871282F7C1D623662367C36622D625B6236623662397C5B7C767C7162277C367C1D7C71625B7C2D7C5B7C277C1F7C76621F7C2D7C2D7C767C5B7C1D7C527C1F621F7C1D2815")

# data from 00007FF7CBD64530 and 00007FF7CBD64540
a1 = [0x23, 0x32, 0x1A, 0x7C, 0x78, 0x48, 0x62, 0x28, 0x62, 0x7F, 0x7C, 0x1F, 0x7D, 0x79, 0x55, 0x69]
a2 = [0x71, 0x39, 0x27, 0x1F, 0x2D, 0x36, 0x5B, 0x52, 0x1D, 0x76, 0x72, 0x2F, 0x74, 0x15, 0x10, 0x6F]

res = b''

for i in range(0, len(data), 2):
    res += bytes([a1.index(data[i])*16 + a2.index(data[i+1])])

print(res)