# xxHash64

xxHash64 is an extremely fast 64-bit non-cryptographic hash function optimized for speed on 64-bit processors while maintaining excellent distribution properties.

## Signatures

- `xxhash64(text)` → `bigint`
- `xxhash64(text, bigint)` → `bigint`
- `xxhash64(bytea)` → `bigint`
- `xxhash64(bytea, bigint)` → `bigint`
- `xxhash64(integer)` → `bigint`
- `xxhash64(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT xxhash64('hello world');
-- Result: Fast 64-bit hash

-- Hash text with custom seed
SELECT xxhash64('hello world', 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash bytea data
SELECT xxhash64('hello world'::bytea);
-- Result: Fast 64-bit hash of bytea data

-- Hash bytea with custom seed
SELECT xxhash64('hello world'::bytea, 42);
-- Result: Fast 64-bit hash with custom seed

-- Hash integer values
SELECT xxhash64(12345);
-- Result: Fast 64-bit hash of integer

-- Hash integer with custom seed
SELECT xxhash64(12345, 42);
-- Result: Fast 64-bit hash with custom seed
```

## Use Cases

- Ultra-fast general-purpose hashing
- High-throughput data processing
- Real-time analytics and streaming
- Performance-critical hash table implementations