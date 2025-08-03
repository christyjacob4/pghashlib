-- Test basic hash functionality with text
SELECT highwayhash256('hello world');

-- Test hash with different text inputs
SELECT highwayhash256('test string');
SELECT highwayhash256('another test');

-- Test text input with custom key (4 bigint values)
SELECT highwayhash256('hello world', 1, 2, 3, 4);
SELECT highwayhash256('hello world', 42, 84, 168, 336);

-- Test bytea input
SELECT highwayhash256('hello world'::bytea);

-- Test bytea input with custom key
SELECT highwayhash256('hello world'::bytea, 1, 2, 3, 4);

-- Test integer input
SELECT highwayhash256(12345);
SELECT highwayhash256(-12345);

-- Test integer input with custom key
SELECT highwayhash256(12345, 1, 2, 3, 4);
SELECT highwayhash256(-12345, 42, 84, 168, 336);

-- Test consistency (same input should give same hash)
SELECT highwayhash256('consistent test') = highwayhash256('consistent test');

-- Test key effect (same input, different keys should give different hashes)
SELECT highwayhash256('key test', 1, 2, 3, 4) != highwayhash256('key test', 5, 6, 7, 8);

-- Test empty string
SELECT highwayhash256('');

-- Test single character
SELECT highwayhash256('a');

-- Test long string
SELECT highwayhash256('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test array access (256-bit hash returns array of four bigints)
SELECT 
    (highwayhash256('hello world'))[1] AS first_64_bits,
    (highwayhash256('hello world'))[2] AS second_64_bits,
    (highwayhash256('hello world'))[3] AS third_64_bits,
    (highwayhash256('hello world'))[4] AS fourth_64_bits;

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'highwayhash256'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';