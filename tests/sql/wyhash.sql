-- Test basic hash functionality with text
SELECT wyhash('hello world');

-- Test hash with different text inputs
SELECT wyhash('test string');
SELECT wyhash('another test');

-- Test text input with custom seed
SELECT wyhash('hello world', 42);
SELECT wyhash('hello world', 84);

-- Test bytea input
SELECT wyhash('hello world'::bytea);

-- Test bytea input with custom seed
SELECT wyhash('hello world'::bytea, 42);

-- Test integer input
SELECT wyhash(12345);
SELECT wyhash(-12345);

-- Test integer input with custom seed
SELECT wyhash(12345, 42);
SELECT wyhash(-12345, 84);

-- Test bigint input
SELECT wyhash(123456789012345::bigint);
SELECT wyhash(-123456789012345::bigint);

-- Test bigint input with custom seed
SELECT wyhash(123456789012345::bigint, 42);
SELECT wyhash(-123456789012345::bigint, 84);

-- Test consistency (same input should give same hash)
SELECT wyhash('consistent test') = wyhash('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT wyhash('seed test', 1) != wyhash('seed test', 2);

-- Test empty string
SELECT wyhash('');

-- Test single character
SELECT wyhash('a');

-- Test long string
SELECT wyhash('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test various input lengths to test different code paths
SELECT wyhash('1234567');           -- 7 bytes
SELECT wyhash('12345678');          -- 8 bytes  
SELECT wyhash('123456789012345');   -- 15 bytes
SELECT wyhash('1234567890123456');  -- 16 bytes
SELECT wyhash('12345678901234567890123456789012'); -- 32 bytes
SELECT wyhash('123456789012345678901234567890123'); -- 33 bytes
SELECT wyhash('1234567890123456789012345678901234567890123456789012345'); -- 49 bytes (> 48)

-- Test zero values
SELECT wyhash(0);
SELECT wyhash(0::bigint);

-- Test maximum values
SELECT wyhash(2147483647);          -- max int4
SELECT wyhash(-2147483648);         -- min int4
SELECT wyhash(9223372036854775807::bigint);  -- max int8
SELECT wyhash(-9223372036854775808::bigint); -- min int8

-- Test with various seeds
SELECT wyhash('test', 0);
SELECT wyhash('test', 1);
SELECT wyhash('test', -1);
SELECT wyhash('test', 9223372036854775807::bigint);   -- max positive seed
SELECT wyhash('test', -9223372036854775808::bigint);  -- max negative seed

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'wyhash'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';