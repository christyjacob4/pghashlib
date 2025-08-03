#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "utils/array.h"
#include "catalog/pg_type.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

/* HighwayHash constants and state */
#define HH_LANES 4
#define HH_KEY_BYTES 32

/* HighwayHash initialization constants */
static const uint64_t HH_INIT0[HH_LANES] = { 0xdbe6d5d5fe4cce2fULL, 0xa4093822299f31d0ULL, 0x13198a2e03707344ULL, 0x243f6a8885a308d3ULL };
static const uint64_t HH_INIT1[HH_LANES] = { 0x3bd39e10cb0ef593ULL, 0xc0acf169b5f18a8cULL, 0xbe5466cf34e90c6cULL, 0x452821e638d01377ULL };

/* HighwayHash state structure */
typedef struct {
    uint64_t v0[HH_LANES];
    uint64_t v1[HH_LANES];
    uint64_t mul0[HH_LANES];
    uint64_t mul1[HH_LANES];
} hh_state;

/* Utility functions */
static uint64_t hh_rotl64(uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}

static uint64_t hh_fetch64(const char *p)
{
    uint64_t result;
    memcpy(&result, p, sizeof(result));
    return result;
}


/* ZipperMerge: Computes (a + b) % 2^64 and (a + b) >> 32 */
static void hh_zipper_merge(uint64_t v1, uint64_t v0, uint64_t *result)
{
    result[0] = (v0 + v1) & 0xFFFFFFFFULL;
    result[1] = (v0 + v1) >> 32;
}

/* HighwayHash Update */
static void hh_update(const uint64_t lanes[HH_LANES], hh_state *state)
{
    int i;
    for (i = 0; i < HH_LANES; ++i) {
        state->v1[i] += state->mul0[i] + lanes[i];
        state->mul0[i] ^= (state->v1[i] & 0xFFFFFFFFULL) * (state->v0[i] >> 32);
        state->v0[i] += state->mul1[i];
        state->mul1[i] ^= (state->v0[i] & 0xFFFFFFFFULL) * (state->v1[i] >> 32);
    }

    /* ZipperMerge and update v0/v1 */
    {
        uint64_t result0[2], result1[2];
        hh_zipper_merge(state->v1[1], state->v1[0], result0);
        hh_zipper_merge(state->v1[3], state->v1[2], result1);
        state->v0[0] = result0[0];
        state->v0[1] = result0[1];
        state->v0[2] = result1[0];
        state->v0[3] = result1[1];

        hh_zipper_merge(state->v0[1], state->v0[0], result0);
        hh_zipper_merge(state->v0[3], state->v0[2], result1);
        state->v1[0] = result0[0];
        state->v1[1] = result0[1];
        state->v1[2] = result1[0];
        state->v1[3] = result1[1];
    }
}

/* HighwayHash PermuteAndUpdate */
static void hh_permute_and_update(hh_state *state)
{
    uint64_t lanes[HH_LANES];
    lanes[0] = hh_rotl64(state->v0[0], 20) ^ state->v1[0];
    lanes[1] = hh_rotl64(state->v0[1], 21) ^ state->v1[1];
    lanes[2] = hh_rotl64(state->v0[2], 22) ^ state->v1[2];
    lanes[3] = hh_rotl64(state->v0[3], 23) ^ state->v1[3];
    hh_update(lanes, state);
}

/* Initialize HighwayHash state with key */
static void hh_reset(const uint64_t key[4], hh_state *state)
{
    int i;
    for (i = 0; i < HH_LANES; ++i) {
        state->mul0[i] = HH_INIT0[i] ^ key[i % 4];
        state->mul1[i] = HH_INIT1[i] ^ key[i % 4];
        state->v0[i] = state->mul0[i];
        state->v1[i] = state->mul1[i];
    }
}

