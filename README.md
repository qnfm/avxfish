# avxfish intrinsics edition

This variant replaces the original NASM-only Threefish-1024 AVX-512 path with a C++17 AVX-512 intrinsics implementation.

## What changed

- `src/avxfish.cpp` implements both encryption and decryption with AVX-512 intrinsics.
- `include/avxfish.h` keeps the original `avxfish(block, subkeys)` encryption entry point.
- New APIs:
  - `avxfish_encrypt_block(in, out, subkeys)`
  - `avxfish_decrypt_block(in, out, subkeys)`
  - `avxfish_decrypt(block, subkeys)`
  - `avxfish_avx512_available()`
- The test program now checks the Threefish-1024 vectors present in `KAT_MCT/skein_golden_kat_internals.txt`: raw encryption, decryption, in-place APIs, and the Skein plaintext-feedforward value computed externally as `ciphertext XOR plaintext`.
- The benchmark now measures both encryption and decryption.

## Build

```sh
make
make run-test
make run-bench
```

The default Makefile uses:

```sh
-mavx512f -mavx512vl
```

`AVX-512VL` is kept for compatibility with the original repo's requirement, although the current 512-bit intrinsics route mainly relies on AVX-512F operations.

## API usage

```c
#include "avxfish.h"

uint64_t subkeys[AVXFISH_EXPANDED_KEY_WORDS];
threefish1024_key_schedule(key_words, tweak_words, subkeys);

avxfish_encrypt_block(plain_128_bytes, cipher_128_bytes, subkeys);
avxfish_decrypt_block(cipher_128_bytes, plain_128_bytes, subkeys);
```

The old in-place encryption call still works:

```c
avxfish(block_128_bytes, subkeys);
```

## Alignment

The original NASM version required 64-byte aligned block and subkey pointers. This intrinsics implementation uses unaligned loads and stores for blocks, so `in` and `out` no longer need special alignment. 64-byte alignment is still recommended for subkeys and buffers.

## Notes

The expanded subkey format is now normal Threefish word order for all 21 subkeys. This is easier to verify against standard test vectors and simpler to embed in other libraries.

## Build outputs

All generated object files and binaries are placed under `build/`:

```bash
make
./build/test
./build/bench
```

The convenience targets `make run-test` and `make run-bench` also use these paths.
