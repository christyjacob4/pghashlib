#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"
#include "mb/pg_wchar.h"
#include "access/htup_details.h"

PG_MODULE_MAGIC;

/* MurmurHash3 32-bit implementation */
static uint32_t
murmurhash3_32(const void *key, size_t len, uint32_t seed)
{
    const uint8_t *data = (const uint8_t *)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t *blocks = (const uint32_t *)(data + nblocks * 4);
    int i;
    const uint8_t *tail;
    uint32_t k1 = 0;

    /* Body */
    for (i = -nblocks; i; i++) {
        uint32_t k1_block = blocks[i];
        k1_block *= c1;
        k1_block = (k1_block << 15) | (k1_block >> 17);
        k1_block *= c2;
        h1 ^= k1_block;
        h1 = (h1 << 13) | (h1 >> 19);
        h1 = h1 * 5 + 0xe6546b64;
    }

    /* Tail */
    tail = (const uint8_t *)(data + nblocks * 4);
    k1 = 0;
    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1;
                k1 = (k1 << 15) | (k1 >> 17);
                k1 *= c2;
                h1 ^= k1;
    }

    /* Finalization */
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

/* MurmurHash3 for text input with default seed (0) */
PG_FUNCTION_INFO_V1(murmurhash3_32_text);

Datum
murmurhash3_32_text(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = murmurhash3_32(data, len, 0);
    PG_RETURN_INT32((int32_t)hash);
}

/* MurmurHash3 for text input with custom seed */
PG_FUNCTION_INFO_V1(murmurhash3_32_text_seed);

Datum
murmurhash3_32_text_seed(PG_FUNCTION_ARGS)
{
    text *input = PG_GETARG_TEXT_PP(0);
    int32_t seed = PG_GETARG_INT32(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = murmurhash3_32(data, len, (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* MurmurHash3 for bytea input with default seed (0) */
PG_FUNCTION_INFO_V1(murmurhash3_32_bytea);

Datum
murmurhash3_32_bytea(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = murmurhash3_32(data, len, 0);
    PG_RETURN_INT32((int32_t)hash);
}

/* MurmurHash3 for bytea input with custom seed */
PG_FUNCTION_INFO_V1(murmurhash3_32_bytea_seed);

Datum
murmurhash3_32_bytea_seed(PG_FUNCTION_ARGS)
{
    bytea *input = PG_GETARG_BYTEA_PP(0);
    int32_t seed = PG_GETARG_INT32(1);
    char *data = VARDATA_ANY(input);
    int len = VARSIZE_ANY_EXHDR(input);
    uint32_t hash = murmurhash3_32(data, len, (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}

/* MurmurHash3 for integer input */
PG_FUNCTION_INFO_V1(murmurhash3_32_int);

Datum
murmurhash3_32_int(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    uint32_t hash = murmurhash3_32(&input, sizeof(int32_t), 0);
    PG_RETURN_INT32((int32_t)hash);
}

/* MurmurHash3 for integer input with custom seed */
PG_FUNCTION_INFO_V1(murmurhash3_32_int_seed);

Datum
murmurhash3_32_int_seed(PG_FUNCTION_ARGS)
{
    int32_t input = PG_GETARG_INT32(0);
    int32_t seed = PG_GETARG_INT32(1);
    uint32_t hash = murmurhash3_32(&input, sizeof(int32_t), (uint32_t)seed);
    PG_RETURN_INT32((int32_t)hash);
}