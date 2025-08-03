# MurmurHash3

MurmurHash3 is a fast, non-cryptographic hash function suitable for general hash-based lookup.

## Signatures

- `murmurhash3_32(text)` → `integer`
- `murmurhash3_32(text, integer)` → `integer` 
- `murmurhash3_32(bytea)` → `integer`
- `murmurhash3_32(bytea, integer)` → `integer`
- `murmurhash3_32(integer)` → `integer`
- `murmurhash3_32(integer, integer)` → `integer`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed (0)
SELECT murmurhash3_32('hello world');
-- Result: 1594632942

-- Hash text with custom seed
SELECT murmurhash3_32('hello world', 42);
-- Result: 2838467652

-- Hash bytea data
SELECT murmurhash3_32('hello world'::bytea);
-- Result: 1594632942

-- Hash bytea with custom seed
SELECT murmurhash3_32('hello world'::bytea, 42);
-- Result: 2838467652

-- Hash integer values
SELECT murmurhash3_32(12345);
-- Result: 2794345569

-- Hash integer with custom seed
SELECT murmurhash3_32(12345, 42);
-- Result: 751823303
```

## Use Cases

- General-purpose hashing for hash tables
- Data partitioning across multiple shards
- Random sampling of datasets
- A/B testing user assignment