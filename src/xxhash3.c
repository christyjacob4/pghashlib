#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* xxHash3 constants */
#define XXH3_SECRET_SIZE_MIN    136
#define XXH3_SECRET_DEFAULT_SIZE 192
#define XXH3_MIDSIZE_MAX        240
#define XXH3_STRIPE_LEN         64
#define XXH3_ACC_NB             8
#define XXH3_SECRET_CONSUME_RATE 8
#define XXH3_SECRET_MERGEACCS_START 11
#define XXH3_SECRET_LASTACC_START 7
#define XXH3_MIDSIZE_STARTOFFSET 3
#define XXH3_MIDSIZE_LASTOFFSET 17

/* Prime constants */
#define PRIME32_1   0x9E3779B1U
#define PRIME32_2   0x85EBCA77U  
#define PRIME32_3   0xC2B2AE3DU
#define PRIME64_1   0x9E3779B185EBCA87ULL
#define PRIME64_2   0xC2B2AE3D27D4EB4FULL
#define PRIME64_3   0x165667B19E3779F9ULL
#define PRIME64_4   0x85EBCA77C2B2AE63ULL
#define PRIME64_5   0x27D4EB2F165667C5ULL

/* xxHash3 128-bit result structure */
typedef struct {
    uint64_t low64;
    uint64_t high64;
} XXH128_hash_t;

/* Forward declarations */
static uint64_t XXH3_avalanche(uint64_t h64);
static XXH128_hash_t XXH3_avalanche128(XXH128_hash_t h128);
static uint64_t XXH3_len_1to3_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed);
static uint64_t XXH3_len_4to8_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed);
static uint64_t XXH3_len_9to16_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed);
static uint64_t XXH3_mix16B(const uint8_t* input, const uint8_t* secret, uint64_t seed);
static XXH128_hash_t XXH3_len_1to3_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed);
static XXH128_hash_t XXH3_len_4to8_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed);
static XXH128_hash_t XXH3_len_9to16_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed);
static XXH128_hash_t XXH3_mix16B_128b(const uint8_t* input, const uint8_t* secret, uint64_t seed);
static void XXH3_hashLong_internal_loop(uint64_t* acc, const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize);
static uint64_t XXH3_64bits_withSecret(const void* input, size_t len, const void* secret, size_t secretSize);
static XXH128_hash_t XXH3_128bits_withSecret(const void* input, size_t len, const void* secret, size_t secretSize);

/* Default secret array - 192 bytes of pseudorandom data */
static const uint8_t kSecret[XXH3_SECRET_DEFAULT_SIZE] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e
};

/* Utility functions */
static uint32_t XXH_read32(const void* memPtr) {
    uint32_t val;
    memcpy(&val, memPtr, sizeof(val));
    return val;
}

static uint64_t XXH_read64(const void* memPtr) {
    uint64_t val;
    memcpy(&val, memPtr, sizeof(val));
    return val;
}

