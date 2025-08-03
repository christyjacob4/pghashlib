#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* WyHash algorithm implementation
 * Based on the wyhash algorithm by Wang Yi
 * Released into the public domain under The Unlicense
 */

/* WyHash constants - default secret parameters */
static const uint64_t _wyp[4] = {
    0x2d358dccaa6c78a5ULL,
    0x8bb84b93962eacc9ULL,
    0x4b33a62ed433d4a3ULL,
    0x4d5a2da51de1aa47ULL
};

/* Endian detection */
#ifndef WYHASH_LITTLE_ENDIAN
#if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define WYHASH_LITTLE_ENDIAN 1
#elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define WYHASH_LITTLE_ENDIAN 0
#else
#warning "Unable to determine endianness, assuming little endian"
#define WYHASH_LITTLE_ENDIAN 1
#endif
#endif

/* Byte reading functions */
static inline uint64_t
_wyr8(const uint8_t *p)
{
    uint64_t v;
    memcpy(&v, p, 8);
#if WYHASH_LITTLE_ENDIAN
    return v;
#else
    return __builtin_bswap64(v);
#endif
}

static inline uint64_t
_wyr4(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, 4);
#if WYHASH_LITTLE_ENDIAN
    return v;
#else
    return __builtin_bswap32(v);
#endif
}

static inline uint64_t
_wyr3(const uint8_t *p, size_t k)
{
    return (((uint64_t)p[0]) << 16) | (((uint64_t)p[k >> 1]) << 8) | p[k - 1];
}

/* 128-bit multiplication function */
static inline void
_wymum(uint64_t *A, uint64_t *B)
{
#ifdef __SIZEOF_INT128__
    __uint128_t r = *A;
    r *= *B;
    *A = (uint64_t)r;
    *B = (uint64_t)(r >> 64);
#else
    /* Fallback for systems without 128-bit integers */
    uint64_t ha = *A >> 32, hb = *B >> 32, la = (uint32_t)*A, lb = (uint32_t)*B, hi, lo;
    uint64_t rh = ha * hb, rm0 = ha * lb, rm1 = hb * la, rl = la * lb, t = rl + (rm0 << 32), c = t < rl;
    lo = t + (rm1 << 32);
    c += lo < t;
    hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
    *A = lo;
    *B = hi;
#endif
}

/* Mixing function */
static inline uint64_t
_wymix(uint64_t A, uint64_t B)
{
    _wymum(&A, &B);
    return A ^ B;
}

/* Main WyHash function */
static uint64_t
wyhash(const void *key, size_t len, uint64_t seed, const uint64_t *secret)
{
    const uint8_t *p = (const uint8_t *)key;
    uint64_t a, b;
    seed ^= *secret;
    
    if (len <= 16)
    {
        if (len >= 4)
        {
            a = (_wyr4(p) << 32) | _wyr4(p + ((len >> 3) << 2));
            b = (_wyr4(p + len - 4) << 32) | _wyr4(p + len - 4 - ((len >> 3) << 2));
        }
        else if (len > 0)
        {
            a = _wyr3(p, len);
            b = 0;
        }
        else
        {
            a = b = 0;
        }
    }
    else
    {
        size_t i = len;
        if (i > 48)
        {
            uint64_t see1 = seed, see2 = seed;
            do
            {
                seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
                see1 = _wymix(_wyr8(p + 16) ^ secret[2], _wyr8(p + 24) ^ see1);
                see2 = _wymix(_wyr8(p + 32) ^ secret[3], _wyr8(p + 40) ^ see2);
                p += 48;
                i -= 48;
            } while (i > 48);
            seed ^= see1 ^ see2;
        }
        while (i > 16)
        {
            seed = _wymix(_wyr8(p) ^ secret[1], _wyr8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }
        a = _wyr8(p + i - 16);
        b = _wyr8(p + i - 8);
    }
    
    return _wymix(secret[1] ^ len, _wymix(a ^ secret[1], b ^ seed));
}

/* PostgreSQL function wrappers */

/* Convert text/bytea data to bytes and get length */
static inline void
get_text_data_and_length(Datum input, Oid input_type, const uint8_t **data, size_t *len)
{
    switch (input_type)
    {
        case TEXTOID:
        case VARCHAROID:
        {
            text *txt = DatumGetTextP(input);
            *data = (const uint8_t *)VARDATA_ANY(txt);
            *len = VARSIZE_ANY_EXHDR(txt);
            break;
        }
        case BYTEAOID:
        {
            bytea *ba = DatumGetByteaP(input);
            *data = (const uint8_t *)VARDATA_ANY(ba);
            *len = VARSIZE_ANY_EXHDR(ba);
            break;
        }
        default:
            ereport(ERROR,
                    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                     errmsg("unsupported data type for wyhash")));
    }
}

