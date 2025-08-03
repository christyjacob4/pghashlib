# lookup3le

lookup3le is Bob Jenkins' lookup3 hash function with little-endian byte order, optimized for Intel/AMD processors and other little-endian systems for maximum performance.

## Signatures

- `lookup3le(text)` → `integer`
- `lookup3le(text, integer)` → `integer`
- `lookup3le(bytea)` → `integer`
- `lookup3le(bytea, integer)` → `integer`
- `lookup3le(integer)` → `integer`
- `lookup3le(integer, integer)` → `integer`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Initial value/seed (default: 0)

## Examples

```sql
-- Hash text with default initval (0)
SELECT lookup3le('hello world');
-- Result: lookup3le hash of text

-- Hash text with custom initval
SELECT lookup3le('hello world', 42);
-- Result: lookup3le hash with custom initval

-- Hash bytea data
SELECT lookup3le('hello world'::bytea);
-- Result: lookup3le hash of bytea data

-- Hash bytea with custom initval
SELECT lookup3le('hello world'::bytea, 42);
-- Result: lookup3le hash with custom initval

-- Hash integer values
SELECT lookup3le(12345);
-- Result: lookup3le hash of integer

-- Hash integer with custom initval
SELECT lookup3le(12345, 42);
-- Result: lookup3le hash with custom initval
```

## Use Cases

- Intel/AMD processor optimization
- Little-endian system performance
- Modern x86/x86_64 hash table implementations
- Cross-platform portable hashing on common architectures