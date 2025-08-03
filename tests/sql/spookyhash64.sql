-- Test basic hash functionality with text
SELECT spookyhash64('hello world');

-- Test hash with different text inputs
SELECT spookyhash64('test string');
SELECT spookyhash64('another test');

-- Test text input with custom seed
SELECT spookyhash64('hello world', 42);
SELECT spookyhash64('hello world', 123);

-- Test bytea input
SELECT spookyhash64('hello world'::bytea);

-- Test bytea input with custom seed
SELECT spookyhash64('hello world'::bytea, 42);

-- Test integer input
SELECT spookyhash64(12345);
SELECT spookyhash64(-12345);

-- Test integer input with custom seed
SELECT spookyhash64(12345, 42);
SELECT spookyhash64(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT spookyhash64('consistent test') = spookyhash64('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT spookyhash64('seed test', 1) != spookyhash64('seed test', 2);

-- Test empty string
SELECT spookyhash64('');

-- Test single character
SELECT spookyhash64('a');

-- Test long string
SELECT spookyhash64('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'spookyhash64'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';