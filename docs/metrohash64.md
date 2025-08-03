# MetroHash64

MetroHash64 is a fast alternative hash function with excellent avalanche properties, algorithmically generated for performance and providing a good balance of speed and distribution quality.

## Signatures

- `metrohash64(text)` → `bigint`
- `metrohash64(text, bigint)` → `bigint`
- `metrohash64(bytea)` → `bigint`
- `metrohash64(bytea, bigint)` → `bigint`
- `metrohash64(integer)` → `bigint`
- `metrohash64(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT metrohash64('hello world');
-- Result: Fast 64-bit MetroHash

-- Hash text with custom seed
SELECT metrohash64('hello world', 42);
-- Result: Fast 64-bit MetroHash with custom seed

-- Hash bytea data
SELECT metrohash64('hello world'::bytea);
-- Result: Fast 64-bit MetroHash of bytea data

-- Hash bytea with custom seed
SELECT metrohash64('hello world'::bytea, 42);
-- Result: Fast 64-bit MetroHash with custom seed

-- Hash integer values
SELECT metrohash64(12345);
-- Result: Fast 64-bit MetroHash of integer

-- Hash integer with custom seed
SELECT metrohash64(12345, 42);
-- Result: Fast 64-bit MetroHash with custom seed
```

## Use Cases

- High-performance hash tables with excellent distribution
- Fast data partitioning with minimal clustering
- Stream processing requiring consistent quality
- Alternative to CityHash with different performance characteristics