/* wyhash(text) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_text);
Datum
wyhash_text(PG_FUNCTION_ARGS)
{
    const uint8_t *data;
    size_t len;
    uint64_t hash;
    
    get_text_data_and_length(PG_GETARG_DATUM(0), get_fn_expr_argtype(fcinfo->flinfo, 0), &data, &len);
    
    hash = wyhash(data, len, 0, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(text, bigint) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_text_seed);
Datum
wyhash_text_seed(PG_FUNCTION_ARGS)
{
    const uint8_t *data;
    size_t len;
    uint64_t seed = (uint64_t)PG_GETARG_INT64(1);
    uint64_t hash;
    
    get_text_data_and_length(PG_GETARG_DATUM(0), get_fn_expr_argtype(fcinfo->flinfo, 0), &data, &len);
    
    hash = wyhash(data, len, seed, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(bytea) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_bytea);
Datum
wyhash_bytea(PG_FUNCTION_ARGS)
{
    const uint8_t *data;
    size_t len;
    uint64_t hash;
    
    get_text_data_and_length(PG_GETARG_DATUM(0), BYTEAOID, &data, &len);
    
    hash = wyhash(data, len, 0, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(bytea, bigint) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_bytea_seed);
Datum
wyhash_bytea_seed(PG_FUNCTION_ARGS)
{
    const uint8_t *data;
    size_t len;
    uint64_t seed = (uint64_t)PG_GETARG_INT64(1);
    uint64_t hash;
    
    get_text_data_and_length(PG_GETARG_DATUM(0), BYTEAOID, &data, &len);
    
    hash = wyhash(data, len, seed, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(integer) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_int4);
Datum
wyhash_int4(PG_FUNCTION_ARGS)
{
    int32 val = PG_GETARG_INT32(0);
    uint64_t hash;
    
    hash = wyhash(&val, sizeof(int32), 0, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(integer, bigint) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_int4_seed);
Datum
wyhash_int4_seed(PG_FUNCTION_ARGS)
{
    int32 val = PG_GETARG_INT32(0);
    uint64_t seed = (uint64_t)PG_GETARG_INT64(1);
    uint64_t hash;
    
    hash = wyhash(&val, sizeof(int32), seed, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(bigint) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_int8);
Datum
wyhash_int8(PG_FUNCTION_ARGS)
{
    int64 val = PG_GETARG_INT64(0);
    uint64_t hash;
    
    hash = wyhash(&val, sizeof(int64), 0, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}

/* wyhash(bigint, bigint) -> bigint */
PG_FUNCTION_INFO_V1(wyhash_int8_seed);
Datum
wyhash_int8_seed(PG_FUNCTION_ARGS)
{
    int64 val = PG_GETARG_INT64(0);
    uint64_t seed = (uint64_t)PG_GETARG_INT64(1);
    uint64_t hash;
    
    hash = wyhash(&val, sizeof(int64), seed, _wyp);
    PG_RETURN_INT64((int64_t)hash);
}