# xxHash3_64

xxHash3_64 is the next-generation 64-bit variant of the xxHash algorithm, offering improved speed and better hash quality compared to xxHash64, especially for small inputs.

## Signatures

- `xxhash3_64(text)` → `bigint`
- `xxhash3_64(text, bigint)` → `bigint`
- `xxhash3_64(bytea)` → `bigint`
- `xxhash3_64(bytea, bigint)` → `bigint`
- `xxhash3_64(integer)` → `bigint`
- `xxhash3_64(integer, bigint)` → `bigint`

## Parameters

- First parameter: Input data to hash (`text`, `bytea`, or `integer`)
- Second parameter (optional): Seed value (default: 0)

## Examples

```sql
-- Hash text with default seed
SELECT xxhash3_64('hello world');
-- Result: Ultra-fast 64-bit hash using xxHash3

-- Hash text with custom seed
SELECT xxhash3_64('hello world', 42);
-- Result: Seeded 64-bit hash using xxHash3

-- Hash bytea data
SELECT xxhash3_64('hello world'::bytea);
-- Result: 64-bit hash of bytea data

-- Hash bytea with custom seed
SELECT xxhash3_64('hello world'::bytea, 42);
-- Result: Seeded 64-bit hash of bytea data

-- Hash integer values
SELECT xxhash3_64(12345);
-- Result: 64-bit hash of integer

-- Hash integer with custom seed
SELECT xxhash3_64(12345, 42);
-- Result: Seeded 64-bit hash of integer
```

## Features

- **Improved Performance**: Significantly faster than xxHash64, especially for small inputs
- **Better Hash Quality**: Superior dispersion and reduced collision rate
- **SIMD Optimization**: Takes advantage of vectorized instructions when available
- **Adaptive Algorithm**: Uses different strategies based on input size for optimal performance
- **Cross-Platform Consistency**: Produces identical results across different architectures

## Algorithm Details

xxHash3_64 uses different algorithms based on input size:

- **Small inputs (0-16 bytes)**: Direct mixing with secret array
- **Medium inputs (17-240 bytes)**: Iterative accumulation with secret material
- **Large inputs (241+ bytes)**: Stripe-based processing with accumulators

## Use Cases

- High-performance hash tables and data structures
- Real-time data processing and analytics
- Stream processing with ultra-low latency requirements
- Applications requiring consistent cross-platform hashing
- Modern applications needing the latest hash algorithm improvements