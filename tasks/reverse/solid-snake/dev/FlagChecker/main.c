typedef unsigned long long int uint64_t;
typedef unsigned int uint32_t;

void write_data(int fd, char* ptr, int size)
{
    __asm__ (
        "xor rax, rax\n\t"
        "inc rax\n\t"
        "syscall\n\t"
    );
};

// 1. check size
// 2. check format "CUP{}"
// 3. check symbols
uint64_t encrypt(uint32_t a, uint32_t b, uint32_t* key)
{
    uint64_t r;
    uint32_t c, kw = 4;

    for (r = 0; r < kw * 4 + 32; r++) 
    {
        c = b;
        b += a + ((b<<6) ^ (b>>8)) + key[r % kw] + r;
        a = c;
    }

    uint64_t result = 0;
    result += (uint64_t)a * 0x100000000;
    result += b;

    return result;
};

int check_flag(char* flag, int size)
{
    uint32_t keys[4];
    memcpy((void*)&keys[0], (void*)0x400000, 16);
    //memcpy((void*)&keys[0], "\x7F\x45\x4C\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16);

    uint64_t corrects[4];
    corrects[0] = 0x8eb862698101d2e3;
    corrects[1] = 0xb94d2d75d8a8e392;
    corrects[2] = 0xae3a9a55e12f625c;
    corrects[3] = 0x598ed563faa45447;
    int idx = 0;

    for (int i = 4; i < size - 1; i += 8)
    {
        uint32_t blocks[2];
        memcpy((void*)&blocks[0], (uint32_t*)(flag + i), 4);
        memcpy((void*)&blocks[1], (uint32_t*)(flag + i + 4), 4);

        uint64_t enc = encrypt(blocks[0], blocks[1], keys);

        if (enc != corrects[idx++])
            return 0;
    }

    return 1;
};

int get_len(char* buf)
{
    __asm__ (
        "xor rax, rax\n\t"
        "_next:\n\t"
        "cmp byte ptr [rdi], 0x0\n\t"
        "jz _fini\n\t"
        "inc rax\n\t"
        "inc rdi\n\t"
        "jmp _next\n\t"
        "_fini:\n\t"
    );
    return;
};

int _start(char* flag)
{
    int size = get_len(flag);

    if (flag[size - 1] == '\n' && flag[size - 2 ] == ')' && flag[size - 3] == '"')
    {
        flag[size - 3] = '\0';
        size = get_len(flag);
    }

    if (size = 37)
    {
        if (check_flag(flag, size))
        {
            write_data(1, (char*)0x76BDFE, 7); // correct 0x76BDFE
        }
        else
        {
            write_data(1, (char*)0x712CB1, 9); // incorrect 0x712CB1
        } 
    }

    return 0;
}