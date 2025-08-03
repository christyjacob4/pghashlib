-- Test basic hash functionality with text
SELECT metrohash128('hello world');

-- Test hash with different text inputs
SELECT metrohash128('test string');
SELECT metrohash128('another test');

-- Test text input with custom seed
SELECT metrohash128('hello world', 42);
SELECT metrohash128('hello world', 84);

-- Test bytea input
SELECT metrohash128('hello world'::bytea);

-- Test bytea input with custom seed
SELECT metrohash128('hello world'::bytea, 42);

-- Test integer input
SELECT metrohash128(12345);
SELECT metrohash128(-12345);

-- Test integer input with custom seed
SELECT metrohash128(12345, 42);
SELECT metrohash128(-12345, 84);

-- Test consistency (same input should give same hash)
SELECT metrohash128('consistent test') = metrohash128('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT metrohash128('seed test', 1) != metrohash128('seed test', 2);

-- Test empty string
SELECT metrohash128('');

-- Test single character
SELECT metrohash128('a');

-- Test long string
SELECT metrohash128('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test various input lengths to test different code paths
SELECT metrohash128('1234567');           -- 7 bytes
SELECT metrohash128('12345678');          -- 8 bytes  
SELECT metrohash128('123456789012345');   -- 15 bytes
SELECT metrohash128('1234567890123456');  -- 16 bytes
SELECT metrohash128('12345678901234567890123456789012'); -- 32 bytes
SELECT metrohash128('123456789012345678901234567890123'); -- 33 bytes

-- Test array access (128-bit hash returns array of two bigints)
SELECT 
    (metrohash128('hello world'))[1] AS first_64_bits,
    (metrohash128('hello world'))[2] AS second_64_bits;

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'metrohash128'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';