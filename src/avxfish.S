.intel_syntax noprefix

.global avxfish

#ifdef _WIN64
# define ARG0 rcx
# define ARG1 rdx
# define ARG2 r8
#else
# define ARG0 rdi
# define ARG1 rsi
# define ARG2 rdx
#endif

.set next, 1
.set load, 2
.set last, 3

// per-round key offsets: round N uses keys at ARG1 + koff<N> and ARG1 + koff<N> + 0x80
.set koff0, 0x000
.set koff1, 0x100
.set koff2, 0x200
.set koff3, 0x300
.set koff4, 0x400
.set koff5, 0x500
.set koff6, 0x600
.set koff7, 0x700
.set koff8, 0x800
.set koff9, 0x900

.macro MIX r:req, mode:req
  .ifeq \mode - load
    vmovdqa64 zmm1\r, [rip + vrotate\r]
  .endif
  vpaddq    zmm0, zmm0, zmm1
  vprolvq   zmm1, zmm1, zmm1\r
  vpxorq    zmm1, zmm1, zmm0
.endm

.macro PERMUTE mode:req
  .ifeq \mode - load
    vmovdqa64 zmm30, [rip + permutea]
    vmovdqa64 zmm31, [rip + permuteb]
  .endif
  vpermq    zmm0, zmm30, zmm0
  vpermq    zmm1, zmm31, zmm1
.endm

.macro ENC round:req, mode:req
  vpaddq zmm0, zmm0, [ARG1 + koff\round + 0x00]
  vpaddq zmm1, zmm1, [ARG1 + koff\round + 0x40]
  MIX 0, \mode
  PERMUTE \mode
  MIX 1, \mode
  PERMUTE next
  MIX 2, \mode
  PERMUTE next
  MIX 3, \mode
  PERMUTE next
  vpaddq zmm0, zmm0, [ARG1 + koff\round + 0x80]
  vpaddq zmm1, zmm1, [ARG1 + koff\round + 0xC0]
  MIX 4, \mode
  PERMUTE next
  MIX 5, \mode
  PERMUTE next
  MIX 6, \mode
  PERMUTE next
  MIX 7, \mode
  .ifne \mode - last
    PERMUTE next
  .endif
.endm

.macro VECTORIZE
  vmovdqa64 zmm0, [rip + permveca]
  vmovdqa64 zmm1, [rip + permvecb]
  vpermi2q  zmm0, zmm2, zmm3
  vpermi2q  zmm1, zmm2, zmm3
.endm

.macro DEVECTORIZE
  vmovdqa64 zmm2, [rip + perminva]
  vmovdqa64 zmm3, [rip + perminvb]
  vpermi2q  zmm2, zmm0, zmm1
  vpermi2q  zmm3, zmm0, zmm1
.endm

.macro ADDLASTKEY
  vpaddq zmm0, zmm2, [ARG1 + 0xA00]
  vpaddq zmm1, zmm3, [ARG1 + 0xA40]
.endm

.text
.type avxfish, @function
avxfish:
  vmovdqa64 zmm2, [ARG0 + 0x00]
  vmovdqa64 zmm3, [ARG0 + 0x40]

  VECTORIZE
  ENC 0, load
  ENC 1, next
  ENC 2, next
  ENC 3, next
  ENC 4, next
  ENC 5, next
  ENC 6, next
  ENC 7, next
  ENC 8, next
  ENC 9, last
  DEVECTORIZE
  ADDLASTKEY

  vmovdqa64 [ARG0 + 0x00], zmm0
  vmovdqa64 [ARG0 + 0x40], zmm1
  ret

  .align 64

  // the vectorization permutation (interleave evens & odds)
permveca: .quad 0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E
permvecb: .quad 0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F

  // the Threefish permutation, accounting for vectorization
permutea: .quad 0x00, 0x01, 0x03, 0x02, 0x05, 0x06, 0x07, 0x04
permuteb: .quad 0x04, 0x06, 0x05, 0x07, 0x03, 0x01, 0x02, 0x00

  // the opposite of the Threefish permutation+vectorization
perminva: .quad 0x00, 0x0C, 0x01, 0x0E, 0x03, 0x0D, 0x02, 0x0F
perminvb: .quad 0x05, 0x0B, 0x06, 0x09, 0x07, 0x0A, 0x04, 0x08

  // the rotation constants from the Skein whitepaper (v1.3)
vrotate0: .quad 0x18, 0x0D, 0x08, 0x2F, 0x08, 0x11, 0x16, 0x25
vrotate1: .quad 0x26, 0x13, 0x0A, 0x37, 0x31, 0x12, 0x17, 0x34
vrotate2: .quad 0x21, 0x04, 0x33, 0x0D, 0x22, 0x29, 0x3B, 0x11
vrotate3: .quad 0x05, 0x14, 0x30, 0x29, 0x2F, 0x1C, 0x10, 0x19
vrotate4: .quad 0x29, 0x09, 0x25, 0x1F, 0x0C, 0x2F, 0x2C, 0x1E
vrotate5: .quad 0x10, 0x22, 0x38, 0x33, 0x04, 0x35, 0x2A, 0x29
vrotate6: .quad 0x1F, 0x2C, 0x2F, 0x2E, 0x13, 0x2A, 0x2C, 0x19
vrotate7: .quad 0x09, 0x30, 0x23, 0x34, 0x17, 0x1F, 0x25, 0x14

  .section .note.GNU-stack,"",@progbits
