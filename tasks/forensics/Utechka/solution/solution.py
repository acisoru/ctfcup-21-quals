import struct

u32 = lambda x : struct.unpack("<L", x)[0]

data = open("stream.hex", 'rb').read()

idx = 0
border = 0
file_map = {}

while idx < len(data):
    header = data[idx:idx+0x10]
    if header[:4] != b'FSUP':
        break

    idx += 0x10
    start = u32(header[4:8])
    size = u32(header[8:12])
    chunk = data[idx:idx+size]
    idx += size

    file_map[chunk] = [start, size]

out = [0 for i in range(0, len(data))]

for i in file_map.keys():
    start = file_map[i][0]
    size = file_map[i][1]
    out[start:start+size] = i

fd = open("dump.bin.lzma", 'wb')
fd.write(bytes(out))
fd.close()