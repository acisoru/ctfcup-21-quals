#include <stdio.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define SHELLCODE_MAX_SIZE 73

void rules(void)
{
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(send), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(connect), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(alarm), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(pwrite64), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(pause), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(nanosleep), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(sendfile), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(sendmsg), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(accept), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(bind), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(listen), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(writev), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(fork), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(clone), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(kill), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(wait4), 0);

    seccomp_load(ctx);
};

void check_shellcode(uint8_t* buf, int size)
{
    uint8_t bad_vals[213] =  {3, 4, 6, 7, 8, 9, 10, 11, 13, 14, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 39, 40, 42, 43, 45, 47, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 81, 82, 86, 89, 92, 93, 96, 98, 99, 100, 101, 104, 105, 107, 109, 110, 111, 112, 113, 114, 115, 117, 118, 119, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 179, 180, 181, 182, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 255};
    
    for (int i = 0; i < size; i++)
    {
        uint8_t val = buf[i];

        for (int j = 0; j < 213; j++)
        {
            if (bad_vals[j] == val)
            {
                puts("{-} You use banned byte!");
                exit(0);
            }
        }
    }
};

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    uint8_t buf[SHELLCODE_MAX_SIZE];
    memset(buf, 0, SHELLCODE_MAX_SIZE);

    puts("--+++-- Shell Batya --+++--");
    printf("{!} Give me your shell: ");

    int nybtes = read(0, buf, SHELLCODE_MAX_SIZE);
    uint64_t arg;
    check_shellcode(buf, nybtes);
    printf("{!} Okay, I'll even let you pass one argument: ");
    scanf("%llu", &arg);

    rules();
    void *ptr = mmap(0, 0x1000, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    memcpy(ptr, buf, nybtes);
    void (*code)(uint64_t);

    close(0);
    close(1);
    close(2);
    code = (void (*)(uint64_t)) ptr;
    code(arg);
    return 0;
}
