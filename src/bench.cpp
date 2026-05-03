#include "../include/avxfish.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef AVXFISH_BENCH_BLOCKS
#define AVXFISH_BENCH_BLOCKS (1u << 18) /* 32 MiB */
#endif

static double seconds_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

int main(void) {
    const size_t blocks = AVXFISH_BENCH_BLOCKS;
    const size_t bytes = blocks * AVXFISH_BLOCK_BYTES;
    uint8_t *buf = (uint8_t *)aligned_alloc(64, bytes);
    uint8_t *tmp = (uint8_t *)aligned_alloc(64, bytes);
    if (!buf || !tmp) {
        fprintf(stderr, "allocation failed\n");
        free(buf); free(tmp);
        return 1;
    }

    uint64_t key[16] = {0};
    uint64_t tweak[2] = {0};
    uint64_t subkeys[AVXFISH_EXPANDED_KEY_WORDS];
    for (unsigned i = 0; i < 16; ++i) key[i] = UINT64_C(0x0102030405060708) * (i + 1);
    tweak[0] = UINT64_C(0x0706050403020100);
    tweak[1] = UINT64_C(0x0F0E0D0C0B0A0908);
    threefish1024_key_schedule(key, tweak, subkeys);

    for (size_t i = 0; i < bytes; ++i) buf[i] = (uint8_t)(i * 131u + 17u);

    double t0 = seconds_now();
    for (size_t i = 0; i < blocks; ++i)
        avxfish_encrypt_block(buf + i * AVXFISH_BLOCK_BYTES, tmp + i * AVXFISH_BLOCK_BYTES, subkeys);
    double t1 = seconds_now();
    for (size_t i = 0; i < blocks; ++i)
        avxfish_decrypt_block(tmp + i * AVXFISH_BLOCK_BYTES, buf + i * AVXFISH_BLOCK_BYTES, subkeys);
    double t2 = seconds_now();

    const double mib = (double)bytes / (1024.0 * 1024.0);
    printf("encrypt: %.2f MiB/s\n", mib / (t1 - t0));
    printf("decrypt: %.2f MiB/s\n", mib / (t2 - t1));

    free(buf); free(tmp);
    return 0;
}