static uint64_t XXH_rotl64(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

static uint32_t XXH_rotl32(uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}

/* 64x64 -> 128 bit multiplication */
static XXH128_hash_t XXH_mult64to128(uint64_t lhs, uint64_t rhs) {
    /*
     * GCC/Clang __int128 method.
     * The __int128 method is simpler and usually faster, but requires GCC-style
     * 128-bit integers. If you are compiling on an odd platform which doesn't
     * support __int128, you'll have to use the portable one.
     */
#if defined(__SIZEOF_INT128__) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 128)
    __uint128_t const product = (__uint128_t)lhs * (__uint128_t)rhs;
    XXH128_hash_t r128;
    r128.low64  = (uint64_t)(product);
    r128.high64 = (uint64_t)(product >> 64);
    return r128;
#else
    /* Portable scalar method - slower but works everywhere */
    uint64_t const lo_lo = (lhs & 0xFFFFFFFF) * (rhs & 0xFFFFFFFF);
    uint64_t const hi_lo = (lhs >> 32)        * (rhs & 0xFFFFFFFF);
    uint64_t const lo_hi = (lhs & 0xFFFFFFFF) * (rhs >> 32);
    uint64_t const hi_hi = (lhs >> 32)        * (rhs >> 32);
    
    uint64_t const cross = (lo_lo >> 32) + (hi_lo & 0xFFFFFFFF) + lo_hi;
    uint64_t const upper = (hi_lo >> 32) + (cross >> 32)        + hi_hi;
    uint64_t const lower = (cross << 32) | (lo_lo & 0xFFFFFFFF);
    
    XXH128_hash_t r128;
    r128.low64  = lower;
    r128.high64 = upper;
    return r128;
#endif
}

/* Core mixing functions */
static uint64_t XXH64_avalanche(uint64_t hash) {
    hash ^= hash >> 33;
    hash *= PRIME64_2;
    hash ^= hash >> 29;
    hash *= PRIME64_3;
    hash ^= hash >> 32;
    return hash;
}

static uint64_t XXH3_rrmxmx(uint64_t h64, uint64_t len) {
    h64 ^= XXH_rotl64(h64, 49) ^ XXH_rotl64(h64, 24);
    h64 *= 0x9FB21C651E98DF25ULL;
    h64 ^= (h64 >> 35) + len;
    h64 *= 0x9FB21C651E98DF25ULL;
    return h64 ^ (h64 >> 28);
}

static uint64_t XXH3_avalanche(uint64_t h64) {
    h64 = h64 ^ (h64 >> 37);
    h64 *= 0x165667919E3779F9ULL;
    h64 = h64 ^ (h64 >> 32);
    return h64;
}

static uint64_t XXH3_mix2Accs(const uint64_t* acc, const uint8_t* secret) {
    return XXH_mult64to128(acc[0] ^ XXH_read64(secret), 
                          acc[1] ^ XXH_read64(secret + 8)).low64;
}

static uint64_t XXH3_mergeAccs(const uint64_t* acc, const uint8_t* secret, uint64_t start) {
    uint64_t result64 = start;
    
    result64 += XXH3_mix2Accs(acc + 0, secret + 0);
    result64 += XXH3_mix2Accs(acc + 2, secret + 16);
    result64 += XXH3_mix2Accs(acc + 4, secret + 32);
    result64 += XXH3_mix2Accs(acc + 6, secret + 48);
    
    return XXH3_avalanche(result64);
}

static XXH128_hash_t XXH3_avalanche128(XXH128_hash_t h128) {
    h128.low64  = h128.low64 ^ (h128.high64 >> 37);
    h128.low64  *= 0x165667919E3779F9ULL;
    h128.low64  = h128.low64 ^ (h128.low64 >> 32);
    
    h128.high64 = h128.high64 ^ (h128.low64 >> 37);
    h128.high64 *= 0x165667919E3779F9ULL;
    h128.high64 = h128.high64 ^ (h128.high64 >> 32);
    
    return h128;
}

/* Accumulation function for processing stripes */
static void XXH3_accumulate_512_64b(uint64_t* acc, const uint8_t* input, const uint8_t* secret) {
    for (size_t i = 0; i < XXH3_ACC_NB; i++) {
        uint64_t const data_val = XXH_read64(input + 8*i);
        uint64_t const data_key = data_val ^ XXH_read64(secret + 8*i);
        acc[i ^ 1] += data_val; /* swap adjacent lanes */
        acc[i] += (uint32_t)data_key * (data_key >> 32);
    }
}

static void XXH3_scrambleAcc(uint64_t* acc, const uint8_t* secret) {
    for (size_t i = 0; i < XXH3_ACC_NB; i++) {
        uint64_t const key64 = XXH_read64(secret + 8*i);
        uint64_t acc64 = acc[i];
        acc64 = acc64 ^ (acc64 >> 47);
        acc64 ^= key64;
        acc64 *= PRIME32_1;
        acc[i] = acc64;
    }
}

/* Helper functions */
static uint32_t bswap32(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(x);
#else
    return ((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x & 0x000000FF) << 24);
#endif
}

static uint64_t bswap64(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap64(x);
#else
    return ((x & 0xFF00000000000000ULL) >> 56) | ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0x0000FF0000000000ULL) >> 24) | ((x & 0x000000FF00000000ULL) >> 8) |
           ((x & 0x00000000FF000000ULL) << 8) | ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x000000000000FF00ULL) << 40) | ((x & 0x00000000000000FFULL) << 56);
