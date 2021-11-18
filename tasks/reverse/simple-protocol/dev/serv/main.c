#include "main.h"

#define P 0xffffffffffffffc5ull
#define G 0xf549e9b5207189bcull

int rnd_dev;

void* alloc(uint64_t size)
{
    void* ptr = HeapPtr + HeapOffset;
    HeapOffset += size;
    return ptr;
};

static inline uint64_t mul_mod_p(uint64_t a, uint64_t b) {
	uint64_t m = 0;
	while(b) {
		if(b&1) {
			uint64_t t = P-a;
			if ( m >= t) {
				m -= t;
			} else {
				m += a;
			}
		}
		if (a >= P - a) {
			a = a * 2 - P;
		} else {
			a = a * 2;
		}
		b>>=1;
	}
	return m;
}

static inline uint64_t pow_mod_p(uint64_t a, uint64_t b) {
	if (b==1) {
		return a;
	}
	uint64_t t = pow_mod_p(a, b>>1);
	t = mul_mod_p(t,t);
	if (b % 2) {
		t = mul_mod_p(t, a);
	}
	return t;
}

void __attribute__ ((constructor)) Init(void)
{
    // setvbuf(stdin, 0, 2, 0);
    // setvbuf(stdout, 0, 2, 0);
    // setvbuf(stderr, 0, 2, 0);

    HeapPtr = makeOwnHeap(HEAP_SIZE);
    HeapOffset = 0;
};

void* makeOwnHeap(uint32_t size)
{
    __asm__(
        "mov rsi, rdi\n\t"
        "xor r9, r9\n\t"
        "xor r8, r8\n\t"
        "xor rdi, rdi\n\t"
        "push 9\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "push 3\n\t"
        "pop rdx\n\t"
        "push 0x22\n\t"
        "pop r10\n\t"
        "syscall\n\t"
        "pop r11\n\t"
    );
};

// read(0, buf, size)
int get_input(int fd, char* buf, uint32_t size)
{
    __asm__(
        "push 0\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "syscall\n\t"
        "pop r11\n\t"
    );
    return 1;
};

void write_data(int fd, char* ptr, int size)
{
    __asm__ (
        "push 1\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "syscall\n\t"
        "pop r10\n\t"
    );
};

// /dev/ura ndom
void get_rnd_bytes(void* buffer, int size)
{   
    // 0x13e
    // 0x131 + 0xf = 0x140
    __asm__ (
        "xchg rsi, rdx\n\t"
        "xor rdx, rdx\n\t"
        "xor rax, rax\n\t"
        "push 0x130\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "add rax, 0xe\n\t"
        "syscall\n\t"
        "pop r10\n\t"
    );
};

void InitSerpentKey(uint8_t* OutKey, uint64_t K)
{
    for (int i = 0; i < 16; i++) {
        OutKey[i] = i * 2;
        int ShiftSize = 8 * (i % 8);
        OutKey[i] ^= (K >> ShiftSize) & 0xff;
    }
};

