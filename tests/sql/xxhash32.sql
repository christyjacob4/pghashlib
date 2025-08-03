-- Test basic hash functionality with text
SELECT xxhash32('hello world');

-- Test hash with different text inputs
SELECT xxhash32('test string');
SELECT xxhash32('another test');

-- Test text input with custom seed
SELECT xxhash32('hello world', 42);
SELECT xxhash32('hello world', 123);

-- Test bytea input
SELECT xxhash32('hello world'::bytea);

-- Test bytea input with custom seed
SELECT xxhash32('hello world'::bytea, 42);

-- Test integer input
SELECT xxhash32(12345);
SELECT xxhash32(-12345);

-- Test integer input with custom seed
SELECT xxhash32(12345, 42);
SELECT xxhash32(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT xxhash32('consistent test') = xxhash32('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT xxhash32('seed test', 1) != xxhash32('seed test', 2);

-- Test empty string
SELECT xxhash32('');

-- Test single character
SELECT xxhash32('a');

-- Test long string
SELECT xxhash32('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'xxhash32'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';