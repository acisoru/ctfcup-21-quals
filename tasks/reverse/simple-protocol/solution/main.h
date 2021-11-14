#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "serpent.h"

uint8_t* SerpentKey;

void __attribute__ ((constructor)) Init(void);
static inline uint64_t mul_mod_p(uint64_t, uint64_t);
static inline uint64_t pow_mod_p(uint64_t, uint64_t);

void InitSerpentKey(uint8_t*, uint64_t);
