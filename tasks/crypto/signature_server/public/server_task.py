from hashlib import sha1
from random import randint
from json import loads, dumps
from ecdsa import SigningKey, SECP160r1
from ECDSA_key import secret_key, flag
from time import time
import binascii



Q = 0x0100000000000000000001f4c8f927aed3ca752257
ORDERLEN = (1 + len("%x" % Q)) // 2

N = pow(2, 100)


def string_to_number(string):
    return int(binascii.hexlify(string), 16)

def string_to_number_fixedlen(string):
    assert len(string) == ORDERLEN, (len(string), ORDERLEN)
    return int(binascii.hexlify(string), 16)

def number_to_string(num):
    fmt_str = "%0" + str(2 * ORDERLEN) + "x"
    string = binascii.unhexlify((fmt_str % num).encode())
    assert len(string) == ORDERLEN, (len(string), ORDERLEN)
    return string

def sigencode_string(r, s):
    def sigencode_strings(r, s):
        r_str = number_to_string(r)
        s_str = number_to_string(s)
        return (r_str, s_str)
    r_str, s_str = sigencode_strings(r, s)
    return r_str + s_str

def sigdecode_string(signature):
    l = ORDERLEN
    if not len(signature) == 2 * l:
        return False
    r = string_to_number_fixedlen(signature[:l])
    s = string_to_number_fixedlen(signature[l:])
    return r, s



class SignServer:
    def __init__(self):

        self.sk = SigningKey.from_string(secret_key, curve=SECP160r1)
        self.q = 0x0100000000000000000001f4c8f927aed3ca752257

    def __sign_number(self, m):
        assert m < N
        k = randint(0, self.q - 1)
        t = time()
        sig = self.sk.sign(number_to_string(m), k=k)
        t = int((time() - t) * 1000000000)
        r, s = sigdecode_string(sig)
        return [r, s], t

    def sign_number(self, m):
        [r, s], t = self.__sign_number(m)
        return [r, s], t

    def sign_array(self, m):
        m = loads(m)
        assert len(m) <= 5000
        r = []
        s = []
        t = []
        for i in xrange(len(m)):
            sig, tm = self.__sign_number(m[i])
            r.append(sig[0])
            s.append(sig[1])
            t.append(tm)
        return dumps({"r": r, "s": s, "t": t})

    def get_flag(self):
        n = randint(N, self.q - 1)

        print("Send signature for %d\n (Result of sk.sign(number_to_string(n)).encode('hex'))" %n)
        sig = input()

        v = self.sk.get_verifying_key().verify(sig.decode("hex"), number_to_string(n), hashfunc=sha1)

        if v:
            print flag
        else:
            print "Wrong signature"


ssv = SignServer()
while True:
    print("1 - Sign number\n"
          "2 - Sign each number in array (less than 5.000 numbers)\n"
          "3 - Get flag\n")
    answer = input()


    if answer == 1:
        print("Send number n < 0x0100000000000000000001f4c8f927aed3ca752257 to sign")
        n = input()
        [r, s], t = ssv.sign_number(n)
        print("Signature: %s, time (in nanoseconds): %d" % (str([r, s]), t))

    elif answer == 2:
        print("Send array in json (divided into blocks of length 1000)")

        ar = ""
        while (True):
            arr = input()
            ar += arr
            print("block recieved")
            if arr[-1] == "]":
                break

        s = ssv.sign_array(ar)
        for i in range(0, len(s), 1000):
            block = s[i: i + 1000]
            print(block)

        print()


    elif answer == 3:
        ssv.get_flag()

    else:
        print("Bad input")

