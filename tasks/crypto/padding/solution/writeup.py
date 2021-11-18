import socket

# The new class to simplify bitwise XOR of two strings
class String(str):
    def __xor__(self, other):
        x = ''.join([chr(ord(self[i]) ^ ord(other[i])) for i in range(len(self))])
        return String(x)

    def __add__(self, other):
        return String(str(self) + str(other))

# Let ct1 = enc(pt1 ^ iv1) be the first block of the ciphertext
# By calling check_padding method we will check the padding of (pt1 = dec(ct1) ^ iv1).
# It is correct if:
#   - the last byte of it is equal 1
#   - the last two bytes of it are equal 2
# ...
#   - the last 16 bytes of it are equal 16
# So, we can XOR the last byte of iv1 with different values 0 < v <= 255 to guess the last byte of pt1.
# If check_padding(pt1_ = dec(ct1) ^ iv1_) is True, v = \x01 ^ pt1[-1]
# Knowing pt1[-1], we can obtain pt[-2] the same way


def query_to_check(block, iv):
        global SOCK
        SOCK.send("2\n")
        SOCK.recv(1024)
        r = "'%s, %s'\n" % (block.encode("hex"), iv.encode("hex"))
        SOCK.send(r)
        ans = SOCK.recv(1024)
        print("ans", ans)
        if ans[:4] == "True":
            return True
        elif ans[:5] == "False":
            return False
        else:
            print("NEW SOCKET")
            SOCK.close()
            SOCK = socket.socket()

            SOCK.connect(("0.0.0.0", 1687))
            data = SOCK.recv(1024)
            print(data)
            return query_to_check(block, iv)


def decrypt_one_block(block, iv):
    s = String("")
    for i in range(1, 17):
        for j in range(2, 256) + [0, 1]:
            iv_new = iv ^ \
                     String("\x00" * (16 - i) + chr(i) * i) ^ \
                     (String("\x00" * (16 - i)) + chr(j) + s)

            if query_to_check(block, iv_new):
                s = String(chr(j)) + s
                break
    return s

def hack(ct, iv):
    s = decrypt_one_block(ct[0:16], iv)
    for i in range(16, len(ct), 16):
        s += decrypt_one_block(ct[i: i + 16], String(ct[i - 16: i]))
    return s


SOCK = socket.socket()
SOCK.connect(("65.21.184.60", 1687))
data = SOCK.recv(1024)
print(data)

SOCK.send("1\n")
data = SOCK.recv(1024)
print(data)

block, iv = data.split("\n")[0][2: -4].split("', '")
print(block, iv)
print(2)
print(hack(String(block.decode("hex")), String(iv.decode("hex"))))

SOCK.close()

# "CUP{Wha7_we_feel_riding_h0rses_made_0f_st3el}"