#endif
}

/* Hash functions for different input lengths */

/* 1-3 bytes */
static uint64_t XXH3_len_1to3_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    uint8_t const c1 = input[0];
    uint8_t const c2 = input[len >> 1];
    uint8_t const c3 = input[len - 1];
    uint32_t const combined = ((uint32_t)c1 << 16) | ((uint32_t)c2 << 24) | ((uint32_t)c3 << 0) | ((uint32_t)len << 8);
    uint64_t const bitflip = (XXH_read32(secret) ^ XXH_read32(secret + 4)) + seed;
    uint64_t const keyed = (uint64_t)combined ^ bitflip;
    return XXH64_avalanche(keyed);
}

/* 4-8 bytes */
static uint64_t XXH3_len_4to8_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    seed ^= (uint64_t)bswap32((uint32_t)seed) << 32;
    uint32_t const input1 = XXH_read32(input);
    uint32_t const input2 = XXH_read32(input + len - 4);
    uint64_t const bitflip = (XXH_read64(secret + 8) ^ XXH_read64(secret + 16)) - seed;
    uint64_t const input64 = input2 + (((uint64_t)input1) << 32);
    uint64_t const keyed = input64 ^ bitflip;
    return XXH3_rrmxmx(keyed, len);
}

/* 9-16 bytes */
static uint64_t XXH3_len_9to16_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    uint64_t const bitflip1 = (XXH_read64(secret + 24) ^ XXH_read64(secret + 32)) + seed;
    uint64_t const bitflip2 = (XXH_read64(secret + 40) ^ XXH_read64(secret + 48)) - seed;
    uint64_t const input_lo = XXH_read64(input) ^ bitflip1;
    uint64_t const input_hi = XXH_read64(input + len - 8) ^ bitflip2;
    uint64_t const acc = len + bswap64(input_lo) + input_hi + XXH_mult64to128(input_lo, input_hi).low64;
    return XXH3_avalanche(acc);
}

/* 0-16 bytes */
static uint64_t XXH3_len_0to16_64b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    if (len > 8) return XXH3_len_9to16_64b(input, len, secret, seed);
    if (len >= 4) return XXH3_len_4to8_64b(input, len, secret, seed);
    if (len) return XXH3_len_1to3_64b(input, len, secret, seed);
    return XXH64_avalanche(seed ^ (XXH_read64(secret + 56) ^ XXH_read64(secret + 64)));
}

/* 16-byte mixing function */
static uint64_t XXH3_mix16B(const uint8_t* input, const uint8_t* secret, uint64_t seed) {
    uint64_t const input_lo = XXH_read64(input);
    uint64_t const input_hi = XXH_read64(input + 8);
    return XXH_mult64to128(
        input_lo ^ (XXH_read64(secret) + seed),
        input_hi ^ (XXH_read64(secret + 8) - seed)
    ).low64;
}

