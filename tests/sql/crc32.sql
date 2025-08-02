-- Test basic hash functionality with text
SELECT crc32('hello world');

-- Test hash with different text inputs
SELECT crc32('test string');
SELECT crc32('another test');

-- Test text input with custom seed
SELECT crc32('hello world', 42);
SELECT crc32('hello world', 123);

-- Test bytea input
SELECT crc32('hello world'::bytea);

-- Test bytea input with custom seed
SELECT crc32('hello world'::bytea, 42);

-- Test integer input
SELECT crc32(12345);
SELECT crc32(-12345);

-- Test integer input with custom seed
SELECT crc32(12345, 42);
SELECT crc32(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT crc32('consistent test') = crc32('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT crc32('seed test', 1) != crc32('seed test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'crc32'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';