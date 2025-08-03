#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* xxHash constants */
#define XXH32_PRIME_1   0x9E3779B1U
#define XXH32_PRIME_2   0x85EBCA77U  
#define XXH32_PRIME_3   0xC2B2AE3DU
#define XXH32_PRIME_4   0x27D4EB2FU
#define XXH32_PRIME_5   0x165667B1U

#define XXH64_PRIME_1   0x9E3779B185EBCA87ULL
#define XXH64_PRIME_2   0xC2B2AE3D27D4EB4FULL
#define XXH64_PRIME_3   0x165667B19E3779F9ULL
#define XXH64_PRIME_4   0x85EBCA77C2B2AE63ULL
#define XXH64_PRIME_5   0x27D4EB2F165667C5ULL

/* Utility functions */
static uint32_t XXH_read32(const void* memPtr)
{
    uint32_t val;
    memcpy(&val, memPtr, sizeof(val));
    return val;
}

static uint64_t XXH_read64(const void* memPtr)
{
    uint64_t val;
    memcpy(&val, memPtr, sizeof(val));
    return val;
}

static uint32_t XXH32_rotl(uint32_t x, int r)
{
    return (x << r) | (x >> (32 - r));
}

static uint64_t XXH64_rotl(uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}

/* XXH32 Implementation */
static uint32_t XXH32_round(uint32_t acc, uint32_t input)
{
    acc += input * XXH32_PRIME_2;
    acc = XXH32_rotl(acc, 13);
    acc *= XXH32_PRIME_1;
    return acc;
}

static uint32_t XXH32_avalanche(uint32_t hash)
{
    hash ^= hash >> 15;
    hash *= XXH32_PRIME_2;
    hash ^= hash >> 13;
    hash *= XXH32_PRIME_3;
    hash ^= hash >> 16;
    return hash;
}

static uint32_t
xxhash32(const void* input, size_t len, uint32_t seed)
{
    const uint8_t* p = (const uint8_t*)input;
    const uint8_t* bEnd = p + len;
    uint32_t h32;

    if (len >= 16) {
        const uint8_t* const limit = bEnd - 16;
        uint32_t v1 = seed + XXH32_PRIME_1 + XXH32_PRIME_2;
        uint32_t v2 = seed + XXH32_PRIME_2;
        uint32_t v3 = seed + 0;
        uint32_t v4 = seed - XXH32_PRIME_1;

        do {
            v1 = XXH32_round(v1, XXH_read32(p)); p += 4;
            v2 = XXH32_round(v2, XXH_read32(p)); p += 4;
            v3 = XXH32_round(v3, XXH_read32(p)); p += 4;
            v4 = XXH32_round(v4, XXH_read32(p)); p += 4;
        } while (p <= limit);

        h32 = XXH32_rotl(v1, 1) + XXH32_rotl(v2, 7) + 
              XXH32_rotl(v3, 12) + XXH32_rotl(v4, 18);
    } else {
        h32 = seed + XXH32_PRIME_5;
    }

    h32 += (uint32_t)len;

    while (p + 4 <= bEnd) {
        h32 += XXH_read32(p) * XXH32_PRIME_3;
        h32 = XXH32_rotl(h32, 17) * XXH32_PRIME_4;
        p += 4;
    }

    while (p < bEnd) {
        h32 += (*p++) * XXH32_PRIME_5;
        h32 = XXH32_rotl(h32, 11) * XXH32_PRIME_1;
    }

    return XXH32_avalanche(h32);
}

/* XXH64 Implementation */
static uint64_t XXH64_round(uint64_t acc, uint64_t input)
{
    acc += input * XXH64_PRIME_2;
    acc = XXH64_rotl(acc, 31);
    acc *= XXH64_PRIME_1;
    return acc;
}

static uint64_t XXH64_mergeRound(uint64_t acc, uint64_t val)
{
    val = XXH64_round(0, val);
    acc ^= val;
    acc = acc * XXH64_PRIME_1 + XXH64_PRIME_4;
    return acc;
}

static uint64_t XXH64_avalanche(uint64_t hash)
{
    hash ^= hash >> 33;
    hash *= XXH64_PRIME_2;
    hash ^= hash >> 29;
    hash *= XXH64_PRIME_3;
    hash ^= hash >> 32;
    return hash;
}

