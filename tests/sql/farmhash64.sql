-- Test basic hash functionality with text
SELECT farmhash64('hello world');

-- Test hash with different text inputs
SELECT farmhash64('test string');
SELECT farmhash64('another test');

-- Test text input with custom seed
SELECT farmhash64('hello world', 42);
SELECT farmhash64('hello world', 123);

-- Test text input with two seeds
SELECT farmhash64('hello world', 42, 84);
SELECT farmhash64('hello world', 123, 456);

-- Test bytea input
SELECT farmhash64('hello world'::bytea);

-- Test bytea input with custom seed
SELECT farmhash64('hello world'::bytea, 42);

-- Test bytea input with two seeds
SELECT farmhash64('hello world'::bytea, 42, 84);

-- Test integer input
SELECT farmhash64(12345);
SELECT farmhash64(-12345);

-- Test integer input with custom seed
SELECT farmhash64(12345, 42);
SELECT farmhash64(-12345, 123);

-- Test integer input with two seeds
SELECT farmhash64(12345, 42, 84);
SELECT farmhash64(-12345, 123, 456);

-- Test consistency (same input should give same hash)
SELECT farmhash64('consistent test') = farmhash64('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT farmhash64('seed test', 1) != farmhash64('seed test', 2);

-- Test two seeds effect (same input, different seed combinations should give different hashes)
SELECT farmhash64('seed test', 1, 2) != farmhash64('seed test', 3, 4);

-- Test empty string
SELECT farmhash64('');

-- Test single character
SELECT farmhash64('a');

-- Test long string
SELECT farmhash64('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'farmhash64'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';