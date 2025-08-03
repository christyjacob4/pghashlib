# CityHash64

CityHash64 is a 64-bit hash function from Google, designed for high performance on strings.

## Signatures

- `cityhash64(text)` → `bigint`
- `cityhash64(text, bigint)` → `bigint`
- `cityhash64(bytea)` → `bigint`
- `cityhash64(bytea, bigint)` → `bigint`
- `cityhash64(integer)` → `bigint`
- `cityhash64(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value

## Examples

```sql
-- Hash text with default seed
SELECT cityhash64('hello world');
-- Result: 2578220239953316063

-- Hash text with custom seed  
SELECT cityhash64('hello world', 42);
-- Result: 6383797005284447264

-- Hash bytea data
SELECT cityhash64('hello world'::bytea);
-- Result: 2578220239953316063

-- Hash bytea with custom seed
SELECT cityhash64('hello world'::bytea, 42);
-- Result: 6383797005284447264

-- Hash integer values
SELECT cityhash64(12345);
-- Result: 2041813223586929814

-- Hash integer with custom seed
SELECT cityhash64(12345, 42);
-- Result: 17692749115209691159
```

## Use Cases

- High-performance data partitioning
- Hash table implementations requiring good distribution
- String deduplication and fingerprinting
- Distributed system key routing