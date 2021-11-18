#include "main.h"

#define P 0xffffffffffffffc5ull
#define G 0xf549e9b5207189bcull

int rnd_dev;
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
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
};

void get_rnd_bytes(void* buffer, int size)
{   
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

    // for (int i = 0; i < 16; i++) {
    //     printf("%02x", OutKey[i]);
    // }
    // puts("");

};

int main(int argc, char* argv[], char* envp[])
{
    if (argc != 3) {
        puts("{-} Usage: ./client <host> <port>");
        exit(0);
    }
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    
    uint16_t Port = atoi(argv[2]);
    serv_addr.sin_port = htons(Port);

    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        puts("Connection Failed!");
        return -1;
    }

    // generate a
    uint64_t a = 0;
    get_rnd_bytes(&a, 8);
    uint64_t A = pow_mod_p(G, a);
    send(sock, &A, 8, 0);

    uint64_t B = 0;
    recv(sock, &B, 8, 0);

    uint64_t K = 0;
    K = pow_mod_p(B, a);

    SerpentKey = (uint8_t*) malloc(16);
    InitSerpentKey(SerpentKey, K);

    // send open file packet
    uint8_t* Packet = (uint8_t*) malloc(32);
    memset(Packet, 0, 32);
    uint8_t* EncPacket = (uint8_t*) malloc(32);
    memset(EncPacket, 0, 32);

    // make magic
    Packet[0] = 0x22;
    Packet[1] = 0x33;
    Packet[2] = 0x01; // open file cmd
    Packet[3] = 0x8; // filename size
    Packet[4] = 0x0;
    Packet[5] = 0x0;
    Packet[6] = 0x0;
    memcpy(Packet+7, "flag.txt", 8);

    serpent_encrypt_bitslice(Packet, SerpentKey, EncPacket, 16);
    serpent_encrypt_bitslice(Packet+16, SerpentKey, EncPacket+16, 16);
    send(sock, EncPacket, 32, 0);

    // read file
    memset(Packet, 0, 32);
    memset(EncPacket, 0, 32);

    Packet[0] = 0x22;
    Packet[1] = 0x33;
    Packet[2] = 0x02; // read file cmd
    Packet[3] = 0x10; // payload size
    Packet[4] = 0x0;
    Packet[5] = 0x0; 
    Packet[6] = 0x0;
    Packet[7] = 0x0;
    Packet[8] = 0x0;
    Packet[9] = 0x30; // read size

    serpent_encrypt_bitslice(Packet, SerpentKey, EncPacket, 16);
    serpent_encrypt_bitslice(Packet+16, SerpentKey, EncPacket+16, 16);
    send(sock, EncPacket, 32, 0);


    // close file
    memset(Packet, 0, 32);
    memset(EncPacket, 0, 32);

    Packet[0] = 0x22;
    Packet[1] = 0x33;
    Packet[2] = 0x03; // read file cmd

    serpent_encrypt_bitslice(Packet, SerpentKey, EncPacket, 16);
    serpent_encrypt_bitslice(Packet+16, SerpentKey, EncPacket+16, 16);
    send(sock, EncPacket, 32, 0);

    // get data
    memset(Packet, 0, 32);
    memset(EncPacket, 0, 32);

    Packet[0] = 0x22;
    Packet[1] = 0x33;
    Packet[2] = 0x04; // open file cmd
    Packet[3] = 0x8; // token size
    Packet[4] = 0x0;
    Packet[5] = 0x0;
    Packet[6] = 0x0;
    // crchack variant
    // token: IkaolIe1n392
    memcpy(Packet+7, "\x61\x62\x63\x64\xF7\x97\xBB\x66", 8);

    serpent_encrypt_bitslice(Packet, SerpentKey, EncPacket, 16);
    serpent_encrypt_bitslice(Packet+16, SerpentKey, EncPacket+16, 16);
    send(sock, EncPacket, 32, 0);

    uint8_t* flag = (uint8_t*) malloc(0x128);
    int nbytes = recv(sock, flag, 128, 0);
    uint8_t* out = (uint8_t*) malloc(nbytes);
    
    for (int i = 0; i < nbytes; i+=16)
        serpent_decrypt_bitslice(flag+i, SerpentKey, out+i, 16);

    printf("flag = %s\n", out);

    close(sock);
    return 0;
};
