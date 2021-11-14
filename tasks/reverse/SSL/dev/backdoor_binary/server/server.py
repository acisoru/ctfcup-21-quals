#!/usr/bin/env python3
from Crypto.Cipher import AES
from binascii import unhexlify, hexlify
import sys
import time
import socketserver

iv = unhexlify("2511f6108e721a8d0528f23796d2b314")
key = unhexlify("3192e59a60611b36ae9b10de68fc62ab")

def add_to_log(data):
    fd = open("tmp.log", 'ab+')
    if type(data) == str:
        data = data.encode()

    data = data.strip()
    fd.write(time.asctime().encode() + b": " + data + b'\n')
    fd.close()

def decrypt(data):
    ctx = AES.new(key, AES.MODE_CBC, iv=iv)
    return ctx.decrypt(data)

def encrypt(data):
    if len(data) % 16 != 0:
        data += b'\x00' * (16 - (len(data) % 16))
    
    ctx = AES.new(key, AES.MODE_CBC, iv=iv)
    return ctx.encrypt(data)

class MyTCPHandler(socketserver.BaseRequestHandler):

    def handle(self):
        # self.request is the TCP socket connected to the client
        self.data = self.request.recv(1024)
        print("{} wrote:".format(self.client_address[0]))
        print(decrypt(self.data))
        # just send back the same data, but upper-cased
        self.request.sendall(encrypt(b"CUP{8e793f089a25ebad5a19641a184b8ea2}"))
        
        self.data = self.request.recv(1024)
        print("{} wrote:".format(self.client_address[0]))
        print(decrypt(self.data))

        self.request.sendall(encrypt(b"CMD|echo 'kekpek1234' > /tmp/kekpek|NULL"))


# CUP{8e793f089a25ebad5a19641a184b8ea2}
if __name__ == "__main__":
    HOST, PORT = "localhost", 8081

    with socketserver.TCPServer((HOST, PORT), MyTCPHandler) as server:
        server.serve_forever()