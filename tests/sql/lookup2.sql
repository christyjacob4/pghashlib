-- Test basic hash functionality with text
SELECT lookup2('hello world');

-- Test hash with different text inputs
SELECT lookup2('test string');
SELECT lookup2('another test');

-- Test text input with custom initval
SELECT lookup2('hello world', 42);
SELECT lookup2('hello world', 123);

-- Test bytea input
SELECT lookup2('hello world'::bytea);

-- Test bytea input with custom initval
SELECT lookup2('hello world'::bytea, 42);

-- Test integer input
SELECT lookup2(12345);
SELECT lookup2(-12345);

-- Test integer input with custom initval
SELECT lookup2(12345, 42);
SELECT lookup2(-12345, 123);

-- Test consistency (same input should give same hash)
SELECT lookup2('consistent test') = lookup2('consistent test');

-- Test initval effect (same input, different initvals should give different hashes)
SELECT lookup2('initval test', 1) != lookup2('initval test', 2);

-- Test function properties
SELECT 
    proname,
    provolatile,
    proisstrict
FROM pg_proc 
WHERE proname = 'lookup2'
ORDER BY proname, proargtypes;

-- Test extension metadata
SELECT 
    extname,
    extversion
FROM pg_extension 
WHERE extname = 'hashlib';