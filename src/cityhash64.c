#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* CityHash64 constants */
static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;
static const uint64_t k2 = 0x9ae16a3b2f90404fULL;

/* Utility functions for CityHash64 */
static uint64_t
Fetch64(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}

static uint64_t
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

static uint64_t
HashLen17to32(const char *s, size_t len)
{
    uint64_t mul = k2 + len * 2;
    uint64_t a = Fetch64(s) * k1;
    uint64_t b = Fetch64(s + 8);
    uint64_t c = Fetch64(s + len - 8) * mul;
    uint64_t d = Fetch64(s + len - 16) * k2;
    return HashLen16Mul(Rotate(a + b, 43) + Rotate(c, 30) + d,
                        a + Rotate(b + k2, 18) + c, mul);
}

static uint64_t
HashLen33to64(const char *s, size_t len)
{
    uint64_t mul = k2 + len * 2;
    uint64_t a = Fetch64(s) * k2;
    uint64_t b = Fetch64(s + 8);
    uint64_t c = Fetch64(s + len - 24);
    uint64_t d = Fetch64(s + len - 32);
    uint64_t e = Fetch64(s + 16) * k2;
    uint64_t f = Fetch64(s + 24) * 9;
    uint64_t g = Fetch64(s + len - 8);
    uint64_t h = Fetch64(s + len - 16) * mul;
    uint64_t u = Rotate(a + g, 43) + (Rotate(b, 30) + c) * 9;
    uint64_t v = ((a + g) ^ d) + f + 1;
    uint64_t w = ((u + v) * mul) + h;
    uint64_t x = Rotate(e + f, 42) + c;
    uint64_t y = (((v + w) * mul) + g) * mul;
    uint64_t z = e + f + c;
    a = ((x + z) * mul + y) * mul + b;
    b = ((Rotate(v, 22) ^ w) * mul) + a;
    return HashLen16Mul(a, b + z, mul);
}

/* CityHash64 main implementation */
static uint64_t
cityhash64_with_seed(const char *s, size_t len, uint64_t seed)
{
    uint64_t x, y, z, v_first, v_second, w_first, w_second, mul;
    const char *end, *last64;
    
    if (len <= 32) {
        if (len <= 16) {
            return HashLen16(len + seed, HashLen0to16(s, len));
        } else {
            return HashLen16(HashLen17to32(s, len) + seed, len);
        }
    }
    
    if (len <= 64) {
        return HashLen16(HashLen33to64(s, len) + seed, len);
    }
    
    /* For strings over 64 bytes, we use a simplified version */
    
    x = seed;
    y = seed * k1 + 113;
    z = ShiftMix(y * k2 + 113) * k2;
    v_first = 0;
    v_second = 0;
    w_first = 0;
    w_second = 0;
    end = s + ((len - 1) / 64) * 64;
    last64 = end + ((len - 1) & 63) - 63;
    
    do {
        x = Rotate(x + y + v_first + Fetch64(s + 8), 37) * k1;
        y = Rotate(y + v_second + Fetch64(s + 48), 42) * k1;
        x ^= w_second;
        y += v_first + Fetch64(s + 40);
        z = Rotate(z + w_first, 33) * k1;
        v_first = Fetch64(s) * k1;
        v_second = Fetch64(s + 8) * k1;
        w_first = Fetch64(s + 16) * k1;
        w_second = Fetch64(s + 24) * k1;
        v_first = Rotate(v_first + v_second, 43) + Rotate(w_first, 30) + w_second;
        v_second = v_first + Rotate(v_second + z, 18) + w_first;
        w_first = Fetch64(s + 32) * k1;
        w_second = Fetch64(s + 40) * k1;
        z = Rotate(z + x, 33) * k1;
        w_first = Rotate(w_first + w_second, 43) + Rotate(x, 30) + y;
        w_second = w_first + Rotate(w_second + z, 18) + x;
        s += 64;
    } while (s != end);
    
    mul = k1 + ((z & 0xff) << 1);
    s = last64;
    w_first += ((len - 1) & 63);
    v_first += w_first;
    w_first += v_second;
    x = Rotate(x + y + v_first + Fetch64(s + 8), 37) * mul;
    y = Rotate(y + v_second + Fetch64(s + 48), 42) * mul;
    x ^= w_second * 9;
    y += v_first * 9 + Fetch64(s + 40);
    z = Rotate(z + w_first, 33) * mul;
    v_first = Fetch64(s) * mul;
    v_second = Fetch64(s + 8) * mul;
    w_first = Fetch64(s + 16) * mul;
    w_second = Fetch64(s + 24) * mul;
    v_first = Rotate(v_first + v_second, 43) + Rotate(w_first, 30) + w_second;
    v_second = v_first + Rotate(v_second + z, 18) + w_first;
    w_first = Fetch64(s + 32) * mul;
    w_second = Fetch64(s + 40) * mul;
    z = Rotate(z + x, 33) * mul;
    w_first = Rotate(w_first + w_second, 43) + Rotate(x, 30) + y;
    w_second = w_first + Rotate(w_second + z, 18) + x;
    
    return HashLen16Mul(HashLen16Mul(v_first, w_first, mul) + ShiftMix(y) * k0 + z,
                        HashLen16Mul(v_second, w_second, mul) + x, mul);
}

static uint64_t
cityhash64(const char *s, size_t len)
{
    if (len <= 32) {
        if (len <= 16) {
            return HashLen0to16(s, len);
        } else {
            return HashLen17to32(s, len);
        }
    } else if (len <= 64) {
        return HashLen33to64(s, len);
    }
    
    return cityhash64_with_seed(s, len, 81);
}

/* CityHash64 for text input with default seed */
PG_FUNCTION_INFO_V1(cityhash64_text);

Datum
cityhash64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = cityhash64(data, len);
    PG_RETURN_INT64((int64_t)hash);
}

/* CityHash64 for text input with custom seed */
PG_FUNCTION_INFO_V1(cityhash64_text_seed);

Datum
cityhash64_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = cityhash64_with_seed(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* CityHash64 for bytea input with default seed */
PG_FUNCTION_INFO_V1(cityhash64_bytea);

Datum
cityhash64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = cityhash64(data, len);
    PG_RETURN_INT64((int64_t)hash);
}

/* CityHash64 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(cityhash64_bytea_seed);

Datum
cityhash64_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = cityhash64_with_seed(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* CityHash64 for integer input */
PG_FUNCTION_INFO_V1(cityhash64_int);

Datum
cityhash64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = cityhash64((const char *)&input, sizeof(int32_t));
    PG_RETURN_INT64((int64_t)hash);
}

/* CityHash64 for integer input with custom seed */
PG_FUNCTION_INFO_V1(cityhash64_int_seed);

Datum
cityhash64_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = cityhash64_with_seed((const char *)&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}