/* 17-128 bytes */
static uint64_t XXH3_len_17to128_64b(const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize, uint64_t seed) {
    uint64_t acc = len * PRIME64_1;
    
    if (len > 32) {
        if (len > 64) {
            if (len > 96) {
                acc += XXH3_mix16B(input + 48, secret + 96, seed);
                acc += XXH3_mix16B(input + len - 64, secret + 112, seed);
            }
            acc += XXH3_mix16B(input + 32, secret + 64, seed);
            acc += XXH3_mix16B(input + len - 48, secret + 80, seed);
        }
        acc += XXH3_mix16B(input + 16, secret + 32, seed);
        acc += XXH3_mix16B(input + len - 32, secret + 48, seed);
    }
    acc += XXH3_mix16B(input + 0, secret + 0, seed);
    acc += XXH3_mix16B(input + len - 16, secret + 16, seed);
    
    return XXH3_avalanche(acc);
}

/* 129-240 bytes */
static uint64_t XXH3_len_129to240_64b(const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize, uint64_t seed) {
    uint64_t acc = len * PRIME64_1;
    size_t const nbRounds = len / 16;
    
    for (size_t i = 0; i < 8; i++) {
        acc += XXH3_mix16B(input + (16 * i), secret + (16 * i), seed);
    }
    acc = XXH3_avalanche(acc);
    
    for (size_t i = 8; i < nbRounds; i++) {
        acc += XXH3_mix16B(input + (16 * i), secret + (16 * (i - 8)) + XXH3_MIDSIZE_STARTOFFSET, seed);
    }
    
    /* last bytes */
    acc += XXH3_mix16B(input + len - 16, secret + XXH3_SECRET_SIZE_MIN - XXH3_MIDSIZE_LASTOFFSET, seed);
    
    return XXH3_avalanche(acc);
}

/* Long hash (241+ bytes) */
static uint64_t XXH3_hashLong_64b(const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize) {
    uint64_t acc[XXH3_ACC_NB] = {PRIME32_3, PRIME64_1, PRIME64_2, PRIME64_3, PRIME64_4, PRIME32_2, PRIME64_5, PRIME32_1};
    
    XXH3_hashLong_internal_loop(acc, input, len, secret, secretSize);
    
    /* converge into final hash */
    return XXH3_mergeAccs(acc, secret + XXH3_SECRET_MERGEACCS_START, (uint64_t)len * PRIME64_1);
}

static void XXH3_accumulate(uint64_t* acc, const uint8_t* input, const uint8_t* secret, size_t nbStripes) {
    for (size_t n = 0; n < nbStripes; n++) {
        XXH3_accumulate_512_64b(acc, input + n*XXH3_STRIPE_LEN, secret + n*XXH3_SECRET_CONSUME_RATE);
    }
}

static void XXH3_hashLong_internal_loop(uint64_t* acc, const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize) {
    size_t const nb_rounds = (secretSize - XXH3_STRIPE_LEN) / XXH3_SECRET_CONSUME_RATE;
    size_t const block_len = XXH3_STRIPE_LEN * nb_rounds;
    size_t const nb_blocks = len / block_len;
    
    for (size_t n = 0; n < nb_blocks; n++) {
        XXH3_accumulate(acc, input + n*block_len, secret, nb_rounds);
        XXH3_scrambleAcc(acc, secret + secretSize - XXH3_STRIPE_LEN);
    }
    
    /* last partial block */
    size_t const nbStripes = ((len - 1) - (block_len * nb_blocks)) / XXH3_STRIPE_LEN;
    XXH3_accumulate(acc, input + nb_blocks*block_len, secret, nbStripes);
    
    /* last stripe */
    if (len & (XXH3_STRIPE_LEN - 1)) {
        const uint8_t* const p = input + len - XXH3_STRIPE_LEN;
        XXH3_accumulate_512_64b(acc, p, secret + secretSize - XXH3_STRIPE_LEN - XXH3_SECRET_LASTACC_START);
    }
}

