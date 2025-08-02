-- Test basic hash functionality with text
SELECT cityhash64('hello world');

-- Test hash with different text inputs
SELECT cityhash64('test string');
SELECT cityhash64('another test');

-- Test text input with custom seed
SELECT cityhash64('hello world', 42);
SELECT cityhash64('hello world', 123);

-- Test bytea input
SELECT cityhash64('hello world'::bytea);

-- Test bytea input with custom seed
SELECT cityhash64('hello world'::bytea, 42);

-- Test integer input
SELECT cityhash64(12345);
SELECT cityhash64(-12345);

-- Test integer input with custom seed
SELECT cityhash64(12345, 42);
SELECT cityhash64(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT cityhash64('consistent test') = cityhash64('consistent test');

-- Test seed effect (same input, different seeds should give different hashes)
SELECT cityhash64('seed test', 1) != cityhash64('seed test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'cityhash64'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';