# WyHash

WyHash is one of the fastest quality hash functions available, designed by Wang Yi. It offers exceptional performance while maintaining excellent hash quality and is used as the default hash algorithm in Go (since 1.17), Zig, V, and Nim languages.

## Key Features

- Extremely fast performance, often faster than xxHash3 and t1ha
- Excellent hash quality, passes all SMHasher tests
- Optimized for modern 64-bit systems
- Particularly efficient with short keys
- Public domain license (The Unlicense)

## Signatures

- `wyhash(text)` → `bigint`
- `wyhash(text, bigint)` → `bigint`
- `wyhash(bytea)` → `bigint`
- `wyhash(bytea, bigint)` → `bigint`
- `wyhash(integer)` → `bigint`
- `wyhash(integer, bigint)` → `bigint`
- `wyhash(bigint)` → `bigint`
- `wyhash(bigint, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, `integer`, or `bigint`)
- Second parameter (optional): Seed value (default: 0)

## Return Value

Returns a 64-bit signed integer (`bigint`) hash value.

## Examples

```sql
-- Hash text data
SELECT wyhash('hello world');
-- Result: Fast, high-quality WyHash of text

-- Hash text with custom seed
SELECT wyhash('hello world', 42);
-- Result: Fast, high-quality WyHash with custom seed

-- Hash bytea data
SELECT wyhash('hello world'::bytea);
-- Result: Fast, high-quality WyHash of bytea data

-- Hash integer values
SELECT wyhash(12345);
-- Result: Fast, high-quality WyHash of integer

-- Hash bigint values
SELECT wyhash(123456789012345::bigint);
-- Result: Fast, high-quality WyHash of bigint value

-- Use in data partitioning (extremely fast)
SELECT 
    user_id,
    abs(wyhash(user_id::text) % 10) as partition
FROM users;

-- Use for sampling (high performance)
SELECT * FROM large_table 
WHERE abs(wyhash(id::text) % 100) < 5;  -- 5% sample
```

## Use Cases

- Maximum performance general-purpose hashing
- Modern programming language compatibility (Go, Zig, V, Nim)
- High-frequency data partitioning
- Performance-critical hash table implementations
- Real-time analytics and streaming data processing