/* Main 64-bit hash functions */
static uint64_t XXH3_64bits_internal(const void* input, size_t len, uint64_t seed, const uint8_t* secret, size_t secretLen) {
    if (len <= 16) return XXH3_len_0to16_64b((const uint8_t*)input, len, secret, seed);
    if (len <= 128) return XXH3_len_17to128_64b((const uint8_t*)input, len, secret, secretLen, seed);
    if (len <= XXH3_MIDSIZE_MAX) return XXH3_len_129to240_64b((const uint8_t*)input, len, secret, secretLen, seed);
    return XXH3_hashLong_64b((const uint8_t*)input, len, secret, secretLen);
}

static uint64_t XXH3_64bits(const void* input, size_t len) {
    return XXH3_64bits_internal(input, len, 0, kSecret, sizeof(kSecret));
}

static uint64_t XXH3_64bits_withSeed(const void* input, size_t len, uint64_t seed) {
    if (seed == 0) return XXH3_64bits(input, len);
    if (len <= XXH3_MIDSIZE_MAX) return XXH3_64bits_internal(input, len, seed, kSecret, sizeof(kSecret));
    return XXH3_64bits_withSecret(input, len, kSecret, sizeof(kSecret));
}

static uint64_t XXH3_64bits_withSecret(const void* input, size_t len, const void* secret, size_t secretSize) {
    return XXH3_64bits_internal(input, len, 0, (const uint8_t*)secret, secretSize);
}

/* 128-bit specific helper functions */
static XXH128_hash_t XXH3_len_0to16_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    if (len > 8) return XXH3_len_9to16_128b(input, len, secret, seed);
    if (len >= 4) return XXH3_len_4to8_128b(input, len, secret, seed);
    if (len) return XXH3_len_1to3_128b(input, len, secret, seed);
    XXH128_hash_t h128;
    uint64_t const bitflipl = XXH_read64(secret + 64) ^ XXH_read64(secret + 72);
    uint64_t const bitfliph = XXH_read64(secret + 80) ^ XXH_read64(secret + 88);
    h128.low64 = XXH64_avalanche(seed ^ bitflipl);
    h128.high64 = XXH64_avalanche(seed ^ bitfliph);
    return h128;
}

static XXH128_hash_t XXH3_len_1to3_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    uint8_t const c1 = input[0];
    uint8_t const c2 = input[len >> 1];
    uint8_t const c3 = input[len - 1];
    uint32_t const combinedl = ((uint32_t)c1 << 16) | ((uint32_t)c2 << 24) | ((uint32_t)c3 << 0) | ((uint32_t)len << 8);
    uint32_t const combinedh = XXH_rotl32(bswap32(combinedl), 13);
    uint64_t const bitflipl = (XXH_read32(secret) ^ XXH_read32(secret + 4)) + seed;
    uint64_t const bitfliph = (XXH_read32(secret + 8) ^ XXH_read32(secret + 12)) - seed;
    uint64_t const keyedl = (uint64_t)combinedl ^ bitflipl;
    uint64_t const keyedh = (uint64_t)combinedh ^ bitfliph;
    XXH128_hash_t h128;
    h128.low64 = XXH64_avalanche(keyedl);
    h128.high64 = XXH64_avalanche(keyedh);
    return h128;
}

static XXH128_hash_t XXH3_len_4to8_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    seed ^= (uint64_t)bswap32((uint32_t)seed) << 32;
    uint32_t const input_lo = XXH_read32(input);
    uint32_t const input_hi = XXH_read32(input + len - 4);
    uint64_t const input_64 = input_lo + ((uint64_t)input_hi << 32);
    uint64_t const bitflip = (XXH_read64(secret + 16) ^ XXH_read64(secret + 24)) + seed;
    uint64_t const keyed = input_64 ^ bitflip;
    XXH128_hash_t m128 = XXH_mult64to128(keyed, PRIME64_1 + (len << 2));
    m128.high64 += (m128.low64 << 1);
    m128.low64 ^= (m128.high64 >> 3);
    m128.low64 = XXH3_rrmxmx(m128.low64, len);
    m128.high64 = XXH3_rrmxmx(m128.high64, len);
    return m128;
}

