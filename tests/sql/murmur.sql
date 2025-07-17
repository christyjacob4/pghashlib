-- Test basic hash functionality with text
SELECT murmurhash3_32('hello world');

-- Test hash with different text inputs
SELECT murmurhash3_32('test string');
SELECT murmurhash3_32('another test');

-- Test text input with custom seed
SELECT murmurhash3_32('hello world', 42);
SELECT murmurhash3_32('hello world', 123);

-- Test bytea input
SELECT murmurhash3_32('hello world'::bytea);

-- Test bytea input with custom seed
SELECT murmurhash3_32('hello world'::bytea, 42);

-- Test integer input
SELECT murmurhash3_32(12345);
SELECT murmurhash3_32(-12345);

-- Test integer input with custom seed
SELECT murmurhash3_32(12345, 42);
SELECT murmurhash3_32(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT murmurhash3_32('consistent test') = murmurhash3_32('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT murmurhash3_32('seed test', 1) != murmurhash3_32('seed test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'murmurhash3_32'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';