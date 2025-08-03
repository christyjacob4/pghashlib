#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"
#include "utils/array.h"
#include "catalog/pg_type.h"

/* SpookyHash constants */
static const uint64_t sc_const = 0xdeadbeefdeadbeefULL;

/* Allow unaligned reads - define based on platform */
#ifndef ALLOW_UNALIGNED_READS
#define ALLOW_UNALIGNED_READS 1
#endif

/* SpookyHash utility functions - removed unused ones */

static void
Short(const void *message, size_t length, uint64_t *hash1, uint64_t *hash2)
{
    uint64_t buf[2 * 12];
    union {
        const uint8_t *p8;
        uint32_t *p32;
        uint64_t *p64;
        size_t i;
    } u;
    size_t remainder;
    uint64_t a, b, c, d;
    u.p8 = (const uint8_t *)message;

    if (!ALLOW_UNALIGNED_READS && (u.i & 0x7)) {
        memcpy(buf, message, length);
        u.p64 = buf;
    }

    remainder = length % 32;
    a = *hash1;
    b = *hash2;
    c = sc_const;
    d = sc_const;

    if (length > 15) {
        const uint64_t *end = u.p64 + (length / 32) * 4;

        /* handle all complete sets of 32 bytes */
        for (; u.p64 < end; u.p64 += 4) {
            c += u.p64[0];
            d += u.p64[1];
            a ^= c; a = (a << 50) | (a >> 14); d ^= a;
            b ^= d; b = (b << 52) | (b >> 12); c ^= b;
            c ^= u.p64[2];
            d ^= u.p64[3];
            a ^= c; a = (a << 23) | (a >> 41); d ^= a;
            b ^= d; b = (b << 5) | (b >> 59); c ^= b;
        }

        /* Handle the case of 16+ remaining bytes. */
        if (remainder >= 16) {
            c += u.p64[0];
            d += u.p64[1];
            a ^= c; a = (a << 50) | (a >> 14); d ^= a;
            b ^= d; b = (b << 52) | (b >> 12); c ^= b;
            u.p64 += 2;
            remainder -= 16;
        }
    }

    /* Handle the last 0..15 bytes, and its length */
    d += ((uint64_t)length) << 56;
    switch (remainder) {
        case 15: d += ((uint64_t)u.p8[14]) << 48; /* fallthrough */
        case 14: d += ((uint64_t)u.p8[13]) << 40; /* fallthrough */
        case 13: d += ((uint64_t)u.p8[12]) << 32; /* fallthrough */
        case 12: d += u.p32[2]; c += u.p64[0]; break;
        case 11: d += ((uint64_t)u.p8[10]) << 16; /* fallthrough */
        case 10: d += ((uint64_t)u.p8[9]) << 8; /* fallthrough */
        case 9: d += (uint64_t)u.p8[8]; /* fallthrough */
        case 8: c += u.p64[0]; break;
        case 7: c += ((uint64_t)u.p8[6]) << 48; /* fallthrough */
        case 6: c += ((uint64_t)u.p8[5]) << 40; /* fallthrough */
        case 5: c += ((uint64_t)u.p8[4]) << 32; /* fallthrough */
        case 4: c += u.p32[0]; break;
        case 3: c += ((uint64_t)u.p8[2]) << 16; /* fallthrough */
        case 2: c += ((uint64_t)u.p8[1]) << 8; /* fallthrough */
        case 1: c += (uint64_t)u.p8[0]; break;
        case 0: c += sc_const; d += sc_const;
    }
    a ^= d; a = (a << 44) | (a >> 20); c ^= a;
    b ^= c; b = (b << 15) | (b >> 49); d ^= b;
    a ^= d; a = (a << 34) | (a >> 30); c ^= a;
    b ^= c; b = (b << 21) | (b >> 43); d ^= b;
    *hash1 = a;
    *hash2 = b;
}

static void
End(uint64_t *data, uint64_t *h0, uint64_t *h1, uint64_t *h2, uint64_t *h3, uint64_t *h4, uint64_t *h5, uint64_t *h6, uint64_t *h7, uint64_t *h8, uint64_t *h9, uint64_t *h10, uint64_t *h11)
{
    *h0 += data[0]; *h1 += data[1]; *h2 += data[2]; *h3 += data[3];
    *h4 += data[4]; *h5 += data[5]; *h6 += data[6]; *h7 += data[7];
    *h8 += data[8]; *h9 += data[9]; *h10 += data[10]; *h11 += data[11];
    *h11^=*h1; *h2^=*h11; *h1 = (*h1<<44)|(*h1>>20);
    *h0^=*h2; *h3^=*h0; *h2 = (*h2<<15)|(*h2>>49);
    *h1^=*h3; *h4^=*h1; *h3 = (*h3<<34)|(*h3>>30);
    *h2^=*h4; *h5^=*h2; *h4 = (*h4<<21)|(*h4>>43);
    *h3^=*h5; *h6^=*h3; *h5 = (*h5<<14)|(*h5>>50);
    *h4^=*h6; *h7^=*h4; *h6 = (*h6<<13)|(*h6>>51);
    *h5^=*h7; *h8^=*h5; *h7 = (*h7<<11)|(*h7>>53);
    *h6^=*h8; *h9^=*h6; *h8 = (*h8<<25)|(*h8>>39);
    *h7^=*h9; *h10^=*h7; *h9 = (*h9<<42)|(*h9>>22);
    *h8^=*h10; *h11^=*h8; *h10 = (*h10<<9)|(*h10>>55);
    *h9^=*h11; *h0^=*h9; *h11 = (*h11<<35)|(*h11>>29);
    *h10^=*h0; *h1^=*h10; *h0 = (*h0<<39)|(*h0>>25);
}

