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

-- CityHash128 function for text (default seed) - returns array of two bigints
CREATE OR REPLACE FUNCTION cityhash128(text)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'cityhash128_text'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash128 function for text with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION cityhash128(text, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'cityhash128_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash128 function for bytea (default seed) - returns array of two bigints
CREATE OR REPLACE FUNCTION cityhash128(bytea)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'cityhash128_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash128 function for bytea with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION cityhash128(bytea, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'cityhash128_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash128 function for integer (default seed) - returns array of two bigints
CREATE OR REPLACE FUNCTION cityhash128(integer)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'cityhash128_int'
LANGUAGE C IMMUTABLE STRICT;

-- CityHash128 function for integer with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION cityhash128(integer, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'cityhash128_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup2 function for text (default initval = 0)
CREATE OR REPLACE FUNCTION lookup2(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup2_text'
LANGUAGE C IMMUTABLE STRICT;

-- lookup2 function for text with custom initval
CREATE OR REPLACE FUNCTION lookup2(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup2_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup2 function for bytea (default initval = 0)
CREATE OR REPLACE FUNCTION lookup2(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup2_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- lookup2 function for bytea with custom initval
CREATE OR REPLACE FUNCTION lookup2(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup2_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup2 function for integer (default initval = 0)
CREATE OR REPLACE FUNCTION lookup2(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup2_int'
LANGUAGE C IMMUTABLE STRICT;

-- lookup2 function for integer with custom initval
CREATE OR REPLACE FUNCTION lookup2(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup2_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3be function for text (default initval = 0)
CREATE OR REPLACE FUNCTION lookup3be(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3be_text'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3be function for text with custom initval
CREATE OR REPLACE FUNCTION lookup3be(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3be_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3be function for bytea (default initval = 0)
CREATE OR REPLACE FUNCTION lookup3be(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3be_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3be function for bytea with custom initval
CREATE OR REPLACE FUNCTION lookup3be(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3be_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3be function for integer (default initval = 0)
CREATE OR REPLACE FUNCTION lookup3be(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3be_int'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3be function for integer with custom initval
CREATE OR REPLACE FUNCTION lookup3be(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3be_int_seed'
LANGUAGE C IMMUTABLE STRICT;