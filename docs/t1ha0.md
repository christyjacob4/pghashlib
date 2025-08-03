# t1ha0

t1ha0 is the fastest available variant of the t1ha (Fast Positive Hash) family. It automatically selects the optimal implementation for the current CPU architecture, prioritizing speed over consistent cross-platform results.

## Signatures

- `t1ha0(text)` → `bigint`
- `t1ha0(text, bigint)` → `bigint`
- `t1ha0(bytea)` → `bigint`
- `t1ha0(bytea, bigint)` → `bigint`
- `t1ha0(integer)` → `bigint`
- `t1ha0(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT t1ha0('hello world');
-- Result: Ultra-fast 64-bit t1ha0 hash

-- Hash text with custom seed
SELECT t1ha0('hello world', 42);
-- Result: Ultra-fast 64-bit t1ha0 hash with custom seed

-- Hash bytea data
SELECT t1ha0('hello world'::bytea);
-- Result: Ultra-fast 64-bit t1ha0 hash of bytea data

-- Hash integer values
SELECT t1ha0(12345);
-- Result: Ultra-fast 64-bit t1ha0 hash of integer
```

## Use Cases

- Maximum performance hashing where speed is critical
- Real-time processing with tight latency requirements
- CPU-optimized hash table implementations
- High-frequency trading and financial systems