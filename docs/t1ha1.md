# t1ha1

t1ha1 is the baseline portable variant of t1ha, providing stable results across different architectures with reasonable quality for checksums and hash tables.

## Signatures

- `t1ha1(text)` → `bigint`
- `t1ha1(text, bigint)` → `bigint`
- `t1ha1(bytea)` → `bigint`
- `t1ha1(bytea, bigint)` → `bigint`
- `t1ha1(integer)` → `bigint`
- `t1ha1(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT t1ha1('hello world');
-- Result: Portable 64-bit t1ha1 hash

-- Hash text with custom seed
SELECT t1ha1('hello world', 42);
-- Result: Portable 64-bit t1ha1 hash with custom seed

-- Hash bytea data
SELECT t1ha1('hello world'::bytea);
-- Result: Portable 64-bit t1ha1 hash of bytea data

-- Hash integer values
SELECT t1ha1(12345);
-- Result: Portable 64-bit t1ha1 hash of integer
```

## Use Cases

- Cross-platform applications requiring consistent results
- Distributed systems spanning different architectures
- Reliable checksums for data integrity
- Portable hash table implementations