# HighwayHash256

HighwayHash256 is Google's SIMD-optimized keyed hash function providing 256-bit output for maximum collision resistance and cryptographic applications.

## Signatures

- `highwayhash256(text)` → `bigint[]`
- `highwayhash256(text, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash256(bytea)` → `bigint[]`
- `highwayhash256(bytea, bigint, bigint, bigint, bigint)` → `bigint[]`
- `highwayhash256(integer)` → `bigint[]`
- `highwayhash256(integer, bigint, bigint, bigint, bigint)` → `bigint[]`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Key parameters (optional): Four 64-bit values forming a 256-bit key (default key used if not provided)

## Return Value

Returns an array of four `bigint` values representing the 256-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash
- `[3]`: Third 64 bits of the hash
- `[4]`: Fourth 64 bits of the hash

## Examples

```sql
-- Hash text with default key
SELECT highwayhash256('hello world');
-- Result: {first_64_bits, second_64_bits, third_64_bits, fourth_64_bits}

-- Hash text with custom key
SELECT highwayhash256('hello world', 1, 2, 3, 4);
-- Result: Keyed 256-bit hash

-- Hash bytea data
SELECT highwayhash256('hello world'::bytea);
-- Result: SIMD-optimized 256-bit hash

-- Hash bytea with custom key
SELECT highwayhash256('hello world'::bytea, 42, 84, 168, 336);
-- Result: Keyed 256-bit hash

-- Hash integer values
SELECT highwayhash256(12345);
-- Result: SIMD-optimized 256-bit hash

-- Hash integer with custom key
SELECT highwayhash256(12345, 1, 2, 3, 4);
-- Result: Keyed 256-bit hash

-- Access individual parts of the 256-bit hash
SELECT 
    (highwayhash256('hello world'))[1] AS first_64_bits,
    (highwayhash256('hello world'))[2] AS second_64_bits,
    (highwayhash256('hello world'))[3] AS third_64_bits,
    (highwayhash256('hello world'))[4] AS fourth_64_bits;
```

## Use Cases

- Maximum collision resistance for critical applications
- Cryptographic content addressing
- Secure distributed hash tables
- High-assurance data integrity verification