static void
EndPartial(uint64_t *h0, uint64_t *h1, uint64_t *h2, uint64_t *h3, uint64_t *h4, uint64_t *h5, uint64_t *h6, uint64_t *h7, uint64_t *h8, uint64_t *h9, uint64_t *h10, uint64_t *h11)
{
    *h11+=*h1; *h2^=*h11; *h1 = (*h1<<44)|(*h1>>20);
    *h0+=*h2; *h3^=*h0; *h2 = (*h2<<15)|(*h2>>49);
    *h1+=*h3; *h4^=*h1; *h3 = (*h3<<34)|(*h3>>30);
    *h2+=*h4; *h5^=*h2; *h4 = (*h4<<21)|(*h4>>43);
    *h3+=*h5; *h6^=*h3; *h5 = (*h5<<14)|(*h5>>50);
    *h4+=*h6; *h7^=*h4; *h6 = (*h6<<13)|(*h6>>51);
    *h5+=*h7; *h8^=*h5; *h7 = (*h7<<11)|(*h7>>53);
    *h6+=*h8; *h9^=*h6; *h8 = (*h8<<25)|(*h8>>39);
    *h7+=*h9; *h10^=*h7; *h9 = (*h9<<42)|(*h9>>22);
    *h8+=*h10; *h11^=*h8; *h10 = (*h10<<9)|(*h10>>55);
    *h9+=*h11; *h0^=*h9; *h11 = (*h11<<35)|(*h11>>29);
    *h10+=*h0; *h1^=*h10; *h0 = (*h0<<39)|(*h0>>25);
}

/* SpookyHash main implementation */
static void
spookyhash_128(const void *message, size_t length, uint64_t *hash1, uint64_t *hash2)
{
    uint64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11;
    uint64_t buf[12];
    uint64_t *end;
    union {
        const uint8_t *p8;
        uint64_t *p64;
        size_t i;
    } u;
    size_t remainder;

    if (length < 192) {
        Short(message, length, hash1, hash2);
        return;
    }

    h0 = h3 = h6 = h9 = *hash1;
    h1 = h4 = h7 = h10 = *hash2;
    h2 = h5 = h8 = h11 = sc_const;

    u.p8 = (const uint8_t *)message;
    end = u.p64 + (length / 96) * 12;

    /* handle all complete sets of 96 bytes */
    if (ALLOW_UNALIGNED_READS || ((u.i & 0x7) == 0)) {
        while (u.p64 < end) {
            h0 += u.p64[0]; h1 += u.p64[1]; h2 += u.p64[2]; h3 += u.p64[3];
            h4 += u.p64[4]; h5 += u.p64[5]; h6 += u.p64[6]; h7 += u.p64[7];
            h8 += u.p64[8]; h9 += u.p64[9]; h10+= u.p64[10]; h11+= u.p64[11];
            EndPartial(&h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,&h8,&h9,&h10,&h11);
            u.p64 += 12;
        }
    } else {
        while (u.p64 < end) {
            memcpy(buf, u.p64, 96);
            h0 += buf[0]; h1 += buf[1]; h2 += buf[2]; h3 += buf[3];
            h4 += buf[4]; h5 += buf[5]; h6 += buf[6]; h7 += buf[7];
            h8 += buf[8]; h9 += buf[9]; h10+= buf[10]; h11+= buf[11];
            EndPartial(&h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,&h8,&h9,&h10,&h11);
            u.p64 += 12;
        }
    }

    /* handle the last partial block of 96 bytes */
    remainder = (length - ((const uint8_t *)end - (const uint8_t *)message));
    memcpy(buf, end, remainder);
    memset(((uint8_t *)buf) + remainder, 0, 96 - remainder);
    ((uint8_t *)buf)[95] = (uint8_t)remainder;

    h0 += buf[0]; h1 += buf[1]; h2 += buf[2]; h3 += buf[3];
    h4 += buf[4]; h5 += buf[5]; h6 += buf[6]; h7 += buf[7];
    h8 += buf[8]; h9 += buf[9]; h10+= buf[10]; h11+= buf[11];
    End(buf, &h0,&h1,&h2,&h3,&h4,&h5,&h6,&h7,&h8,&h9,&h10,&h11);

    *hash1 = h0;
    *hash2 = h1;
}

