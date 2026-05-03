#include "../include/avxfish.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int same_words(const uint64_t *a, const uint64_t *b, size_t n) {
    return memcmp(a, b, n * sizeof(uint64_t)) == 0;
}

static void print_words(const char *name, const uint64_t *w, size_t n) {
    fprintf(stderr, "%s:", name);
    for (size_t i = 0; i < n; ++i) fprintf(stderr, " %016llX", (unsigned long long)w[i]);
    fprintf(stderr, "\n");
}

static int run_vector(const char *name, const uint64_t key[16], const uint64_t tweak[2],
                      const uint64_t pt[16], const uint64_t ct[16]) {
    uint64_t subkeys[AVXFISH_EXPANDED_KEY_WORDS];
    uint64_t out[16];
    uint64_t dec[16];

    threefish1024_key_schedule(key, tweak, subkeys);
    avxfish_encrypt_block(pt, out, subkeys);
    if (!same_words(out, ct, 16)) {
        fprintf(stderr, "%s encrypt failed\n", name);
        print_words("expected", ct, 16);
        print_words("actual  ", out, 16);
        return 1;
    }

    avxfish_decrypt_block(ct, dec, subkeys);
    if (!same_words(dec, pt, 16)) {
        fprintf(stderr, "%s decrypt failed\n", name);
        print_words("expected", pt, 16);
        print_words("actual  ", dec, 16);
        return 1;
    }

    memcpy(out, pt, sizeof(out));
    avxfish(out, subkeys);
    if (!same_words(out, ct, 16)) {
        fprintf(stderr, "%s in-place encrypt failed\n", name);
        return 1;
    }
    avxfish_decrypt(out, subkeys);
    if (!same_words(out, pt, 16)) {
        fprintf(stderr, "%s in-place decrypt failed\n", name);
        return 1;
    }

    return 0;
}

int main(void) {
    const uint64_t zero_key[16] = {0};
    const uint64_t zero_tw[2] = {0};
    const uint64_t zero_pt[16] = {0};
    const uint64_t zero_ct[16] = {
        UINT64_C(0x04B3053D0A3D5CF0), UINT64_C(0x0136E0D1C7DD85F7),
        UINT64_C(0x067B212F6EA78A5C), UINT64_C(0x0DA9C10B4C54E1C6),
        UINT64_C(0x0F4EC27394CBACF0), UINT64_C(0x32437F0568EA4FD5),
        UINT64_C(0xCFF56D1D7654B49C), UINT64_C(0xA2D5FB14369B2E7B),
        UINT64_C(0x540306B460472E0B), UINT64_C(0x71C18254BCEA820D),
        UINT64_C(0xC36B4068BEAF32C8), UINT64_C(0xFA4329597A360095),
        UINT64_C(0xC4A36C28434A5B9A), UINT64_C(0xD54331444B1046CF),
        UINT64_C(0xDF11834830B2A460), UINT64_C(0x1E39E8DFE1F7EE4F)
    };

    const uint64_t tv10_tw[2] = {UINT64_C(0x0706050403020100), UINT64_C(0x0F0E0D0C0B0A0908)};
    const uint64_t tv10_key[16] = {
        UINT64_C(0x1716151413121110), UINT64_C(0x1F1E1D1C1B1A1918),
        UINT64_C(0x2726252423222120), UINT64_C(0x2F2E2D2C2B2A2928),
        UINT64_C(0x3736353433323130), UINT64_C(0x3F3E3D3C3B3A3938),
        UINT64_C(0x4746454443424140), UINT64_C(0x4F4E4D4C4B4A4948),
        UINT64_C(0x5756555453525150), UINT64_C(0x5F5E5D5C5B5A5958),
        UINT64_C(0x6766656463626160), UINT64_C(0x6F6E6D6C6B6A6968),
        UINT64_C(0x7776757473727170), UINT64_C(0x7F7E7D7C7B7A7978),
        UINT64_C(0x8786858483828180), UINT64_C(0x8F8E8D8C8B8A8988)
    };
    const uint64_t tv10_pt[16] = {
        UINT64_C(0xF8F9FAFBFCFDFEFF), UINT64_C(0xF0F1F2F3F4F5F6F7),
        UINT64_C(0xE8E9EAEBECEDEEEF), UINT64_C(0xE0E1E2E3E4E5E6E7),
        UINT64_C(0xD8D9DADBDCDDDEDF), UINT64_C(0xD0D1D2D3D4D5D6D7),
        UINT64_C(0xC8C9CACBCCCDCECF), UINT64_C(0xC0C1C2C3C4C5C6C7),
        UINT64_C(0xB8B9BABBBCBDBEBF), UINT64_C(0xB0B1B2B3B4B5B6B7),
        UINT64_C(0xA8A9AAABACADAEAF), UINT64_C(0xA0A1A2A3A4A5A6A7),
        UINT64_C(0x98999A9B9C9D9E9F), UINT64_C(0x9091929394959697),
        UINT64_C(0x88898A8B8C8D8E8F), UINT64_C(0x8081828384858687)
    };
    const uint64_t tv10_ct[16] = {
        UINT64_C(0xB0C33CD7DB4D65A6), UINT64_C(0xBC49A85A1077D75D),
        UINT64_C(0x6855FCAFEA7293E4), UINT64_C(0x1C5385AB1B7754D2),
        UINT64_C(0x30E4AAFFE780F794), UINT64_C(0xE1BBEE708CAFD8D5),
        UINT64_C(0x9CA837B7423B0F76), UINT64_C(0xBD1403670D4963B3),
        UINT64_C(0x451F2E3CE61EA48A), UINT64_C(0xB360832F9277D4FB),
        UINT64_C(0x0AAFC7A65E12D688), UINT64_C(0xC8906E79016D05D7),
        UINT64_C(0xB316570A15F41333), UINT64_C(0x74E98A2869F5D50E),
        UINT64_C(0x57CE6F9247432BCE), UINT64_C(0xDE7CDD77215144DE)
    };

    if (avxfish_avx512_available()) {
        printf("AVX-512F available: yes\n");
    } else {
        printf("AVX-512F available according to CPUID/XGETBV: no\n");
        printf("The binary may still run only on AVX-512-capable machines.\n");
    }

    if (run_vector("Threefish-1024 TV9", zero_key, zero_tw, zero_pt, zero_ct)) return 1;
    if (run_vector("Threefish-1024 TV10", tv10_key, tv10_tw, tv10_pt, tv10_ct)) return 1;

    puts("avxfish AVX-512 intrinsics encrypt/decrypt tests passed");
    return 0;
}
