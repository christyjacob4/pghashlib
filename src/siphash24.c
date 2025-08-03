#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* SipHash-2-4 constants */
static const uint64_t SIP_K0_DEFAULT = 0x0706050403020100ULL;
static const uint64_t SIP_K1_DEFAULT = 0x0f0e0d0c0b0a0908ULL;

/* SipHash utility functions */
static uint64_t
U8TO64_LE(const unsigned char *p)
{
    return ((uint64_t)p[0]) |
           ((uint64_t)p[1] << 8) |
           ((uint64_t)p[2] << 16) |
           ((uint64_t)p[3] << 24) |
           ((uint64_t)p[4] << 32) |
           ((uint64_t)p[5] << 40) |
           ((uint64_t)p[6] << 48) |
           ((uint64_t)p[7] << 56);
}

static uint64_t
ROTL(uint64_t x, int b)
{
    return (x << b) | (x >> (64 - b));
}

static void
SIPROUND(uint64_t *v0, uint64_t *v1, uint64_t *v2, uint64_t *v3)
{
    *v0 += *v1;
    *v1 = ROTL(*v1, 13);
    *v1 ^= *v0;
    *v0 = ROTL(*v0, 32);
    *v2 += *v3;
    *v3 = ROTL(*v3, 16);
    *v3 ^= *v2;
    *v0 += *v3;
    *v3 = ROTL(*v3, 21);
    *v3 ^= *v0;
    *v2 += *v1;
    *v1 = ROTL(*v1, 17);
    *v1 ^= *v2;
    *v2 = ROTL(*v2, 32);
}

/* SipHash-2-4 main implementation */
static uint64_t
siphash24(const unsigned char *in, size_t inlen, uint64_t k0, uint64_t k1)
{
    uint64_t v0 = 0x736f6d6570736575ULL ^ k0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ k1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ k0;
    uint64_t v3 = 0x7465646279746573ULL ^ k1;
    uint64_t b;
    size_t left = inlen & 7;
    const unsigned char *end = in + inlen - left;
    
    b = ((uint64_t)inlen) << 56;
    
    /* Process 8-byte blocks */
    for (; in != end; in += 8) {
        uint64_t m = U8TO64_LE(in);
        v3 ^= m;
        
        /* SipRound x 2 (c=2) */
        SIPROUND(&v0, &v1, &v2, &v3);
        SIPROUND(&v0, &v1, &v2, &v3);
        
        v0 ^= m;
    }
    
    /* Process remaining bytes */
    switch (left) {
        case 7: b |= ((uint64_t)in[6]) << 48; /* fallthrough */
        case 6: b |= ((uint64_t)in[5]) << 40; /* fallthrough */
        case 5: b |= ((uint64_t)in[4]) << 32; /* fallthrough */
        case 4: b |= ((uint64_t)in[3]) << 24; /* fallthrough */
        case 3: b |= ((uint64_t)in[2]) << 16; /* fallthrough */
        case 2: b |= ((uint64_t)in[1]) << 8;  /* fallthrough */
        case 1: b |= ((uint64_t)in[0]);       /* fallthrough */
        case 0: break;
    }
    
    v3 ^= b;
    
    /* SipRound x 2 (c=2) */
    SIPROUND(&v0, &v1, &v2, &v3);
    SIPROUND(&v0, &v1, &v2, &v3);
    
    v0 ^= b;
    
    /* Finalization */
    v2 ^= 0xff;
    
    /* SipRound x 4 (d=4) */
    SIPROUND(&v0, &v1, &v2, &v3);
    SIPROUND(&v0, &v1, &v2, &v3);
    SIPROUND(&v0, &v1, &v2, &v3);
    SIPROUND(&v0, &v1, &v2, &v3);
    
    return v0 ^ v1 ^ v2 ^ v3;
}

/* Helper function to convert two 32-bit integers to a 128-bit key */
static void
derive_key_from_seeds(uint32_t seed1, uint32_t seed2, uint64_t *k0, uint64_t *k1)
{
    *k0 = SIP_K0_DEFAULT ^ ((uint64_t)seed1 | ((uint64_t)seed2 << 32));
    *k1 = SIP_K1_DEFAULT ^ ((uint64_t)seed2 | ((uint64_t)seed1 << 32));
}

/* SipHash24 for text input with default key */
PG_FUNCTION_INFO_V1(siphash24_text);

Datum
siphash24_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = siphash24((const unsigned char *)data, len, SIP_K0_DEFAULT, SIP_K1_DEFAULT);
    PG_RETURN_INT64((int64_t)hash);
}

/* SipHash24 for text input with custom seeds */
PG_FUNCTION_INFO_V1(siphash24_text_seed);

Datum
siphash24_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int32_t seed1 = PG_GETARG_INT32(1);
    int32_t seed2 = PG_GETARG_INT32(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t k0, k1;
    uint64_t hash;
    
    derive_key_from_seeds(seed1, seed2, &k0, &k1);
    hash = siphash24((const unsigned char *)data, len, k0, k1);
    PG_RETURN_INT64((int64_t)hash);
}

/* SipHash24 for bytea input with default key */
PG_FUNCTION_INFO_V1(siphash24_bytea);

Datum
siphash24_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t hash = siphash24((const unsigned char *)data, len, SIP_K0_DEFAULT, SIP_K1_DEFAULT);
    PG_RETURN_INT64((int64_t)hash);
}

/* SipHash24 for bytea input with custom seeds */
PG_FUNCTION_INFO_V1(siphash24_bytea_seed);

Datum
siphash24_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int32_t seed1 = PG_GETARG_INT32(1);
    int32_t seed2 = PG_GETARG_INT32(2);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t k0, k1;
    uint64_t hash;
    
    derive_key_from_seeds(seed1, seed2, &k0, &k1);
    hash = siphash24((const unsigned char *)data, len, k0, k1);
    PG_RETURN_INT64((int64_t)hash);
}

/* SipHash24 for integer input with default key */
PG_FUNCTION_INFO_V1(siphash24_int);

Datum
siphash24_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint64_t hash = siphash24((const unsigned char *)&input, sizeof(int32_t), SIP_K0_DEFAULT, SIP_K1_DEFAULT);
    PG_RETURN_INT64((int64_t)hash);
}

/* SipHash24 for integer input with custom seeds */
PG_FUNCTION_INFO_V1(siphash24_int_seed);

Datum
siphash24_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int32_t seed1 = PG_GETARG_INT32(1);
    int32_t seed2 = PG_GETARG_INT32(2);
    uint64_t k0, k1;
    uint64_t hash;
    
    derive_key_from_seeds(seed1, seed2, &k0, &k1);
    hash = siphash24((const unsigned char *)&input, sizeof(int32_t), k0, k1);
    PG_RETURN_INT64((int64_t)hash);
}