-- Test basic hash functionality with text
SELECT t1ha2('hello world');

-- Test hash with different text inputs
SELECT t1ha2('test string');
SELECT t1ha2('another test');

-- Test text input with custom seed
SELECT t1ha2('hello world', 42);
SELECT t1ha2('hello world', 84);

-- Test bytea input
SELECT t1ha2('hello world'::bytea);

-- Test bytea input with custom seed
SELECT t1ha2('hello world'::bytea, 42);

-- Test integer input
SELECT t1ha2(12345);
SELECT t1ha2(-12345);

-- Test integer input with custom seed
SELECT t1ha2(12345, 42);
SELECT t1ha2(-12345, 84);

-- Test consistency (same input should give same hash)
SELECT t1ha2('consistent test') = t1ha2('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT t1ha2('seed test', 1) != t1ha2('seed test', 2);

-- Test empty string
SELECT t1ha2('');

-- Test single character
SELECT t1ha2('a');

-- Test long string
SELECT t1ha2('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test various input lengths to test different code paths
SELECT t1ha2('1234567');           -- 7 bytes
SELECT t1ha2('12345678');          -- 8 bytes  
SELECT t1ha2('123456789012345');   -- 15 bytes
SELECT t1ha2('1234567890123456');  -- 16 bytes
SELECT t1ha2('12345678901234567890123456789012'); -- 32 bytes
SELECT t1ha2('123456789012345678901234567890123'); -- 33 bytes

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 't1ha2'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';