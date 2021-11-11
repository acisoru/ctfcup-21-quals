from Crypto.Cipher import AES
from binascii import unhexlify, hexlify

# get from aeskeyfind
KEY = unhexlify('6d9f20b29cb117dd3b955fcbcf795eb4db92b85da3135d2ea83473fe2ffd44c1')
# packet id == 554
packet = unhexlify('32f5d1f71c451f17c1c99888995f199f4ef3455f32f5d1f632f5d13f32f5d1f65190ce30d2d2cf627ecc4c763386d2f7ac63adac4301a6de0ab7bd3a9b412215bdae8b464ce480c71dcb532cc3d79354b150177e4200eaafe550e25be86a717a5349413cd3c20d660a4100c52445d14a3a59baaafdffc79b922a77a415302cef6859d99cddb8b6bba8efcf34b9e2ed171821d4263625feb1bf87840c51f0e80182ac6310b76788cdb7ddf8936a1b004b0b5dd23fee89050799fb55cea3a8d033a70510a488ddf5f2e5f7a36545a557e48e6d4502441ad05aef7a9e3038e4176b')

def main():
    xk = packet[:4]
    data = packet[4:]
    res = b''
    
    for i in range(len(data)):
        res += bytes([data[i] ^ xk[i % 4]])
    
    iv = res[0x1c:0x1c+0x10]
    print(hexlify(res))
    ctx = AES.new(KEY, AES.MODE_CBC, iv=iv)

    print(ctx.decrypt(res[0x2c:]))

if __name__ == "__main__":
    main()