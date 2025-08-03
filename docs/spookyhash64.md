# SpookyHash64

SpookyHash64 is a 64-bit hash function by Bob Jenkins, optimized for speed on 64-bit processors with excellent avalanche properties.

## Signatures

- `spookyhash64(text)` → `bigint`
- `spookyhash64(text, bigint)` → `bigint`
- `spookyhash64(bytea)` → `bigint`
- `spookyhash64(bytea, bigint)` → `bigint`
- `spookyhash64(integer)` → `bigint`
- `spookyhash64(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT spookyhash64('hello world');
-- Result: Fast 64-bit hash

-- Hash text with custom seed
SELECT spookyhash64('hello world', 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash bytea data
SELECT spookyhash64('hello world'::bytea);
-- Result: Fast 64-bit hash of bytea data

-- Hash bytea with custom seed
SELECT spookyhash64('hello world'::bytea, 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash integer values
SELECT spookyhash64(12345);
-- Result: Fast 64-bit hash of integer

-- Hash integer with custom seed
SELECT spookyhash64(12345, 42);
-- Result: Fast 64-bit hash with custom seed
```

## Use Cases

- High-performance hash tables on 64-bit systems
- Fast data deduplication
- Bloom filter implementations
- General-purpose non-cryptographic hashing