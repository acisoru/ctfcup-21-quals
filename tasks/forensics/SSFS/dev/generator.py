#!/usr/bin/env python3
import random
import struct
import zlib
import os
from Crypto.Cipher import AES, ARC4, DES

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

    def gen_key(self, algo_type):
        if algo_type == EncryptAlgorithm.EMPTY:
            return b'\x00' * 16
        elif algo_type == EncryptAlgorithm.XOR:
            return os.urandom(16)
        elif algo_type == EncryptAlgorithm.RC4:
            return os.urandom(16)
        elif algo_type == EncryptAlgorithm.DES:
            return os.urandom(8) + b'\x00' * 8
        elif algo_type == EncryptAlgorithm.AES:
            return os.urandom(16)
    
    def encrypt(self, algo_type, key, data):
        result = b''

        if algo_type == EncryptAlgorithm.EMPTY:
            result = data
        
        elif algo_type == EncryptAlgorithm.XOR:
            for i in range(len(data)):
                result += bytes([data[i] ^ key[i % len(key)]])
        
        elif algo_type == EncryptAlgorithm.RC4:
            result = ARC4.new(key).encrypt(data)
        
        elif algo_type == EncryptAlgorithm.DES:
            result = DES.new(key[:8], DES.MODE_ECB).encrypt(data)
        
        elif algo_type == EncryptAlgorithm.AES:
            result = AES.new(key, AES.MODE_ECB).encrypt(data)

        return result

class GlobalDescriptor:
    def __init__(self):
        self.m_guid = 0 # global uid
        self.m_off_free_chunk = START_OFFSET_OF_FIRST_CHUNK

    def get_new_uid(self):
        uid = self.m_guid
        self.m_guid += 1
        return uid
    
    def get_new_off(self, size):
        offset = self.m_off_free_chunk
        self.m_off_free_chunk += size + 0x15 # 0x15 - size of file header
        return offset
    
    def get_off(self):
        return self.m_off_free_chunk

    def pack(self):
        result = p64(self.m_guid)
        result += p64(self.m_off_free_chunk)
        return result


GFSD = GlobalDescriptor() # Global FS descriptor
FDTable = []
FilesTable = []

# 56 bytes - 1 entry
class FileTableDescriptor:
    def __init__(self, data, name):
        self.m_uid = GFSD.get_new_uid()
        self.m_crc32 = zlib.crc32(data) & 0xffffffff
        if len(data) % CHUNK_SIZE != 0:
            data += b'\x00' * (CHUNK_SIZE - (len(data) % CHUNK_SIZE))
        self.m_file_size = len(data)
        self.m_file_off = GFSD.get_new_off(self.m_file_size)
        if len(name) < 32:
            name = name.encode() + b'\x00' * (32 - (len(name) % 32))
        self.m_fname = name

    def pack(self):
        result = p32(self.m_uid)
        result += p32(self.m_crc32)
        result += p64(self.m_file_size)
        result += p64(self.m_file_off)
        result += self.m_fname
        return result

class File:
    def __init__(self, data, enc_algo):
        self.m_enc_algo = enc_algo
        self.m_key = EncryptAlgorithm().gen_key(self.m_enc_algo)
        
        if len(data) % CHUNK_SIZE != 0:
            data += b'\x00' * (CHUNK_SIZE - (len(data) % CHUNK_SIZE))

        self.m_size = len(data)
        self.m_data = EncryptAlgorithm().encrypt(self.m_enc_algo, self.m_key, data)
    
    def pack(self):
        result = p8(self.m_enc_algo)
        result += p32(self.m_size)
        result += self.m_key
        result += self.m_data
        return result
    
if __name__ == "__main__":
    files = os.listdir("files/")

    for fn in files:
        data = open("files/{}".format(fn), 'rb').read()
        ftd = FileTableDescriptor(data, fn)

        algo_type = random.randint(0, 4)
        if fn == "flag_part1.jpg":
            algo_type = 1
        if fn == "flag_part2.jpg":
            algo_type = 2
        if fn == "flag_part3.jpg":
            algo_type = 3
        if fn == "flag_part4.jpg":
            algo_type = 4
            
        fd = File(data, algo_type)

        FDTable.append(ftd)
        FilesTable.append(fd)

    table = b''
    for i in FDTable:
        table += i.pack()
    
    fd = open("image.ssfs", 'wb')
    header = b"SSFS" + GFSD.pack() + p32(0x18) + table
    header = header.ljust(START_OFFSET_OF_FIRST_CHUNK, b"\x00")
    fd.write(header)

    for i in FilesTable:
        fd.write(i.pack())
    
    fd.close()
