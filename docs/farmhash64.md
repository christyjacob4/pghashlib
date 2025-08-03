# FarmHash64

FarmHash64 is Google's 64-bit successor to CityHash, designed for better distribution properties with support for dual seed hashing.

## Signatures

- `farmhash64(text)` → `bigint`
- `farmhash64(text, bigint)` → `bigint`
- `farmhash64(text, bigint, bigint)` → `bigint`
- `farmhash64(bytea)` → `bigint`
- `farmhash64(bytea, bigint)` → `bigint`
- `farmhash64(bytea, bigint, bigint)` → `bigint`
- `farmhash64(integer)` → `bigint`
- `farmhash64(integer, bigint)` → `bigint`
- `farmhash64(integer, bigint, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): First seed value (default: 0)
- Third parameter (optional): Second seed value for dual seed hashing

## Examples

```sql
-- Hash text with default seed
SELECT farmhash64('hello world');
-- Result: Fast 64-bit FarmHash

-- Hash text with custom seed
SELECT farmhash64('hello world', 42);
-- Result: Fast 64-bit FarmHash with custom seed

-- Hash text with two seeds for stronger customization
SELECT farmhash64('hello world', 42, 84);
-- Result: Fast 64-bit FarmHash with dual seeds

-- Hash bytea data
SELECT farmhash64('hello world'::bytea);
-- Result: Fast 64-bit FarmHash of bytea data

-- Hash bytea with custom seed
SELECT farmhash64('hello world'::bytea, 42);
-- Result: Fast 64-bit FarmHash with custom seed

-- Hash bytea with two seeds
SELECT farmhash64('hello world'::bytea, 42, 84);
-- Result: Fast 64-bit FarmHash with dual seeds

-- Hash integer values
SELECT farmhash64(12345);
-- Result: Fast 64-bit FarmHash of integer

-- Hash integer with custom seed
SELECT farmhash64(12345, 42);
-- Result: Fast 64-bit FarmHash with custom seed

-- Hash integer with two seeds
SELECT farmhash64(12345, 42, 84);
-- Result: Fast 64-bit FarmHash with dual seeds
```

## Use Cases

- High-quality data partitioning with superior distribution
- Distributed systems requiring consistent hashing
- Advanced hash table implementations
- Google ecosystem compatibility