int main()
{
    // checkToken("IkaolIe1n392\x00", 12);
    // open_file("./flag.txt");
    // get A from client
    uint64_t A = 0;
    get_input(0, (char*)&A, 8);

    // generate and print B
    uint64_t b = 0;
    get_rnd_bytes(&b, 8);
    uint64_t B = pow_mod_p(G, b);
    write_data(1, (char*)&B, 8);

    uint64_t K = 0;
    K = pow_mod_p(A, b);

    SerpentKey = alloc(16);
    InitSerpentKey(SerpentKey, K);

    while (1) {
        // get packet
        uint8_t* encClientPacket = alloc(32);
        uint8_t* decClientPacket = alloc(32);

        get_input(0, encClientPacket, 32);
        // decrypt packet
        serpent_decrypt_bitslice(encClientPacket, SerpentKey, decClientPacket, 16);
        serpent_decrypt_bitslice(encClientPacket+16, SerpentKey, decClientPacket+16, 16);

        if (decClientPacket[0] != 0x22 || decClientPacket[1] != 0x33)
            EXIT(0);
        
        switch (decClientPacket[2]) {
            // open file
            case 1:
            {
                uint32_t size = 0;
                MyMemcpy(&size, decClientPacket+3, 4);
                if (size > 24)
                    EXIT(0);
                char* fname = alloc(size);
                MyMemcpy(fname, decClientPacket+7, size);

                if (GlobalFD == 0)
                {
                    GlobalFD = open_file(fname);
                    if (GlobalFD == -1)
                        EXIT(0);
                }
                else
                    EXIT(0);

                break;
            }
            // read file
            case 2:
            {
                uint16_t offset = 0;
                uint32_t psize = 0;
                uint32_t rsize = 0;
                
                MyMemcpy(&psize, decClientPacket + 3, 4);
                if (psize > 24)
                    EXIT(0);

                MyMemcpy(&offset, decClientPacket + 7, 2);
                MyMemcpy(&rsize, decClientPacket + 9, 4);

                if (rsize > 128)
                    EXIT(0);
                
                FileData = alloc(rsize);

                if (GlobalFD != 0 && GlobalFD != -1)
                {
                    get_input(GlobalFD, FileData, rsize);
                    DataReaded = 1;
                    FileDataSize = rsize;
                }
                else
                    EXIT(0);

                break;
            }
            // close file
            case 3:
            {
                if (GlobalFD != 0 && GlobalFD != -1) {
                    close_file(GlobalFD);
                    GlobalFD = 0;
                } else {
                    EXIT(0);
                }
                break;
            }
            // get file data
            case 4:
            {
                if (GlobalFD != 0)
                    EXIT(0);
                if (DataReaded == 0)
                    EXIT(0);
                
                uint32_t psize = 0;
                MyMemcpy(&psize, decClientPacket + 3, 4);

                if (psize > 24)
                    EXIT(0);

                uint8_t* token = alloc(psize);
                MyMemcpy(token, decClientPacket + 7, psize);

                if (checkToken(token, psize)) {
                    uint8_t* EncFileData = EncData(FileData, FileDataSize);
                    write_data(1, EncFileData, FileDataSize + 16 - (FileDataSize % 16));
                }
                else
                    EXIT(0);

                break;
            }
            default:
                EXIT(0);
        }
    }

    return 0;
};

uint8_t* EncData(uint8_t* Data, uint32_t size)
{
    uint32_t outSize = size + (16 - (size % 16));
    uint8_t* outData = alloc(outSize);

    for (int i = 0; i < size; i+=16) {
        serpent_encrypt_bitslice(Data + i, SerpentKey, outData+i, 16);
    }
    return outData;
};
// void log_to_file(void* data, uint32_t size)
// {
//     FILE* fp = fopen("log", "wb");
//     fwrite(data, 1, size, fp);
//     fclose(fp);
// };

int open_file(char* filename) {
    __asm__ (
        "push 0x2\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "pop r10\n\t"
        "xor rsi, rsi\n\t"
        "xor rdx, rdx\n\t"
        "syscall\n\t"
    );
    return;
};

void close_file(int fd) {
    __asm__ (
        "push 0x3\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "pop r10\n\t"
        "syscall\n\t"
    );
    return;
};

unsigned int xcrc32 (const unsigned char *buf, int len, unsigned int init)
{
  unsigned int crc = init;
  while (len--)
    {
      crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
      buf++;
    }
  return crc;
}

int checkToken(uint8_t* token, uint32_t tokenSize)
{   
    // IkaolIe1n392 - one of valid token
    uint32_t Crc = xcrc32(token, tokenSize, 0xffffffff);
    
    if (Crc != 0xdf705098)
        return 0;

    return 1;
};

void EXIT(int code)
{
    __asm__ (
        "push 0x3c\n\t"
        "mov rax, qword ptr[rsp]\n\t"
        "syscall\n\t"
    );
};

void MyMemcpy(void* dst, void* src, int size)
{
    uint8_t* Dst = (uint8_t*) dst;
    uint8_t* Src = (uint8_t*) src;

    for (int i = 0; i < size; i++) {
        Dst[i] = Src[i];
    }
}
