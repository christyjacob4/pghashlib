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