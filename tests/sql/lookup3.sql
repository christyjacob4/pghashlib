-- Test basic hash functionality with text
SELECT lookup3('hello world') IS NOT NULL;

-- Test hash with different text inputs give different results
SELECT lookup3('test string') != lookup3('hello world');
SELECT lookup3('another test') != lookup3('hello world');

-- Test text input with custom initval gives different results
SELECT lookup3('hello world', 42) != lookup3('hello world');
SELECT lookup3('hello world', 123) != lookup3('hello world', 42);

-- Test bytea input consistency
SELECT lookup3('hello world'::bytea) = lookup3('hello world');

-- Test bytea input with custom initval
SELECT lookup3('hello world'::bytea, 42) = lookup3('hello world', 42);

-- Test integer input gives consistent results
SELECT lookup3(12345) = lookup3(12345);
SELECT lookup3(-12345) = lookup3(-12345);

-- Test integer input with custom initval
SELECT lookup3(12345, 42) = lookup3(12345, 42);
SELECT lookup3(-12345, 123) != lookup3(-12345, 42);

-- Test consistency (same input should give same hash)
SELECT lookup3('consistent test') = lookup3('consistent test');

-- Test initval effect (same input, different initvals should give different hashes)
SELECT lookup3('initval test', 1) != lookup3('initval test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'lookup3'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';