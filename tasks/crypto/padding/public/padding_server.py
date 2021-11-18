import os
from Crypto.Cipher import AES
from padding_key import KEY, FLAG

class Encryptor:
    def __init__(self, key):
        self.KEY = key
        self.block_length = 16

    def __padding(self, s):
        l = len(s) % self.block_length
        n = self.block_length - l
        s += chr(n) * n
        return s

    def __unpadding(self, s):
        if len(s) % self.block_length != 0 or ord(s[-1]) == 0:
            text = "Bad block size %d" % (len(s) % self.block_length)
            raise Exception(text)

        n = ord(s[-1])
        for i in range(n):
            if s[-1-i] != s[-1]:
                text = "Bad padding %d" % ord(s[-1])
                raise Exception(text)
        return s[: - n]

    def get_encrypted_flag(self):
        iv = os.urandom(self.block_length)
        cipher = AES.new(self.KEY, AES.MODE_CBC, iv)
        return cipher.encrypt(self.__padding(FLAG)), iv ## TODO: remove

    def check_padding(self, s, iv):
        cipher = AES.new(self.KEY, AES.MODE_CBC, iv)
        t = cipher.decrypt(s)
        try:
            self.__unpadding(t)
            return True
        except:
            return False

print(os.urandom(16).encode("hex"))

print("____NEW GAME______")
e = Encryptor(KEY)
while True:
    print("1 - Get encrypted flag. Format: {text, iv} in hex encoding\n"
          "2 - Check padding of ciphertext. Format: {\"text, iv\"} in hex encoding\n")
    answer = input()
    if answer == 1:
        tx, iv = e.get_encrypted_flag()
        print(tx.encode("hex"), iv.encode("hex"))
    elif answer == 2:
        print("Enter text: ")
        text = input()
        ct, iv = text.split(', ')
        print(str(e.check_padding(ct.decode("hex"), iv.decode("hex"))))
    else:
        print("Bad input")

