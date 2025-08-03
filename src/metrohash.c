#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* MetroHash64 constants */
static const uint64_t k0_64 = 0xD6D018F5;
static const uint64_t k1_64 = 0xA2AA033B;
static const uint64_t k2_64 = 0x62992FC1;
static const uint64_t k3_64 = 0x30BC5B29;

/* MetroHash128 constants */
static const uint64_t k0_128 = 0xC83A91E1;
static const uint64_t k1_128 = 0x8648DBDB;
static const uint64_t k2_128 = 0x7BDEC03B;
static const uint64_t k3_128 = 0x2F5870A5;

/* 128-bit hash result structure */
typedef struct {
    uint64_t low;
    uint64_t high;
} uint128_t;

/* Utility functions */
static uint64_t
metro_fetch64(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint32_t
metro_fetch32(const char *p)
{
    uint32_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint16_t
metro_fetch16(const char *p)
{
    uint16_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint64_t
metro_rotate_right(uint64_t v, unsigned k)
{
    return (v >> k) | (v << (64 - k));
}

/* MetroHash64 implementation */
static uint64_t
metrohash64(const char *buffer, size_t length, uint64_t seed)
{
    const char *ptr = buffer;
    const char *const end = ptr + length;
    
    uint64_t hash = (seed + k2_64) * k0_64;
    
    if (length >= 32)
    {
        uint64_t v[4];
        v[0] = hash;
        v[1] = hash;
        v[2] = hash;
        v[3] = hash;
        
        do
        {
            v[0] += metro_fetch64(ptr) * k0_64; ptr += 8; v[0] = metro_rotate_right(v[0], 29) + v[2];
            v[1] += metro_fetch64(ptr) * k1_64; ptr += 8; v[1] = metro_rotate_right(v[1], 29) + v[3];
            v[2] += metro_fetch64(ptr) * k2_64; ptr += 8; v[2] = metro_rotate_right(v[2], 29) + v[0];
            v[3] += metro_fetch64(ptr) * k3_64; ptr += 8; v[3] = metro_rotate_right(v[3], 29) + v[1];
        }
        while (ptr <= (end - 32));
        
        v[2] ^= metro_rotate_right(((v[0] + v[3]) * k0_64) + v[1], 37) * k1_64;
        v[3] ^= metro_rotate_right(((v[1] + v[2]) * k1_64) + v[0], 37) * k0_64;
        v[0] ^= metro_rotate_right(((v[0] + v[2]) * k0_64) + v[3], 37) * k1_64;
        v[1] ^= metro_rotate_right(((v[1] + v[3]) * k1_64) + v[2], 37) * k0_64;
        hash += v[0] ^ v[1];
    }
    
    if ((end - ptr) >= 16)
    {
        uint64_t v0, v1;
        v0 = hash + (metro_fetch64(ptr) * k2_64); ptr += 8; v0 = metro_rotate_right(v0, 29) * k3_64;
        v1 = hash + (metro_fetch64(ptr) * k2_64); ptr += 8; v1 = metro_rotate_right(v1, 29) * k3_64;
        v0 ^= metro_rotate_right(v0 * k0_64, 21) + v1;
        v1 ^= metro_rotate_right(v1 * k3_64, 21) + v0;
        hash += v1;
    }
    
    if ((end - ptr) >= 8)
    {
        hash += metro_fetch64(ptr) * k3_64; ptr += 8;
        hash ^= metro_rotate_right(hash, 55) * k1_64;
    }
    
    if ((end - ptr) >= 4)
    {
        hash += metro_fetch32(ptr) * k3_64; ptr += 4;
        hash ^= metro_rotate_right(hash, 26) * k1_64;
    }
    
    if ((end - ptr) >= 2)
    {
        hash += metro_fetch16(ptr) * k3_64; ptr += 2;
        hash ^= metro_rotate_right(hash, 48) * k1_64;
    }
    
    if ((end - ptr) >= 1)
    {
        hash += (*ptr) * k3_64;
        hash ^= metro_rotate_right(hash, 37) * k1_64;
    }
    
    hash ^= metro_rotate_right(hash, 28);
    hash *= k0_64;
    hash ^= metro_rotate_right(hash, 29);
    
    return hash;
}

/* MetroHash128 implementation */
static uint128_t
metrohash128(const char *buffer, size_t length, uint64_t seed)
{
    const char *ptr = buffer;
    const char *const end = ptr + length;
    uint128_t result;
    
    uint64_t v[4];
    
    v[0] = ((seed - k0_128) * k3_128) + length;
    v[1] = ((seed + k1_128) * k2_128) + length;
    
    if (length >= 32)
    {
        v[2] = ((seed + k0_128) * k2_128) + length;
        v[3] = ((seed - k1_128) * k3_128) + length;
        
        do
        {
            v[0] += metro_fetch64(ptr) * k0_128; ptr += 8; v[0] = metro_rotate_right(v[0], 29) + v[2];
            v[1] += metro_fetch64(ptr) * k1_128; ptr += 8; v[1] = metro_rotate_right(v[1], 29) + v[3];
            v[2] += metro_fetch64(ptr) * k2_128; ptr += 8; v[2] = metro_rotate_right(v[2], 29) + v[0];
            v[3] += metro_fetch64(ptr) * k3_128; ptr += 8; v[3] = metro_rotate_right(v[3], 29) + v[1];
        }
        while (ptr <= (end - 32));
        
        v[2] ^= metro_rotate_right(((v[0] + v[3]) * k0_128) + v[1], 21) + v[2];
        v[3] ^= metro_rotate_right(((v[1] + v[2]) * k1_128) + v[0], 21) + v[3];
        v[0] ^= metro_rotate_right(((v[0] + v[2]) * k0_128) + v[3], 21) + v[0];
        v[1] ^= metro_rotate_right(((v[1] + v[3]) * k1_128) + v[2], 21) + v[1];
    }
    
    if ((end - ptr) >= 16)
    {
        v[0] += metro_fetch64(ptr) * k2_128; ptr += 8; v[0] = metro_rotate_right(v[0], 33) * k3_128;
        v[1] += metro_fetch64(ptr) * k2_128; ptr += 8; v[1] = metro_rotate_right(v[1], 33) * k3_128;
        v[0] ^= metro_rotate_right((v[0] * k2_128) + v[1], 45) * k1_128;
        v[1] ^= metro_rotate_right((v[1] * k3_128) + v[0], 45) * k0_128;
    }
    
    if ((end - ptr) >= 8)
    {
        v[0] += metro_fetch64(ptr) * k2_128; ptr += 8;
        v[0] = metro_rotate_right(v[0], 33) * k3_128;
        v[0] ^= metro_rotate_right(v[0] * k2_128, 37) * k1_128;
    }
    
    if ((end - ptr) >= 4)
    {
        v[1] += metro_fetch32(ptr) * k2_128; ptr += 4;
        v[1] = metro_rotate_right(v[1], 33) * k3_128;
        v[1] ^= metro_rotate_right(v[1] * k3_128, 37) * k0_128;
    }
    
    if ((end - ptr) >= 2)
    {
        v[0] += metro_fetch16(ptr) * k2_128; ptr += 2;
        v[0] = metro_rotate_right(v[0], 33) * k3_128;
        v[0] ^= metro_rotate_right(v[0] * k2_128, 37) * k1_128;
    }
    
    if ((end - ptr) >= 1)
    {
        v[1] += (*ptr) * k2_128;
        v[1] = metro_rotate_right(v[1], 33) * k3_128;
        v[1] ^= metro_rotate_right(v[1] * k3_128, 37) * k0_128;
    }
    
    v[0] += metro_rotate_right((v[0] * k0_128) + v[1], 13);
    v[1] += metro_rotate_right((v[1] * k1_128) + v[0], 13);
    v[0] += metro_rotate_right((v[0] * k2_128) + v[1], 13);
    v[1] += metro_rotate_right((v[1] * k3_128) + v[0], 13);
    
    result.low = v[0];
    result.high = v[1];
    return result;
}

/* PostgreSQL function wrappers for MetroHash64 */

/* MetroHash64 for text input with default seed */
PG_FUNCTION_INFO_V1(metrohash64_text);

Datum
metrohash64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = metrohash64(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* MetroHash64 for text input with custom seed */
PG_FUNCTION_INFO_V1(metrohash64_text_seed);

Datum
metrohash64_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = metrohash64(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* MetroHash64 for bytea input with default seed */
PG_FUNCTION_INFO_V1(metrohash64_bytea);

Datum
metrohash64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = metrohash64(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* MetroHash64 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(metrohash64_bytea_seed);

Datum
metrohash64_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = metrohash64(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* MetroHash64 for integer input with default seed */
PG_FUNCTION_INFO_V1(metrohash64_int);

Datum
metrohash64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = metrohash64((char*)&input, sizeof(int32_t), 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* MetroHash64 for integer input with custom seed */
PG_FUNCTION_INFO_V1(metrohash64_int_seed);

Datum
metrohash64_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = metrohash64((char*)&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* PostgreSQL function wrappers for MetroHash128 */

/* MetroHash128 for text input with default seed - returns array of two bigints */
PG_FUNCTION_INFO_V1(metrohash128_text);

Datum
metrohash128_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = metrohash128(data, len, 0);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* MetroHash128 for text input with custom seed */
PG_FUNCTION_INFO_V1(metrohash128_text_seed);

Datum
metrohash128_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = metrohash128(data, len, (uint64_t)seed);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* MetroHash128 for bytea input with default seed */
PG_FUNCTION_INFO_V1(metrohash128_bytea);

Datum
metrohash128_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = metrohash128(data, len, 0);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* MetroHash128 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(metrohash128_bytea_seed);

Datum
metrohash128_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = metrohash128(data, len, (uint64_t)seed);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* MetroHash128 for integer input with default seed */
PG_FUNCTION_INFO_V1(metrohash128_int);

Datum
metrohash128_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint128_t hash = metrohash128((char*)&input, sizeof(int32_t), 0);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* MetroHash128 for integer input with custom seed */
PG_FUNCTION_INFO_V1(metrohash128_int_seed);

Datum
metrohash128_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint128_t hash = metrohash128((char*)&input, sizeof(int32_t), (uint64_t)seed);
    
    Datum result[2];
    ArrayType *array;
    
    result[0] = Int64GetDatum((int64_t)hash.low);
    result[1] = Int64GetDatum((int64_t)hash.high);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}