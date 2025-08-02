#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"
#include "utils/array.h"

/* CityHash128 constants */
static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;
static const uint64_t k2 = 0x9ae16a3b2f90404fULL;

/* 128-bit hash result structure */
typedef struct {
    uint64_t low;
    uint64_t high;
} uint128_t;

/* Utility functions for CityHash128 */
static uint64_t
Fetch64(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint32_t
Fetch32(const char *p)
{
    uint32_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint64_t
Rotate(uint64_t val, int shift)
{
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

static uint64_t
ShiftMix(uint64_t val)
{
    return val ^ (val >> 47);
}

static uint64_t
HashLen16(uint64_t u, uint64_t v)
{
    return ShiftMix((u ^ v) * k2) * k2;
}

static uint64_t
HashLen16Mul(uint64_t u, uint64_t v, uint64_t mul)
{
    uint64_t a = (u ^ v) * mul;
    uint64_t b;
    a ^= (a >> 47);
    b = (v ^ a) * mul;
    b ^= (b >> 47);
    b *= mul;
    return b;
}

static uint128_t
HashLen16_128(uint64_t u, uint64_t v, uint64_t mul)
{
    uint128_t result;
    uint64_t a, b;
    a = (u ^ v) * mul;
    a ^= (a >> 47);
    b = (v ^ a) * mul;
    b ^= (b >> 47);
    b *= mul;
    result.low = a;
    result.high = b;
    return result;
}

static uint64_t
HashLen0to16(const char *s, size_t len)
{
    if (len >= 8) {
        uint64_t mul = k2 + len * 2;
        uint64_t a = Fetch64(s) + k2;
        uint64_t b = Fetch64(s + len - 8);
        uint64_t c = Rotate(b, 37) * mul + a;
        uint64_t d = (Rotate(a, 25) + b) * mul;
        return HashLen16Mul(c, d, mul);
    }
    if (len >= 4) {
        uint64_t mul = k2 + len * 2;
        uint64_t a = Fetch32(s);
        return HashLen16Mul(len + (a << 3), Fetch32(s + len - 4), mul);
    }
    if (len > 0) {
        uint8_t a = s[0];
        uint8_t b = s[len >> 1];
        uint8_t c = s[len - 1];
        uint32_t y = (uint32_t)(a) + ((uint32_t)(b) << 8);
        uint32_t z = len + ((uint32_t)(c) << 2);
        return ShiftMix(y * k2 ^ z * k0) * k2;
    }
    return k2;
}

static uint128_t
HashLen0to16_128(const char *s, size_t len)
{
    uint128_t result;
    if (len > 8) {
        uint64_t mul = k2 + len * 2;
        uint64_t a = Fetch64(s) + k2;
        uint64_t b = Fetch64(s + len - 8);
        uint64_t c = Rotate(b, 37) * mul + a;
        uint64_t d = (Rotate(a, 25) + b) * mul;
        return HashLen16_128(c, d, mul);
    } else {
        uint64_t hash64 = HashLen0to16(s, len);
        result.low = hash64;
        result.high = hash64;
        return result;
    }
}

static uint128_t
HashLen17to32_128(const char *s, size_t len)
{
    uint64_t mul = k2 + len * 2;
    uint64_t a = Fetch64(s) * k1;
    uint64_t b = Fetch64(s + 8);
    uint64_t c = Fetch64(s + len - 8) * mul;
    uint64_t d = Fetch64(s + len - 16) * k2;
    uint64_t u = Rotate(a + b, 43) + Rotate(c, 30) + d;
    uint64_t v = a + Rotate(b + k2, 18) + c;
    return HashLen16_128(u, v, mul);
}

static uint128_t
HashLen33to64_128(const char *s, size_t len)
{
    uint64_t mul, a, b, c, d, y, e, f, u, v;
    mul = k2 + len * 2;
    a = Fetch64(s) * k2;
    b = Fetch64(s + 8);
    c = Fetch64(s + len - 8) * mul;
    d = Fetch64(s + len - 16) * k2;
    y = Rotate(a + b, 43) + Rotate(c, 30) + d;
    e = Fetch64(s + 16) * mul;
    f = Fetch64(s + 24);
    u = Rotate(e, 43) + Rotate(f, 30) + c;
    v = e + Rotate(f + a, 18) + d;
    return HashLen16_128(Rotate(u, 33) * mul, Rotate(v, 33) * mul, mul);
}

/* CityHash128 main implementation */
static uint128_t
cityhash128_with_seed(const char *s, size_t len, uint128_t seed)
{
    uint128_t result;
    uint64_t x, y, z, v_low, v_high, w_low, w_high, mul;
    const char *end, *last128;
    
    if (len < 128) {
        if (len >= 32) {
            if (len < 64) {
                result = HashLen33to64_128(s, len);
            } else {
                x = seed.low;
                y = seed.high;
                z = len * k1;
                result.low = HashLen16(x + z, y);
                result.high = HashLen16(x, y + z);
                return result;
            }
        } else {
            if (len >= 16) {
                result = HashLen17to32_128(s, len);
            } else {
                result = HashLen0to16_128(s, len);
            }
        }
        result.low = HashLen16(result.low + seed.high, result.high + seed.low);
        result.high = HashLen16(result.high + seed.low, result.low + seed.high);
        return result;
    }

    x = seed.low;
    y = seed.high;
    z = len * k1;
    v_low = Rotate(y ^ k1, 49) * k1 + Fetch64(s);
    v_high = Rotate(v_low, 42) * k1 + Fetch64(s + 8);
    w_low = Rotate(y + z, 35) * k1 + x;
    w_high = Rotate(x + Fetch64(s + 88), 53) * k1;

    end = s + ((len - 1) / 128) * 128;
    last128 = end + ((len - 1) & 127) - 127;
    
    do {
        x = Rotate(x + y + v_low + Fetch64(s + 8), 37) * k1;
        y = Rotate(y + v_high + Fetch64(s + 48), 42) * k1;
        x ^= w_high;
        y += v_low + Fetch64(s + 40);
        z = Rotate(z + w_low, 33) * k1;
        v_low *= k1;
        v_high *= k1;
        w_low = Rotate(w_low + w_high, 43) + Rotate(v_low, 30) + v_high;
        w_high = w_low + Rotate(w_high + z, 18) + v_low;
        s += 128;
    } while (s != end);
    
    mul = k1 + ((z & 0xff) << 1);
    s = last128;
    w_low += ((len - 1) & 127);
    v_low += w_low;
    w_low += v_high;
    x = Rotate(x + y + v_low + Fetch64(s + 8), 37) * mul;
    y = Rotate(y + v_high + Fetch64(s + 48), 42) * mul;
    x ^= w_high * 9;
    y += v_low * 9 + Fetch64(s + 40);
    z = Rotate(z + w_low, 33) * mul;
    
    result.low = HashLen16Mul(HashLen16Mul(v_low, w_low, mul) + ShiftMix(y) * k0 + z,
                              HashLen16Mul(v_high, w_high, mul) + x, mul);
    result.high = HashLen16Mul(HashLen16Mul(v_high, w_high, mul) + ShiftMix(x) * k0 + w_low,
                               HashLen16Mul(v_low, w_low, mul) + y, mul);
    return result;
}

static uint128_t
cityhash128(const char *s, size_t len)
{
    uint128_t seed;
    
    if (len >= 16) {
        seed.low = Fetch64(s);
        seed.high = Fetch64(s + 8) + k0;
        return cityhash128_with_seed(s + 16, len - 16, seed);
    }
    
    seed.low = k0;
    seed.high = k1; 
    return cityhash128_with_seed(s, len, seed);
}

/* CityHash128 for text input with default seed - returns array */
PG_FUNCTION_INFO_V1(cityhash128_text);

Datum
cityhash128_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = cityhash128(data, len);
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    elems[0] = Int64GetDatum((int64_t)hash.low);
    elems[1] = Int64GetDatum((int64_t)hash.high);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* CityHash128 for text input with custom seed - returns array */
PG_FUNCTION_INFO_V1(cityhash128_text_seed);

Datum
cityhash128_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed_low = PG_GETARG_INT64(1);
    int64_t seed_high = PG_GETARG_INT64(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t seed, hash;
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    seed.low = (uint64_t)seed_low;
    seed.high = (uint64_t)seed_high;
    
    hash = cityhash128_with_seed(data, len, seed);
    
    elems[0] = Int64GetDatum((int64_t)hash.low);
    elems[1] = Int64GetDatum((int64_t)hash.high);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* CityHash128 for bytea input with default seed - returns array */
PG_FUNCTION_INFO_V1(cityhash128_bytea);

Datum
cityhash128_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t hash = cityhash128(data, len);
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    elems[0] = Int64GetDatum((int64_t)hash.low);
    elems[1] = Int64GetDatum((int64_t)hash.high);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* CityHash128 for bytea input with custom seed - returns array */
PG_FUNCTION_INFO_V1(cityhash128_bytea_seed);

Datum
cityhash128_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed_low = PG_GETARG_INT64(1);
    int64_t seed_high = PG_GETARG_INT64(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint128_t seed, hash;
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    seed.low = (uint64_t)seed_low;
    seed.high = (uint64_t)seed_high;
    
    hash = cityhash128_with_seed(data, len, seed);
    
    elems[0] = Int64GetDatum((int64_t)hash.low);
    elems[1] = Int64GetDatum((int64_t)hash.high);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* CityHash128 for integer input - returns array */
PG_FUNCTION_INFO_V1(cityhash128_int);

Datum
cityhash128_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint128_t hash = cityhash128((const char *)&input, sizeof(int32_t));
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    elems[0] = Int64GetDatum((int64_t)hash.low);
    elems[1] = Int64GetDatum((int64_t)hash.high);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* CityHash128 for integer input with custom seed - returns array */
PG_FUNCTION_INFO_V1(cityhash128_int_seed);

Datum
cityhash128_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed_low = PG_GETARG_INT64(1);
    int64_t seed_high = PG_GETARG_INT64(2);
    uint128_t seed, hash;
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    seed.low = (uint64_t)seed_low;
    seed.high = (uint64_t)seed_high;
    
    hash = cityhash128_with_seed((const char *)&input, sizeof(int32_t), seed);
    
    elems[0] = Int64GetDatum((int64_t)hash.low);
    elems[1] = Int64GetDatum((int64_t)hash.high);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}