static XXH128_hash_t XXH3_len_9to16_128b(const uint8_t* input, size_t len, const uint8_t* secret, uint64_t seed) {
    uint64_t const bitflipl = (XXH_read64(secret + 32) ^ XXH_read64(secret + 40)) + seed;
    uint64_t const bitfliph = (XXH_read64(secret + 48) ^ XXH_read64(secret + 56)) - seed;
    uint64_t const input_lo = XXH_read64(input) ^ bitflipl;
    uint64_t const input_hi = XXH_read64(input + len - 8) ^ bitfliph;
    XXH128_hash_t m128 = XXH_mult64to128(input_lo, PRIME64_1);
    m128.low64 += (uint64_t)(len - 1) << 54;
    uint64_t input_hi_lo = input_hi + 0x9FB21C651E98DF25ULL;
    m128.high64 += input_hi_lo + XXH_mult64to128(input_hi, PRIME32_2).high64;
    m128.low64 ^= bswap64(m128.high64);
    XXH128_hash_t h128 = XXH_mult64to128(m128.low64, PRIME64_2);
    h128.high64 += m128.high64 * PRIME64_2;
    h128.low64 = XXH3_avalanche(h128.low64);
    h128.high64 = XXH3_avalanche(h128.high64);
    return h128;
}

static XXH128_hash_t XXH3_len_17to128_128b(const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize, uint64_t seed) {
    XXH128_hash_t acc;
    acc.low64 = len * PRIME64_1;
    acc.high64 = 0;
    if (len > 32) {
        if (len > 64) {
            if (len > 96) {
                XXH128_hash_t mix = XXH3_mix16B_128b(input + 48, secret + 96, seed);
                acc.low64 += mix.low64;
                acc.high64 += mix.high64;
                mix = XXH3_mix16B_128b(input + len - 64, secret + 112, seed);
                acc.low64 += mix.low64;
                acc.high64 += mix.high64;
            }
            XXH128_hash_t mix = XXH3_mix16B_128b(input + 32, secret + 64, seed);
            acc.low64 += mix.low64;
            acc.high64 += mix.high64;
            mix = XXH3_mix16B_128b(input + len - 48, secret + 80, seed);
            acc.low64 += mix.low64;
            acc.high64 += mix.high64;
        }
        XXH128_hash_t mix = XXH3_mix16B_128b(input + 16, secret + 32, seed);
        acc.low64 += mix.low64;
        acc.high64 += mix.high64;
        mix = XXH3_mix16B_128b(input + len - 32, secret + 48, seed);
        acc.low64 += mix.low64;
        acc.high64 += mix.high64;
    }
    XXH128_hash_t mix = XXH3_mix16B_128b(input + 0, secret + 0, seed);
    acc.low64 += mix.low64;
    acc.high64 += mix.high64;
    mix = XXH3_mix16B_128b(input + len - 16, secret + 16, seed);
    acc.low64 += mix.low64;
    acc.high64 += mix.high64;
    
    return XXH3_avalanche128(acc);
}

static XXH128_hash_t XXH3_mix16B_128b(const uint8_t* input, const uint8_t* secret, uint64_t seed) {
    uint64_t const input_lo = XXH_read64(input);
    uint64_t const input_hi = XXH_read64(input + 8);
    return XXH_mult64to128(
        input_lo ^ (XXH_read64(secret) + seed),
        input_hi ^ (XXH_read64(secret + 8) - seed)
    );
}

