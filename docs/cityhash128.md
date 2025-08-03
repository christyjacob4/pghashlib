# CityHash128

CityHash128 is a 128-bit hash function from Google, providing stronger hash distribution than the 64-bit version.

## Signatures

- `cityhash128(text)` → `bigint[]`
- `cityhash128(text, bigint, bigint)` → `bigint[]`
- `cityhash128(bytea)` → `bigint[]`
- `cityhash128(bytea, bigint, bigint)` → `bigint[]`
- `cityhash128(integer)` → `bigint[]`
- `cityhash128(integer, bigint, bigint)` → `bigint[]`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Low 64-bit seed value
- Third parameter (optional): High 64-bit seed value

## Return Value

Returns an array of two `bigint` values representing the 128-bit hash:
- `[1]`: Low 64 bits of the hash
- `[2]`: High 64 bits of the hash

## Examples

```sql
-- Hash text with default seed
SELECT cityhash128('hello world');
-- Result: {-7119421456246056744,-4082676536336963091}

-- Hash text with custom seed (two 64-bit values)
SELECT cityhash128('hello world', 42, 84);
-- Result: {4409783961438234325,7356734009537733524}

-- Hash bytea data
SELECT cityhash128('hello world'::bytea);
-- Result: {-7119421456246056744,-4082676536336963091}

-- Hash bytea with custom seed
SELECT cityhash128('hello world'::bytea, 42, 84);
-- Result: {4409783961438234325,7356734009537733524}

-- Hash integer values
SELECT cityhash128(12345);
-- Result: {-8264812632162517731,-9113745412911669670}

-- Hash integer with custom seed
SELECT cityhash128(12345, 42, 84);
-- Result: {-4383119689242713753,-6416156085369945475}

-- Access individual parts of the 128-bit hash
SELECT 
    (cityhash128('hello world'))[1] AS low_64_bits,
    (cityhash128('hello world'))[2] AS high_64_bits;
```

## Use Cases

- Strong deduplication fingerprints requiring collision resistance
- Large-scale data partitioning with millions of partitions
- Content-based addressing systems
- Database sharding with high distribution quality