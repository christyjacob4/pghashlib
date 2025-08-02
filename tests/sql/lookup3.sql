-- Test basic hash functionality with text
SELECT lookup3('hello world');

-- Test hash with different text inputs
SELECT lookup3('test string');
SELECT lookup3('another test');

-- Test text input with custom initval
SELECT lookup3('hello world', 42);
SELECT lookup3('hello world', 123);

-- Test bytea input
SELECT lookup3('hello world'::bytea);

-- Test bytea input with custom initval
SELECT lookup3('hello world'::bytea, 42);

-- Test integer input
SELECT lookup3(12345);
SELECT lookup3(-12345);

-- Test integer input with custom initval
SELECT lookup3(12345, 42);
SELECT lookup3(-12345, 123);

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