static XXH128_hash_t XXH3_len_129to240_128b(const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize, uint64_t seed) {
    XXH128_hash_t acc;
    acc.low64 = len * PRIME64_1;
    acc.high64 = 0;
    size_t const nbRounds = len / 16;
    
    for (size_t i = 0; i < 8; i++) {
        XXH128_hash_t mix = XXH3_mix16B_128b(input + (16 * i), secret + (16 * i), seed);
        acc.low64 += mix.low64;
        acc.high64 += mix.high64;
    }
    acc = XXH3_avalanche128(acc);
    
    for (size_t i = 8; i < nbRounds; i++) {
        XXH128_hash_t mix = XXH3_mix16B_128b(input + (16 * i), secret + (16 * (i - 8)) + XXH3_MIDSIZE_STARTOFFSET, seed);
        acc.low64 += mix.low64;
        acc.high64 += mix.high64;
    }
    
    /* last bytes */
    XXH128_hash_t mix = XXH3_mix16B_128b(input + len - 16, secret + XXH3_SECRET_SIZE_MIN - XXH3_MIDSIZE_LASTOFFSET, seed);
    acc.low64 += mix.low64;
    acc.high64 += mix.high64;
    
    return XXH3_avalanche128(acc);
}

static XXH128_hash_t XXH3_hashLong_128b(const uint8_t* input, size_t len, const uint8_t* secret, size_t secretSize) {
    uint64_t acc[XXH3_ACC_NB] = {PRIME32_3, PRIME64_1, PRIME64_2, PRIME64_3, PRIME64_4, PRIME32_2, PRIME64_5, PRIME32_1};
    
    XXH3_hashLong_internal_loop(acc, input, len, secret, secretSize);
    
    /* converge into final hash */
    XXH128_hash_t h128;
    h128.low64 = XXH3_mergeAccs(acc, secret + XXH3_SECRET_MERGEACCS_START, (uint64_t)len * PRIME64_1);
    h128.high64 = XXH3_mergeAccs(acc, secret + secretSize - sizeof(acc) - XXH3_SECRET_MERGEACCS_START, ~((uint64_t)len * PRIME64_2));
    
    return h128;
}

/* 128-bit hash functions */
static XXH128_hash_t XXH3_128bits_internal(const void* input, size_t len, uint64_t seed, const uint8_t* secret, size_t secretLen) {
    if (len <= 16) return XXH3_len_0to16_128b((const uint8_t*)input, len, secret, seed);
    if (len <= 128) return XXH3_len_17to128_128b((const uint8_t*)input, len, secret, secretLen, seed);
    if (len <= XXH3_MIDSIZE_MAX) return XXH3_len_129to240_128b((const uint8_t*)input, len, secret, secretLen, seed);
    return XXH3_hashLong_128b((const uint8_t*)input, len, secret, secretLen);
}

static XXH128_hash_t XXH3_128bits(const void* input, size_t len) {
    return XXH3_128bits_internal(input, len, 0, kSecret, sizeof(kSecret));
}

static XXH128_hash_t XXH3_128bits_withSeed(const void* input, size_t len, uint64_t seed) {
    if (seed == 0) return XXH3_128bits(input, len);
    if (len <= XXH3_MIDSIZE_MAX) return XXH3_128bits_internal(input, len, seed, kSecret, sizeof(kSecret));
    return XXH3_128bits_withSecret(input, len, kSecret, sizeof(kSecret));
}

static XXH128_hash_t XXH3_128bits_withSecret(const void* input, size_t len, const void* secret, size_t secretSize) {
    return XXH3_128bits_internal(input, len, 0, (const uint8_t*)secret, secretSize);
}

/* PostgreSQL function wrappers for XXH3_64bits */

/* XXH3_64bits for text input with default seed */
PG_FUNCTION_INFO_V1(xxhash3_64_text);

Datum
xxhash3_64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = XXH3_64bits(data, len);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH3_64bits for text input with custom seed */
PG_FUNCTION_INFO_V1(xxhash3_64_text_seed);