static uint64_t
xxhash64(const void* input, size_t len, uint64_t seed)
{
    const uint8_t* p = (const uint8_t*)input;
    const uint8_t* bEnd = p + len;
    uint64_t h64;

    if (len >= 32) {
        const uint8_t* const limit = bEnd - 32;
        uint64_t v1 = seed + XXH64_PRIME_1 + XXH64_PRIME_2;
        uint64_t v2 = seed + XXH64_PRIME_2;
        uint64_t v3 = seed + 0;
        uint64_t v4 = seed - XXH64_PRIME_1;

        do {
            v1 = XXH64_round(v1, XXH_read64(p)); p += 8;
            v2 = XXH64_round(v2, XXH_read64(p)); p += 8;
            v3 = XXH64_round(v3, XXH_read64(p)); p += 8;
            v4 = XXH64_round(v4, XXH_read64(p)); p += 8;
        } while (p <= limit);

        h64 = XXH64_rotl(v1, 1) + XXH64_rotl(v2, 7) + 
              XXH64_rotl(v3, 12) + XXH64_rotl(v4, 18);
        h64 = XXH64_mergeRound(h64, v1);
        h64 = XXH64_mergeRound(h64, v2);
        h64 = XXH64_mergeRound(h64, v3);
        h64 = XXH64_mergeRound(h64, v4);
    } else {
        h64 = seed + XXH64_PRIME_5;
    }

    h64 += (uint64_t)len;

    while (p + 8 <= bEnd) {
        uint64_t k1 = XXH64_round(0, XXH_read64(p));
        h64 ^= k1;
        h64 = XXH64_rotl(h64, 27) * XXH64_PRIME_1 + XXH64_PRIME_4;
        p += 8;
    }

    if (p + 4 <= bEnd) {
        h64 ^= (uint64_t)(XXH_read32(p)) * XXH64_PRIME_1;
        h64 = XXH64_rotl(h64, 23) * XXH64_PRIME_2 + XXH64_PRIME_3;
        p += 4;
    }

    while (p < bEnd) {
        h64 ^= (*p++) * XXH64_PRIME_5;
        h64 = XXH64_rotl(h64, 11) * XXH64_PRIME_1;
    }

    return XXH64_avalanche(h64);
}

/* PostgreSQL function wrappers for XXH32 */

/* XXH32 for text input with default seed */
PG_FUNCTION_INFO_V1(xxhash32_text);

Datum
xxhash32_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = xxhash32(data, len, 0);
    PG_RETURN_INT32((int32_t)hash);
}

/* XXH32 for text input with custom seed */
PG_FUNCTION_INFO_V1(xxhash32_text_seed);

Datum
xxhash32_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int32_t seed = PG_GETARG_INT32(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = xxhash32(data, len, (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* XXH32 for bytea input with default seed */
PG_FUNCTION_INFO_V1(xxhash32_bytea);

Datum
xxhash32_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = xxhash32(data, len, 0);
    PG_RETURN_INT32((int32_t)hash);
}

/* XXH32 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(xxhash32_bytea_seed);

Datum
xxhash32_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int32_t seed = PG_GETARG_INT32(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = xxhash32(data, len, (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* XXH32 for integer input with default seed */
PG_FUNCTION_INFO_V1(xxhash32_int);

Datum
xxhash32_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint32_t hash = xxhash32(&input, sizeof(int32_t), 0);
    PG_RETURN_INT32((int32_t)hash);
}

/* XXH32 for integer input with custom seed */
PG_FUNCTION_INFO_V1(xxhash32_int_seed);

Datum
xxhash32_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int32_t seed = PG_GETARG_INT32(1);
    uint32_t hash = xxhash32(&input, sizeof(int32_t), (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* PostgreSQL function wrappers for XXH64 */

/* XXH64 for text input with default seed */
PG_FUNCTION_INFO_V1(xxhash64_text);

Datum
xxhash64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = xxhash64(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH64 for text input with custom seed */
PG_FUNCTION_INFO_V1(xxhash64_text_seed);

Datum
xxhash64_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = xxhash64(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH64 for bytea input with default seed */
PG_FUNCTION_INFO_V1(xxhash64_bytea);

Datum
xxhash64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = xxhash64(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH64 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(xxhash64_bytea_seed);

Datum
xxhash64_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = xxhash64(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH64 for integer input with default seed */
PG_FUNCTION_INFO_V1(xxhash64_int);

Datum
xxhash64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = xxhash64(&input, sizeof(int32_t), 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* XXH64 for integer input with custom seed */
PG_FUNCTION_INFO_V1(xxhash64_int_seed);

Datum
xxhash64_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = xxhash64(&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}