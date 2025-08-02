-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION hashlib" to load this file. \quit

-- MurmurHash3 32-bit function for text (default seed = 0)
CREATE OR REPLACE FUNCTION murmurhash3_32(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'murmurhash3_32_text'
LANGUAGE C IMMUTABLE STRICT;

-- MurmurHash3 32-bit function for text with custom seed
CREATE OR REPLACE FUNCTION murmurhash3_32(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'murmurhash3_32_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MurmurHash3 32-bit function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION murmurhash3_32(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'murmurhash3_32_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- MurmurHash3 32-bit function for bytea with custom seed
CREATE OR REPLACE FUNCTION murmurhash3_32(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'murmurhash3_32_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MurmurHash3 32-bit function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION murmurhash3_32(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'murmurhash3_32_int'
LANGUAGE C IMMUTABLE STRICT;

-- MurmurHash3 32-bit function for integer with custom seed
CREATE OR REPLACE FUNCTION murmurhash3_32(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'murmurhash3_32_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CRC32 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION crc32(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'crc32_text'
LANGUAGE C IMMUTABLE STRICT;

-- CRC32 function for text with custom seed
CREATE OR REPLACE FUNCTION crc32(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'crc32_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CRC32 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION crc32(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'crc32_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- CRC32 function for bytea with custom seed
CREATE OR REPLACE FUNCTION crc32(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'crc32_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CRC32 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION crc32(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'crc32_int'
LANGUAGE C IMMUTABLE STRICT;

-- CRC32 function for integer with custom seed
CREATE OR REPLACE FUNCTION crc32(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'crc32_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash64 function for text (default seed)
CREATE OR REPLACE FUNCTION cityhash64(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'cityhash64_text'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash64 function for text with custom seed
CREATE OR REPLACE FUNCTION cityhash64(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'cityhash64_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash64 function for bytea (default seed)
CREATE OR REPLACE FUNCTION cityhash64(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'cityhash64_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash64 function for bytea with custom seed
CREATE OR REPLACE FUNCTION cityhash64(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'cityhash64_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash64 function for integer (default seed)
CREATE OR REPLACE FUNCTION cityhash64(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'cityhash64_int'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash64 function for integer with custom seed
CREATE OR REPLACE FUNCTION cityhash64(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'cityhash64_int_seed'
LANGUAGE C IMMUTABLE STRICT;