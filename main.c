#include "postgres.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

/* Forward declarations for MurmurHash3 functions */
extern Datum murmurhash3_32_text(PG_FUNCTION_ARGS);
extern Datum murmurhash3_32_text_seed(PG_FUNCTION_ARGS);
extern Datum murmurhash3_32_bytea(PG_FUNCTION_ARGS);
extern Datum murmurhash3_32_bytea_seed(PG_FUNCTION_ARGS);
extern Datum murmurhash3_32_int(PG_FUNCTION_ARGS);
extern Datum murmurhash3_32_int_seed(PG_FUNCTION_ARGS);

/* Forward declarations for CRC32 functions */
extern Datum crc32_text(PG_FUNCTION_ARGS);
extern Datum crc32_text_seed(PG_FUNCTION_ARGS);
extern Datum crc32_bytea(PG_FUNCTION_ARGS);
extern Datum crc32_bytea_seed(PG_FUNCTION_ARGS);
extern Datum crc32_int(PG_FUNCTION_ARGS);
extern Datum crc32_int_seed(PG_FUNCTION_ARGS);

/* Module initialization function */
void _PG_init(void);

void
_PG_init(void)
{
    /* Module initialization code can go here if needed */
    /* For now, we just ensure the module is properly loaded */
}