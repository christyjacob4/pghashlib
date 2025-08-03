# SpookyHash128

SpookyHash128 is a 128-bit hash function by Bob Jenkins, providing stronger hash distribution with two 64-bit output values.

## Signatures

- `spookyhash128(text)` → `bigint[]`
- `spookyhash128(text, bigint, bigint)` → `bigint[]`
- `spookyhash128(bytea)` → `bigint[]`
- `spookyhash128(bytea, bigint, bigint)` → `bigint[]`
- `spookyhash128(integer)` → `bigint[]`
- `spookyhash128(integer, bigint, bigint)` → `bigint[]`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): First 64-bit seed value
- Third parameter (optional): Second 64-bit seed value

## Return Value

Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: First 64 bits of the hash
- `[2]`: Second 64 bits of the hash

## Examples

```sql
-- Hash text with default seed
SELECT spookyhash128('hello world');
-- Result: {first_64_bits, second_64_bits}

-- Hash text with custom seeds (two 64-bit values)
SELECT spookyhash128('hello world', 42, 84);
-- Result: {first_64_bits, second_64_bits}

-- Hash bytea data
SELECT spookyhash128('hello world'::bytea);
-- Result: {first_64_bits, second_64_bits}

-- Hash bytea with custom seeds
SELECT spookyhash128('hello world'::bytea, 42, 84);
-- Result: {first_64_bits, second_64_bits}

-- Hash integer values
SELECT spookyhash128(12345);
-- Result: {first_64_bits, second_64_bits}

-- Hash integer with custom seeds
SELECT spookyhash128(12345, 42, 84);
-- Result: {first_64_bits, second_64_bits}

-- Access individual parts of the 128-bit hash
SELECT 
    (spookyhash128('hello world'))[1] AS first_64_bits,
    (spookyhash128('hello world'))[2] AS second_64_bits;
```

## Use Cases

- Strong fingerprinting requiring collision resistance
- Large-scale data partitioning with excellent distribution
- Content addressing systems
- High-quality pseudorandom number generation