# HighwayHash64

HighwayHash64 is Google's SIMD-optimized keyed hash function providing 64-bit output. It uses a 256-bit key and is designed for high-throughput hashing with cryptographic-level security.

## Signatures

- `highwayhash64(text)` → `bigint`
- `highwayhash64(text, bigint, bigint, bigint, bigint)` → `bigint`
- `highwayhash64(bytea)` → `bigint`
- `highwayhash64(bytea, bigint, bigint, bigint, bigint)` → `bigint`
- `highwayhash64(integer)` → `bigint`
- `highwayhash64(integer, bigint, bigint, bigint, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Key parameters (optional): Four 64-bit values forming a 256-bit key (default key used if not provided)

## Examples

```sql
-- Hash text with default key
SELECT highwayhash64('hello world');
-- Result: SIMD-optimized 64-bit hash

-- Hash text with custom key (4 bigint values)
SELECT highwayhash64('hello world', 1, 2, 3, 4);
-- Result: Keyed hash with custom 256-bit key

-- Hash bytea data
SELECT highwayhash64('hello world'::bytea);
-- Result: SIMD-optimized hash of bytea data

-- Hash bytea with custom key
SELECT highwayhash64('hello world'::bytea, 42, 84, 168, 336);
-- Result: Keyed hash with custom key

-- Hash integer values
SELECT highwayhash64(12345);
-- Result: SIMD-optimized hash of integer

-- Hash integer with custom key
SELECT highwayhash64(12345, 1, 2, 3, 4);
-- Result: Keyed hash with custom key
```

## Use Cases

- High-throughput data processing with security requirements
- SIMD-optimized hash table implementations
- Secure message authentication
- Performance-critical cryptographic applications