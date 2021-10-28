#!/usr/bin/env python3 

import sys
import os

def main(args):
    if len(args) < 2:
        print("[+] Usage: ./injector.py <src-binary> <injected-binary>")
        sys.exit(0)
    
    srcBin = open(args[1], 'ab+')
    srcSize = os.path.getsize(args[1])

    data = open(args[2], 'rb').read()
    
    if srcSize % 8 != 0:
        srcBin.write(b'\x00' * (8 - srcSize % 8))
    
    srcBin.write(b"\xe2\xe4\xe7\xe5\xd2\xd4\xe5\xd4")

    val = 1
    for i in range(len(data)):
        srcBin.write(bytes([data[i] ^ val]))
        val = (val * 11) % 251

    srcBin.close()

if __name__ == "__main__":
    main(sys.argv)