/* Process remaining bytes (less than 32) */
static void hh_update_remainder(const char *bytes, size_t size_mod32, hh_state *state)
{
    char packet[32];
    uint64_t lanes[HH_LANES];
    int i;
    
    /* Clear packet and copy remaining bytes */
    memset(packet, 0, 32);
    memcpy(packet, bytes, size_mod32);
    
    /* Set the size in the last byte */
    packet[31] = (char)size_mod32;

    /* Convert to lanes */
    for (i = 0; i < HH_LANES; ++i) {
        lanes[i] = hh_fetch64(packet + i * 8);
    }
    
    hh_update(lanes, state);
}

/* HighwayHash main computation */
static void hh_highway_hash(const uint64_t key[4], const char *bytes, size_t size, hh_state *state)
{
    size_t remainder = size & 31;
    size_t truncated_size = size - remainder;
    size_t i;
    uint64_t lanes[HH_LANES];
    int j;

    hh_reset(key, state);

    /* Process 32-byte chunks */
    for (i = 0; i < truncated_size; i += 32) {
        for (j = 0; j < HH_LANES; ++j) {
            lanes[j] = hh_fetch64(bytes + i + j * 8);
        }
        hh_update(lanes, state);
    }

    /* Process remainder */
    if (remainder != 0) {
        hh_update_remainder(bytes + truncated_size, remainder, state);
    }
}

/* Finalize 64-bit hash */
static uint64_t hh_finalize64(hh_state *state)
{
    int i;
    for (i = 0; i < 4; ++i) {
        hh_permute_and_update(state);
    }
    return state->v0[0] + state->v1[0] + state->mul0[0] + state->mul1[0];
}

/* Finalize 128-bit hash */
static void hh_finalize128(hh_state *state, uint64_t hash[2])
{
    int i;
    for (i = 0; i < 6; ++i) {
        hh_permute_and_update(state);
    }
    hash[0] = state->v0[0] + state->mul0[0] + state->v1[2] + state->mul1[2];
    hash[1] = state->v0[1] + state->mul0[1] + state->v1[3] + state->mul1[3];
}

/* Finalize 256-bit hash */
static void hh_finalize256(hh_state *state, uint64_t hash[4])
{
    int i;
    for (i = 0; i < 10; ++i) {
        hh_permute_and_update(state);
    }
    
    /* Modular reduction from 1024 to 256 bits */
    for (i = 0; i < HH_LANES; ++i) {
        hash[i] = state->v0[i] + state->v1[i] + state->mul0[i] + state->mul1[i];
    }
}

/* Default key for functions without explicit key */
static const uint64_t HH_DEFAULT_KEY[4] = {
    0x0706050403020100ULL, 0x0F0E0D0C0B0A0908ULL,
    0x1716151413121110ULL, 0x1F1E1D1C1B1A1918ULL
};

/* PostgreSQL function wrappers for HighwayHash64 */

/* HighwayHash64 for text input with default key */
PG_FUNCTION_INFO_V1(highwayhash64_text);

Datum
highwayhash64_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    hh_state state;
    uint64_t hash;
    
    hh_highway_hash(HH_DEFAULT_KEY, data, len, &state);
    hash = hh_finalize64(&state);
    PG_RETURN_INT64((int64_t)hash);
}

/* HighwayHash64 for text input with custom key (4 bigint values) */
PG_FUNCTION_INFO_V1(highwayhash64_text_key);

Datum
highwayhash64_text_key(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t key[4];
    hh_state state;
    uint64_t hash;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, data, len, &state);
    hash = hh_finalize64(&state);
    PG_RETURN_INT64((int64_t)hash);
}

/* HighwayHash64 for bytea input with default key */
PG_FUNCTION_INFO_V1(highwayhash64_bytea);

Datum
highwayhash64_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    hh_state state;
    uint64_t hash;
    
    hh_highway_hash(HH_DEFAULT_KEY, data, len, &state);
    hash = hh_finalize64(&state);
    PG_RETURN_INT64((int64_t)hash);
}

/* HighwayHash64 for bytea input with custom key */
PG_FUNCTION_INFO_V1(highwayhash64_bytea_key);