Datum
xxhash3_64_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = XXH3_64bits_withSeed(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH3_64bits for bytea input with default seed */
PG_FUNCTION_INFO_V1(xxhash3_64_bytea);

Datum
xxhash3_64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = XXH3_64bits(data, len);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH3_64bits for bytea input with custom seed */
PG_FUNCTION_INFO_V1(xxhash3_64_bytea_seed);

Datum
xxhash3_64_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = XXH3_64bits_withSeed(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH3_64bits for integer input with default seed */
PG_FUNCTION_INFO_V1(xxhash3_64_int);

Datum
xxhash3_64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = XXH3_64bits(&input, sizeof(int32_t));
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH3_64bits for integer input with custom seed */
PG_FUNCTION_INFO_V1(xxhash3_64_int_seed);

Datum
xxhash3_64_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = XXH3_64bits_withSeed(&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* PostgreSQL function wrappers for XXH3_128bits */
/* Note: PostgreSQL doesn't have native 128-bit integer type, so we return as text */

/* XXH3_128bits for text input with default seed */
PG_FUNCTION_INFO_V1(xxhash3_128_text);

Datum
xxhash3_128_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    XXH128_hash_t hash = XXH3_128bits(data, len);
    
    /* Format as hex string: high64:low64 */
    char result[33]; /* 32 chars + null terminator */
    snprintf(result, sizeof(result), "%016llx%016llx", 
             (unsigned long long)hash.high64, 
             (unsigned long long)hash.low64);
    
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

/* XXH3_128bits for text input with custom seed */
PG_FUNCTION_INFO_V1(xxhash3_128_text_seed);

Datum
xxhash3_128_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    XXH128_hash_t hash = XXH3_128bits_withSeed(data, len, (uint64_t)seed);
    
    /* Format as hex string: high64:low64 */
    char result[33]; /* 32 chars + null terminator */
    snprintf(result, sizeof(result), "%016llx%016llx", 
             (unsigned long long)hash.high64, 
             (unsigned long long)hash.low64);
    
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

/* XXH3_128bits for bytea input with default seed */
PG_FUNCTION_INFO_V1(xxhash3_128_bytea);

Datum
xxhash3_128_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    XXH128_hash_t hash = XXH3_128bits(data, len);
    
    /* Format as hex string: high64:low64 */
    char result[33]; /* 32 chars + null terminator */
    snprintf(result, sizeof(result), "%016llx%016llx", 
             (unsigned long long)hash.high64, 
             (unsigned long long)hash.low64);
    
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

/* XXH3_128bits for bytea input with custom seed */
PG_FUNCTION_INFO_V1(xxhash3_128_bytea_seed);

Datum
xxhash3_128_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    XXH128_hash_t hash = XXH3_128bits_withSeed(data, len, (uint64_t)seed);
    
    /* Format as hex string: high64:low64 */
    char result[33]; /* 32 chars + null terminator */
    snprintf(result, sizeof(result), "%016llx%016llx", 
             (unsigned long long)hash.high64, 
             (unsigned long long)hash.low64);
    
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

/* XXH3_128bits for integer input with default seed */
PG_FUNCTION_INFO_V1(xxhash3_128_int);

Datum
xxhash3_128_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    XXH128_hash_t hash = XXH3_128bits(&input, sizeof(int32_t));
    
    /* Format as hex string: high64:low64 */
    char result[33]; /* 32 chars + null terminator */
    snprintf(result, sizeof(result), "%016llx%016llx", 
             (unsigned long long)hash.high64, 
             (unsigned long long)hash.low64);
    
    PG_RETURN_TEXT_P(cstring_to_text(result));
}

/* XXH3_128bits for integer input with custom seed */
PG_FUNCTION_INFO_V1(xxhash3_128_int_seed);

Datum
xxhash3_128_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    XXH128_hash_t hash = XXH3_128bits_withSeed(&input, sizeof(int32_t), (uint64_t)seed);
    
    /* Format as hex string: high64:low64 */
    char result[33]; /* 32 chars + null terminator */
    snprintf(result, sizeof(result), "%016llx%016llx", 
             (unsigned long long)hash.high64, 
             (unsigned long long)hash.low64);
    
    PG_RETURN_TEXT_P(cstring_to_text(result));
}