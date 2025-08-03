# t1ha2

t1ha2 is the recommended variant of t1ha, providing good quality for checksums and hash tables while being optimized for 64-bit systems. It offers the best balance of speed and quality.

## Signatures

- `t1ha2(text)` → `bigint`
- `t1ha2(text, bigint)` → `bigint`
- `t1ha2(bytea)` → `bigint`
- `t1ha2(bytea, bigint)` → `bigint`
- `t1ha2(integer)` → `bigint`
- `t1ha2(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT t1ha2('hello world');
-- Result: High-quality 64-bit t1ha2 hash

-- Hash text with custom seed
SELECT t1ha2('hello world', 42);
-- Result: High-quality 64-bit t1ha2 hash with custom seed

-- Hash bytea data
SELECT t1ha2('hello world'::bytea);
-- Result: High-quality 64-bit t1ha2 hash of bytea data

-- Hash integer values
SELECT t1ha2(12345);
-- Result: High-quality 64-bit t1ha2 hash of integer
```

## Use Cases

- General-purpose hashing with excellent speed/quality balance
- Production hash table implementations
- Data partitioning requiring good distribution
- Modern 64-bit system optimization