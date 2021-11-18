#include <stdio.h>
#include <string.h>

int main() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);

    puts("Test app!");
    char tmp[128];
    memset(tmp, 0, 128);
    printf("Enter input: ");
    gets(tmp);
    printf("Your input: %s\n", tmp);
    return 0;
}