static uint64_t
spookyhash_64(const void *message, size_t length, uint64_t seed)
{
    uint64_t hash1 = seed;
    uint64_t hash2 = seed;
    spookyhash_128(message, length, &hash1, &hash2);
    return hash1;
}

/* SpookyHash64 for text input with default seed */
PG_FUNCTION_INFO_V1(spookyhash64_text);

Datum
spookyhash64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = spookyhash_64(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* SpookyHash64 for text input with custom seed */
PG_FUNCTION_INFO_V1(spookyhash64_text_seed);

Datum
spookyhash64_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = spookyhash_64(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* SpookyHash64 for bytea input with default seed */
PG_FUNCTION_INFO_V1(spookyhash64_bytea);

Datum
spookyhash64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = spookyhash_64(data, len, 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* SpookyHash64 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(spookyhash64_bytea_seed);

Datum
spookyhash64_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed = PG_GETARG_INT64(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = spookyhash_64(data, len, (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* SpookyHash64 for integer input with default seed */
PG_FUNCTION_INFO_V1(spookyhash64_int);

Datum
spookyhash64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = spookyhash_64(&input, sizeof(int32_t), 0);
    PG_RETURN_INT64((int64_t)hash);
}

/* SpookyHash64 for integer input with custom seed */
PG_FUNCTION_INFO_V1(spookyhash64_int_seed);

Datum
spookyhash64_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed = PG_GETARG_INT64(1);
    uint64_t hash = spookyhash_64(&input, sizeof(int32_t), (uint64_t)seed);
    PG_RETURN_INT64((int64_t)hash);
}

/* SpookyHash128 for text input with default seed */
PG_FUNCTION_INFO_V1(spookyhash128_text);

Datum
spookyhash128_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash1 = 0, hash2 = 0;
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    spookyhash_128(data, len, &hash1, &hash2);
    
    elems[0] = Int64GetDatum((int64_t)hash1);
    elems[1] = Int64GetDatum((int64_t)hash2);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* SpookyHash128 for text input with custom seeds */
PG_FUNCTION_INFO_V1(spookyhash128_text_seed);

Datum
spookyhash128_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t seed1 = PG_GETARG_INT64(1);
    int64_t seed2 = PG_GETARG_INT64(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash1 = (uint64_t)seed1, hash2 = (uint64_t)seed2;
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    spookyhash_128(data, len, &hash1, &hash2);
    
    elems[0] = Int64GetDatum((int64_t)hash1);
    elems[1] = Int64GetDatum((int64_t)hash2);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* SpookyHash128 for bytea input with default seed */
PG_FUNCTION_INFO_V1(spookyhash128_bytea);

Datum
spookyhash128_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash1 = 0, hash2 = 0;
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    spookyhash_128(data, len, &hash1, &hash2);
    
    elems[0] = Int64GetDatum((int64_t)hash1);
    elems[1] = Int64GetDatum((int64_t)hash2);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* SpookyHash128 for bytea input with custom seeds */
PG_FUNCTION_INFO_V1(spookyhash128_bytea_seed);

Datum
spookyhash128_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t seed1 = PG_GETARG_INT64(1);
    int64_t seed2 = PG_GETARG_INT64(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash1 = (uint64_t)seed1, hash2 = (uint64_t)seed2;
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    spookyhash_128(data, len, &hash1, &hash2);
    
    elems[0] = Int64GetDatum((int64_t)hash1);
    elems[1] = Int64GetDatum((int64_t)hash2);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* SpookyHash128 for integer input with default seed */
PG_FUNCTION_INFO_V1(spookyhash128_int);

Datum
spookyhash128_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash1 = 0, hash2 = 0;
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    spookyhash_128(&input, sizeof(int32_t), &hash1, &hash2);
    
    elems[0] = Int64GetDatum((int64_t)hash1);
    elems[1] = Int64GetDatum((int64_t)hash2);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}

/* SpookyHash128 for integer input with custom seeds */
PG_FUNCTION_INFO_V1(spookyhash128_int_seed);

Datum
spookyhash128_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t seed1 = PG_GETARG_INT64(1);
    int64_t seed2 = PG_GETARG_INT64(2);
    uint64_t hash1 = (uint64_t)seed1, hash2 = (uint64_t)seed2;
    
    Datum elems[2];
    ArrayType *result;
    int dims[1];
    int lbs[1];
    
    spookyhash_128(&input, sizeof(int32_t), &hash1, &hash2);
    
    elems[0] = Int64GetDatum((int64_t)hash1);
    elems[1] = Int64GetDatum((int64_t)hash2);
    
    dims[0] = 2;
    lbs[0] = 1;
    
    result = construct_md_array(elems, NULL, 1, dims, lbs,
                               INT8OID, 8, true, 'd');
    
    PG_RETURN_ARRAYTYPE_P(result);
}