#!/usr/bin/env python3

import sys
import socket
import random
import struct
import time

p32 = lambda x : struct.pack("<L", x)

host = '13.37.13.37'
port = 1337
server = (host, port)

if __name__ == "__main__":

    if len(sys.argv) < 2:
        print("{-} Usage: ./loader.py <file>")
        sys.exit(0)
    
    data = open(sys.argv[1], 'rb').read()

    file_map = {}
    idx = 0
    # split data to chunks
    while idx < len(data):
        chunk_size = random.randint(0x80, 0x300)

        if idx + chunk_size > len(data):
            chunk_size = len(data) - idx
            
        transmit_part = data[idx:idx+chunk_size]

        ranges = [idx,chunk_size]
        file_map[transmit_part] = ranges
        idx += chunk_size

    keys = list(file_map.keys())
    random.shuffle(keys)
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    for i in keys:
        time.sleep(0.1)
        packet = b'FSUP'
        packet += p32(file_map[i][0])
        packet += p32(file_map[i][1])
        packet += p32(0x0)
        packet += i
        
        print(len(i), file_map[i][1])
        sock.sendto(packet, server)
    
    sock.close()