Datum
highwayhash64_bytea_key(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t key[4];
    hh_state state;
    uint64_t hash;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, data, len, &state);
    hash = hh_finalize64(&state);
    PG_RETURN_INT64((int64_t)hash);
}

/* HighwayHash64 for integer input with default key */
PG_FUNCTION_INFO_V1(highwayhash64_int);

Datum
highwayhash64_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    hh_state state;
    uint64_t hash;
    
    hh_highway_hash(HH_DEFAULT_KEY, (char*)&input, sizeof(int32_t), &state);
    hash = hh_finalize64(&state);
    PG_RETURN_INT64((int64_t)hash);
}

/* HighwayHash64 for integer input with custom key */
PG_FUNCTION_INFO_V1(highwayhash64_int_key);

Datum
highwayhash64_int_key(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    uint64_t key[4];
    hh_state state;
    uint64_t hash;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, (char*)&input, sizeof(int32_t), &state);
    hash = hh_finalize64(&state);
    PG_RETURN_INT64((int64_t)hash);
}

/* PostgreSQL function wrappers for HighwayHash128 */

/* HighwayHash128 for text input with default key - returns array of two bigints */
PG_FUNCTION_INFO_V1(highwayhash128_text);

Datum
highwayhash128_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    hh_state state;
    uint64_t hash[2];
    Datum result[2];
    ArrayType *array;
    
    hh_highway_hash(HH_DEFAULT_KEY, data, len, &state);
    hh_finalize128(&state, hash);
    
    result[0] = Int64GetDatum((int64_t)hash[0]);
    result[1] = Int64GetDatum((int64_t)hash[1]);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash128 for text input with custom key */
PG_FUNCTION_INFO_V1(highwayhash128_text_key);

Datum
highwayhash128_text_key(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t key[4];
    hh_state state;
    uint64_t hash[2];
    Datum result[2];
    ArrayType *array;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, data, len, &state);
    hh_finalize128(&state, hash);
    
    result[0] = Int64GetDatum((int64_t)hash[0]);
    result[1] = Int64GetDatum((int64_t)hash[1]);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash128 for bytea input with default key */
PG_FUNCTION_INFO_V1(highwayhash128_bytea);

Datum
highwayhash128_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    hh_state state;
    uint64_t hash[2];
    Datum result[2];
    ArrayType *array;
    
    hh_highway_hash(HH_DEFAULT_KEY, data, len, &state);
    hh_finalize128(&state, hash);
    
    result[0] = Int64GetDatum((int64_t)hash[0]);
    result[1] = Int64GetDatum((int64_t)hash[1]);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash128 for bytea input with custom key */
PG_FUNCTION_INFO_V1(highwayhash128_bytea_key);

Datum
highwayhash128_bytea_key(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t key[4];
    hh_state state;
    uint64_t hash[2];
    Datum result[2];
    ArrayType *array;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, data, len, &state);
    hh_finalize128(&state, hash);
    
    result[0] = Int64GetDatum((int64_t)hash[0]);
    result[1] = Int64GetDatum((int64_t)hash[1]);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash128 for integer input with default key */
PG_FUNCTION_INFO_V1(highwayhash128_int);

Datum
highwayhash128_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    hh_state state;
    uint64_t hash[2];
    Datum result[2];
    ArrayType *array;
    
    hh_highway_hash(HH_DEFAULT_KEY, (char*)&input, sizeof(int32_t), &state);
    hh_finalize128(&state, hash);
    
    result[0] = Int64GetDatum((int64_t)hash[0]);
    result[1] = Int64GetDatum((int64_t)hash[1]);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash128 for integer input with custom key */
PG_FUNCTION_INFO_V1(highwayhash128_int_key);

