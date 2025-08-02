-- Test basic hash functionality with text
SELECT lookup3le('hello world');

-- Test hash with different text inputs
SELECT lookup3le('test string');
SELECT lookup3le('another test');

-- Test text input with custom initval
SELECT lookup3le('hello world', 42);
SELECT lookup3le('hello world', 123);

-- Test bytea input
SELECT lookup3le('hello world'::bytea);

-- Test bytea input with custom initval
SELECT lookup3le('hello world'::bytea, 42);

-- Test integer input
SELECT lookup3le(12345);
SELECT lookup3le(-12345);

-- Test integer input with custom initval
SELECT lookup3le(12345, 42);
SELECT lookup3le(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT lookup3le('consistent test') = lookup3le('consistent test');

-- Test initval effect (same input, different initvals should give different hashes)
SELECT lookup3le('initval test', 1) != lookup3le('initval test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'lookup3le'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';