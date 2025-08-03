-- Test basic hash functionality with text
SELECT t1ha2_128('hello world');

-- Test hash with different text inputs
SELECT t1ha2_128('test string');
SELECT t1ha2_128('another test');

-- Test text input with custom seed
SELECT t1ha2_128('hello world', 42);
SELECT t1ha2_128('hello world', 84);

-- Test bytea input
SELECT t1ha2_128('hello world'::bytea);

-- Test bytea input with custom seed
SELECT t1ha2_128('hello world'::bytea, 42);

-- Test integer input
SELECT t1ha2_128(12345);
SELECT t1ha2_128(-12345);

-- Test integer input with custom seed
SELECT t1ha2_128(12345, 42);
SELECT t1ha2_128(-12345, 84);

-- Test consistency (same input should give same hash)
SELECT t1ha2_128('consistent test') = t1ha2_128('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT t1ha2_128('seed test', 1) != t1ha2_128('seed test', 2);

-- Test empty string
SELECT t1ha2_128('');

-- Test single character
SELECT t1ha2_128('a');

-- Test long string
SELECT t1ha2_128('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test various input lengths to test different code paths
SELECT t1ha2_128('1234567');           -- 7 bytes
SELECT t1ha2_128('12345678');          -- 8 bytes  
SELECT t1ha2_128('123456789012345');   -- 15 bytes
SELECT t1ha2_128('1234567890123456');  -- 16 bytes
SELECT t1ha2_128('12345678901234567890123456789012'); -- 32 bytes
SELECT t1ha2_128('123456789012345678901234567890123'); -- 33 bytes

-- Test array access (128-bit hash returns array of two bigints)
SELECT 
    (t1ha2_128('hello world'))[1] AS first_64_bits,
    (t1ha2_128('hello world'))[2] AS second_64_bits;

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 't1ha2_128'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';