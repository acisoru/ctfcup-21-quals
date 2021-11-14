#include <string.h>
#include <stdio.h>

typedef unsigned long long int uint64_t;
typedef unsigned int uint32_t;

uint64_t decrypt(uint32_t a, uint32_t b, uint32_t* key)
{
    uint64_t r;
    uint32_t c, kw = 4;

    for (r = kw * 4 + 31; r != -1; r--) {
        c = a;
        a = b -= a + ((a << 6) ^ (a >> 8)) + key[r % kw] + r;
        b = c;
    }

    uint64_t result = 0;
    result += (uint64_t)a * 0x100000000;
    result += b;

    return result;
};

int main()
{
    uint32_t keys[4];
    memcpy((void*)&keys[0], "\x7F\x45\x4C\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

    uint64_t corrects[4];
    corrects[0] = 0x8eb862698101d2e3;
    corrects[1] = 0xb94d2d75d8a8e392;
    corrects[2] = 0xae3a9a55e12f625c;
    corrects[3] = 0x598ed563faa45447;
    int idx = 0;

    printf("CUP{");

    for (int i = 0; i < 4; i++)
    {
        uint32_t blocks[2];
        blocks[0] = corrects[i] >> 32;
        blocks[1] = corrects[i] & 0xffffffff;

        uint64_t dec = decrypt(blocks[0], blocks[1], keys);
        uint32_t a = dec >> 32;
        uint32_t b = dec & 0xffffffff;

        printf("%c%c%c%c", ((char*)&a)[0], ((char*)&a)[1], ((char*)&a)[2], ((char*)&a)[3]);
        printf("%c%c%c%c", ((char*)&b)[0], ((char*)&b)[1], ((char*)&b)[2], ((char*)&b)[3]); 

    }
    puts("}");

    return 1;
};