Datum
highwayhash128_int_key(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    uint64_t key[4];
    hh_state state;
    uint64_t hash[2];
    Datum result[2];
    ArrayType *array;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, (char*)&input, sizeof(int32_t), &state);
    hh_finalize128(&state, hash);
    
    result[0] = Int64GetDatum((int64_t)hash[0]);
    result[1] = Int64GetDatum((int64_t)hash[1]);
    
    array = construct_array(result, 2, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* PostgreSQL function wrappers for HighwayHash256 */

/* HighwayHash256 for text input with default key - returns array of four bigints */
PG_FUNCTION_INFO_V1(highwayhash256_text);

Datum
highwayhash256_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    hh_state state;
    uint64_t hash[4];
    Datum result[4];
    ArrayType *array;
    int i;
    
    hh_highway_hash(HH_DEFAULT_KEY, data, len, &state);
    hh_finalize256(&state, hash);
    
    for (i = 0; i < 4; ++i) {
        result[i] = Int64GetDatum((int64_t)hash[i]);
    }
    
    array = construct_array(result, 4, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash256 for text input with custom key */
PG_FUNCTION_INFO_V1(highwayhash256_text_key);

Datum
highwayhash256_text_key(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t key[4];
    hh_state state;
    uint64_t hash[4];
    Datum result[4];
    ArrayType *array;
    int i;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, data, len, &state);
    hh_finalize256(&state, hash);
    
    for (i = 0; i < 4; ++i) {
        result[i] = Int64GetDatum((int64_t)hash[i]);
    }
    
    array = construct_array(result, 4, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash256 for bytea input with default key */
PG_FUNCTION_INFO_V1(highwayhash256_bytea);

Datum
highwayhash256_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    hh_state state;
    uint64_t hash[4];
    Datum result[4];
    ArrayType *array;
    int i;
    
    hh_highway_hash(HH_DEFAULT_KEY, data, len, &state);
    hh_finalize256(&state, hash);
    
    for (i = 0; i < 4; ++i) {
        result[i] = Int64GetDatum((int64_t)hash[i]);
    }
    
    array = construct_array(result, 4, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash256 for bytea input with custom key */
PG_FUNCTION_INFO_V1(highwayhash256_bytea_key);

Datum
highwayhash256_bytea_key(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint64_t key[4];
    hh_state state;
    uint64_t hash[4];
    Datum result[4];
    ArrayType *array;
    int i;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, data, len, &state);
    hh_finalize256(&state, hash);
    
    for (i = 0; i < 4; ++i) {
        result[i] = Int64GetDatum((int64_t)hash[i]);
    }
    
    array = construct_array(result, 4, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash256 for integer input with default key */
PG_FUNCTION_INFO_V1(highwayhash256_int);

Datum
highwayhash256_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    hh_state state;
    uint64_t hash[4];
    Datum result[4];
    ArrayType *array;
    int i;
    
    hh_highway_hash(HH_DEFAULT_KEY, (char*)&input, sizeof(int32_t), &state);
    hh_finalize256(&state, hash);
    
    for (i = 0; i < 4; ++i) {
        result[i] = Int64GetDatum((int64_t)hash[i]);
    }
    
    array = construct_array(result, 4, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}

/* HighwayHash256 for integer input with custom key */
PG_FUNCTION_INFO_V1(highwayhash256_int_key);

Datum
highwayhash256_int_key(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int64_t key0 = PG_GETARG_INT64(1);
    int64_t key1 = PG_GETARG_INT64(2);
    int64_t key2 = PG_GETARG_INT64(3);
    int64_t key3 = PG_GETARG_INT64(4);
    uint64_t key[4];
    hh_state state;
    uint64_t hash[4];
    Datum result[4];
    ArrayType *array;
    int i;
    
    key[0] = (uint64_t)key0;
    key[1] = (uint64_t)key1;
    key[2] = (uint64_t)key2;
    key[3] = (uint64_t)key3;
    
    hh_highway_hash(key, (char*)&input, sizeof(int32_t), &state);
    hh_finalize256(&state, hash);
    
    for (i = 0; i < 4; ++i) {
        result[i] = Int64GetDatum((int64_t)hash[i]);
    }
    
    array = construct_array(result, 4, INT8OID, 8, true, 'd');
    PG_RETURN_ARRAYTYPE_P(array);
}