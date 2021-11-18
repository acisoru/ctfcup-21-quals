#!/usr/bin/env python3
import random
import struct
import zlib
import os
from Crypto.Cipher import AES, ARC4, DES

u64 = lambda x : struct.unpack("<Q", x)[0]
u32 = lambda x : struct.unpack("<L", x)[0]
u16 = lambda x : struct.unpack("<H", x)[0]
u8 = lambda x : struct.unpack("<B", x)[0]

p64 = lambda x : struct.pack("<Q", x)
p32 = lambda x : struct.pack("<L", x)
p16 = lambda x : struct.pack("<H", x)
p8 = lambda x : struct.pack("<B", x)

START_OFFSET_OF_FIRST_CHUNK = 0x100000
CHUNK_SIZE = 0x1000

class EncryptAlgorithm:
    EMPTY = 0
    XOR = 1 
    RC4 = 2
    DES = 3 
    AES = 4
    
    def decrypt(self, algo_type, key, data):
        result = b''

        if algo_type == EncryptAlgorithm.EMPTY:
            result = data
        
        elif algo_type == EncryptAlgorithm.XOR:
            for i in range(len(data)):
                result += bytes([data[i] ^ key[i % len(key)]])
        
        elif algo_type == EncryptAlgorithm.RC4:
            result = ARC4.new(key).decrypt(data)
        
        elif algo_type == EncryptAlgorithm.DES:
            result = DES.new(key[:8], DES.MODE_ECB).decrypt(data)
        
        elif algo_type == EncryptAlgorithm.AES:
            result = AES.new(key, AES.MODE_ECB).decrypt(data)

        return result
    
if __name__ == "__main__":    
    fd = open("image.ssfs", 'rb')
    data = fd.read()

    entriesCnt = data[4]
    off = u32(data[0x14:0x18])

    for i in range(entriesCnt):
        tableFD = data[off: off+0x38]
        off += 0x38

        FileSize = u64(tableFD[8:16])
        FileHeaderOffset = u64(tableFD[16:24])
        Filename = tableFD[24:56].replace(b'\x00', b'').decode()

        FileHeader = data[FileHeaderOffset:FileHeaderOffset+0x15]
        EncAlgo = FileHeader[0]
        Key = FileHeader[5:21]
        FileData = data[FileHeaderOffset+0x15:FileHeaderOffset+0x15+FileSize]

        fd = open("{}".format(Filename), 'wb')
        fd.write(EncryptAlgorithm().decrypt(EncAlgo, Key, FileData))
        fd.close()