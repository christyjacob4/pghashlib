-- Test basic hash functionality with text
SELECT xxhash64('hello world');

-- Test hash with different text inputs
SELECT xxhash64('test string');
SELECT xxhash64('another test');

-- Test text input with custom seed
SELECT xxhash64('hello world', 42);
SELECT xxhash64('hello world', 123);

-- Test bytea input
SELECT xxhash64('hello world'::bytea);

-- Test bytea input with custom seed
SELECT xxhash64('hello world'::bytea, 42);

-- Test integer input
SELECT xxhash64(12345);
SELECT xxhash64(-12345);

-- Test integer input with custom seed
SELECT xxhash64(12345, 42);
SELECT xxhash64(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT xxhash64('consistent test') = xxhash64('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT xxhash64('seed test', 1) != xxhash64('seed test', 2);

-- Test empty string
SELECT xxhash64('');

-- Test single character
SELECT xxhash64('a');

-- Test long string
SELECT xxhash64('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'xxhash64'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';