#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* FarmHash constants */
#define FARMHASH_K0 0xc3a5c85c97cb3127ULL
#define FARMHASH_K1 0xb492b66fbe98f273ULL
#define FARMHASH_K2 0x9ae16a3b2f90404fULL

#define FARMHASH_C1 0xcc9e2d51U
#define FARMHASH_C2 0x1b873593U

/* Utility functions */
static uint32_t farmhash_rotl32(uint32_t x, int r)
{
    return (x << r) | (x >> (32 - r));
}

static uint64_t farmhash_rotl64(uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}

static uint32_t farmhash_fetch32(const char *p)
{
    uint32_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint64_t farmhash_fetch64(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint32_t farmhash_fmix32(uint32_t h)
{
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}


static uint64_t farmhash_shift_mix(uint64_t val)
{
    return val ^ (val >> 47);
}

static uint64_t farmhash_hash_len_16(uint64_t u, uint64_t v, uint64_t mul)
{
    uint64_t a, b;
    a = (u ^ v) * mul;
    a ^= (a >> 47);
    b = (v ^ a) * mul;
    b ^= (b >> 47);
    b *= mul;
    return b;
}

/* FarmHash32 implementation */
static uint32_t
farmhash32_mur(uint32_t a, uint32_t h)
{
    a *= FARMHASH_C1;
    a = farmhash_rotl32(a, 17);
    a *= FARMHASH_C2;
    h ^= a;
    h = farmhash_rotl32(h, 19);
    return h * 5 + 0xe6546b64;
}

static uint32_t
farmhash32_len_13_to_24(const char *s, size_t len)
{
    uint32_t a = farmhash_fetch32(s - 4 + (len >> 1));
    uint32_t b = farmhash_fetch32(s + 4);
    uint32_t c = farmhash_fetch32(s + len - 8);
    uint32_t d = farmhash_fetch32(s + (len >> 1));
    uint32_t e = farmhash_fetch32(s);
    uint32_t f = farmhash_fetch32(s + len - 4);
    uint32_t h = (uint32_t)len;

    return farmhash_fmix32(farmhash32_mur(f, farmhash32_mur(e, farmhash32_mur(d, farmhash32_mur(c, farmhash32_mur(b, farmhash32_mur(a, h)))))));
}

static uint32_t
farmhash32_len_0_to_4(const char *s, size_t len)
{
    uint32_t b = 0;
    uint32_t c = 9;
    for (size_t i = 0; i < len; i++) {
        int8_t v = s[i];
        b = b * FARMHASH_C1 + (uint32_t)v;
        c ^= b;
    }
    return farmhash_fmix32(farmhash32_mur(b, farmhash32_mur((uint32_t)len, c)));
}

static uint32_t
farmhash32_len_5_to_12(const char *s, size_t len)
{
    uint32_t a = (uint32_t)len;
    uint32_t b = (uint32_t)len * 5;
    uint32_t c = 9;
    uint32_t d = b;

    a += farmhash_fetch32(s);
    b += farmhash_fetch32(s + len - 4);
    c += farmhash_fetch32(s + ((len >> 1) & 4));
    return farmhash_fmix32(farmhash32_mur(c, farmhash32_mur(b, farmhash32_mur(a, d))));
}

static uint32_t
farmhash32_impl(const char *s, size_t len)
{
    if (len <= 4) {
        return farmhash32_len_0_to_4(s, len);
    } else if (len <= 12) {
        return farmhash32_len_5_to_12(s, len);
    } else if (len <= 24) {
        return farmhash32_len_13_to_24(s, len);
    }

    /* Hash longer strings */
    {
        uint32_t h, g, f, a0, a1, a2, a3, a4;
        size_t iters;
        
        h = (uint32_t)len;
        g = FARMHASH_C1 * (uint32_t)len;
        f = g;
        a0 = farmhash_rotl32(farmhash_fetch32(s + len - 4) * FARMHASH_C1, 17) * FARMHASH_C2;
        a1 = farmhash_rotl32(farmhash_fetch32(s + len - 8) * FARMHASH_C1, 17) * FARMHASH_C2;
        a2 = farmhash_rotl32(farmhash_fetch32(s + len - 16) * FARMHASH_C1, 17) * FARMHASH_C2;
        a3 = farmhash_rotl32(farmhash_fetch32(s + len - 12) * FARMHASH_C1, 17) * FARMHASH_C2;
        a4 = farmhash_rotl32(farmhash_fetch32(s + len - 20) * FARMHASH_C1, 17) * FARMHASH_C2;
    
    h ^= a0;
    h = farmhash_rotl32(h, 19);
    h = h * 5 + 0xe6546b64;
    h ^= a2;
    h = farmhash_rotl32(h, 19);
    h = h * 5 + 0xe6546b64;
    g ^= a1;
    g = farmhash_rotl32(g, 19);
    g = g * 5 + 0xe6546b64;
    g ^= a3;
    g = farmhash_rotl32(g, 19);
    g = g * 5 + 0xe6546b64;
        f += a4;
        f = farmhash_rotl32(f, 19) + 113;
        
        iters = (len - 1) / 20;
        do {
            uint32_t a, b, c, d, e;
            a = farmhash_fetch32(s);
            b = farmhash_fetch32(s + 4);
            c = farmhash_fetch32(s + 8);
            d = farmhash_fetch32(s + 12);
            e = farmhash_fetch32(s + 16);
            h += a;
            g += b;
            f += c;
            h = farmhash32_mur(d, h) + e;
            g = farmhash32_mur(c, g) + a;
            f = farmhash32_mur(b + e * FARMHASH_C1, f) + d;
            f += g;
            g += f;
            s += 20;
        } while (--iters != 0);
        
        g = farmhash_rotl32(g, 11) * FARMHASH_C1;
        g = farmhash_rotl32(g, 17) * FARMHASH_C1;
        f = farmhash_rotl32(f, 11) * FARMHASH_C1;
        f = farmhash_rotl32(f, 17) * FARMHASH_C1;
        h = farmhash_rotl32(h + g, 19);
        h = h * 5 + 0xe6546b64;
        h = farmhash_rotl32(h, 17) * FARMHASH_C1;
        h = farmhash_rotl32(h + f, 19);
        h = h * 5 + 0xe6546b64;
        h = farmhash_rotl32(h, 17) * FARMHASH_C1;
        return h;
    }
}

static uint32_t
farmhash32_with_seed(const char *s, size_t len, uint32_t seed)
{
    if (len <= 24) {
        return len <= 12 ? 
            (len <= 4 ? farmhash32_len_0_to_4(s, len) : farmhash32_len_5_to_12(s, len)) :
            farmhash32_len_13_to_24(s, len);
    }
    return farmhash32_mur(farmhash32_impl(s, len), seed);
}

/* FarmHash64 implementation */

static uint64_t
farmhash64_len_0_to_16(const char *s, size_t len)
{
    if (len >= 8) {
        uint64_t mul = FARMHASH_K2 + len * 2;
        uint64_t a = farmhash_fetch64(s) + FARMHASH_K2;
        uint64_t b = farmhash_fetch64(s + len - 8);
        uint64_t c = farmhash_rotl64(b, 37) * mul + a;
        uint64_t d = (farmhash_rotl64(a, 25) + b) * mul;
        return farmhash_hash_len_16(c, d, mul);
    }
    if (len >= 4) {
        uint64_t mul = FARMHASH_K2 + len * 2;
        uint64_t a = farmhash_fetch32(s);
        return farmhash_hash_len_16(len + (a << 3), farmhash_fetch32(s + len - 4), mul);
    }
    if (len > 0) {
        uint8_t a = s[0];
        uint8_t b = s[len >> 1];
        uint8_t c = s[len - 1];
        uint32_t y = (uint32_t)a + ((uint32_t)b << 8);
        uint32_t z = (uint32_t)len + ((uint32_t)c << 2);
        return farmhash_shift_mix(y * FARMHASH_K2 ^ z * FARMHASH_K0) * FARMHASH_K2;
    }
    return FARMHASH_K2;
}

static uint64_t
farmhash64_len_17_to_32(const char *s, size_t len)
{
    uint64_t mul = FARMHASH_K2 + len * 2;
    uint64_t a = farmhash_fetch64(s) * FARMHASH_K1;
    uint64_t b = farmhash_fetch64(s + 8);
    uint64_t c = farmhash_fetch64(s + len - 8) * mul;
    uint64_t d = farmhash_fetch64(s + len - 16) * FARMHASH_K2;
    return farmhash_hash_len_16(farmhash_rotl64(a + b, 43) + farmhash_rotl64(c, 30) + d,
                               a + farmhash_rotl64(b + FARMHASH_K2, 18) + c, mul);
}

static uint64_t
farmhash64_impl(const char *s, size_t len)
{
    if (len <= 16) {
        return farmhash64_len_0_to_16(s, len);
    }
    if (len <= 32) {
        return farmhash64_len_17_to_32(s, len);
    }
    if (len <= 64) {
        uint64_t mul, a, b, c, d, y, z, e, f, g, h;
        mul = FARMHASH_K2 + len * 2;
        a = farmhash_fetch64(s) * FARMHASH_K2;
        b = farmhash_fetch64(s + 8);
        c = farmhash_fetch64(s + len - 8) * mul;
        d = farmhash_fetch64(s + len - 16) * FARMHASH_K2;
        y = farmhash_rotl64(a + b, 43) + farmhash_rotl64(c, 30) + d;
        z = farmhash_hash_len_16(y, a + farmhash_rotl64(b + FARMHASH_K2, 18) + c, mul);
        e = farmhash_fetch64(s + 16) * mul;
        f = farmhash_fetch64(s + 24);
        g = (y + farmhash_fetch64(s + len - 32)) * mul;
        h = (z + farmhash_fetch64(s + len - 24)) * mul;
        return farmhash_hash_len_16(farmhash_rotl64(e + f, 43) + farmhash_rotl64(g, 30) + h,
                                   e + farmhash_rotl64(f + a, 18) + g, mul);
    }
    
    /* For longer strings, use a simplified hash */
    {
        uint64_t x, y, z;
        x = farmhash_fetch64(s + len - 40);
        y = farmhash_fetch64(s + len - 16) + farmhash_fetch64(s + len - 56);
        z = farmhash_hash_len_16(farmhash_fetch64(s + len - 48) + len, 
                                         farmhash_fetch64(s + len - 24), FARMHASH_K2);
        return farmhash_hash_len_16(farmhash_rotl64(y, 37) * FARMHASH_K1 + x,
                                   farmhash_rotl64(z, 33) * FARMHASH_K1, FARMHASH_K1);
    }
}

static uint64_t
farmhash64_with_seed(const char *s, size_t len, uint64_t seed)
{
    return farmhash_hash_len_16(farmhash64_impl(s, len) - seed, seed, FARMHASH_K1);
}

static uint64_t
farmhash64_with_seeds(const char *s, size_t len, uint64_t seed0, uint64_t seed1)
{
    return farmhash_hash_len_16(farmhash64_impl(s, len) - seed0, seed1, FARMHASH_K1);
}

/* PostgreSQL function wrappers for FarmHash32 */

/* FarmHash32 for text input with default seed */
PG_FUNCTION_INFO_V1(farmhash32_text);

Datum
farmhash32_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = farmhash32_impl(data, len);
    PG_RETURN_INT32((int32_t)hash);
}

/* FarmHash32 for text input with custom seed */
PG_FUNCTION_INFO_V1(farmhash32_text_seed);

Datum
farmhash32_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int32_t seed = PG_GETARG_INT32(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = farmhash32_with_seed(data, len, (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* FarmHash32 for bytea input with default seed */
PG_FUNCTION_INFO_V1(farmhash32_bytea);

Datum
farmhash32_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = farmhash32_impl(data, len);
    PG_RETURN_INT32((int32_t)hash);
}

/* FarmHash32 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(farmhash32_bytea_seed);

Datum
farmhash32_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int32_t seed = PG_GETARG_INT32(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = farmhash32_with_seed(data, len, (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* FarmHash32 for integer input with default seed */
PG_FUNCTION_INFO_V1(farmhash32_int);

Datum
farmhash32_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint32_t hash = farmhash32_impl((char*)&input, sizeof(int32_t));
    PG_RETURN_INT32((int32_t)hash);
}

/* FarmHash32 for integer input with custom seed */
PG_FUNCTION_INFO_V1(farmhash32_int_seed);

Datum
farmhash32_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int32_t seed = PG_GETARG_INT32(1);
    uint32_t hash = farmhash32_with_seed((char*)&input, sizeof(int32_t), (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* PostgreSQL function wrappers for FarmHash64 */

/* FarmHash64 for text input with default seed */
PG_FUNCTION_INFO_V1(farmhash64_text);

Datum
farmhash64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = farmhash64_impl(data, len);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for text input with custom seed */
PG_FUNCTION_INFO_V1(farmhash64_text_seed);

Datum
farmhash64_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = farmhash64_with_seed(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for text input with two seeds */
PG_FUNCTION_INFO_V1(farmhash64_text_seeds);

Datum
farmhash64_text_seeds(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed0 = PG_GETARG_INT64(1);
    int64_t seed1 = PG_GETARG_INT64(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = farmhash64_with_seeds(data, len, (uint64_t)seed0, (uint64_t)seed1);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for bytea input with default seed */
PG_FUNCTION_INFO_V1(farmhash64_bytea);

Datum
farmhash64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = farmhash64_impl(data, len);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(farmhash64_bytea_seed);

Datum
farmhash64_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = farmhash64_with_seed(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for bytea input with two seeds */
PG_FUNCTION_INFO_V1(farmhash64_bytea_seeds);

Datum
farmhash64_bytea_seeds(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed0 = PG_GETARG_INT64(1);
    int64_t seed1 = PG_GETARG_INT64(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = farmhash64_with_seeds(data, len, (uint64_t)seed0, (uint64_t)seed1);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for integer input with default seed */
PG_FUNCTION_INFO_V1(farmhash64_int);

Datum
farmhash64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = farmhash64_impl((char*)&input, sizeof(int32_t));
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for integer input with custom seed */
PG_FUNCTION_INFO_V1(farmhash64_int_seed);

Datum
farmhash64_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = farmhash64_with_seed((char*)&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* FarmHash64 for integer input with two seeds */
PG_FUNCTION_INFO_V1(farmhash64_int_seeds);

Datum
farmhash64_int_seeds(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed0 = PG_GETARG_INT64(1);
    int64_t seed1 = PG_GETARG_INT64(2);
    uint64_t hash = farmhash64_with_seeds((char*)&input, sizeof(int32_t), (uint64_t)seed0, (uint64_t)seed1);
    PG_RETURN_INT64((int64_t)hash);
}