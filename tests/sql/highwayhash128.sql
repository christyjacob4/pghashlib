-- Test basic hash functionality with text
SELECT highwayhash128('hello world');

-- Test hash with different text inputs
SELECT highwayhash128('test string');
SELECT highwayhash128('another test');

-- Test text input with custom key (4 bigint values)
SELECT highwayhash128('hello world', 1, 2, 3, 4);
SELECT highwayhash128('hello world', 42, 84, 168, 336);

-- Test bytea input
SELECT highwayhash128('hello world'::bytea);

-- Test bytea input with custom key
SELECT highwayhash128('hello world'::bytea, 1, 2, 3, 4);

-- Test integer input
SELECT highwayhash128(12345);
SELECT highwayhash128(-12345);

-- Test integer input with custom key
SELECT highwayhash128(12345, 1, 2, 3, 4);
SELECT highwayhash128(-12345, 42, 84, 168, 336);

-- Test consistency (same input should give same hash)
SELECT highwayhash128('consistent test') = highwayhash128('consistent test');

-- Test key effect (same input, different keys should give different hashes)
SELECT highwayhash128('key test', 1, 2, 3, 4) != highwayhash128('key test', 5, 6, 7, 8);

-- Test empty string
SELECT highwayhash128('');

-- Test single character
SELECT highwayhash128('a');

-- Test long string
SELECT highwayhash128('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test array access (128-bit hash returns array of two bigints)
SELECT 
    (highwayhash128('hello world'))[1] AS first_64_bits,
    (highwayhash128('hello world'))[2] AS second_64_bits;

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'highwayhash128'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';