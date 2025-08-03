#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* t1ha constants */
static const uint64_t t1ha_prime_0 = 0x9E3779B185EBCA87ULL;
static const uint64_t t1ha_prime_1 = 0xC2B2AE3D27D4EB4FULL;
static const uint64_t t1ha_prime_2 = 0x165667B19E3779F9ULL;
static const uint64_t t1ha_prime_3 = 0x85EBCA77C2B2AE63ULL;
static const uint64_t t1ha_prime_4 = 0x27D4EB2F165667C5ULL;
static const uint64_t t1ha_prime_5 = 0x9E3779B185EBCA87ULL;
static const uint64_t t1ha_prime_6 = 0xC2B2AE3D27D4EB4FULL;

/* 128-bit hash result structure */
typedef struct {
    uint64_t low;
    uint64_t high;
} uint128_t;

/* Utility functions */
static uint64_t
t1ha_fetch64(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint32_t
t1ha_fetch32(const char *p)
{
    uint32_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint16_t
t1ha_fetch16(const char *p)
{
    uint16_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint64_t
t1ha_rot64(uint64_t v, int s)
{
    return (v >> s) | (v << (64 - s));
}

static uint64_t
t1ha_mix64(uint64_t v, uint64_t p)
{
    v *= p;
    return v ^ t1ha_rot64(v, 41);
}

/* t1ha1 implementation */
static uint64_t
t1ha1_le(const char *data, size_t len, uint64_t seed)
{
    const char *ptr = data;
    const char *end = data + len;
    uint64_t a = seed;
    uint64_t b = len;
    
    if (len >= 32)
    {
        uint64_t c = t1ha_rot64(len, 17) + seed;
        uint64_t d = len ^ t1ha_rot64(seed, 17);
        
        do
        {
            uint64_t w0, w1, w2, w3;
            w0 = t1ha_fetch64(ptr); ptr += 8;
            w1 = t1ha_fetch64(ptr); ptr += 8;
            w2 = t1ha_fetch64(ptr); ptr += 8;
            w3 = t1ha_fetch64(ptr); ptr += 8;
            
            a ^= t1ha_mix64(w0, t1ha_prime_0);
            b ^= t1ha_mix64(w1, t1ha_prime_1);
            c ^= t1ha_mix64(w2, t1ha_prime_2);
            d ^= t1ha_mix64(w3, t1ha_prime_3);
        } while (ptr <= end - 32);
        
        a ^= t1ha_rot64(c, 17);
        b ^= t1ha_rot64(d, 17);
    }
    
    switch ((end - ptr) >> 3)
    {
        default:
            break;
        case 3:
            {
                uint64_t c = t1ha_fetch64(ptr); ptr += 8;
                c *= t1ha_prime_2;
                a ^= t1ha_rot64(c, 41);
            }
            /* fall through */
        case 2:
            b ^= t1ha_mix64(t1ha_fetch64(ptr), t1ha_prime_1);
            ptr += 8;
            /* fall through */
        case 1:
            a ^= t1ha_mix64(t1ha_fetch64(ptr), t1ha_prime_0);
            ptr += 8;
            /* fall through */
        case 0:
            switch (end - ptr)
            {
                case 7:
                    b ^= (uint64_t)ptr[6] << 48;
                    /* fall through */
                case 6:
                    b ^= (uint64_t)ptr[5] << 40;
                    /* fall through */
                case 5:
                    b ^= (uint64_t)ptr[4] << 32;
                    /* fall through */
                case 4:
                    b ^= t1ha_fetch32(ptr);
                    break;
                case 3:
                    a ^= (uint64_t)ptr[2] << 16;
                    /* fall through */
                case 2:
                    a ^= t1ha_fetch16(ptr);
                    break;
                case 1:
                    a ^= ptr[0];
                    break;
            }
    }
    
    /* Final mixing */
    a *= t1ha_prime_4;
    b *= t1ha_prime_5;
    return t1ha_mix64(a ^ b, t1ha_prime_6);
}

/* t1ha2 implementation */
static uint64_t
t1ha2_atonce(const char *data, size_t len, uint64_t seed)
{
    const char *ptr = data;
    const char *end = data + len;
    uint64_t a, b, c, d;
    
    a = seed;
    b = len;
    c = t1ha_rot64(len, 17) + seed;
    d = len ^ t1ha_rot64(seed, 17);
    
    if (len >= 32)
    {
        do
        {
            uint64_t w0, w1, w2, w3;
            w0 = t1ha_fetch64(ptr); ptr += 8;
            w1 = t1ha_fetch64(ptr); ptr += 8;
            w2 = t1ha_fetch64(ptr); ptr += 8;
            w3 = t1ha_fetch64(ptr); ptr += 8;
            
            a += w0 * t1ha_prime_0;
            a = t1ha_rot64(a, 32);
            b += w1 * t1ha_prime_1;
            b = t1ha_rot64(b, 32);
            c += w2 * t1ha_prime_2;
            c = t1ha_rot64(c, 32);
            d += w3 * t1ha_prime_3;
            d = t1ha_rot64(d, 32);
        } while (ptr <= end - 32);
    }
    
    /* Handle remaining bytes */
    if (ptr < end)
    {
        switch ((end - ptr) >> 3)
        {
            default:
                break;
            case 3:
                c += t1ha_fetch64(ptr) * t1ha_prime_2;
                ptr += 8;
                /* fall through */
            case 2:
                b += t1ha_fetch64(ptr) * t1ha_prime_1;
                ptr += 8;
                /* fall through */
            case 1:
                a += t1ha_fetch64(ptr) * t1ha_prime_0;
                ptr += 8;
                /* fall through */
            case 0:
                switch (end - ptr)
                {
                    case 7:
                        d += (uint64_t)ptr[6] << 48;
                        /* fall through */
                    case 6:
                        d += (uint64_t)ptr[5] << 40;
                        /* fall through */
                    case 5:
                        d += (uint64_t)ptr[4] << 32;
                        /* fall through */
                    case 4:
                        d += t1ha_fetch32(ptr);
                        break;
                    case 3:
                        c += (uint64_t)ptr[2] << 16;
                        /* fall through */
                    case 2:
                        c += t1ha_fetch16(ptr);
                        break;
                    case 1:
                        c += ptr[0];
                        break;
                }
        }
    }
    
    /* Final mixing */
    a ^= t1ha_rot64(c, 17);
    b ^= t1ha_rot64(d, 17);
    c ^= t1ha_rot64(a, 17);
    d ^= t1ha_rot64(b, 17);
    
    return t1ha_mix64(a + b, t1ha_prime_4) + t1ha_mix64(c + d, t1ha_prime_5);
}

/* t1ha2 128-bit implementation */
static uint128_t
t1ha2_atonce128(const char *data, size_t len, uint64_t seed)
{
    const char *ptr = data;
    const char *end = data + len;
    uint64_t a, b, c, d;
    uint128_t result;
    
    a = seed;
    b = len;
    c = t1ha_rot64(len, 17) + seed;
    d = len ^ t1ha_rot64(seed, 17);
    
    if (len >= 32)
    {
        do
        {
            uint64_t w0, w1, w2, w3;
            w0 = t1ha_fetch64(ptr); ptr += 8;
            w1 = t1ha_fetch64(ptr); ptr += 8;
            w2 = t1ha_fetch64(ptr); ptr += 8;
            w3 = t1ha_fetch64(ptr); ptr += 8;
            
            a += w0 * t1ha_prime_0;
            a = t1ha_rot64(a, 32);
            b += w1 * t1ha_prime_1;
            b = t1ha_rot64(b, 32);
            c += w2 * t1ha_prime_2;
            c = t1ha_rot64(c, 32);
            d += w3 * t1ha_prime_3;
            d = t1ha_rot64(d, 32);
        } while (ptr <= end - 32);
    }
    
    /* Handle remaining bytes - same as t1ha2_atonce */
    if (ptr < end)
    {
        switch ((end - ptr) >> 3)
        {
            default:
                break;
            case 3:
                c += t1ha_fetch64(ptr) * t1ha_prime_2;
                ptr += 8;
                /* fall through */
            case 2:
                b += t1ha_fetch64(ptr) * t1ha_prime_1;
                ptr += 8;
                /* fall through */
            case 1:
                a += t1ha_fetch64(ptr) * t1ha_prime_0;
                ptr += 8;
                /* fall through */
            case 0:
                switch (end - ptr)
                {
                    case 7:
                        d += (uint64_t)ptr[6] << 48;
                        /* fall through */
                    case 6:
                        d += (uint64_t)ptr[5] << 40;
                        /* fall through */
                    case 5:
                        d += (uint64_t)ptr[4] << 32;
                        /* fall through */
                    case 4:
                        d += t1ha_fetch32(ptr);
                        break;
                    case 3:
                        c += (uint64_t)ptr[2] << 16;
                        /* fall through */
                    case 2:
                        c += t1ha_fetch16(ptr);
                        break;
                    case 1:
                        c += ptr[0];
                        break;
                }
        }
    }
    
    /* Final mixing for 128-bit result */
    a ^= t1ha_rot64(c, 17);
    b ^= t1ha_rot64(d, 17);
    c ^= t1ha_rot64(a, 17);
    d ^= t1ha_rot64(b, 17);
    
    result.low = t1ha_mix64(a + b, t1ha_prime_4);
    result.high = t1ha_mix64(c + d, t1ha_prime_5);
    
    return result;
}

/* t1ha0 - dispatcher (use t1ha2 as fastest available) */
static uint64_t
t1ha0(const char *data, size_t len, uint64_t seed)
{
    return t1ha2_atonce(data, len, seed);
}

/* PostgreSQL function wrappers for t1ha0 */

/* t1ha0 for text input with default seed */
PG_FUNCTION_INFO_V1(t1ha0_text);

Datum
t1ha0_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha0(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha0 for text input with custom seed */
PG_FUNCTION_INFO_V1(t1ha0_text_seed);

Datum
t1ha0_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha0(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha0 for bytea input with default seed */
PG_FUNCTION_INFO_V1(t1ha0_bytea);

Datum
t1ha0_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha0(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha0 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(t1ha0_bytea_seed);

Datum
t1ha0_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha0(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha0 for integer input with default seed */
PG_FUNCTION_INFO_V1(t1ha0_int);

Datum
t1ha0_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = t1ha0((char*)&input, sizeof(int32_t), 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha0 for integer input with custom seed */
PG_FUNCTION_INFO_V1(t1ha0_int_seed);

Datum
t1ha0_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = t1ha0((char*)&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* PostgreSQL function wrappers for t1ha1 */

/* t1ha1 for text input with default seed */
PG_FUNCTION_INFO_V1(t1ha1_text);

Datum
t1ha1_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha1_le(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha1 for text input with custom seed */
PG_FUNCTION_INFO_V1(t1ha1_text_seed);

Datum
t1ha1_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha1_le(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha1 for bytea input with default seed */
PG_FUNCTION_INFO_V1(t1ha1_bytea);

Datum
t1ha1_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha1_le(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha1 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(t1ha1_bytea_seed);

Datum
t1ha1_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha1_le(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha1 for integer input with default seed */
PG_FUNCTION_INFO_V1(t1ha1_int);

Datum
t1ha1_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = t1ha1_le((char*)&input, sizeof(int32_t), 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha1 for integer input with custom seed */
PG_FUNCTION_INFO_V1(t1ha1_int_seed);

Datum
t1ha1_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = t1ha1_le((char*)&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* PostgreSQL function wrappers for t1ha2 */

/* t1ha2 for text input with default seed */
PG_FUNCTION_INFO_V1(t1ha2_text);

Datum
t1ha2_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha2_atonce(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha2 for text input with custom seed */
PG_FUNCTION_INFO_V1(t1ha2_text_seed);

Datum
t1ha2_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha2_atonce(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha2 for bytea input with default seed */
PG_FUNCTION_INFO_V1(t1ha2_bytea);

Datum
t1ha2_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha2_atonce(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha2 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(t1ha2_bytea_seed);

Datum
t1ha2_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = t1ha2_atonce(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha2 for integer input with default seed */
PG_FUNCTION_INFO_V1(t1ha2_int);

Datum
t1ha2_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = t1ha2_atonce((char*)&input, sizeof(int32_t), 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* t1ha2 for integer input with custom seed */
PG_FUNCTION_INFO_V1(t1ha2_int_seed);

Datum
t1ha2_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = t1ha2_atonce((char*)&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* PostgreSQL function wrappers for t1ha2_128 */

/* t1ha2_128 for text input with default seed - returns array of two bigints */
PG_FUNCTION_INFO_V1(t1ha2_128_text);

Datum
t1ha2_128_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = t1ha2_atonce128(data, len, 0);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* t1ha2_128 for text input with custom seed */
PG_FUNCTION_INFO_V1(t1ha2_128_text_seed);

Datum
t1ha2_128_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = t1ha2_atonce128(data, len, (uint64_t)seed);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* t1ha2_128 for bytea input with default seed */
PG_FUNCTION_INFO_V1(t1ha2_128_bytea);

Datum
t1ha2_128_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = t1ha2_atonce128(data, len, 0);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* t1ha2_128 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(t1ha2_128_bytea_seed);

Datum
t1ha2_128_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = t1ha2_atonce128(data, len, (uint64_t)seed);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* t1ha2_128 for integer input with default seed */
PG_FUNCTION_INFO_V1(t1ha2_128_int);

Datum
t1ha2_128_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint128_t hash = t1ha2_atonce128((char*)&input, sizeof(int32_t), 0);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* t1ha2_128 for integer input with custom seed */
PG_FUNCTION_INFO_V1(t1ha2_128_int_seed);

Datum
t1ha2_128_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint128_t hash = t1ha2_atonce128((char*)&input, sizeof(int32_t), (uint64_t)seed);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}