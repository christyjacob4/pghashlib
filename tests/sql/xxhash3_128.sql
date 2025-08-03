-- Test basic hash functionality with text
SELECT xxhash3_128('hello world');

-- Test hash with different text inputs
SELECT xxhash3_128('test string');
SELECT xxhash3_128('another test');

-- Test text input with custom seed
SELECT xxhash3_128('hello world', 42);
SELECT xxhash3_128('hello world', 123);

-- Test bytea input  
SELECT xxhash3_128('hello world'::bytea);

-- Test bytea input with custom seed
SELECT xxhash3_128('hello world'::bytea, 42);

-- Test integer input
SELECT xxhash3_128(12345);
SELECT xxhash3_128(-12345);

-- Test integer input with custom seed
SELECT xxhash3_128(12345, 42);
SELECT xxhash3_128(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT xxhash3_128('consistent test') = xxhash3_128('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT xxhash3_128('seed test', 1) != xxhash3_128('seed test', 2);

-- Test empty string
SELECT xxhash3_128('');

-- Test single character
SELECT xxhash3_128('a');

-- Test long string
SELECT xxhash3_128('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.');

-- Test different input sizes to verify different algorithm paths
SELECT xxhash3_128('a');           -- 1 byte (small)
SELECT xxhash3_128('hello');        -- 5 bytes (small)
SELECT xxhash3_128('hello world!'); -- 12 bytes (small)
SELECT xxhash3_128('this is a test string with more than sixteen characters'); -- medium size
SELECT xxhash3_128(repeat('x', 250)); -- large size (>240 bytes)

-- Test that results are 32-character hex strings
SELECT length(xxhash3_128('test'));
SELECT xxhash3_128('test') ~ '^[0-9a-f]{32}$';

-- Test that different inputs produce different hashes
SELECT xxhash3_128('input1') != xxhash3_128('input2');

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'xxhash3_128'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';