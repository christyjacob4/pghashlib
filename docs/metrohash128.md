# MetroHash128

MetroHash128 is a fast alternative hash function providing 128-bit output with excellent avalanche properties and a strong statistical profile similar to MD5 but much faster.

## Signatures

- `metrohash128(text)` → `bigint[]`
- `metrohash128(text, bigint)` → `bigint[]`
- `metrohash128(bytea)` → `bigint[]`
- `metrohash128(bytea, bigint)` → `bigint[]`
- `metrohash128(integer)` → `bigint[]`
- `metrohash128(integer, bigint)` → `bigint[]`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Return Value

Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

## Examples

```sql
-- Hash text with default seed
SELECT metrohash128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom seed
SELECT metrohash128('hello world', 42);
-- Result: Fast 128-bit MetroHash with custom seed

-- Hash bytea data
SELECT metrohash128('hello world'::bytea);
-- Result: Fast 128-bit MetroHash of bytea data

-- Hash bytea with custom seed
SELECT metrohash128('hello world'::bytea, 42);
-- Result: Fast 128-bit MetroHash with custom seed

-- Hash integer values
SELECT metrohash128(12345);
-- Result: Fast 128-bit MetroHash of integer

-- Hash integer with custom seed
SELECT metrohash128(12345, 42);
-- Result: Fast 128-bit MetroHash with custom seed

-- Access individual parts of the 128-bit hash
SELECT 
    (metrohash128('hello world'))[1] AS first_64_bits,
    (metrohash128('hello world'))[2] AS second_64_bits;
```

## Use Cases

- Strong fingerprinting with MD5-like statistical properties
- Fast 128-bit hash table implementations
- Data deduplication requiring collision resistance
- High-performance content addressing