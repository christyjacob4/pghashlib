-- Test basic hash functionality with text
SELECT farmhash32('hello world');

-- Test hash with different text inputs
SELECT farmhash32('test string');
SELECT farmhash32('another test');

-- Test text input with custom seed
SELECT farmhash32('hello world', 42);
SELECT farmhash32('hello world', 123);

-- Test bytea input
SELECT farmhash32('hello world'::bytea);

-- Test bytea input with custom seed
SELECT farmhash32('hello world'::bytea, 42);

-- Test integer input
SELECT farmhash32(12345);
SELECT farmhash32(-12345);

-- Test integer input with custom seed
SELECT farmhash32(12345, 42);
SELECT farmhash32(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT farmhash32('consistent test') = farmhash32('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT farmhash32('seed test', 1) != farmhash32('seed test', 2);

-- Test empty string
SELECT farmhash32('');

-- Test single character
SELECT farmhash32('a');

-- Test long string
SELECT farmhash32('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'farmhash32'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';