# HighwayHash128

HighwayHash128 is Google's SIMD-optimized keyed hash function providing 128-bit output for stronger collision resistance and data integrity verification.

## Signatures

- `highwayhash128(text)` → `bigint[]`
- `highwayhash128(text, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash128(bytea)` → `bigint[]`
- `highwayhash128(bytea, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash128(integer)` → `bigint[]`
- `highwayhash128(integer, bigint, bigint, bigint, bigint)` → `bigint[]`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Key parameters (optional): Four 64-bit values forming a 256-bit key (default key used if not provided)

## Return Value

Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

## Examples

```sql
-- Hash text with default key
SELECT highwayhash128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom key
SELECT highwayhash128('hello world', 1, 2, 3, 4);
-- Result: Keyed 128-bit hash

-- Hash bytea data
SELECT highwayhash128('hello world'::bytea);
-- Result: SIMD-optimized 128-bit hash

-- Hash bytea with custom key
SELECT highwayhash128('hello world'::bytea, 42, 84, 168, 336);
-- Result: Keyed 128-bit hash

-- Hash integer values
SELECT highwayhash128(12345);
-- Result: SIMD-optimized 128-bit hash

-- Hash integer with custom key
SELECT highwayhash128(12345, 1, 2, 3, 4);
-- Result: Keyed 128-bit hash

-- Access individual parts of the 128-bit hash
SELECT 
    (highwayhash128('hello world'))[1] AS first_64_bits,
    (highwayhash128('hello world'))[2] AS second_64_bits;
```

## Use Cases

- Strong data integrity verification
- Secure content addressing systems
- High-performance secure fingerprinting
- SIMD-accelerated cryptographic hashing