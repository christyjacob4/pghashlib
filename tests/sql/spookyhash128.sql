-- Test basic hash functionality with text
SELECT spookyhash128('hello world');

-- Test hash with different text inputs
SELECT spookyhash128('test string');
SELECT spookyhash128('another test');

-- Test text input with custom seeds (two 64-bit values)
SELECT spookyhash128('hello world', 42, 84);
SELECT spookyhash128('hello world', 123, 456);

-- Test bytea input
SELECT spookyhash128('hello world'::bytea);

-- Test bytea input with custom seeds
SELECT spookyhash128('hello world'::bytea, 42, 84);

-- Test integer input
SELECT spookyhash128(12345);
SELECT spookyhash128(-12345);

-- Test integer input with custom seeds
SELECT spookyhash128(12345, 42, 84);
SELECT spookyhash128(-12345, 123, 456);

-- Test consistency (same input should give same hash)
SELECT spookyhash128('consistent test') = spookyhash128('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT spookyhash128('seed test', 1, 2) != spookyhash128('seed test', 3, 4);

-- Test empty string
SELECT spookyhash128('');

-- Test single character
SELECT spookyhash128('a');

-- Test long string
SELECT spookyhash128('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test accessing individual parts of the 128-bit hash
SELECT 
    (spookyhash128('hello world'))[1] AS low_64_bits,
    (spookyhash128('hello world'))[2] AS high_64_bits;

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'spookyhash128'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';