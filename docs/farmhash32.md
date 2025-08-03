# FarmHash32

FarmHash32 is Google's 32-bit successor to CityHash, designed for better distribution properties while maintaining high performance.

## Signatures

- `farmhash32(text)` → `integer`
- `farmhash32(text, integer)` → `integer`
- `farmhash32(bytea)` → `integer`
- `farmhash32(bytea, integer)` → `integer`
- `farmhash32(integer)` → `integer`
- `farmhash32(integer, integer)` → `integer`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT farmhash32('hello world');
-- Result: Fast 32-bit FarmHash

-- Hash text with custom seed
SELECT farmhash32('hello world', 42);
-- Result: Fast 32-bit FarmHash with custom seed

-- Hash bytea data
SELECT farmhash32('hello world'::bytea);
-- Result: Fast 32-bit FarmHash of bytea data

-- Hash bytea with custom seed
SELECT farmhash32('hello world'::bytea, 42);
-- Result: Fast 32-bit FarmHash with custom seed

-- Hash integer values
SELECT farmhash32(12345);
-- Result: Fast 32-bit FarmHash of integer

-- Hash integer with custom seed
SELECT farmhash32(12345, 42);
-- Result: Fast 32-bit FarmHash with custom seed
```

## Use Cases

- Improved hash table implementations requiring better distribution than CityHash
- Data partitioning with enhanced uniformity
- String deduplication with reduced collisions
- Google-compatible hashing for consistency with other systems