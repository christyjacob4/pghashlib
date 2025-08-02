-- Test basic hash functionality with text
SELECT cityhash128('hello world');

-- Test hash with different text inputs
SELECT cityhash128('test string');
SELECT cityhash128('another test');

-- Test text input with custom seed
SELECT cityhash128('hello world', 42, 84);
SELECT cityhash128('hello world', 123, 246);

-- Test bytea input
SELECT cityhash128('hello world'::bytea);

-- Test bytea input with custom seed
SELECT cityhash128('hello world'::bytea, 42, 84);

-- Test integer input
SELECT cityhash128(12345);
SELECT cityhash128(-12345);

-- Test integer input with custom seed
SELECT cityhash128(12345, 42, 84);
SELECT cityhash128(-12345, 123, 246);

-- Test consistency (same input should give same hash)
SELECT cityhash128('consistent test') = cityhash128('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT cityhash128('seed test', 1, 2) != cityhash128('seed test', 3, 4);

-- Test that results are arrays with exactly 2 elements
SELECT array_length(cityhash128('test'), 1);

-- Test accessing individual elements of the hash
SELECT (cityhash128('hash parts'))[1] AS low_part, (cityhash128('hash parts'))[2] AS high_part;

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'cityhash128'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';