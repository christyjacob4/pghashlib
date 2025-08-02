-- Test basic hash functionality with text
SELECT lookup3be('hello world');

-- Test hash with different text inputs
SELECT lookup3be('test string');
SELECT lookup3be('another test');

-- Test text input with custom initval
SELECT lookup3be('hello world', 42);
SELECT lookup3be('hello world', 123);

-- Test bytea input
SELECT lookup3be('hello world'::bytea);

-- Test bytea input with custom initval
SELECT lookup3be('hello world'::bytea, 42);

-- Test integer input
SELECT lookup3be(12345);
SELECT lookup3be(-12345);

-- Test integer input with custom initval
SELECT lookup3be(12345, 42);
SELECT lookup3be(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT lookup3be('consistent test') = lookup3be('consistent test');

-- Test initval effect (same input, different initvals should give different hashes)
SELECT lookup3be('initval test', 1) != lookup3be('initval test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'lookup3be'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';