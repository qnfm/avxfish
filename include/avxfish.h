#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AVXFISH_BLOCK_BYTES 128u
#define AVXFISH_BLOCK_WORDS 16u
#define AVXFISH_TWEAK_WORDS 2u
#define AVXFISH_SUBKEY_COUNT 21u
#define AVXFISH_EXPANDED_KEY_WORDS (AVXFISH_SUBKEY_COUNT * AVXFISH_BLOCK_WORDS)
#define AVXFISH_EXPANDED_KEY_BYTES (AVXFISH_EXPANDED_KEY_WORDS * sizeof(uint64_t))

/*
 * Expanded subkey layout:
 *   - all 21 subkeys are stored in normal Threefish word order.
 *
 * Alignment is no longer required for block pointers. The implementation uses
 * unaligned loads/stores internally. 64-byte alignment is still recommended for
 * expanded subkeys and buffers for best performance.
 */

/* Backward-compatible name from the original repo. */
void threefish1024_key_schedule(const void *key, const void *tweak, uint64_t *subkeys);

/* Backward-compatible encryption API: encrypts one 1024-bit block in place. */
void avxfish(void *block, const void *subkeys);

/* New explicit block APIs. in and out may alias. */
void avxfish_encrypt_block(const void *in, void *out, const void *subkeys);
void avxfish_decrypt_block(const void *in, void *out, const void *subkeys);

/* New in-place decryption API. */
void avxfish_decrypt(void *block, const void *subkeys);

/* Returns non-zero when OS and CPU state allow AVX-512F execution. */
int avxfish_avx512_available(void);

#ifdef __cplusplus
}
#endif
