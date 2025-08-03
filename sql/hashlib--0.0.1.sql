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

-- lookup3le function for text (default initval = 0)
CREATE OR REPLACE FUNCTION lookup3le(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3le_text'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3le function for text with custom initval
CREATE OR REPLACE FUNCTION lookup3le(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3le_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3le function for bytea (default initval = 0)
CREATE OR REPLACE FUNCTION lookup3le(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3le_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3le function for bytea with custom initval
CREATE OR REPLACE FUNCTION lookup3le(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3le_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3le function for integer (default initval = 0)
CREATE OR REPLACE FUNCTION lookup3le(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3le_int'
LANGUAGE C IMMUTABLE STRICT;

-- lookup3le function for integer with custom initval
CREATE OR REPLACE FUNCTION lookup3le(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'lookup3le_int_seed'
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

-- SipHash24 function for text (default key)
CREATE OR REPLACE FUNCTION siphash24(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'siphash24_text'
LANGUAGE C IMMUTABLE STRICT;

-- SipHash24 function for text with custom seeds (two 32-bit values for 128-bit key)
CREATE OR REPLACE FUNCTION siphash24(text, integer, integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'siphash24_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SipHash24 function for bytea (default key)
CREATE OR REPLACE FUNCTION siphash24(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'siphash24_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- SipHash24 function for bytea with custom seeds (two 32-bit values for 128-bit key)
CREATE OR REPLACE FUNCTION siphash24(bytea, integer, integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'siphash24_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SipHash24 function for integer (default key)
CREATE OR REPLACE FUNCTION siphash24(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'siphash24_int'
LANGUAGE C IMMUTABLE STRICT;

-- SipHash24 function for integer with custom seeds (two 32-bit values for 128-bit key)
CREATE OR REPLACE FUNCTION siphash24(integer, integer, integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'siphash24_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash64 function for text (default seed)
CREATE OR REPLACE FUNCTION spookyhash64(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'spookyhash64_text'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash64 function for text with custom seed
CREATE OR REPLACE FUNCTION spookyhash64(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'spookyhash64_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash64 function for bytea (default seed)
CREATE OR REPLACE FUNCTION spookyhash64(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'spookyhash64_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash64 function for bytea with custom seed
CREATE OR REPLACE FUNCTION spookyhash64(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'spookyhash64_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash64 function for integer (default seed)
CREATE OR REPLACE FUNCTION spookyhash64(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'spookyhash64_int'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash64 function for integer with custom seed
CREATE OR REPLACE FUNCTION spookyhash64(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'spookyhash64_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash128 function for text (default seed) - returns array of two bigints
CREATE OR REPLACE FUNCTION spookyhash128(text)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'spookyhash128_text'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash128 function for text with custom seeds - returns array of two bigints
CREATE OR REPLACE FUNCTION spookyhash128(text, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'spookyhash128_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash128 function for bytea (default seed) - returns array of two bigints
CREATE OR REPLACE FUNCTION spookyhash128(bytea)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'spookyhash128_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash128 function for bytea with custom seeds - returns array of two bigints
CREATE OR REPLACE FUNCTION spookyhash128(bytea, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'spookyhash128_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash128 function for integer (default seed) - returns array of two bigints
CREATE OR REPLACE FUNCTION spookyhash128(integer)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'spookyhash128_int'
LANGUAGE C IMMUTABLE STRICT;

-- SpookyHash128 function for integer with custom seeds - returns array of two bigints
CREATE OR REPLACE FUNCTION spookyhash128(integer, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'spookyhash128_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash32 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION xxhash32(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'xxhash32_text'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash32 function for text with custom seed
CREATE OR REPLACE FUNCTION xxhash32(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'xxhash32_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash32 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION xxhash32(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'xxhash32_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash32 function for bytea with custom seed
CREATE OR REPLACE FUNCTION xxhash32(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'xxhash32_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash32 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION xxhash32(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'xxhash32_int'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash32 function for integer with custom seed
CREATE OR REPLACE FUNCTION xxhash32(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'xxhash32_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash64 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION xxhash64(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'xxhash64_text'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash64 function for text with custom seed
CREATE OR REPLACE FUNCTION xxhash64(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'xxhash64_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash64 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION xxhash64(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'xxhash64_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash64 function for bytea with custom seed
CREATE OR REPLACE FUNCTION xxhash64(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'xxhash64_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash64 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION xxhash64(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'xxhash64_int'
LANGUAGE C IMMUTABLE STRICT;

-- xxHash64 function for integer with custom seed
CREATE OR REPLACE FUNCTION xxhash64(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'xxhash64_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash32 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION farmhash32(text)
RETURNS integer
AS 'MODULE_PATHNAME', 'farmhash32_text'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash32 function for text with custom seed
CREATE OR REPLACE FUNCTION farmhash32(text, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'farmhash32_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash32 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION farmhash32(bytea)
RETURNS integer
AS 'MODULE_PATHNAME', 'farmhash32_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash32 function for bytea with custom seed
CREATE OR REPLACE FUNCTION farmhash32(bytea, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'farmhash32_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash32 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION farmhash32(integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'farmhash32_int'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash32 function for integer with custom seed
CREATE OR REPLACE FUNCTION farmhash32(integer, integer)
RETURNS integer
AS 'MODULE_PATHNAME', 'farmhash32_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION farmhash64(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_text'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for text with custom seed
CREATE OR REPLACE FUNCTION farmhash64(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for text with two seeds
CREATE OR REPLACE FUNCTION farmhash64(text, bigint, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_text_seeds'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION farmhash64(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for bytea with custom seed
CREATE OR REPLACE FUNCTION farmhash64(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for bytea with two seeds
CREATE OR REPLACE FUNCTION farmhash64(bytea, bigint, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_bytea_seeds'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION farmhash64(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_int'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for integer with custom seed
CREATE OR REPLACE FUNCTION farmhash64(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- FarmHash64 function for integer with two seeds
CREATE OR REPLACE FUNCTION farmhash64(integer, bigint, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'farmhash64_int_seeds'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash64 function for text (default key)
CREATE OR REPLACE FUNCTION highwayhash64(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'highwayhash64_text'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash64 function for text with custom key (4 bigint values)
CREATE OR REPLACE FUNCTION highwayhash64(text, bigint, bigint, bigint, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'highwayhash64_text_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash64 function for bytea (default key)
CREATE OR REPLACE FUNCTION highwayhash64(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'highwayhash64_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash64 function for bytea with custom key (4 bigint values)
CREATE OR REPLACE FUNCTION highwayhash64(bytea, bigint, bigint, bigint, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'highwayhash64_bytea_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash64 function for integer (default key)
CREATE OR REPLACE FUNCTION highwayhash64(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'highwayhash64_int'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash64 function for integer with custom key (4 bigint values)
CREATE OR REPLACE FUNCTION highwayhash64(integer, bigint, bigint, bigint, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'highwayhash64_int_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash128 function for text (default key) - returns array of two bigints
CREATE OR REPLACE FUNCTION highwayhash128(text)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash128_text'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash128 function for text with custom key (4 bigint values) - returns array of two bigints
CREATE OR REPLACE FUNCTION highwayhash128(text, bigint, bigint, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash128_text_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash128 function for bytea (default key) - returns array of two bigints
CREATE OR REPLACE FUNCTION highwayhash128(bytea)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash128_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash128 function for bytea with custom key (4 bigint values) - returns array of two bigints
CREATE OR REPLACE FUNCTION highwayhash128(bytea, bigint, bigint, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash128_bytea_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash128 function for integer (default key) - returns array of two bigints
CREATE OR REPLACE FUNCTION highwayhash128(integer)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash128_int'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash128 function for integer with custom key (4 bigint values) - returns array of two bigints
CREATE OR REPLACE FUNCTION highwayhash128(integer, bigint, bigint, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash128_int_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash256 function for text (default key) - returns array of four bigints
CREATE OR REPLACE FUNCTION highwayhash256(text)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash256_text'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash256 function for text with custom key (4 bigint values) - returns array of four bigints
CREATE OR REPLACE FUNCTION highwayhash256(text, bigint, bigint, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash256_text_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash256 function for bytea (default key) - returns array of four bigints
CREATE OR REPLACE FUNCTION highwayhash256(bytea)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash256_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash256 function for bytea with custom key (4 bigint values) - returns array of four bigints
CREATE OR REPLACE FUNCTION highwayhash256(bytea, bigint, bigint, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash256_bytea_key'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash256 function for integer (default key) - returns array of four bigints
CREATE OR REPLACE FUNCTION highwayhash256(integer)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash256_int'
LANGUAGE C IMMUTABLE STRICT;

-- HighwayHash256 function for integer with custom key (4 bigint values) - returns array of four bigints
CREATE OR REPLACE FUNCTION highwayhash256(integer, bigint, bigint, bigint, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'highwayhash256_int_key'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash64 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION metrohash64(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 'metrohash64_text'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash64 function for text with custom seed
CREATE OR REPLACE FUNCTION metrohash64(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'metrohash64_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash64 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION metrohash64(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 'metrohash64_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash64 function for bytea with custom seed
CREATE OR REPLACE FUNCTION metrohash64(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'metrohash64_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash64 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION metrohash64(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 'metrohash64_int'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash64 function for integer with custom seed
CREATE OR REPLACE FUNCTION metrohash64(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 'metrohash64_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash128 function for text (default seed = 0) - returns array of two bigints
CREATE OR REPLACE FUNCTION metrohash128(text)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'metrohash128_text'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash128 function for text with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION metrohash128(text, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'metrohash128_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash128 function for bytea (default seed = 0) - returns array of two bigints
CREATE OR REPLACE FUNCTION metrohash128(bytea)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'metrohash128_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash128 function for bytea with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION metrohash128(bytea, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'metrohash128_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash128 function for integer (default seed = 0) - returns array of two bigints
CREATE OR REPLACE FUNCTION metrohash128(integer)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'metrohash128_int'
LANGUAGE C IMMUTABLE STRICT;

-- MetroHash128 function for integer with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION metrohash128(integer, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 'metrohash128_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha0 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha0(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha0_text'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha0 function for text with custom seed
CREATE OR REPLACE FUNCTION t1ha0(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha0_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha0 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha0(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha0_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha0 function for bytea with custom seed
CREATE OR REPLACE FUNCTION t1ha0(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha0_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha0 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha0(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha0_int'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha0 function for integer with custom seed
CREATE OR REPLACE FUNCTION t1ha0(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha0_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha1 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha1(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha1_text'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha1 function for text with custom seed
CREATE OR REPLACE FUNCTION t1ha1(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha1_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha1 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha1(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha1_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha1 function for bytea with custom seed
CREATE OR REPLACE FUNCTION t1ha1(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha1_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha1 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha1(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha1_int'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha1 function for integer with custom seed
CREATE OR REPLACE FUNCTION t1ha1(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha1_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2 function for text (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha2(text)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha2_text'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2 function for text with custom seed
CREATE OR REPLACE FUNCTION t1ha2(text, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha2_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2 function for bytea (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha2(bytea)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha2_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2 function for bytea with custom seed
CREATE OR REPLACE FUNCTION t1ha2(bytea, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha2_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2 function for integer (default seed = 0)
CREATE OR REPLACE FUNCTION t1ha2(integer)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha2_int'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2 function for integer with custom seed
CREATE OR REPLACE FUNCTION t1ha2(integer, bigint)
RETURNS bigint
AS 'MODULE_PATHNAME', 't1ha2_int_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2_128 function for text (default seed = 0) - returns array of two bigints
CREATE OR REPLACE FUNCTION t1ha2_128(text)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 't1ha2_128_text'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2_128 function for text with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION t1ha2_128(text, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 't1ha2_128_text_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2_128 function for bytea (default seed = 0) - returns array of two bigints
CREATE OR REPLACE FUNCTION t1ha2_128(bytea)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 't1ha2_128_bytea'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2_128 function for bytea with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION t1ha2_128(bytea, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 't1ha2_128_bytea_seed'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2_128 function for integer (default seed = 0) - returns array of two bigints
CREATE OR REPLACE FUNCTION t1ha2_128(integer)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 't1ha2_128_int'
LANGUAGE C IMMUTABLE STRICT;

-- t1ha2_128 function for integer with custom seed - returns array of two bigints
CREATE OR REPLACE FUNCTION t1ha2_128(integer, bigint)
RETURNS bigint[]
AS 'MODULE_PATHNAME', 't1ha2_128_int_seed'
LANGUAGE C IMMUTABLE STRICT;