-- Test basic hash functionality with text
SELECT siphash24('hello world');

-- Test hash with different text inputs
SELECT siphash24('test string');
SELECT siphash24('another test');

-- Test text input with custom seeds (two 32-bit values for 128-bit key)
SELECT siphash24('hello world', 42, 84);
SELECT siphash24('hello world', 123, 456);

-- Test bytea input
SELECT siphash24('hello world'::bytea);

-- Test bytea input with custom seeds
SELECT siphash24('hello world'::bytea, 42, 84);

-- Test integer input
SELECT siphash24(12345);
SELECT siphash24(-12345);

-- Test integer input with custom seeds
SELECT siphash24(12345, 42, 84);
SELECT siphash24(-12345, 123, 456);

-- Test consistency (same input should give same hash)
SELECT siphash24('consistent test') = siphash24('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT siphash24('seed test', 1, 2) != siphash24('seed test', 3, 4);

-- Test empty string
SELECT siphash24('');

-- Test single character
SELECT siphash24('a');

-- Test long string
SELECT siphash24('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'siphash24'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';