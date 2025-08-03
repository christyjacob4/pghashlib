-- Test basic hash functionality with text
SELECT lookup3('hello world') IS NOT NULL;

-- Test text input with different seeds
SELECT lookup3('hello world', 0) != lookup3('hello world', 1);

-- Test bytea input
SELECT lookup3('hello world'::bytea) IS NOT NULL;

-- Test integer input  
SELECT lookup3(12345) IS NOT NULL;

-- Test consistency (same input should give same hash)
SELECT lookup3('test') = lookup3('test');

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